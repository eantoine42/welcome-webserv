/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 18:05:52 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 22:19:17 by lfrederi         ###   ########.fr       */
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
		_serverConfs = src._serverConfs;
	}
	return *this;
}

Server::~Server()
{}

Server::Server(int fd, std::vector<ServerConf> const & serverConfs)
	: 	AFileDescriptor(fd),
		_serverConfs(serverConfs)
{}

void	Server::doOnRead(WebServ & webServ)
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
		webServ.updateEpoll(cs, EPOLLIN, EPOLL_CTL_ADD);
	} 
	catch (EpollInitError & e)
	{
		close(cs);
		throw e;
	}

	webServ.addFd(new Client(cs, _serverConfs));
	webServ.addClientTimes(std::make_pair(cs, TimeUtils::getTimeOfDayMs()));
}

void	Server::doOnWrite(WebServ & webServ)
{
	(void) webServ;
}

void	Server::doOnError(WebServ & webServ, uint32_t event)
{
	(void) webServ;
	std::cout << "Client on error, event = " << event << std::endl;
}