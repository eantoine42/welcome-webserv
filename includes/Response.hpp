/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eantoine <eantoine@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:08 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/31 22:19:51 by eantoine         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "HttpUtils.hpp"

class Client;

class Response
{
    private:

        static std::string  commonResponse(status_code_t status);
        static std::string  bodyHeaders(std::string extension, unsigned int size);

    public:
  
        static std::string cgiSimpleResponse(std::string & body);
   		static void cgiResponse(std::vector<unsigned char> & clientRawData,
                                std::string headers, std::vector<unsigned char> & body);
        static void errorResponse(status_code_t code, Client & client);
		static int deleteResponse(std::string path);
};




#endif