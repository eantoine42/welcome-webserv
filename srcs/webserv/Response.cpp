/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:11 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/10 14:59:33 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>

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
    response += "Content-Length: 159\r\n";
    response += "Connection: keep-alive\r\n\r\n";
    response += "<html>\n<head><title>400 Bad Request</title></head>\n<body>\n<center><h1>400 Bad Request</h1></center>\n<hr><center>nginx/1.18.0 (Ubuntu)</center>\n</body>\n</html>\n";

    return (response);
}
