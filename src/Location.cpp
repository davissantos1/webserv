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

std::ostream&	operator<<( std::ostream& out, const Location& loc )
{
	out << "  Location Path: " << loc.getPath() << "\n";
	out << "    Root: " << loc.getRoot() << "\n";
	out << "    Autoindex: " << (loc.getAutoindex() ? "on" : "off") << "\n";

	out << "    Index: ";
	for (std::size_t i = 0; i < loc.getIndex().size(); ++i)
		out << loc.getIndex()[i] << " ";
	out << "\n";

	out << "    Allowed Methods: ";
	for (std::size_t i = 0; i < loc.getAllowedMethods().size(); ++i)
		out << loc.getAllowedMethods()[i] << " ";
	out << "\n";

	out << "    Upload Enable: " << (loc.getUploadEnable() ? "on" : "off") << "\n";
	if (!loc.getUploadStore().empty())
		out << "    Upload Store: " << loc.getUploadStore() << "\n";

	if (!loc.getCgiExtension().empty())
	{
		out << "    CGI Extension: " << loc.getCgiExtension() << "\n";
		out << "    CGI Path: " << loc.getCgiPath() << "\n";
	}
	return out;
}
