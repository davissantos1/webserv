/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:59:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/04 15:45:59 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

Location::Location() {}

Location::~Location() {}

Location::Location(const Location& other)
{
	if (this != &other)
		*this = other;
}

Location&	operator=(const Location& other)
{
	if (this != &other)
	{
		this->_root = other._root;
		this->_index = other._index;
		this->_paths = other._paths;
		this->_allowedMethods = other._allowedMethods;
	}
	return (*this);
}
