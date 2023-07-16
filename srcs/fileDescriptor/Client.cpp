/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eantoine <eantoine@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:02:19 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/16 13:01:34 by eantoine         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Autoindex.hpp"
#include "Debugger.hpp"
#include "StringUtils.hpp"
#include "FileUtils.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "Exception.hpp"
#include "WebServ.hpp"
#include "FileUtils.hpp"
#include "Cgi.hpp"
#include <cstddef>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h> // read
#include <cstdio>	// perror
#include <fstream>
#include <sys/socket.h> // recv
#include <algorithm>	// search

/*****************
 * CANNONICAL FORM
 *****************/

Client::Client(void)
	: AFileDescriptor(), _responseReady(false), _cgi(NULL)
{
}

Client::Client(Client const &copy)
	: AFileDescriptor(copy),
	  _rawData(copy._rawData),
	  _serverInfo(copy._serverInfo),
	  _serverInfoCurr(copy._serverInfoCurr),
	  _request(copy._request),
	  _responseReady(copy._responseReady),
	  _cgi(copy._cgi)
{
}

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_rawData = rhs._rawData;
		_serverInfo = rhs._serverInfo;
		_serverInfoCurr = rhs._serverInfoCurr;
		_request = rhs._request;
		_responseReady = rhs._responseReady;
		_cgi = rhs._cgi;
	}

	return (*this);
}

Client::~Client()
{
}
/******************************************************************************/

/**************
 * CONSTRUCTORS
 ***************/
Client::Client(int fd, std::vector<ServerConf> const &serverInfo)
	: AFileDescriptor(fd), _serverInfo(serverInfo)
{
}
/******************************************************************************/

/***********
 * ACCESSORS
 ************/

Request const &Client::getRequest() const
{
	return (this->_request);
}

ServerConf const &Client::getServerInfo() const
{
	return (this->_serverInfoCurr);
}
/******************************************************************************/

/****************
 * PUBLIC METHODS
 ****************/

/// @brief
/// @return
void Client::doOnRead(WebServ &webServ)
{
	char buffer[BUFFER_SIZE];
	ssize_t n;
	std::vector<unsigned char>::iterator it;

	if ((n = recv(_fd, buffer, BUFFER_SIZE, 0)) > 0)
		_rawData.assign(buffer, buffer + n);

	if (n < 0) // Try to read next time fd is NON_BLOCK and we can't check errno
		return;
	if (n == 0) // Client close connection
	{
		webServ.removeFd(_fd);
		return;
	}
	try 
	{
		if (_request.getHttpMethod().empty())
			searchRequestLine();
		if (_request.getHeaders().empty())
			searchHeaders();
		if (_request.hasMessageBody())
			_request.handleMessageBody(_rawData);
	}
	catch (RequestUncomplete & exception)
	{
		return ;
	}
	catch (std::exception & exception)
	{
		handleException(exception);
		webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
		return ;
	}

	_serverInfoCurr = getCorrectServer(); 
	webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
}

/// @brief
void Client::doOnWrite(WebServ &webServ)
{
	if (_responseReady == true)
	{
		send(_fd, &(_rawData[0]), _rawData.size(), 0);
		webServ.updateEpoll(_fd, EPOLLIN, EPOLL_CTL_MOD);
		_responseReady = false;
		_request = Request();
		return;
	}

	/*
		TODO: Verification de la requete avec le fichier conf:
			- Si pathRequest == "/" -> verifier les indexs
			- Cas du POST/DELETE sans extension php -> verifier si le chemin permet l'uploading
			- Construction du path en fonction du bloc location ...
	*/ 
		

	if (_request.getExtension().compare("php") == 0)
	{
		Cgi cgi = Cgi(*this);
		if (cgi.run() < 0)
		{
			errorResponse(INTERNAL_SERVER_ERROR);
			webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
			return;
		}
		webServ.addFd(new Cgi(cgi));
		webServ.updateEpoll(_fd, 0, EPOLL_CTL_MOD);
		webServ.updateEpoll(cgi.getReadFd(), EPOLLIN, EPOLL_CTL_ADD);
	}
	else
	{
		std::string filename = setPathRequest();
		getResponse(filename);
		webServ.updateEpoll(_fd, EPOLLOUT, EPOLL_CTL_MOD);
	}
}

/// @brief
/// @param mapFd
/// @param event
void Client::doOnError(WebServ &webServ, uint32_t event)
{
	std::cout << "Client on error, event = " << event << std::endl;
	webServ.removeFd(_fd);
}

void Client::responseCgi(std::string const &response)
{
	_responseReady = true;
	_rawData.assign(response.begin(), response.end());
}

/******************************************************************************/

