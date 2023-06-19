/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 19:40:20 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/02 09:10:59 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "Server.hpp"
#include "WebServ.hpp"

class Parser
{
    private:

        std::string const _configFile;

        std::string     getStringConf() const;
        void            parseServers(std::vector<Server> & servers, std::string strConfig) const;
        void            createServerSockets(std::vector<Server> const & servers, WebServ & webServ) const;
		int				avoid_double_socket(std::vector<Server> const & servers);
    public:

    	// Cannonical Form
		Parser();
		Parser(Parser const & copy);
		Parser & operator=(Parser const & rhs);
		~Parser();

        Parser(std::string configFile);

        void    parseConfFile(WebServ & webServ) const;
};

#endif