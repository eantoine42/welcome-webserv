/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/18 18:15:45 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketFd.hpp"
#include "AFileDescriptor.hpp"

#include <iostream>
#include <unistd.h> // read

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
	std::cout << "doOnRead()" << std::endl;
	
	char	buffer[BUFFER_SIZE];
	size_t	bytes;

	bytes = read(this->_fd, buffer, BUFFER_SIZE - 1);
	if (bytes <= 0)
		close(this->_fd);
	buffer[bytes] = '\0';

	this->_rawData.append(buffer);
	// Check if headers in request is empty or not

	return (0);
}

int		SocketFd::doOnWrite()
{
	std::cout << "doOnWrite()" << std::endl;
	return (0);
}
