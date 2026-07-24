/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:59:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/24 14:31:04 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location( void ): _autoindex(false), _uploadEnable(false) {}

Location::~Location( void ) {}

Location::Location( const Location& other )
{
	if (this != &other)
		*this = other;
}

Location&	Location::operator=( const Location& other )
{
	if (this != &other)
	{
		this->_root = other._root;
		this->_index = other._index;
		this->_path = other._path;
		this->_allowedMethods = other._allowedMethods;
		this->_autoindex = other._autoindex;
		this->_uploadEnable = other._uploadEnable;
		this->_uploadStore = other._uploadStore;
		this->_cgiExtension = other._cgiExtension;
		this->_cgiPath = other._cgiPath;
	}
	return (*this);
}
