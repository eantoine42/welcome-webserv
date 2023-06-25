/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 18:06:05 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/24 18:14:56 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "AFileDescriptor.hpp"
#include "ServerConf.hpp"
#include <vector>

#define MAX_CLIENT 10 

class Server : public AFileDescriptor
{
	private:

		std::vector<ServerConf> _serverConfs;

	public:
		Server(void);
		Server(Server const &src);
		Server &operator=(Server const &src);
		virtual ~Server();

		Server(int fd, std::vector<ServerConf> const & serverConfs);

		std::vector<ServerConf> const & getServerConfs() const;

		virtual void doOnRead(WebServ & webServ);
		virtual void doOnWrite(WebServ & webServ);
		virtual void doOnError(WebServ & webServ, uint32_t event);

};

#endif
