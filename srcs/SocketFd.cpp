/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/18 23:06:55 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"
#include "AFileDescriptor.hpp"

#include <cstddef>
#include <iostream>
#include <unistd.h> // read
#include <cstdio> // perror

SocketFd::SocketFd(void) : AFileDescriptor()
{}

SocketFd::SocketFd(SocketFd const & copy)
	:	AFileDescriptor(copy)
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
{
	std::cout << "Destructor SocketFd" << std::endl;
}

SocketFd::SocketFd(int fd)
	:	AFileDescriptor(fd)
{}

int		SocketFd::doOnRead()
{
	char	buffer[BUFFER_SIZE];
	size_t	bytes;
	size_t	posHeadersEnd;

	if ((bytes = read(this->_fd, buffer, BUFFER_SIZE - 1)) <= 0)
	{
		if (bytes == (size_t) -1)
			perror("read");
		close(this->_fd);
	}
	buffer[bytes] = '\0';

	this->_rawData.append(buffer);
	posHeadersEnd = this->_rawData.find("\r\n\r\n");
	// Try to fill hearders if empty
	if (this->_request.getHeaders().empty())
	{
		if (posHeadersEnd == std::string::npos)
			return (Request::requestUncomplete);
		this->_request.fillHeaders(this->_rawData);
		std::cout << this->_rawData;
	}

	if (!this->_request.hasMessageBody())
		return (Request::requestComplete);
	this->_request.fillMessageBody(this->_rawData);
	if (this->_request.isMessageBodyTerminated())
		return (Request::requestComplete);

	return (Request::requestUncomplete);
}

int		SocketFd::doOnWrite()
{
	std::cout << "doOnWrite()" << std::endl;
	return (0);
}
