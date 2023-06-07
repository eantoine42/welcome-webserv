/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AFileDescriptor.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 14:06:22 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/05 17:35:07 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_FILE_DESCRIPTOR_HPP
#define A_FILE_DESCRIPTOR_HPP

#include <vector>

class AFileDescriptor
{
	protected:

		int							_fd;
		std::vector<unsigned char>	_rawData;
		bool						_open;

		// Cannonical fornm
		AFileDescriptor();
		AFileDescriptor(AFileDescriptor const & copy);
		AFileDescriptor & operator=(AFileDescriptor const & rhs);

		// Constructors
		AFileDescriptor(int fd);

	public:
		virtual ~AFileDescriptor();

		// Geters
		int									getFd() const;
		std::vector<unsigned char> const &	getRawData() const;
		bool								isOpen() const;

};


#endif
