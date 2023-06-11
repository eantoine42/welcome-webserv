/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/07 22:48:15 by lfrederi         ###   ########.fr       */
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

class SocketFd : public AFileDescriptor
{
	private:
	
		std::vector<Server> const *	_serverInfo;
		Request			_request;

		SocketFd(void);

		int		searchRequestLine();
		int		searchHeaders();

	public:
		
		SocketFd(SocketFd const & copy);
		SocketFd & operator=(SocketFd const & rhs);
		virtual ~SocketFd();

		// Constructors
		SocketFd(int fd, std::vector<Server> const * serverInfo);


		// Geters
		Request const & getRequest() const;
		Server const &	getServerInfo() const;

		// Public methods
		int		readRequest();
		void	prepareResponse(int ret, int epollFd);
		void	sendResponse();
};

#endif
