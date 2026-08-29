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
#include <ostream>

Location::Location( void ): _maxBodySize(0), _autoindex(false) {}

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
		this->_maxBodySize = other._maxBodySize;
		this->_allowedMethods = other._allowedMethods;
		this->_autoindex = other._autoindex;
		this->_uploadPath = other._uploadPath;
		this->_cgiExtension = other._cgiExtension;
		this->_cgiPath = other._cgiPath;
		this->_errorPages = other._errorPages;
		this->_return = other._return;
	}
	return (*this);
}

std::ostream&	operator<<( std::ostream& out, const Location& loc )
{
	out << "  Location Path: " << loc.getPath() << "\n";
	out << "    Root: " << loc.getRoot() << "\n";
	out << "    Autoindex: " << (loc.getAutoindex() ? "on" : "off") << "\n";

	out << "    Index: ";
	for (std::size_t i = 0; i < loc.getIndex().size(); ++i)
		out << loc.getIndex()[i] << " ";
	out << "\n";

	out << "    Max body size: " << loc.getMaxBodySize() << std::endl;

	out << "    Allowed Methods: ";
	for (std::size_t i = 0; i < loc.getAllowedMethods().size(); ++i)
		out << loc.getAllowedMethods()[i] << " ";
	out << "\n";

	if (!loc.getUploadPath().empty())
		out << "    Upload Path: " << loc.getUploadPath() << "\n";

	if (!loc.getCgiExtension().empty())
	{
		out << "    CGI Extension: " << loc.getCgiExtension() << "\n";
		out << "    CGI Path: " << loc.getCgiPath() << "\n";
	}

	if (loc.getReturn().first != 0)
	{
		out <<  "    Return code: " << loc.getReturn().first << "\n";
		out <<  "    Return URL: " << loc.getReturn().second << "\n";
	}
	return out;
}
