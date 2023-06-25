/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFileDescriptor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:40:16 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/24 18:16:45 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AFileDescriptor.hpp"

/*****************
 * CANNONICAL FORM
 *****************/

AFileDescriptor::AFileDescriptor(void)
{}

AFileDescriptor::AFileDescriptor(AFileDescriptor const &copy) : _fd(copy._fd)
{}

AFileDescriptor &AFileDescriptor::operator=(AFileDescriptor const &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
	}
	return (*this);
}

AFileDescriptor::~AFileDescriptor()
{}

/******************************************************************************/

/**************
 * CONSTRUCTORS
 ***************/

AFileDescriptor::AFileDescriptor(int fd) : _fd(fd)
{}

/******************************************************************************/

/************
 * ACCESSORS
 ************/

int AFileDescriptor::getFd() const
{
	return (_fd);
}

void	AFileDescriptor::setFd(int fd)
{
	_fd = fd;
}

/******************************************************************************/
