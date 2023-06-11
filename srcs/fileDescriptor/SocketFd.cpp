/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/07 22:57:27 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"
#include "Debugger.hpp"

#include <cstddef>
#include <iostream>
#include <unistd.h> // read
#include <cstdio> // perror
#include <fstream>
#include <sys/socket.h> // recv
#include <algorithm> // search

/*****************
* CANNONICAL FORM
*****************/

SocketFd::SocketFd(void) : AFileDescriptor(), _serverInfo(NULL)
{}

SocketFd::SocketFd(SocketFd const & copy)
	:	AFileDescriptor(copy), _serverInfo(copy._serverInfo)
{}

SocketFd & SocketFd::operator=(SocketFd const & rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_rawData = rhs._rawData;
		this->_open = rhs._open;
	}

	return (*this);

}

SocketFd::~SocketFd()
{}
/******************************************************************************/

/**************
* CONSTRUCTORS
***************/

SocketFd::SocketFd(int fd, std::vector<Server> const * serverInfo)
	:	AFileDescriptor(fd), _serverInfo(serverInfo)
{}
/******************************************************************************/

/***********
* ACCESSORS
************/

Request const &	SocketFd::getRequest() const
{
	return (this->_request);
}

Server const &	SocketFd::getServerInfo() const
{
	return (*this->_serverInfo);
}
/******************************************************************************/

/****************
* PUBLIC METHODS
****************/

/// @brief 
/// @return 
int		SocketFd::readRequest()
{
	char	buffer[BUFFER_SIZE];
	ssize_t	n;
	std::vector<unsigned char>::iterator it;
	int ret;

	if ((n = recv(this->_fd, buffer, BUFFER_SIZE, 0)) > 0)
		this->_rawData.assign(buffer, buffer + n);
	
	// Try to read next time fd is NON_BLOCK and we must not check errno
	if (n < 0)
		return READ_AGAIN;
	// Socket connection close, a EOF was present
	if (n == 0)
		return CLIENT_CLOSE;
	// Try to retrieve request line
	if (_request.getHttpMethod().empty() && (ret = searchRequestLine()) != 0)
		return (ret);
	// Try to retrieve headers
	if (!_request.getHttpMethod().empty() && _request.getHeaders().empty()
		&& (ret = searchHeaders()) != 0)
			return (ret);
	// Try to retrieve message body if necessary
	if (_request.hasMessageBody())
	{
		if ((ret = _request.handleMessageBody(_rawData)) != 0)
			return ret;
	}
	_rawData.erase(_rawData.begin(), _rawData.end());
	return SUCCESS;
}

/// @brief 
void	SocketFd::sendResponse()
{
	send(_fd, &(_rawData[0]), _rawData.size(), 0);
}

/// @brief 
void	SocketFd::prepareResponse(int ret, int epollFd)
{
	(void) ret;
	(void) epollFd;
	
	std::cout << "======= REQUEST ======" << std::endl;
	_request.print();
}


/******************************************************************************/

/*****************
* PRIVATE METHODS
*****************/

int		SocketFd::searchRequestLine()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 2);
	if (it == _rawData.end())
		return READ_AGAIN;
	if (!_request.handleRequestLine(std::string(_rawData.begin(), it)))
		return ERROR;
	_rawData.erase(_rawData.begin(), it);
	return SUCCESS;
}

int		SocketFd::searchHeaders()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n', '\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 4);
	if (it == _rawData.end())
		return READ_AGAIN;
	if (!_request.handleHeaders(std::string(_rawData.begin(), it)))
		return ERROR;
	_rawData.erase(_rawData.begin(), it + 4);
	return SUCCESS;
}