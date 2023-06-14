/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 18:24:01 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/31 10:19:25 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Syntax.hpp"
#include "SocketFd.hpp"
#include "Exception.hpp"
#include "Debugger.hpp"
#include "WebServ.hpp"

#include <cstdlib> // atoi
#include <unistd.h> // close
#include <sys/socket.h> // accept
#include <fcntl.h> // fcntl
#include <cstring> // bzero
#include <sys/epoll.h> // epoll_ctl
#include <algorithm>

/*****************
* CANNONICAL FORM
*****************/

Server::Server(void)
	:	AFileDescriptor(),
		_root("html"),
		_port(8080),
		_server_name(""),
		_IP("0.0.0.0"),
		_index("index index.html"),
		_autoindex(false),
		_client_body_size(1)
{}

Server::Server(Server const &src)
	:	AFileDescriptor(src),
		_root(src._root),
		_port(src._port),
		_server_name(src._server_name),
		_IP(src._IP),
		_error_pages(src._error_pages),
		_index(src._index),
		_autoindex(src._autoindex),
		_client_body_size(src._client_body_size),
		_cgi(src._cgi),
		_location(src._location)
{}

Server	&Server::operator=(Server const &src)
{
		Server tmp(src);
		std::swap(tmp, *this);
		_fd = src._fd;
		_epollFd = src._epollFd;
		return (*this);
}

Server::~Server()
{}

/******************************************************************************/

/***********
* ACCESSORS
************/

std::string							const &Server::getRoot() const{return (_root);}
std::map<std::string, std::string>	const &Server::getCgi() const{return (_cgi);}
int									const &Server::getPort() const{return (_port);}
std::string							const &Server::getName() const{return (_server_name);}
std::string							const &Server::getIndex() const{return (_index);	}
bool								const &Server::getAutoindex() const{return (_autoindex);}
std::string							const &Server::getIp() const{return (_IP);}
std::string							const &Server::getError() const{return (_error_pages);}
int									const &Server::getClientBodySize() const{return (_client_body_size);}
std::vector<Location>				const &Server::getLocation() const{return (_location);}

void	Server::setRoot(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Only one root allowed"));
	_root = token[1].erase(token[1].size() - 1);
}

void	Server::setCgi(std::vector<std::string> token)
{
	if (token.size() != 3)
		throw(ConfFileParseError("cgi argument problem"));
	_cgi.insert(std::pair<std::string, std::string>(token[1], token[2].erase(token[2].size() - 1)));
}

void	Server::setPort(std::vector<std::string> token)
{
	setIp(token);
	std::string str;
	if (token.size() > 2)
		throw(ConfFileParseError("Only one port allowed"));
	std::vector<std::string> tmp = Syntax::splitString(token[1],":");
	if (tmp.size() == 1)
		str = tmp[0].substr(0, tmp[0].size() - 1);
	else 
		str = tmp[1].substr(0, tmp[1].size() - 1);
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] < 48 || str[i] > 57)
			throw(ConfFileParseError("put only digits for port"));
	_port = atoi(str.c_str());
}

void	Server::setIp(std::vector<std::string> token)
{
	std::string str;
	if (token.size() > 2)
		throw(ConfFileParseError("Only one IP:port allowed"));
	std::vector<std::string> tmp = Syntax::splitString(token[1],":");
	if ((!tmp[0].compare("*") && tmp[0].size() == 1))
	{
		_IP = tmp[0];
		return ;
	}
	if ((!tmp[0].compare("localhost") && tmp[0].size() == 9))
	{
		_IP = "127.0.0.1";
		return ;
	}
	std::vector<std::string> tmp1 = Syntax::splitString(tmp[0],".");
	if (tmp1.size() != 4)
		throw(ConfFileParseError("IP formating problem"));
	for (int j = 0; j< 4; j++)
	{
		if(atoi(tmp1[j].c_str()) < 0 || atoi(tmp1[j].c_str()) > 255)
			throw(ConfFileParseError("IP formating problem"));
	}
	_IP = tmp[0];
}

void	Server::setName(std::vector<std::string> token)
{
	size_t i = 0;
	_server_name = "";
	if (token.size() <2)
		throw(ConfFileParseError("Parameter problem with Server name"));
	token.erase(token.begin());
	for (std::vector<std::string>::iterator it = token.begin(); it != token.end(); ++it) {
		std::string& str = *it;
		str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
	}
	std::sort(token.begin(), token.end());
	token.erase(std::unique(token.begin(), token.end()), token.end());//enleve les noms en doublons dans chaque serveur
	for (; i < token.size(); i++)
	{
		_server_name += token[i];
		if (i != token.size() - 1)
			_server_name += " ";
	}
}

