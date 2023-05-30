/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 18:24:01 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/23 19:21:50 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"
#include "Syntax.hpp"
#include "Exception.hpp"
#include "Debugger.hpp"

#include <cstdlib> // atoi
#include <algorithm>
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_addr
#include <netdb.h> // getprotobyname

ServerConf::ServerConf(void)
	:	_root("html"),
		_port(8080),
		_ServerConf_name(""),
		_IP("0.0.0.0"),
		_index("index index.html"),
		_autoindex(false),
		_client_body_size(1)
{}

ServerConf::ServerConf(ServerConf const &src)
	:	_root(src._root),
		_port(src._port),
		_ServerConf_name(src._ServerConf_name),
		_IP(src._IP),
		_error_pages(src._error_pages),
		_index(src._index),
		_client_body_size(src._client_body_size),
		_cgi(src._cgi),
		_location(src._location)
{}

ServerConf	&ServerConf::operator=(ServerConf const &src)
{
		ServerConf tmp(src);
		std::swap(tmp, *this);
		return (*this);
}

ServerConf::~ServerConf()
{}

/*
 ** Server getters
 */
std::string							const &ServerConf::getRoot() const{return (_root);}
std::map<std::string, std::string>	const &ServerConf::getCgi() const{return (_cgi);}
int									const &ServerConf::getPort() const{return (_port);}
std::string							const &ServerConf::getName() const{return (_ServerConf_name);}
std::string							const &ServerConf::getIndex() const{return (_index);	}
bool								const &ServerConf::getAutoindex() const{return (_autoindex);}
std::string							const &ServerConf::getIp() const{return (_IP);}
std::string							const &ServerConf::getError() const{return (_error_pages);}
int									const &ServerConf::getClientBodySize() const{return (_client_body_size);}
std::vector<Location>				const &ServerConf::getLocation() const{return (_location);}


/**
 * @brief returns the number of line of location bloc
 * 
 * @param str 
 * @param count 
 * @return int 
 */
int	ServerConf::skipLocationBlock(std::string str, int count)
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

/**
 * @brief from the config string
 * parse the ServerConf and locations blocs
 * 
 * @param str 
 */
void	ServerConf::setServer(const std::string &str)
{
	int count = 0;
	int pos_end = Syntax::findClosingBracket(str);
	int location_ct = 0;
	//DEBUG_COUT("\n****Server to parse without line ServerConf {  \n\n" + str);
	while (count < pos_end)
	{
		ServerConf::parseServer(str, count);
		count++;
	}
	count = 0;
	while (count < pos_end)
	{
		if (ServerConf::getLocationBloc(str,count) == -1)
			count++;
		else ServerConf::addLocation(str, count, ++location_ct);
	}
	//std::cout<<*this<<std::endl;
}

int	ServerConf::getLocationBloc(std::string str, int &count)
{
	std::vector<std::string> token;
	std::string line = Syntax::getLine(str, count);
	if ((token = Syntax::splitString(line, WHITESPACES)).empty())
		return -1;
	else if (Syntax::correctServerInstruction(token) != LOCATION_INSTRUCTION)
		return -1;
	else return 1;
}

void ServerConf::init_vector_ServerConf_fct(std::vector<ServerConf_func> &funcs)
{
	funcs.push_back(&ServerConf::setRoot);
	funcs.push_back(&ServerConf::setPort);
	funcs.push_back(&ServerConf::setName);
	funcs.push_back(&ServerConf::setError);
	funcs.push_back(&ServerConf::setIndex);
	funcs.push_back(&ServerConf::setAutoindex);
	funcs.push_back(&ServerConf::setClientBodySize);
	funcs.push_back(&ServerConf::setCgi);
}


/**
 * @brief fills all the ServerConf data
 * 
 * @param str 
 */
void	ServerConf::parseServer(std::string str, int &count)
{
	std::vector<ServerConf_func>	funcs;
	init_vector_ServerConf_fct(funcs);

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
		throw (ConfFileParseError("Wrong input in ServerConf : Directive " + token[0]+" invalid"));
}

void	ServerConf::addLocation(std::string str, int &count, int &ServerConf_ct)
{
	Location loc(getPort(),ServerConf_ct, getCgi(), getAutoindex(), getIndex(), getRoot(), getClientBodySize());
	loc.setLocation(str, count);
	this->_location.push_back(loc);
}


void	ServerConf::setRoot(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Only one root allowed"));
	_root = token[1].erase(token[1].size() - 1);
}

