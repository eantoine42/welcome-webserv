/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 18:05:52 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/21 23:25:35 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Exception.hpp"
#include "TimeUtils.hpp"
#include "Client.hpp"
#include "WebServ.hpp"

#include <unistd.h>		// close
#include <sys/socket.h> // accept
#include <fcntl.h>		// fcntl
#include <cstring>		// bzero
#include <sys/epoll.h>	// epoll_ctl
#include <cerrno>		// errno

/*****************
 * CANNONICAL FORM
 *****************/

Server::Server(void) : AFileDescriptor()
{}

Server::Server(Server const &src)
	: 	AFileDescriptor(src),
		_serverConfs(src._serverConfs)
{}

Server &	Server::operator=(Server const &src)
{
	if (this != &src)
	{
		_fd = src._fd;
		_webServ = src._webServ;
		_serverConfs = src._serverConfs;
	}
	return *this;
}

Server::~Server()
{}
/******************************************************************************/

/**************
 * CONSTRUCTORS
 ***************/

Server::Server(int fd, WebServ & webServ, std::vector<ServerConf> const & serverConfs)
	: 	AFileDescriptor(fd, webServ),
		_serverConfs(serverConfs)
{}
/******************************************************************************/

/****************
 * PUBLIC METHODS
 ****************/

/**
 * @brief 
 * @param webServ 
 */
void	Server::doOnRead()
{
    int		cs;

	if ((cs = accept(_fd, NULL, NULL)) < 0)
	{
		std::cerr << "Accept error" << std::endl;
		return ;
	}

	if (fcntl(cs, F_SETFL, O_NONBLOCK) < 0)
		throw FileDescriptorError(strerror(errno));

	try
	{
		_webServ->updateEpoll(cs, EPOLLIN, EPOLL_CTL_ADD);
	} 
	catch (EpollInitError & e)
	{
		close(cs);
		throw e;
	}

	_webServ->addFd(cs, new Client(cs, *_webServ, _serverConfs));
	_webServ->addClientTimes(std::make_pair(cs, TimeUtils::getTimeOfDayMs()));
}


/**
 * @brief 
 */
void	Server::doOnWrite()
{
}


/**
 * @brief 
 * @param event 
 */
void	Server::doOnError(uint32_t event)
{
	std::cout << "Client on error, event = " << event << std::endl;
}
/******************************************************************************/