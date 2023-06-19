/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/19 17:51:39 by lfrederi         ###   ########.fr       */
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
        _mapFd(copy._mapFd)
{}

WebServ &   WebServ::operator=(WebServ const & rhs)
{
    if (this != &rhs)
    {
        this->_epollFd = rhs._epollFd;
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
	close(_epollFd);

}
/******************************************************************************/

/****************
* PUBLIC METHODS
****************/

/// @brief Add a pair to map servers
/// @param server <file descriptor, Server object>
void    WebServ::addServer(int socketFd, Server const & server)
{
	Server * serv = new Server(server);
	serv->setFd(socketFd);
    _mapFd[socketFd] = serv;


  std::pair<int, std::vector<Server> > serv;
	serv.first = server.first;
	serv.second.push_back(server.second);
	this->_mapServers.insert(serv);

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
		it->second->setEpollFd(_epollFd);
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
		nfds = epoll_wait(this->_epollFd, events, MAX_EVENTS, -1);



		for (int i = 0; i < nfds; i++)
		{
			int					fd = events[i].data.fd;
			uint32_t			event = events[i].events;
			AFileDescriptor *	aFd = _mapFd[fd];
			if (event & EPOLLIN)
				aFd->doOnRead(_mapFd);
			if (event & EPOLLOUT)
				aFd->doOnWrite(_mapFd);
			if (!(event & EPOLLIN) && !(event & EPOLLOUT))
				aFd->doOnError(_mapFd, event);
		}
	}
}
/******************************************************************************/

/****************
* PRIVATE METHODS
****************/

/******************************************************************************/
=======
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
	this->_mapFileDescriptors[socketConnect] = new SocketFd(socketConnect, &(this->_mapServers[serverFd]));
}


//check if the server has same IP/port matching in the Webserv map
// returns -1 if creates new socket and returns >=0 if only need to add server to the vector within the map
int WebServ::avoidDoubleSocket(Server const & server)
{
	if (this->_mapServers.empty())
		return (-1);
	std::map<int, std::vector<Server> >::iterator it;
	int i = 0;
	it = _mapServers.begin();
	for (;it!=_mapServers.end();it++, i++)
	{
		if(server.getPort() == (*it).second[0].getPort() && server.getIp() == (*it).second[0].getIp())
			return (i);
	}
	return (-1);
}

void	WebServ::addServerInVector(int i, Server const & servers)
{
	std::map<int, std::vector<Server> >::iterator it;
	it = _mapServers.begin();
	while (i > 0)
	{
		it++;
		i--;
	}
	it->second.push_back(servers);
}

void 	WebServ::print_serv()
{
	std::map<int, std::vector<Server> >::iterator itm;
	std::vector<Server>::iterator itv;
	itm=_mapServers.begin();
	for(;itm !=_mapServers.end(); itm++)
	{
		std::cout<<"nom IP : "<<(*itm).second[0].getIp()<<"  port: "<<(*itm).second[0].getPort()<<std::endl;
		itv=(*itm).second.begin();
		for(;itv !=(*itm).second.end(); itv++)
			std::cout<<"nom de serveur : "<<(*itv).getName()<<std::endl;
	}
}
>>>>>>> origin/eric

/***********************
* PUBLIC STATIC METHODS
***********************/
void	WebServ::updateEpoll(int epoll, int fd, u_int32_t event, int mod)
{
	struct epoll_event ev;

	bzero(&ev, sizeof(ev));
	ev.events = event;
	ev.data.fd = fd;
	if (epoll_ctl(epoll, mod, fd, &ev) < 0)
		throw EpollInitError(strerror(errno));
}


void	WebServ::doOnWrite(int fd)
{
	AFileDescriptor * fileDescriptor = this->_mapFd[fd];
	SocketFd * socketFd = NULL;
	//Cgi * cgi = NULL;
	//int ret;
	
	if ((socketFd = reinterpret_cast<SocketFd *>(fileDescriptor)) != NULL)
	{
		socketFd->sendResponse();
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