void	ServerConf::setCgi(std::vector<std::string> token)
{
	if (token.size() != 3)
		throw(ConfFileParseError("cgi argument problem"));
	_cgi.insert(std::pair<std::string, std::string>(token[1], token[2].erase(token[2].size() - 1)));
}

void	ServerConf::setPort(std::vector<std::string> token)
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

void	ServerConf::setIp(std::vector<std::string> token)
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

void	ServerConf::setName(std::vector<std::string> token)
{
	size_t i = 0;
	_ServerConf_name = "";
	if (token.size() <2)
		throw(ConfFileParseError("Parameter problem with ServerConf name"));
	token.erase(token.begin());
	for (std::vector<std::string>::iterator it = token.begin(); it != token.end(); ++it) {
		std::string& str = *it;
		str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
	}
	std::sort(token.begin(), token.end());
	token.erase(std::unique(token.begin(), token.end()), token.end());//enleve les noms en doublons dans chaque serveur
	for (; i < token.size(); i++)
	{
		_ServerConf_name += token[i];
		if (i != token.size() - 1)
			_ServerConf_name += " ";
	}
}

void	ServerConf::setError(std::vector<std::string> token)
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

void	ServerConf::setIndex(std::vector<std::string> token)
{
	_index = "";
	size_t i = 1;
	if (token.size() < 2)
		throw(ConfFileParseError("problem with number of arguments for index"));
	for (; i < token.size() - 1; i++)
		_index += token[i] + " ";
	_index += token[i].erase(token[i].size() - 1);
}

void	ServerConf::setAutoindex(std::vector<std::string> token)
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

void	ServerConf::setClientBodySize(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Only one client body size max"));
	_client_body_size = atoi(token[1].erase(token[1].size() - 1).c_str());
}


void ServerConf::cleanDupServer(std::vector<ServerConf> ServerConf_info)
{
	if (ServerConf_info.size() == 0)
		return ;
	std::vector<ServerConf>::iterator it = ServerConf_info.begin();
	std::vector<std::string>::iterator it2;
	std::string list_names;
	std::string res;
	std::vector<std::string> vect_names1;
	std::vector<std::string> vect_names2 = Syntax::splitString(this->_ServerConf_name);

	for(; it != ServerConf_info.end(); it++)
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
				std::cerr<<"Duplicate ServerConf name : ["<<*it2<<"] only the first instance will be kept, the other will be ignored\n";	

			std::vector<std::string> vect_names3;
			std::set_difference(vect_names2.begin(), vect_names2.end(),
					vect_names1.begin(), vect_names1.end(), std::inserter(vect_names3, vect_names3.begin()));
			for (std::vector<std::string>::iterator it = vect_names3.begin(); it != vect_names3.end(); it++){
				res += *it;
				if (it != vect_names3.end() - 1)
					res+=" ";
			}
			this->_ServerConf_name = res;
		}

	}

}

