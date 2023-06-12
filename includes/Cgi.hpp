/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 23:39:09 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/12 22:03:44 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "AFileDescriptor.hpp"
#include "SocketFd.hpp"

class Cgi : public AFileDescriptor
{
    private:

        SocketFd *  _socketInfo;
        int         _fdRead;
        int         _fdWrite;

        char **     mapCgiParams();
        int         initPipe(int toCgi[2], int fromCgi[2]);
        
        Cgi(void);

    public:
        
		Cgi(Cgi const & copy);
		Cgi & operator=(Cgi const & rhs);
		virtual ~Cgi();

        Cgi(SocketFd & SocketFd);

        int     getReadFd() const;

        int     run();
        void    readCgi(int epollFd);


};

#endif