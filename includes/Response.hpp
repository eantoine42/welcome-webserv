/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/03 19:19:08 by lfrederi          #+#    #+#             */
/*   Updated: 2023/07/16 21:07:01 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "HttpUtils.hpp"

struct resp_t
{
    status_code_t                   status;
    std::vector<unsigned char>      body;
    std::string                     extension;
    std::vector<unsigned char> &    rawData;
    bool                            keepAlive;
};

class Response
{
    private:

        static std::string  commonResponse(status_code_t status);
        static std::string  bodyHeaders(std::vector<unsigned char> body, std::string extension);

    public:

        static void badRequest(std::vector<unsigned char> & rawData);
        static void createResponse(resp_t resp);
        static std::string cgiSimpleResponse(std::string & body);
        static std::string  errorResponse(status_code_t code);

};




#endif