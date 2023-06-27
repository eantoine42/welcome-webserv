/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:11 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/27 09:36:20 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

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

std::string     Response::commonResponse(status_code_t status)
{
    std::string common = std::string("HTTP/1.1 ");
    common += Syntax::intToString(status) + " " + Syntax::responseStatus.at(status) + "\r\n";
    common += "Server: webserv (Ubuntu)\r\n";
    common += "Date: " + Syntax::getFormattedDate(time(NULL)) + "\r\n";

    return (common);
}

std::string     Response::bodyHeaders(std::vector<unsigned char> body, std::string extension)
{
    std::string headers = "";
    if (body.empty())
        return headers;
    headers += "Content-Type: " + Syntax::mimeTypes.at(extension) + "\r\n";
    headers += "Content-Length: " + Syntax::intToString(body.size()) + "\r\n";
    return headers;
}

void    Response::createResponse(resp_t resp)
{
    std::string response = commonResponse(resp.status);
    response += bodyHeaders(resp.body, resp.extension);
    
    if (resp.keepAlive)
        response += "Connection: keep-alive\r\n";
    else
        response += "Connection: close\r\n";

    response += "\r\n";
    if (resp.body.empty())
        resp.rawData.assign(response.begin(), response.end());
    else
    {
        resp.rawData.assign(response.begin(), response.end());
        resp.rawData.insert(resp.rawData.end(), resp.body.begin(), resp.body.end());
    }
}
