/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/12 09:16:18 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Exception.hpp"
#include "SocketFd.hpp"
#include "Cgi.hpp"
#include <cstring> // strerror, bzero
#include <errno.h> // errno
#include <unistd.h> // close
#include <sys/socket.h> // accept
#include <fcntl.h> // fcntl
#include <stdio.h> // REMOVE

/*****************
* CANNONICAL FORM
*****************/

WebServ::WebServ()
{}

WebServ::WebServ(WebServ const & copy)
    :   _epollFd(copy._epollFd),
        _mapServs(copy._mapServs),
        _mapFd(copy._mapFd)
{}

WebServ &   WebServ::operator=(WebServ const & rhs)
{
    if (this != &rhs)
    {
        this->_epollFd = rhs._epollFd;
        this->_mapServs = rhs._mapServs;
        this->_mapFd = rhs._mapFd;
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

	std::map<int, Server>::iterator it1;
	for (it1 = this->_mapServs.begin(); it1 != this->_mapServs.end(); it1++)
		close(it1->first);
	close(this->_epollFd);
}
/******************************************************************************/

/****************
* PUBLIC METHODS
****************/

/// @brief Add a pair to map servers
/// @param server <file descriptor, Server object>
void    WebServ::addServer(std::pair<int, Server> server)
{
    this->_mapServs.insert(server);
}

/// @brief Init an epoll and add listening socket
/// @throw EpollInitError
void    WebServ::epollInit()
{
    std::map<int, Server>::const_iterator   it;
    struct epoll_event                      event;

    if ((this->_epollFd = epoll_create(1)) < 0)
        throw EpollInitError(strerror(errno));

    bzero(&event, sizeof(event));
    event.events = EPOLLIN;
    for (it = this->_mapServs.begin(); it != this->_mapServs.end(); it++)
    {
        event.data.fd = it->first;
        if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, it->first, &event) < 0)
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
		nfds = epoll_wait(this->_epollFd, events, MAX_EVENTS, -1);

		for (int i = 0; i < nfds; i++)
		{
			int			fd = events[i].data.fd;
			uint32_t	event = events[i].events;

			if (this->_mapServs.find(fd) != this->_mapServs.end())
				clientConnect(fd);
			else 
			{
				if (event & EPOLLIN)
					doOnRead(fd);
				if (event & EPOLLOUT)
					doOnWrite(fd);
				if (event & EPOLLHUP)
				{
					doOnRead(fd);
					// TODO: Handle event error
					// std::cout << "default => event = " << event;
					// _mapFd.erase(fd); // REMOVE TEST
					// close(fd); // REMOVE TEST
				}
			}

		}
	}
}
/******************************************************************************/

/****************
* PRIVATE METHODS
****************/

void    WebServ::clientConnect(int serverFd)
{
    int					cs;
	struct epoll_event	event;

	if ((cs = accept(serverFd, NULL, NULL)) < 0)
	{
		std::cerr << "Accept error" << std::endl;
		return ;
	}

	if (fcntl(cs, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Fcntl error" << std::endl;
		return ;
	}

	bzero(&event, sizeof(event));
    event.events = EPOLLIN;
	event.data.fd = cs;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, cs, &event) < 0)
	{
		close(cs);
		std::cerr << "Epoll_ctl error" << std::endl;
		return ;
	}
	this->_mapFd[cs] = new SocketFd(cs, (this->_mapServs[serverFd]));
}

void	WebServ::doOnRead(int fd)
{
	AFileDescriptor * fileDescriptor = this->_mapFd[fd];
	SocketFd * socketFd = NULL;
	Cgi * cgi = NULL;
	
	if ((socketFd = dynamic_cast<SocketFd *>(fileDescriptor)) != NULL)
	{
		try {
			socketFd->readRequest(_epollFd);
		}
		catch (std::exception & exception)
		{
			
		}
	}
	else
	{
		cgi = dynamic_cast<Cgi *>(fileDescriptor);
		cgi->readCgi();
	}

}

void	WebServ::doOnWrite(int fd)
{
	AFileDescriptor * fileDescriptor = this->_mapFd[fd];
	SocketFd * socketFd = NULL;
	//Cgi * cgi = NULL;
	//int ret;
	
	if ((socketFd = dynamic_cast<SocketFd *>(fileDescriptor)) != NULL)
	{
		socketFd->sendResponse(_epollFd, _mapFd);
	}
	else
	{
		//cgi = reinterpret_cast<Cgi *>(fileDescriptor);
		//cgi->sendBody();
	}
}

//void	WebServ::popFd(int fd)
//{
//	close()
//}

void	WebServ::updateEpoll(int epoll, int fd, u_int32_t event, int mod)
{
	struct epoll_event ev;

	bzero(&ev, sizeof(ev));
	ev.events = event;
	ev.data.fd = fd;
	if (epoll_ctl(epoll, mod, fd, &ev) < 0)
		throw EpollInitError(strerror(errno));
}
