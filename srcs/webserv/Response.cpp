/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:11 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/13 20:25:23 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>
#include <sstream>

void    Response::badRequest(std::vector<unsigned char> & rawData)
{
    std::string response = commonResponse();
    std::cout << response;
    rawData.assign(response.begin(), response.end());
}

void    Response::createResponse(std::vector<unsigned char> & rawData)
{
    std::string response = commonResponse();
    std::cout << response;
    rawData.assign(response.begin(), response.end());
}

std::string     Response::commonResponse()
{
    
    std::string response = std::string("HTTP/1.1 ") + "400 " + "Bad Request\r\n";
    response += "Server: webserv (Ubuntu)\r\n";
    response += "Date: Sat, 10, Jun 2023 09:15:38 GMT\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: 950\r\n";
    response += "Connection: keep-alive\r\n\r\n";
    response += "";

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
