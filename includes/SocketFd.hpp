/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/19 17:42:07 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_FD
#define SOCKET_FD

#include "AFileDescriptor.hpp"
#include "Request.hpp"
#include "Server.hpp"

#define BUFFER_SIZE		1024

class SocketFd : public AFileDescriptor
{
	private:
		std::vector<unsigned char>	_rawData;
		std::vector<Server> const *	_serverInfo;
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
		SocketFd(int fd, std::vector<Server> const * serverInfo);



		// Geters
		Request const & getRequest() const;
		Server const &	getServerInfo() const;

		// Public methods
		virtual void doOnRead(std::map<int, AFileDescriptor *> & mapFd);
		virtual void doOnWrite(std::map<int, AFileDescriptor *> & mapFd);
		virtual void doOnError(std::map<int, AFileDescriptor *> & mapFd, uint32_t event);

		void	responseCgi(std::string const & response);
};

#endif
