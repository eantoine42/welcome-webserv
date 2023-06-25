/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:10 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/24 19:09:54 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERV_HPP
#define WEB_SERV_HPP

#include <map>
#include <sys/epoll.h>
#include "Server.hpp"
#include "AFileDescriptor.hpp"
#include "Cgi.hpp"

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

        void    addServer(Server const & server);
        void    addClient(Client const & client);
        void    addCgi(Cgi const & cgi);
        void    removeFd(int fd);
        void    epollInit();
        void    start();

        void	updateEpoll(int fd, u_int32_t event, int mod);
};

#endif