/*****************
 * PRIVATE METHODS
 *****************/

void	Client::searchRequestLine()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 2);
	if (it == _rawData.end())
		throw RequestUncomplete();
	_request.handleRequestLine(std::string(_rawData.begin(), it));
	_rawData.erase(_rawData.begin(), it);
}

void	Client::searchHeaders()
{
	std::vector<unsigned char>::iterator it;
	unsigned char src[] = {'\r', '\n', '\r', '\n'};

	it = std::search(_rawData.begin(), _rawData.end(), src, src + 4);
	if (it == _rawData.end())
		throw RequestUncomplete();
	_request.handleHeaders(std::string(_rawData.begin(), it));
	_rawData.erase(_rawData.begin(), it + 4);
}

void	Client::errorResponse(status_code_t status)
{
	//TODO: Find error file from server
	std::string extension = "html";

	std::string error = "<html>\n<head><title>" + StringUtils::intToString(status) + " " + HttpUtils::RESPONSE_STATUS.at(status) + "</title></head>\n<body>\n<center><h1>" + StringUtils::intToString(status) + HttpUtils::RESPONSE_STATUS.at(status) + "</h1></center>\n<hr><center>webserv (Ubuntu)</center>\n</body>\n</html>\n";
	std::vector<unsigned char> body = std::vector<unsigned char>(error.begin(), error.end());

	resp_t resp = {status, body, extension, _rawData, false};
	Response::createResponse(resp);
	_responseReady = true;
}

void	Client::errorResponse(status_code_t status, std::string errorFile)
{
	//TODO: has error file from location
	(void) errorFile;
	std::string extension = "html";

	std::string error = "<html>\n<head><title>" + StringUtils::intToString(status) + " " + HttpUtils::RESPONSE_STATUS.at(status) + "</title></head>\n<body>\n<center><h1>" + StringUtils::intToString(status) + HttpUtils::RESPONSE_STATUS.at(status) + "</h1></center>\n<hr><center>webserv (Ubuntu)</center>\n</body>\n</html>\n";
	std::vector<unsigned char> body = std::vector<unsigned char>(error.begin(), error.end());

	resp_t resp = {status, body, extension, _rawData, false};
	Response::createResponse(resp);
	_responseReady = true;
}

void	Client::getResponse(std::string filename)
{
	std::cout << _request;
	std::vector<unsigned char> body;
    std::ifstream is (filename.c_str(), std::ifstream::binary);

    if (is.good()) {
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);

        char * buffer = new char [length];
        is.read (buffer,length);
		body = std::vector<unsigned char>(buffer, buffer+length);
        is.close();
		delete [] buffer;

		resp_t resp = {OK, body, _request.getExtension(), _rawData, true};//plante si pas d'extension, ie c'est un dossier a gerer avec autoindex
		Response::createResponse(resp);
		_responseReady = true;
	}
	else
	{
		errorResponse(NOT_FOUND);
	}
}

ServerConf Client::getCorrectServer()
{
	std::vector<ServerConf>::iterator it = _serverInfo.begin();
	for (; it !=_serverInfo.end(); it++)
	{
		if (_request.getHeaders().find("Host")->second == it->getName())
			return (*it);
	}
	it = _serverInfo.begin();
	for (; it !=_serverInfo.end(); it++)
	{
		std::vector<Location>::const_iterator it1 = it->getLocation().begin();
		for (; it1 != it->getLocation().end(); it1++)
			if (_request.getPathRequest() == it1->getlocRoot())
				return (*it);
	}
	return (*(_serverInfo.begin()));

}

void	Client::handleException(std::exception & exception)
{
	DEBUG_COUT(exception.what());
	try
	{ 
		dynamic_cast<RequestError &>(exception);
		errorResponse(BAD_REQUEST);
	} catch (std::exception & exception)
	{
		errorResponse(INTERNAL_SERVER_ERROR);
	}
}

int		Client::pathIsValid(std::string const path)
{
	if (path.empty() || !FileUtils::fileExists(path.c_str()) ||
		!FileUtils::fileRead(path.c_str()))
		return 0;
	else if (FileUtils::isDirectory(path.c_str()))
		return 2;
	else
		return 1;
	
}

