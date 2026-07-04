/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:29:09 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/04 15:41:44 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

class	Location
{
	private:
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_paths;
		std::vector<std::string>	_allowedMethods;
	public:
		Location();
		~Location();
		Location(const Location& other);
		Location&	operator=(const Location& other);
};

#endif
