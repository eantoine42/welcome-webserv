/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:11 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/21 22:13:59 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

void    Response::badRequest(std::vector<unsigned char> & rawData)
{
    std::string body = "bad request";
    std::string response = commonResponse(body.c_str(), "10");
    std::cout << response;
    rawData.assign(response.begin(), response.end());
}

void    Response::createResponse(std::vector<unsigned char> & rawData, Client const & Client)
{
    std::string filename = Client.getServerInfo().getRoot() + "/" + Client.getRequest().getFileName();
    std::string response;
    std::stringstream ss;
    std::ifstream is (filename.c_str(), std::ifstream::binary);

    if (is.good()) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);

        ss << length;

        char * buffer = new char [length];
        is.read (buffer,length);
        is.close();

        response = commonResponse(buffer, ss.str());

        delete[] buffer;
  }

    rawData.assign(response.begin(), response.end());
}

std::string     Response::commonResponse(const char * body, std::string size)
{
    
    std::string response = std::string("HTTP/1.1 ") + "200 " + "Ok\r\n";
    response += "Server: webserv (Ubuntu)\r\n";
    response += "Date: Sat, 10, Jun 2023 09:15:38 GMT\r\n";
    response += "Content-Type: text/html; charset=utf-8\r\n";
    response += "Content-Length: " + size + "\r\n";
    response += "Connection: keep-alive\r\n\r\n";
    response += body;

    return (response);
}

std::string     Response::cgiSimpleResponse(std::string & body)
{
    std::string response = std::string("HTTP/1.1 ") + "200 " + "Ok\r\n";
    std::stringstream ss;
    ss << body.size();
    

    response += "Server: webserv (Ubuntu)\r\n";
    response += "Date: Sat, 10, Jun 2023 09:15:38 GMT\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "Connection: keep-alive\r\n\r\n";
    response += body;

    return (response);
}
