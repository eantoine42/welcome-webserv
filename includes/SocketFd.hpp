/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketFd.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:13 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/18 18:19:55 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_FD
#define SOCKET_FD

#include "AFileDescriptor.hpp"
#include "Request.hpp"

#define BUFFER_SIZE 10

class SocketFd : public AFileDescriptor
{
	private:
	
		Request	_request;

	public:

		SocketFd(void);
		SocketFd(SocketFd const & copy);
		SocketFd & operator=(SocketFd const & rhs);
		virtual ~SocketFd();

		SocketFd(int fd);

		int	doOnRead();
		int	doOnWrite();

};

#endif
