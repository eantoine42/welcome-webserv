/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:10 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/21 22:06:56 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERV_HPP
#define WEB_SERV_HPP

#include <map>
#include <sys/epoll.h>
#include "Server.hpp"
#include "AFileDescriptor.hpp"

#define MAX_EVENTS 1024

class WebServ
{
    private:

        int                                     _epollFd;
        std::map<int, AFileDescriptor *>        _mapFd;
        std::vector<std::pair<int, long long> > _times;

    public:

        // Cannonical Form
		WebServ();
		WebServ(WebServ const & copy);
		WebServ & operator=(WebServ const & rhs);
		~WebServ();

        void    addServer(int socketFd, Server const & server);
        void    epollInit();
        void    start();


        static void	updateEpoll(int epoll, int fd, u_int32_t event, int mod);
};

#endif