std::ostream    &operator<<(std::ostream &o, ServerConf const &i)
{

	o << "************* Server bloc [" << i.getName() << "] *************"<< std::endl;
	if (i.getIp().empty() == false)
		o << "    listen			=	[" << i.getIp() << "]" << std::endl;
	if (i.getPort() >0 )
		o << "    portNumber			=	[" << i.getPort() << "]" << std::endl;
	if (i.getName().empty() == false)
		o << "    ServerConfName			=	[" << i.getName() << "]" << std::endl;
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

std::ostream    &operator<<(std::ostream &o, std::vector<ServerConf>  const &srv)
{
	for (size_t i = 0; i< srv.size(); i++)
		o<<srv[i]<< std::endl;
	return (o);
}

//TODO ignore a ServerConf if same ip:port or name



/**
 * @brief 
 * Parse server info in vector<ServerConf>
 * server_info is updated with correct info
 * @param std::vector<server> server_info 
 * @param std::string str_config 
 */
static void parseServers(std::vector<ServerConf> &server_info, std::string str_config){
	size_t i = 0;
	if (DEBUG_STATUS)
	{
		//DEBUG_COUT("conf file nb lines :" + Syntax::intToString(Syntax::nb_lines(str_config)));
		//DEBUG_COUT("\n****Server to parse without line server {****  \n\n" + str_config);
	}
	while (i < Syntax::nbLines(str_config))
	{
		if (!Syntax::isNothing(str_config, i))
		{
			std::string	line = Syntax::getLine(str_config, i);
			if (!line.compare(0, 6, "server") && line.size() == 6)
			{
				i++;
				std::string	line = Syntax::getLine(str_config, i);
				if (line.compare("{"))
					throw(ConfFileParseError("Invalid Server Header"));
				ServerConf temp_server;
				temp_server.setServer( Syntax::trimLineToI(str_config, i + 1));
				temp_server.cleanDupServer(server_info);
				server_info.push_back(temp_server);
			}
		}
		i++;
	}

	if (DEBUG_STATUS)
		std::cout<<server_info<<std::endl;
}

/**
 * @brief 
 *	gets the file .conf transformed in a single string 
 *	with #comments removed, whitespaces begin and end removed
 *	and many whitespaces reduced to a space
 *	return : std::string
 * @param std::string const &path 
 * @return std::string 
 */
static std::string getStringConf(std::string const &path)
{
	std::string content;
	std::string line;
	std::vector<std::string> tokens;
	std::ifstream file;
    file.open(path.c_str(), std::ifstream::in);
	if (!file)
    	throw (CantOpenConfFile());
	while (std::getline(file, line))
	{
		std::string temp;

		temp = Syntax::trimComments(line);
		temp = Syntax::trimWhitespaces(temp);
		content += temp;
		content +="\n";
	}
	if (!content.empty())
		content.erase(content.end() - 1);
//	if (DEBUG_STATUS)
  //  	std::cout << content << std::endl;
	file.close();
	if (!Syntax::checkBrackets(content))
		throw ConfFileParseError("Error in the backets");
	return content;
}

/**
 * @brief gets infor from conffile
 *	parse to a vector of server
 * 
 * @param std::string path 
 * @return std::vector<server> 
 */
std::vector<ServerConf> parseConfFile(std::string const &path)
{
	std::vector<ServerConf> server_list;
	std::string conf_string;
	std::string conf_string_formated = "";
	std::string temp;

	Syntax::testPath(path);
	conf_string = getStringConf(path);
	Syntax::formatConfFile(conf_string);
	int j = Syntax::nbLines(conf_string);
	for (int i = 0; i < j; i++){
		temp = Syntax::trimWhitespaces(Syntax::getLine(conf_string, i));
		conf_string_formated += temp + Syntax::checkChar(temp);
	}
	Syntax::formatConfFile(conf_string_formated);
	conf_string_formated.erase(conf_string_formated.size() - 1);
	parseServers(server_list, conf_string_formated);
	return (server_list);
}

static int	createSocket(ServerConf &srv)
{
	int	socket_fd, enabled = 1;
	struct sockaddr_in sockaddr;
	struct protoent *proto;
	//The getprotobyname function is a part of the C library functions which is used to map a protocol name such as 
	// "tcp" to the corresponding protocol number defined in the netinet/in.h header file.
	if (!(proto = getprotobyname("tcp")))
		throw(SetServerException("Problem using protobyname, protocol not found"));
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto)) == -1)
		throw(SetServerException("Problem creating Socket"));
	DEBUG_COUT("Server " + srv.getName() + " created");
	// allows a socket to be bound to an address that is already in use, provided that the original socket using the address is no longer active. 
	//This behavior is useful in cases where the server needs to restart after a crash or when multiple instances of the server need to run on the same machine.
	//This option allows the socket to be bound to a previously used address and port, which is useful in cases where the socket is closed and then immediately reopened,
	// without waiting for the operating system to release the socket resources. Without this option,
	//the socket may fail to bind to the address and port, resulting in a "Address already in use" error.
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1)
		throw(SetServerException("Problem setting Socket options"));//
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(srv.getPort());
	sockaddr.sin_addr.s_addr = inet_addr(srv.getIp().c_str());
	if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&sockaddr), sizeof(sockaddr)) == -1)
		throw(SetServerException("Problem binding socket"));
	DEBUG_COUT("Server with file descriptor " <<  socket_fd << " has been successfully bind on port: " << srv.getPort());

	//if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) //makes the socket nonblock
    //    throw(SetServerException("Problem setting the socket"));
	if (listen(socket_fd, MAX_CLIENT))
		throw(SetServerException("Problem with listen"));
	
	return socket_fd;
}

std::map<int, ServerConf *> createServerSockets(std::vector<ServerConf>& serverlist)
{
	std::map<int, ServerConf *> result; 
	std::vector<ServerConf>::iterator it;
	for (it = serverlist.begin();it != serverlist.end(); it++)
	{
		result[createSocket(*it)] = &(*it);
	}
	return (result);
}