std::string 	Client::setPathRequest()
{
	std::string answer;
	std::string rootPathRelative;
	std::vector<Location>::const_iterator locIt = findLongestMatch();
	if (locIt != _serverInfoCurr.getLocation().end())
	{
		if (locIt->getlocRoot().c_str()[0]== '/')
			rootPathRelative = _serverInfoCurr.getRoot();
		else
			rootPathRelative = "";
		if (validFolder(rootPathRelative))
		{
			std::vector<std::string>::const_iterator indexIt = locIt->getIndex().begin();
			//parcours les index du bloc location
			for (; indexIt !=locIt->getIndex().end(); indexIt++)
			{
				if (pathIsValid((rootPathRelative + _request.getPathRequest() +"/"+ *indexIt))){
					if ((*indexIt).rfind(".") != std::string::npos)
						_request.setExtension((*indexIt).substr((*indexIt).rfind(".") + 1));
					//TODO set extension to the value of index extension
					return ((rootPathRelative + _request.getPathRequest() +"/"+ *indexIt));//si index existe renvoie le chemin root included
				}
			}
			//comme le folder est valide, renvoie le chemin avec le folder
			return (rootPathRelative + _request.getPathRequest()+"/");
		}
		//teste le chemin avec fichier voir si le fichier existe
		else if (pathIsValid(_serverInfoCurr.getRoot() + _request.getPathRequest()))
			return (_serverInfoCurr.getRoot() + _request.getPathRequest());
		else {
			try{
			answer +=_serverInfoCurr.getRoot();
			answer += _request.getPathRequest().substr(0, _request.getPathRequest().size() - _request.getFileName().size());
			answer += (locIt->getError()).at(NOT_FOUND);
			}
			catch (FileNotFound & exception)
	{
		errorResponse(NOT_FOUND);
	}
			//leve une exception si pas trouve
			if (pathIsValid(answer)){
				errorResponse(NOT_FOUND, answer);
			}
			else 
				errorResponse(NOT_FOUND);//TODO gestion erreurs a revoir
		}
	}
	else{
		if (validFolder2(_serverInfoCurr.getRoot() + _request.getPathRequest())){
			std::vector<std::string>::const_iterator indexIt = _serverInfoCurr.getIndex().begin();
			//parcours les index du bloc server
			for (; indexIt !=_serverInfoCurr.getIndex().end(); indexIt++)
			{
				if (pathIsValid((_serverInfoCurr.getRoot() + _request.getPathRequest() +"/"+ *indexIt))){
					if ((*indexIt).rfind(".") != std::string::npos)
						_request.setExtension((*indexIt).substr((*indexIt).rfind(".") + 1));
					//TODO set extension to the value of index extension
					return ((_serverInfoCurr.getRoot() + _request.getPathRequest() +"/"+ *indexIt));//si index existe renvoie le chemin root included
				}
			}
			//comme le folder est valide, renvoie le chemin avec le folder
			return (rootPathRelative + _request.getPathRequest()+"/");
		}
		else if (pathIsValid(_serverInfoCurr.getRoot() + _request.getPathRequest()))
			return (_serverInfoCurr.getRoot() + _request.getPathRequest());
		else try{
			answer +=_serverInfoCurr.getRoot();
			answer += _request.getPathRequest().substr(0, _request.getPathRequest().size() - _request.getFileName().size());
			answer += (_serverInfoCurr.getError()).at(NOT_FOUND);}
			catch (FileNotFound & exception)
	{
		errorResponse(NOT_FOUND);
	}//leve une exception si pas trouve
			if (pathIsValid(answer)){
				errorResponse(NOT_FOUND, answer);
			}
			else 
				errorResponse(NOT_FOUND);//TODO gestion erreurs a revoir
		}
	return ("");
}

std::vector<Location>::const_iterator Client::findLongestMatch()
{
	std::vector<Location>::const_iterator it;
	std::string substring =  _request.getPathRequest();
	std::string path = _serverInfoCurr.getRoot() + substring;
	if (validFolder2(path+ "/") && substring.find_last_of('/')!=substring.size()-1)
		substring +="/";
	std::size_t lastIndex = substring.find_last_of('/');
	if (lastIndex != std::string::npos) {
        substring = substring.substr(0, lastIndex + 1 );
    }
	while (lastIndex != std::string::npos)
	{
		it = _serverInfoCurr.getLocation().begin();
		for (;it !=_serverInfoCurr.getLocation().end(); it++){
       		if (!it->getlocRoot().compare(substring) || !(it->getlocRoot()+ "/").compare(substring))
				return(it);
		}
		if (lastIndex == 0)
			break;
		lastIndex = substring.erase(substring.length() - 1, 1).find_last_of('/');
		if (lastIndex != std::string::npos) {
       		substring = substring.substr(0, lastIndex + 1);
    	}

	}
	return (_serverInfoCurr.getLocation().end());
}

bool Client::validFolder(std::string rootPath)
{
	if (FileUtils::isDirectory((rootPath + _request.getPathRequest()).c_str()))
			return (true);
		else
			return (false);
}

bool Client::validFolder2(std::string path)
{
	if (FileUtils::isDirectory(path.c_str()))
			return (true);
		else
			return (false);
}