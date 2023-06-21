/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 23:39:09 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/21 22:09:03 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "AFileDescriptor.hpp"
#include "Client.hpp"

class Cgi : public AFileDescriptor
{
    private:

        std::vector<unsigned char>	_rawData;
        Client *  _socketInfo;
        int         _fdRead;
        int         _fdWrite;

        char **     mapCgiParams();
        int         initPipe(int toCgi[2], int fromCgi[2]);
        
        Cgi(void);

    public:
        
		Cgi(Cgi const & copy);
		Cgi & operator=(Cgi const & rhs);
		virtual ~Cgi();

        Cgi(Client & Client);

        int     getReadFd() const;

       	virtual void doOnRead(std::map<int, AFileDescriptor *> & mapFd);
		virtual void doOnWrite(std::map<int, AFileDescriptor *> & mapFd);
		virtual void doOnError(std::map<int, AFileDescriptor *> & mapFd, uint32_t event);

        int     run();


};

#endif