/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 18:12:31 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/23 18:22:03 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP

#include "Location.hpp"
#include <string>
#include <vector>
#include <map>

#define MAX_CLIENT 10 

class Server
{
	private:

		std::string							_root;
		int									_port;
		std::string							_server_name;
		std::string							_IP;
		std::string							_error_pages;
		std::string							_index;
		bool								_autoindex;
		int									_client_body_size;
		std::map<std::string, std::string>	_cgi;
		std::vector<Location>				_location;


		void						setCgi(std::vector<std::string> token);
		void						setPort(std::vector<std::string> token);
		void						setRoot(std::vector<std::string> token);
		void						setIp(std::vector<std::string> token);
		void						setIndex(std::vector<std::string> token);
		void						setAutoindex(std::vector<std::string> token);
		void						setName(std::vector<std::string> token);
		void						setError(std::vector<std::string> token);
		void						setClientBodySize(std::vector<std::string> token);
		int							skipLocationBlock(std::string str, int count);
		void						parseServer(std::string str, int &count);
		int							getLocationBloc(std::string str, int &count);
		typedef void (Server::*Server_func)(std::vector<std::string> );
		void init_vector_Server_fct(std::vector<Server_func> &funcs);


	public:
		Server(void);
		Server(Server const &src);
		Server &operator=(Server const &src);
		virtual ~Server();

		void						addLocation(std::string str, int &count, int &Server_ct);

		/*
		 ** Server getters
		 */
		int							const &getPort() const;
		std::string					const &getRoot() const;
		std::string					const &getName() const;
		std::string					const &getIp() const;
		std::string					const &getError() const;
		std::string					const &getIndex() const;
		bool						const &getAutoindex() const;
		int							const &getClientBodySize() const;
		std::vector<Location>		const &getLocation() const;
		std::map<std::string, std::string>	const &getCgi() const;

		void						setServer(const std::string &str);
		void 						cleanNames(Server &serv2);
		void 						cleanDupServer(std::vector<Server> ServerInfo);

};

std::ostream    &operator<<(std::ostream &o, Server const &i);

std::ostream    &operator<<(std::ostream &o, std::vector<Server>  const &srv);

#endif