void	Server::setError(std::vector<std::string> token)
{
	if (token.size() != 3 )
		throw(ConfFileParseError("problem with number of arguments for error_page"));
	for (size_t i= 0 ; i < token[1].size(); i++)
		if (token[1][i] <48 &&  token[1][i]> 57)
			throw(ConfFileParseError("error_page : fisrt argument must be numeric"));
	if (atoi(token[1].c_str()) < 300 && atoi(token[1].c_str()) > 599)
		throw(ConfFileParseError("error_page : fisrt argument must be between 300 and 599"));
	_error_pages = token[1] + " " + token[2].erase(token[2].size() - 1);
}

void	Server::setIndex(std::vector<std::string> token)
{
	_index = "";
	size_t i = 1;
	if (token.size() < 2)
		throw(ConfFileParseError("problem with number of arguments for index"));
	for (; i < token.size() - 1; i++)
		_index += token[i] + " ";
	_index += token[i].erase(token[i].size() - 1);
}

void	Server::setAutoindex(std::vector<std::string> token)
{
	if (token.size() != 2 || !(token[1].compare("on") || token[1].compare("off")))
		throw(ConfFileParseError("problem with autoindex argument"));
	switch (token[1].erase(token[1].size() - 1).compare("on"))
	{
		case 0:
			_autoindex = true;
			break;

		default:
			_autoindex = false;
			break;
	}
}

void	Server::setClientBodySize(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Only one client body size max"));
	_client_body_size = atoi(token[1].erase(token[1].size() - 1).c_str());
}

/******************************************************************************/

/****************
* PUBLIC METHODS
****************/

/// @brief 
/// @param Server_info 
void Server::cleanDupServer(std::vector<Server> Server_info)
{
	if (Server_info.size() == 0)
		return ;
	std::vector<Server>::iterator it = Server_info.begin();
	std::vector<std::string>::iterator it2;
	std::string list_names;
	std::string res;
	std::vector<std::string> vect_names1;
	std::vector<std::string> vect_names2 = Syntax::splitString(this->_server_name);

	for(; it != Server_info.end(); it++)
	{
		if (!(this->_IP.compare(it->_IP)) && (this->_port ==(it->_port)))
		{		
			list_names = (*it).getName();
			vect_names1 = Syntax::splitString(list_names);
			std::vector<std::string> vect_names4(vect_names1.size() + vect_names2.size());
			it2 = std::set_intersection(vect_names1.begin(), vect_names1.end(), vect_names2.begin(), vect_names2.end(), vect_names4.begin());
			vect_names4.resize(it2 - vect_names4.begin());
			it2 = vect_names4.begin();
			for(;it2 !=vect_names4.end();it2++)
				std::cerr<<"Duplicate Server name : ["<<*it2<<"] only the first instance will be kept, the other will be ignored\n";	

			std::vector<std::string> vect_names3;
			std::set_difference(vect_names2.begin(), vect_names2.end(),
					vect_names1.begin(), vect_names1.end(), std::inserter(vect_names3, vect_names3.begin()));
			for (std::vector<std::string>::iterator it = vect_names3.begin(); it != vect_names3.end(); it++){
				res += *it;
				if (it != vect_names3.end() - 1)
					res+=" ";
			}
			this->_server_name = res;
		}

	}

}

/**
 * @brief from the config string
 * parse the Server and locations blocs
 * 
 * @param str 
 */
void	Server::setServer(const std::string &str)
{
	int count = 0;
	int pos_end = Syntax::findClosingBracket(str);
	int location_ct = 0;
	//DEBUG_COUT("\n****Server to parse without line Server {  \n\n" + str);
	while (count < pos_end)
	{
		Server::parseServer(str, count);
		count++;
	}
	count = 0;
	while (count < pos_end)
	{
		if (Server::getLocationBloc(str,count) == -1)
			count++;
		else Server::addLocation(str, count, ++location_ct);
	}
	//std::cout<<*this<<std::endl;
}

void	Server::doOnRead(std::map<int, AFileDescriptor *> & mapFd)
{
    int		cs;

	if ((cs = accept(_fd, NULL, NULL)) < 0)
	{
		std::cerr << "Accept error" << std::endl;
		return ;
	}

	if (fcntl(cs, F_SETFL, O_NONBLOCK) < 0)
		throw FileDescriptorError(strerror(errno));

	try
	{
		WebServ::updateEpoll(_epollFd, cs, EPOLLIN, EPOLL_CTL_ADD);
	} 
	catch (EpollInitError & e)
	{
		close(cs);
		throw e;
	}

	mapFd[cs] = new SocketFd(_epollFd, cs, *this);
}

void	Server::doOnWrite(std::map<int, AFileDescriptor *> & mapFd)
{
	(void) mapFd;
}

void	Server::doOnError(std::map<int, AFileDescriptor *> & mapFd, uint32_t event)
{
	(void) mapFd;
	std::cout << "SocketFd on error, event = " << event << std::endl;
}
/******************************************************************************/

