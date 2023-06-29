/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:41 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 21:42:39 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TimeUtils.hpp"
#include "Exception.hpp"
#include <sys/time.h> // gettimeofday

/*****************
* CANNONICAL FORM
*****************/

TimeUtils::TimeUtils()
{}

TimeUtils::TimeUtils(TimeUtils const & copy)
{
    (void) copy;
}

TimeUtils & TimeUtils::operator=(TimeUtils const & rhs)
{
    (void) rhs;
    return (*this);
}

TimeUtils::~TimeUtils()
{}
/******************************************************************************/

/***************
* STATIC METHOD
***************/

std::string TimeUtils::getFormattedDate(std::time_t time)
{
    char		date[100];
	int			ret;

    ret = std::strftime(date, sizeof(date), "%a, %d %b %Y %X GMT", std::localtime(&time));
	if (!ret)
	{
		throw (FatalError("Webserv error: strftime() function failed"));
	}
	return(std::string(date));	
}

long long	TimeUtils::getTimeOfDayMs()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<long long>(tv.tv_sec) *1000LL + static_cast<long long> (tv.tv_usec)/1000LL;
}