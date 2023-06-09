/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/02 16:55:54 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Exception.hpp"
#include "SocketFd.hpp"
#include <sys/epoll.h> // epoll_create
#include <cstring> // strerror
#include <errno.h> // errno
#include <unistd.h> // bzero, close
#include <sys/socket.h> // accept
#include <fcntl.h> // fcntl

WebServ::WebServ()
{}

WebServ::WebServ(WebServ const & copy)
    :   _epollFd(copy._epollFd),
        _mapServers(copy._mapServers),
        _mapFileDescriptors(copy._mapFileDescriptors)
{}

WebServ &   WebServ::operator=(WebServ const & rhs)
{
    if (this != &rhs)
    {
        this->_epollFd = rhs._epollFd;
        this->_mapServers = rhs._mapServers;
        this->_mapFileDescriptors = rhs._mapFileDescriptors;
    }
    return (*this);
}

WebServ::~WebServ()
{
	std::map<int, AFileDescriptor *>::iterator it;
	for (it = this->_mapFileDescriptors.begin(); it != this->_mapFileDescriptors.end(); it++)
	{
		delete it->second;
		close(it->first);
	}
	std::map<int, std::vector<Server> >::iterator it1;
	for (it1 = this->_mapServers.begin(); it1 != this->_mapServers.end(); it1++)
		close(it1->first);
	close(this->_epollFd);
}

void    WebServ::addServer(std::pair<int, Server> server)
{
    std::pair<int, std::vector<Server> > serv;
	serv.first = server.first;
	serv.second.push_back(server.second);
	this->_mapServers.insert(serv);
 }

void    WebServ::epollInit()
{
    std::map<int, std::vector<Server> >::const_iterator   it;
    struct epoll_event                      event;

    if ((this->_epollFd = epoll_create(1)) < 0)
        throw EpollInitError(strerror(errno));

    bzero(&event, sizeof(event));
    event.events = EPOLLIN;
    for (it = this->_mapServers.begin(); it != this->_mapServers.end(); it++)
    {
        event.data.fd = it->first;
        if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, it->first, &event) < 0)
            throw EpollInitError(strerror(errno));
    }
}

void    WebServ::start()
{
    int nfds;
    struct epoll_event ev, events[MAX_EVENTS];
    int j = 0;

	while (j != 1)
	{
        // How handle if nfds < 0
		nfds = epoll_wait(this->_epollFd, events, MAX_EVENTS, -1);

		for (int i = 0; i < nfds; i++)
		{
			int			fd = events[i].data.fd;
			uint32_t	event = events[i].events;

			if (this->_mapServers.find(fd) != this->_mapServers.end())
				clientConnect(fd);
			else 
			{
				switch (event)
				{
					case EPOLLIN:
						if (this->_mapFileDescriptors[fd]->doOnRead() == Request::requestComplete)
						{
							bzero(&ev, sizeof(ev));
							ev.events = EPOLLOUT;
							ev.data.fd = fd;
							epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, fd, &ev);
						}
						break;
					case EPOLLOUT:
						this->_mapFileDescriptors[fd]->doOnWrite();
						// Don't close if if header alive is present
                        delete this->_mapFileDescriptors[fd];
						this->_mapFileDescriptors.erase(fd);
						close(fd);
						//j = 1;
						break;
					default:
						std::cout << "default => event = " << event;
						/* DEBUG_COUT("Error"); */
				}
			}

		}
	}
}

void    WebServ::clientConnect(int serverFd)
{
    int					socketConnect;
	struct epoll_event	event;

	if ((socketConnect = accept(serverFd, NULL, NULL)) < 0)
	{
		std::cerr << "Accept error" << std::endl;
		return ;
	}

	if (fcntl(socketConnect, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Fcntl error" << std::endl;
		return ;
	}

	bzero(&event, sizeof(event));
    event.events = EPOLLIN;
	event.data.fd = socketConnect;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, socketConnect, &event) < 0)
	{
		close(socketConnect);
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