/*****************
* PRIVATE METHODS
*****************/

/**
 * @brief returns the number of line of location bloc
 * 
 * @param str 
 * @param count 
 * @return int 
 */
int	Server::skipLocationBlock(std::string str, int count)
{
	int i = 0;
	int in = 0;
	int ct = 0;
	while (str[i] && ct < count)
	{
		if (str[i] == '\n')
			ct++;
		i++;
	}
	i +=8;
	while (str[i] && str[i] != '\n' && in == 0)
	{
		if (str[i] == '{')
			in = 1;
		i++;
	}
	ct += Syntax::findClosingBracket(str.substr(i + 2));
	return (ct -1 - count);
}

/// @brief 
/// @param str 
/// @param count 
/// @return 
int	Server::getLocationBloc(std::string str, int &count)
{
	std::vector<std::string> token;
	std::string line = Syntax::getLine(str, count);
	if ((token = Syntax::splitString(line, WHITESPACES)).empty())
		return -1;
	else if (Syntax::correctServerInstruction(token) != LOCATION_INSTRUCTION)
		return -1;
	else return 1;
}

/// @brief 
/// @param funcs 
void Server::init_vector_Server_fct(std::vector<Server_func> &funcs)
{
	funcs.push_back(&Server::setRoot);
	funcs.push_back(&Server::setPort);
	funcs.push_back(&Server::setName);
	funcs.push_back(&Server::setError);
	funcs.push_back(&Server::setIndex);
	funcs.push_back(&Server::setAutoindex);
	funcs.push_back(&Server::setClientBodySize);
	funcs.push_back(&Server::setCgi);
}

/**
 * @brief fills all the Server data
 * 
 * @param str 
 */
void	Server::parseServer(std::string str, int &count)
{
	std::vector<Server_func>	funcs;
	init_vector_Server_fct(funcs);

	std::vector<std::string> token;
	int instruct;
	std::string line = Syntax::getLine(str, count);
	if ((token = Syntax::splitString(line, WHITESPACES)).empty())
		return ;
	else if ((instruct = Syntax::correctServerInstruction(token)) != -1)
	{
		if (instruct == LOCATION_INSTRUCTION)
			count += skipLocationBlock(str, count);
		else
			if (instruct < TOTAL_SERVER_INSTRUCTIONS && instruct != LOCATION_INSTRUCTION)
				(this->*funcs[instruct])(token);		
	}
	else
		throw (ConfFileParseError("Wrong input in Server : Directive " + token[0]+" invalid"));
}

/// @brief 
/// @param str 
/// @param count 
/// @param Server_ct 
void	Server::addLocation(std::string str, int &count, int &Server_ct)
{
	Location loc(getPort(),Server_ct, getCgi(), getAutoindex(), getIndex(), getRoot(), getClientBodySize());
	loc.setLocation(str, count);
	this->_location.push_back(loc);
}
/******************************************************************************/

/***********
* FUNCTIONS
************/

std::ostream    &operator<<(std::ostream &o, Server const &i)
{

	o << "************* Server bloc [" << i.getName() << "] *************"<< std::endl;
	if (i.getIp().empty() == false)
		o << "    listen			=	[" << i.getIp() << "]" << std::endl;
	if (i.getPort() >0 )
		o << "    portNumber			=	[" << i.getPort() << "]" << std::endl;
	if (i.getName().empty() == false)
		o << "    ServerName			=	[" << i.getName() << "]" << std::endl;
	if (i.getRoot().empty() == false)
		o << "    root			=	[" << i.getRoot() << "]" << std::endl;
	if (i.getIndex().empty() == false)
		o << "    indexPage			=	[" << i.getIndex() << "]" << std::endl;
	o << "    autoindex			=	[" << i.getAutoindex() << "]" << std::endl;
	if (i.getError().empty() == false)
		o << "    errorPage			=	[" << i.getError() << "]" << std::endl;
	if (i.getCgi().empty() == false)
	{
		std::map<std::string, std::string>::const_iterator ite; 
		for (ite = i.getCgi().begin();ite != i.getCgi().end(); ite++)
			o << "    cgi 			=	[" << ite->first<<" ; " << ite->second<<"]" << std::endl;
	}
	if (i.getClientBodySize() > 0)
		o << "    clientMaxBodySize		=	[" << i.getClientBodySize() << "]" << std::endl;
	for (size_t j = 0; j< i.getLocation().size(); j++)
		o<<i.getLocation()[j]<< std::endl;
	return (o);
};

std::ostream    &operator<<(std::ostream &o, std::vector<Server>  const &srv)
{
	for (size_t i = 0; i< srv.size(); i++)
		o<<srv[i]<< std::endl;
	return (o);
}

//TODO ignore a Server if same ip:port or name
/******************************************************************************/