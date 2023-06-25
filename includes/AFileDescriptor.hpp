/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFileDescriptor.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 14:06:22 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/24 19:59:55 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_FILE_DESCRIPTOR_HPP
#define A_FILE_DESCRIPTOR_HPP

#include <sys/epoll.h>
class WebServ;

class AFileDescriptor
{
	protected:

		int		_fd;

		// Cannonical form
		AFileDescriptor();
		AFileDescriptor(AFileDescriptor const & copy);
		AFileDescriptor & operator=(AFileDescriptor const & rhs);

		// Constructors
		AFileDescriptor(int fd);

	public:
		virtual ~AFileDescriptor();

		// Accessors
		int		getFd() const;
		void	setFd(int fd);

		// Public methods
		virtual void doOnRead(WebServ & webserv) = 0;
		virtual void doOnWrite(WebServ & webServ) = 0;
		virtual void doOnError(WebServ & webServ, uint32_t event) = 0;

};


#endif
