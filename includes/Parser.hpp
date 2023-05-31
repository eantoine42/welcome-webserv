/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 19:40:20 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/31 22:22:36 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include "Server.hpp"

class Parser
{
    private:

        std::string const _configFile;

        std::string     getStringConf();
        void            parseServers(std::vector<Server> & servers, std::string strConfig);
        void            createServerSockets(std::vector<Server> const & servers, std::map<int, Server> & mapServers);

    public:

    	// Cannonical Form
		Parser();
		Parser(Parser const & copy);
		Parser & operator=(Parser const & rhs);
		~Parser();

        Parser(std::string configFile);

        void    parseConfFile(std::map<int, Server> & mapServers);
};

#endif