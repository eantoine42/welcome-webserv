/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimeUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 21:22:45 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 21:41:29 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_UTILS_HPP
#define TIME_UTILS_HPP

#include <string>
#include <ctime>

#define WHITESPACES " \n\r\t\f\v"

class TimeUtils
{
    private:

        TimeUtils();
		TimeUtils(const TimeUtils &copy);
		TimeUtils &operator=(const TimeUtils &copy);
		~TimeUtils();

    public:

        static std::string  getFormattedDate(std::time_t time);
        static long long    getTimeOfDayMs();

};

#endif