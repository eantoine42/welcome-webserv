/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 23:39:09 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/19 09:26:29 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "AFileDescriptor.hpp"
#include <vector>

class Client;

class Cgi : public AFileDescriptor
{
    private:

        std::vector<unsigned char>	_rawData;
        Client *                    _clientInfo;
        int                         _fdRead;
        int                         _fdWrite;
        int                         _pidChild;

        char **     mapCgiParams();
        int         initChildProcess(int toCgi[2], int fromCgi[2]);
        void        runChildProcess(int pipeToCgi[2], int pipeFromCgi[2]);        

    public:
        
        Cgi(void);
		Cgi(Cgi const & copy);
		Cgi & operator=(Cgi const & rhs);
		virtual ~Cgi();

        Cgi(Client & client);

        int     getReadFd() const;
        int     getWriteFd() const;
        int     getPidChild() const;

       	virtual void doOnRead(WebServ & webServ);
		virtual void doOnWrite(WebServ & webServ);
		virtual void doOnError(WebServ & webServ, uint32_t event);

        int     run();


};

#endif