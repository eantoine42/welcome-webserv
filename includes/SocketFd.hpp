/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/12 22:01:35 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_FD
#define SOCKET_FD

#include "AFileDescriptor.hpp"
#include "Request.hpp"
#include "Server.hpp"

#define BUFFER_SIZE		1024
#define	SUCCESS			0
#define	READ_AGAIN		1
#define	CLIENT_CLOSE	2
#define ERROR			3
#define	BY_CGI			4

class SocketFd : public AFileDescriptor
{
	private:
	
		Server const *	_serverInfo;
		Request			_request;
		bool			_responseReady;

		SocketFd(void);

		bool	searchRequestLine();
		bool	searchHeaders();

	public:
		
		SocketFd(SocketFd const & copy);
		SocketFd & operator=(SocketFd const & rhs);
		virtual ~SocketFd();

		// Constructors
		SocketFd(int fd, Server const & serverInfo);

		// Geters
		Request const & getRequest() const;
		Server const &	getServerInfo() const;

		// Public methods
		void	readRequest(int epoll);
		void	sendResponse(int epoll, std::map<int, AFileDescriptor *> & mapFd);
		void	responseCgi(std::string const & response);
};

#endif
