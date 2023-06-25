/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/25 22:18:48 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Debugger.hpp"
#include "Response.hpp"
#include "Exception.hpp"
#include "WebServ.hpp"
#include "Cgi.hpp"

#include <cstddef>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h> // read
#include <cstdio>	// perror
#include <fstream>
#include <sys/socket.h> // recv
#include <algorithm>	// search

/*****************
 * CANNONICAL FORM
 *****************/

Client::Client(void)
	: AFileDescriptor(), _responseReady(false)
{
}

Client::Client(Client const &copy)
	: AFileDescriptor(copy),
	  _rawData(copy._rawData),
	  _serverInfo(copy._serverInfo),
	  _serverInfoCurr(copy._serverInfoCurr),
	  _request(copy._request),
	  _responseReady(copy._responseReady)
{
}

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_rawData = rhs._rawData;
		_serverInfoCurr = rhs._serverInfoCurr;
		_request = rhs._request;
		_responseReady = rhs._responseReady;
	}

	return (*this);
}

Client::~Client()
{
}
/******************************************************************************/

/**************
 * CONSTRUCTORS
 ***************/
Client::Client(int fd, std::vector<ServerConf> const &serverInfo)
	: AFileDescriptor(fd), _serverInfo(serverInfo)
{
}
/******************************************************************************/

/***********
 * ACCESSORS
 ************/

Request const &Client::getRequest() const
{
	return (this->_request);
}

ServerConf const &Client::getServerInfo() const
{
	return (this->_serverInfoCurr);
}
/******************************************************************************/

/****************
 * PUBLIC METHODS
 ****************/

/// @brief
/// @return
void Client::doOnRead(WebServ &webServ)
{
	char buffer[BUFFER_SIZE];
	ssize_t n;
	std::vector<unsigned char>::iterator it;

	if ((n = recv(_fd, buffer, BUFFER_SIZE, 0)) > 0)
		_rawData.assign(buffer, buffer + n);

	// Try to read next time fd is NON_BLOCK and we must not check errno
	if (n < 0)
		return;
	// Socket connection close, a EOF was present
	if (n == 0)
	{
		close(_fd);
		webServ.removeFd(_fd);
		return;
	}
	if (_request.getHttpMethod().empty() && searchRequestLine() == false)
		return;
	if (_request.getHeaders().empty() && searchHeaders() == false)
		return;
	if (_request.hasMessageBody() && _request.handleMessageBody(_rawData) == false)
		return;
	_rawData.erase(_rawData.begin(), _rawData.end());
	_serverInfoCurr = _serverInfo[0]; // TODO: Find the good server info
	webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
}

/// @brief
void Client::doOnWrite(WebServ &webServ)
{
	if (_responseReady == true)
	{
		send(_fd, &(_rawData[0]), _rawData.size(), 0);
		webServ.updateEpoll(_fd, EPOLLIN, EPOLL_CTL_MOD);
		_responseReady = false;
		_request = Request();
		return;
	}

	if (_request.getExtension().compare("php") == 0)
	{
		Cgi cgi = Cgi(*this);
		if (cgi.run() < 0)
		{
			Response::createResponse(_rawData, *this);
			_responseReady = true;
			webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
			return;
		}
		webServ.addCgi(cgi);
		webServ.updateEpoll(_fd, 0, EPOLL_CTL_MOD);
		webServ.updateEpoll(cgi.getReadFd(), EPOLLIN, EPOLL_CTL_ADD);
	}
	else
	{
		Response::createResponse(_rawData, *this);
		_responseReady = true;
		webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
	}
}

/// @brief
/// @param mapFd
/// @param event
void Client::doOnError(WebServ &webServ, uint32_t event)
{
	std::cout << "Client on error, event = " << event << std::endl;
	webServ.removeFd(_fd);
}

void Client::responseCgi(std::string const &response)
{
	_responseReady = true;
	_rawData.assign(response.begin(), response.end());
}

/******************************************************************************/

/*****************
 * PRIVATE METHODS
 *****************/

bool Client::searchRequestLine()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 2);
	if (it == _rawData.end())
		return false;
	if (!_request.handleRequestLine(std::string(_rawData.begin(), it)))
		throw RequestError("Invalid request line");
	_rawData.erase(_rawData.begin(), it);
	return true;
}

bool Client::searchHeaders()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n', '\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 4);
	if (it == _rawData.end())
		return false;
	if (!_request.handleHeaders(std::string(_rawData.begin(), it)))
		throw RequestError("Invalid headers");
	_rawData.erase(_rawData.begin(), it + 4);
	return true;
}