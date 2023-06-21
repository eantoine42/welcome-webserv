/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:08 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/21 22:08:45 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Client.hpp"

class Response
{
    private:

        static std::string commonResponse(const char * body, std::string size);

    public:

        static void badRequest(std::vector<unsigned char> & rawData);
        static void createResponse(std::vector<unsigned char> & rawData, Client const & Client);
        static std::string cgiSimpleResponse(std::string & body);

};

#endif