/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 19:39:10 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/02 11:02:58 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERV_HPP
#define WEB_SERV_HPP

#include <map>
#include "Server.hpp"
#include "AFileDescriptor.hpp"

#define MAX_EVENTS 1024

class WebServ
{
    private:

        int                                 _epollFd;
        std::map<int, Server>               _mapServers;
        std::map<int, AFileDescriptor *>    _mapFileDescriptors;

        void    clientConnect(int serverFd);

    public:

        // Cannonical Form
		WebServ();
		WebServ(WebServ const & copy);
		WebServ & operator=(WebServ const & rhs);
		~WebServ();

        void    addServer(std::pair<int, Server> server);
        void    epollInit();
        void    start();
        
};

#endif