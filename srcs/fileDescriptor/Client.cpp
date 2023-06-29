/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eantoine <eantoine@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/10 19:37:24 by eantoine         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Autoindex.hpp"
#include "Debugger.hpp"
#include "StringUtils.hpp"
#include "FileUtils"
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
	: AFileDescriptor(), _responseReady(false), _cgi(NULL)
{
}

Client::Client(Client const &copy)
	: AFileDescriptor(copy),
	  _rawData(copy._rawData),
	  _serverInfo(copy._serverInfo),
	  _serverInfoCurr(copy._serverInfoCurr),
	  _request(copy._request),
	  _responseReady(copy._responseReady),
	  _cgi(copy._cgi)
{
}

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_rawData = rhs._rawData;
		_serverInfo = rhs._serverInfo;
		_serverInfoCurr = rhs._serverInfoCurr;
		_request = rhs._request;
		_responseReady = rhs._responseReady;
		_cgi = rhs._cgi;
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

	if (n < 0) // Try to read next time fd is NON_BLOCK and we can't check errno
		return;
	if (n == 0) // Client close connection
	{
		webServ.removeFd(_fd);
		return;
	}

	try 
	{
		if (_request.getHttpMethod().empty())
			searchRequestLine();
		if (_request.getHeaders().empty())
			searchHeaders();
		if (_request.hasMessageBody())
			_request.handleMessageBody(_rawData);
	}
	catch (RequestUncomplete & exception)
	{
		return ;
	}
	catch (std::exception & exception)
	{
		handleException(exception);
		webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
		return ;
	}

	_serverInfoCurr = getCorrectServer(); 
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
			errorResponse(INTERNAL_SERVER_ERROR);
			webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
			return;
		}
		webServ.addFd(new Cgi(cgi));
		webServ.updateEpoll(_fd, 0, EPOLL_CTL_MOD);
		webServ.updateEpoll(cgi.getReadFd(), EPOLLIN, EPOLL_CTL_ADD);
	}
	else
	{
		getResponse();
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

void	Client::searchRequestLine()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 2);
	if (it == _rawData.end())
		throw RequestUncomplete();
	_request.handleRequestLine(std::string(_rawData.begin(), it));
	_rawData.erase(_rawData.begin(), it);
}

void	Client::searchHeaders()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n', '\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 4);
	if (it == _rawData.end())
		throw RequestUncomplete();
	_request.handleHeaders(std::string(_rawData.begin(), it));
	_rawData.erase(_rawData.begin(), it + 4);
}

void	Client::errorResponse(status_code_t status)
{
	//TODO: Find error file 
	std::string extension = "html";

	std::string error = "<html>\n<head><title>" + StringUtils::intToString(status) + " " + HttpUtils::RESPONSE_STATUS.at(status) + "</title></head>\n<body>\n<center><h1>" + StringUtils::intToString(status) + HttpUtils::RESPONSE_STATUS.at(status) + "</h1></center>\n<hr><center>webserv (Ubuntu)</center>\n</body>\n</html>\n";
	std::vector<unsigned char> body = std::vector<unsigned char>(error.begin(), error.end());

	resp_t resp = {status, body, extension, _rawData, false};
	Response::createResponse(resp);
	_responseReady = true;
}

void	Client::getResponse()
{
	//TODO: Verifier avec le serverConf le path du fichier et son existence OU errorResponse(NOT_FOUND) and change / to index.html
	std::cout << _request;
	//TODO: Verifier avec le serverConf le path du fichier et son existence OU errorResponse(NOT_FOUND)
	int Valid = pathIsValid(_request.getPathRequest());
	if (!Valid)
	{
		errorResponse(NOT_FOUND);
	}
	else if (Valid == 2)
	{
	//if directory and autoindex is on use autoindex to generate answer
	}

	std::vector<unsigned char> body;
    std::string filename = _serverInfoCurr.getRoot() + "/" + _request.getPathRequest();
    std::ifstream is (filename.c_str(), std::ifstream::binary);

    if (is.good()) {
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);

        char * buffer = new char [length];
        is.read (buffer,length);
		body = std::vector<unsigned char>(buffer, buffer+length);
        is.close();
		delete buffer;

		resp_t resp = {OK, body, _request.getExtension(), _rawData, true};
		Response::createResponse(resp);
		_responseReady = true;
	}
	else
	{
		errorResponse(NOT_FOUND);
	}
}

ServerConf Client::getCorrectServer()
{
	std::vector<ServerConf>::iterator it = _serverInfo.begin();
	for (; it !=_serverInfo.end(); it++)
	{
		if (_request.getHeaders().find("Host")->second == it->getName())
			return (*it);
	}
	it = _serverInfo.begin();
	for (; it !=_serverInfo.end(); it++)
	{
		std::vector<Location>::const_iterator it1 = it->getLocation().begin();
		for (; it1 != it->getLocation().end(); it1++)
			if (_request.getPathRequest() == it1->getlocRoot())
				return (*it);
	}
	return (*(_serverInfo.begin()));

}

void	Client::handleException(std::exception & exception)
{
	DEBUG_COUT(exception.what());
	try
	{ 
		dynamic_cast<RequestError &>(exception);
		errorResponse(BAD_REQUEST);
	} catch (std::exception & exception)
	{
		errorResponse(INTERNAL_SERVER_ERROR);
	}
}

int		pathIsValid(std::string const path)
{
	if (path.empty() || !FileUtils::fileExists(path.c_str()) ||
		!FileUtils::fileRead(path.c_str()))
		return 0;
	else if (FileUtils::isDirectory(path.c_str()))
		return 2;
	else
		return 1;
	
}