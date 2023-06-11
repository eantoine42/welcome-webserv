
#include "Location.hpp"
#include "Syntax.hpp"
#include "Exception.hpp"
#include "utils.hpp"

#include <cstdlib> // atoi

Location::Location(int port, int loc_index, std::map<std::string, std::string>	cgi, bool autoindex, std::string index, std::string root, int client_body_size)
:	_port(port),
	_loc_index(loc_index),
	_root(root),
	_index(index),
	_cgi(cgi),
	_autoindex(autoindex),
	_client_body_size(client_body_size)
{
		_allow_method.push_back("POST");
		_allow_method.push_back("GET");
		_allow_method.push_back("DELETE");
		_upload_dir = "";
}

Location::Location()
{}

Location::Location(const Location &src)
:	_port(src._port),
	_loc_index(src._loc_index),	
	_root(src._root),
	_allow_method(src._allow_method),
	_index(src._index),
	_cgi(src._cgi),
	_autoindex(src._autoindex),
	_upload_dir(src._upload_dir),
	_return(src._return),
	_client_body_size(src._client_body_size),
	_error_pages(src._error_pages),
	_uri(src._uri) {}

Location &Location::operator=(const Location &src)
{
	Location	tmp(src);
	std::swap(tmp, *this);
	return (*this);
}

Location::~Location(){}

/*
** Location getters
*/
int									const &Location::getLocIndex() const{return (_loc_index);}
int									const &Location::getPort() const{return (_port);}
std::string							const &Location::getUri() const{return _uri;}
bool								const &Location::getAutoindex() const{return _autoindex;}
std::string							const &Location::getIndex() const{return _index;}
std::string							const &Location::getReturn() const{return _return;}
std::vector<std::string>			const &Location::getAllowMethod() const{return _allow_method;}
std::string							const &Location::getRoot() const{return _root;}
std::string							const &Location::getUploadDir() const{return _upload_dir;}
std::map<std::string, std::string>	const &Location::getCgi() const{return _cgi;}
long int							const &Location::getClientBodySize() const{return (_client_body_size);}
std::string							const &Location::getError() const{return (_error_pages);}

void	Location::setLocation(const std::string &str,  int &count)
{
	
	std::string line = Syntax::getLine(str, count);
	std::vector<std::string> token;
	token = Syntax::splitString(line, WHITESPACES);
	setUri(token);
	count ++;
	if(Syntax::getLine(str, count).compare("{"))
		throw(ConfFileParseError("Invalid Location directive : no opening {"));
	count ++;
	int i = 0;
	int ct = 0;
	int closing_line;
	while (ct < count)
	{
		if(str[i] == '\n')
			ct++;
		i++;
	}
	closing_line = Syntax::findClosingBracket(str.substr(i));
	for (i = 0; i < closing_line; i++)
	{
		line = Syntax::getLine(str, count++);
		parseLocation(line);
	}
}
void Location::init_vector_loc_fct(std::vector<loc_func> &funcs)
{
	funcs.push_back(&Location::setRoot);
	funcs.push_back(&Location::setAllowMethod);
	funcs.push_back(&Location::setIndex);
	funcs.push_back(&Location::setCgi);
	funcs.push_back(&Location::setAutoindex);
	funcs.push_back(&Location::setUploadDir);
	funcs.push_back(&Location::setReturn);
	funcs.push_back(&Location::setClientBodySize);
	funcs.push_back(&Location::setErrorPages);
}


void	Location::parseLocation(std::string &line)
{
	std::vector<loc_func> funcs;
	init_vector_loc_fct(funcs);
	std::vector<std::string> token;
	int instruct;
	if ((token = Syntax::splitString(line, WHITESPACES)).empty())
		return;
	else if ((instruct = Syntax::correctLocationInstruction(token)) != -1 && instruct < TOTAL_LOCATION_INSTRUCTIONS)
		(this->*funcs[instruct])(token);
	else if (instruct == TOTAL_LOCATION_INSTRUCTIONS)
		return;
	else
		throw (ConfFileParseError("Wrong input in Location bloc [" + Syntax::intToString(_loc_index)+"] : Directive " + token[0]+" invalid"));
}





/*
** Location setters
*/
void	Location::setUri(std::vector<std::string> token)
{
	if(token.size() < 2 )
		throw(ConfFileParseError("Invalid Location directive : not enough arguments"));
	else if (token.size() == 2 )
		_uri = token[1];
	else if (token.size() == 3)
	{
		if (!token[1].compare("="))
			_uri =token[1] +" " + token[2];
		else throw(ConfFileParseError("Invalid Location directive : invalid Location modifier " + token[1]));
	}
}

void	Location::setAutoindex(std::vector<std::string> token)
{
	if (token.size() != 2 || !(token[1].compare("on") || token[1].compare("off")))
	throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : problem with autoindex argument, on or off only"));
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
void	Location::setIndex(std::vector<std::string> token)
{
	_index = "";
	size_t i = 1;
	if (token.size() < 2)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : problem with number of arguments for index"));
	for (; i < token.size() - 1; i++)
		_index += token[i] + " ";
	_index += token[i].erase(token[i].size() - 1);
}

void	Location::setAllowMethod(std::vector<std::string> token)
{
	_allow_method.clear();
	if (Syntax::correctMethodInstruction(token) == -1)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) + "] : Method not allowed"));
	size_t i = 1;
	for (; i < token.size() - 1; i++)
		_allow_method.push_back(token[i]);
	_allow_method.push_back(token[i].erase(token[i].size() - 1));
}

