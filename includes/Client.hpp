/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/19 20:13:42 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AFileDescriptor.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Syntax.hpp"

#define BUFFER_SIZE		1024

class Cgi;

class Client : public AFileDescriptor
{
	private:
		std::vector<unsigned char>		_rawData;
		std::vector<ServerConf> 		_serverInfo;
		ServerConf 						_serverInfoCurr; 
		Request							_request;
		bool							_responseReady;
		Cgi *							_cgi;

		Client(void);

		bool	searchRequestLine();
		bool	searchHeaders();

	public:
		
		Client(Client const & copy);
		Client & operator=(Client const & rhs);
		virtual ~Client();

		// Constructors
		Client(int fd, std::vector<ServerConf> const & serverInfo);

		// Geters
		Request const & getRequest() const;
		ServerConf const &	getServerInfo() const;

		// Public methods
		virtual void doOnRead(WebServ & webserv);
		virtual void doOnWrite(WebServ & webServ);
		virtual void doOnError(WebServ & webServ, uint32_t event);

		void	errorResponse(status_code_t status);
		void	getResponse();
		void	responseCgi(std::string const & response);
};

#endif
