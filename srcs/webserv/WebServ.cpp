/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/03 16:56:34 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Exception.hpp"
#include "Client.hpp"
#include "Cgi.hpp"

#include <cstring> // strerror, bzero
#include <errno.h> // errno
#include <unistd.h> // close
#include <sys/socket.h> // accept
#include <fcntl.h> // fcntl
#include <stdio.h> // REMOVE
#include "Debugger.hpp"

/*****************
* CANNONICAL FORM
*****************/

WebServ::WebServ()
{}

WebServ::WebServ(WebServ const & copy)
    :   _epollFd(copy._epollFd),
        _mapFd(copy._mapFd),
		_clientTimes(copy._clientTimes)
{}

WebServ &   WebServ::operator=(WebServ const & rhs)
{
    if (this != &rhs)
    {
        _epollFd = rhs._epollFd;
        _mapFd = rhs._mapFd;
		_clientTimes = rhs._clientTimes;
    }
    return (*this);
}

WebServ::~WebServ()
{
	std::map<int, AFileDescriptor *>::iterator it = this->_mapFd.begin();
	for (; it != this->_mapFd.end(); it++)
	{
		delete it->second;
		close(it->first);
	}
	close(_epollFd);

}
/******************************************************************************/

/****************
* PUBLIC METHODS
****************/

/// @brief 
/// @param fileDescriptor 
void    WebServ::addFd(AFileDescriptor * fileDescriptor)
{
    _mapFd[fileDescriptor->getFd()] = fileDescriptor;
}

/// @brief 
/// @param fdTime 
void	WebServ::addClientTimes(std::pair<int, long long> clientInfo)
{
	_clientTimes.push_back(clientInfo);
}

/// @brief Init an epoll and add listening socket
/// @throw EpollInitError
void    WebServ::epollInit()
{
    std::map<int, AFileDescriptor *>::const_iterator   	it;
    struct epoll_event                      			event;

    if ((_epollFd = epoll_create(1)) < 0)
        throw EpollInitError(strerror(errno));

    bzero(&event, sizeof(event));
    event.events = EPOLLIN;
    for (it = _mapFd.begin(); it != _mapFd.end(); it++)
    {
        event.data.fd = it->first;
        if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, it->first, &event) < 0)
            throw EpollInitError(strerror(errno));
    }
}

/// @brief Start webServ and handle events with epoll
void    WebServ::start()
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];
    int j = 0;

	while (j != 1)
	{
        // How handle if nfds < 0
		nfds = epoll_wait(this->_epollFd, events, MAX_EVENTS, 0);

		for (int i = 0; i < nfds; i++)
		{
			int					fd = events[i].data.fd;
			uint32_t			event = events[i].events;
			AFileDescriptor *	aFd = _mapFd[fd];
			
			if (event & EPOLLIN)
				aFd->doOnRead(*this);
			if (event & EPOLLOUT)
				aFd->doOnWrite(*this);
			if (!(event & EPOLLIN) && !(event & EPOLLOUT))
				aFd->doOnError(*this, event);
		}
		//handleTimeout();
	}
}

void	WebServ::updateEpoll(int fd, u_int32_t event, int mod)
{
	struct epoll_event ev;

	bzero(&ev, sizeof(ev));
	ev.events = event;
	ev.data.fd = fd;
	if (epoll_ctl(_epollFd, mod, fd, &ev) < 0)
		throw EpollInitError(strerror(errno));
}

void	WebServ::removeFd(int fd)
{
	close(fd);
	if (_mapFd.find(fd) == _mapFd.end())
		return ;
	delete _mapFd[fd];
	_mapFd.erase(fd);
	
	std::vector<std::pair<int, long long> >::iterator it = _clientTimes.begin();
	for (; it != _clientTimes.end(); it++)
	{
		if (it->first == fd)
		{
			_clientTimes.erase(it);
			break;
		}
	}
}
/******************************************************************************/

/****************
* PRIVATE METHODS
****************/

/******************************************************************************/


/***********************
* PUBLIC STATIC METHODS
***********************/

//void	WebServ::popFd(int fd)
//{
//	close()
//}

