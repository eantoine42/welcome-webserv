/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/28 16:27:42 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 23:05:42 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <ostream>
#include <vector>

#define WHITESPACES " \n\r\t\f\v"

class StringUtils
{
    private:

        StringUtils();
		StringUtils(const StringUtils &copy);
		StringUtils &operator=(const StringUtils &copy);
		~StringUtils();

    public:

		static std::string				trimComments(const std::string &str);
		static std::string				trimWhitespaces(const std::string& str);
		static std::string				replaceConsecutiveSpaces(const std::string& str);
		static std::string				getLine(std::string str, size_t n);
		static size_t 					nbLines(std::string &str_config);
		static bool 					isNothing(std::string str, int pos = 0);
		static bool 					checkBrackets(std::string &config_string);
		static std::string 				trimLineToI(std::string &str, size_t pos);
		static int						findClosingBracket(std::string str);
		static std::vector<std::string> splitString(std::string str, const std::string &charset = " ");
		static std::string				intToString(int num);
};

template < class T >
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it == v.begin())
            os << *it;
        else
            os << "; " << *it;
    }
    return os;
}

#endif