void	Location::setRoot(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : Only one root allowed"));
	_root = token[1].erase(token[1].size() - 1);
}

void	Location::setUploadDir(std::vector<std::string> token)
{
	_upload_dir = "";
	size_t i = 1;
	if (token.size() > 2)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : problem with number of arguments for upload dir"));
	_upload_dir += token[i].erase(token[i].size() - 1);
}
void	Location::setCgi(std::vector<std::string> token)
{
	if (token.size() != 3)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : cgi argument problem"));
	_cgi.clear();
	_cgi.insert(std::pair<std::string, std::string>(token[1], token[2].erase(token[2].size() - 1)));
}
void	Location::setErrorPages(std::vector<std::string> token)
{
	
	if (token.size() != 3 )
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : problem with number of arguments for error_page"));
	for (size_t i= 0 ; i < token[1].size(); i++)
		if (token[1][i] <48 &&  token[1][i]> 57)
			throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : error_page : fisrt argument must be numeric"));
	if (atoi(token[1].c_str()) < 300 && atoi(token[1].c_str()) > 599)
		throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : error_page : fisrt argument must be between 300 and 599"));
	_error_pages = token[1] + " " + token[2].erase(token[2].size() - 1);
}
void	Location::setClientBodySize(std::vector<std::string> token)
{
	if (token.size() > 2)
		throw(ConfFileParseError("Location bloc [" + syntax::intToString(_loc_index) +"] : Only one client body size max"));
	std::string str = token[1].erase(token[1].size() - 1);
	size_t i=0;
	while (i < str.length() && (std::isspace(str[i]) || std::isdigit(str[i])))
		i++;
	if (str.length()!=i && str[i]!='M' && str[i]!='m' && str[i]!='G' && str[i]!='g' && str[i]!='k' && str[i]!='K')
		throw(ConfFileParseError("error client body size syntax"));
	if (str.length()!=i && str.length() !=i+1)
		throw(ConfFileParseError("error client body size syntax"));
	_client_body_size = atoi(token[1].erase(token[1].size() - 1).c_str());
	if (str[i] && (str[i]=='M' || str[i]=='m'))
		_client_body_size *=1000000;
	else if (str[i] && (str[i]=='G' || str[i]=='g'))
		_client_body_size *=1000000000;
	else if (str[i] && (str[i]=='k' || str[i]=='K'))
		_client_body_size *=1000;
}

void	Location::setReturn(std::vector<std::string> token)
	{
		if (token.size() == 2)
			_return = token[1].erase(token[1].size() - 1);
		if (token.size() != 3 )
			throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : problem with number of arguments for return"));
		for (size_t i= 0 ; i < token[1].size(); i++)
			if (token[1][i] <48 &&  token[1][i]> 57)
				throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : _redirect : fisrt argument must be numeric"));
		if (atoi(token[1].c_str()) < 300 && atoi(token[1].c_str()) > 599)
			throw(ConfFileParseError("Location bloc [" + Syntax::intToString(_loc_index) +"] : _redirect : fisrt argument must be between 300 and 599"));
		_return = token[1] + " " + token[2].erase(token[2].size() - 1);
	}

std::ostream    &operator<<(std::ostream &o, Location  const &i) 
{
	o << "************* Location bloc number ["<<i.getLocIndex()<<"] *************"<< std::endl;
	if (i.getUri().empty() == false)
		o << "    uri				=	[" << i.getUri() << "] :" << std::endl;
	o << "    portNumber			=	[" << i.getPort() << "]" << std::endl;
	o << "    autoindex			=	[" << i.getAutoindex() << "]" << std::endl;
	if (i.getIndex().empty() == false)
		o << "    index			=	[" << i.getIndex() << "]" << std::endl;
	if (i.getRoot().empty() == false)
		o << "    root			=	[" << i.getRoot() << "]" << std::endl;
	if (i.getReturn().empty() == false)
		o << "    redirect			=	[" << i.getReturn() << "]" << std::endl;
	if (i.getAllowMethod().empty() == false)
		o << "    methods			=	[" << i.getAllowMethod() << "]" << std::endl;
	if (i.getUploadDir().empty() == false)
		o << "    uploadDir			=	[" << i.getUploadDir() << "]" << std::endl;
	if (i.getCgi().empty() == false)
	{
		std::map<std::string, std::string>::const_iterator ite; 
		for (ite = i.getCgi().begin();ite != i.getCgi().end(); ite++)
			o << "    cgi				=	[" << ite->first<<" ; " << ite->second<<"]" << std::endl;
	}
	if (i.getClientBodySize() > 0)
		o << "    clientMaxBodySize		=	[" << i.getClientBodySize() << "]" << std::endl;
	if (i.getError().empty() == false)
		o << "    errorPage			=	[" << i.getError() << "]" << std::endl;
	return (o);
};
