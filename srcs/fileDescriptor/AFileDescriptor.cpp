/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFileDescriptor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:40:16 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/12 22:25:00 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AFileDescriptor.hpp"

/*****************
 * CANNONICAL FORM
 *****************/

AFileDescriptor::AFileDescriptor(void)
{
}

AFileDescriptor::AFileDescriptor(AFileDescriptor const &copy)
	: _fd(copy._fd), _rawData(copy._rawData)
{
}

AFileDescriptor &AFileDescriptor::operator=(AFileDescriptor const &rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_rawData = rhs._rawData;
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
	return (this->_fd);
}

std::vector<unsigned char> const &AFileDescriptor::getRawData() const
{
	return (this->_rawData);
}
/******************************************************************************/
