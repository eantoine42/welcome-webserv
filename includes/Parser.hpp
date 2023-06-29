/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 19:40:20 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 21:12:30 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "ServerConf.hpp"
#include "WebServ.hpp"

typedef std::pair<std::string, int> pairHostPort;

class Parser
{
    private:

        std::string const                                 _configFile;
        std::map< pairHostPort, std::vector<ServerConf> > _map;

        // Methods
        std::string     getStringConf() const;
        void            parseServers(std::vector<ServerConf> & servers, std::string strConfig);
        void            createServerSockets(WebServ & webServ) const;
		void            fillServersMap(std::vector<ServerConf> & serverConfs);
        char            checkChar(std::string str);
        void            formatConfFile(std::string &conf);
        void            testPath(const std::string &path);

    public:

    	// Cannonical Form
		Parser();
		Parser(Parser const & copy);
		Parser & operator=(Parser const & rhs);
		~Parser();

        // Constructors
        Parser(std::string configFile);

        // Methods
        void    parseConfFile(WebServ & webServ);
};

#endif