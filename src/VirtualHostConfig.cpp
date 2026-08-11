/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHostConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:37:20 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/04 20:22:34 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualHostConfig.hpp"
#include "Location.hpp"
#include <algorithm>
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

VirtualHostConfig::VirtualHostConfig(): _maxBodySize(0) {}

VirtualHostConfig::~VirtualHostConfig() {}

VirtualHostConfig::VirtualHostConfig(const std::string configPath):_maxBodySize(0)
{
    (void)configPath;
}

VirtualHostConfig&	VirtualHostConfig::operator=(const VirtualHostConfig& other)
{
	if (this != &other)
	{
		this->_serverNames = other._serverNames;
		this->_hostIp = other._hostIp;
		this->_port= other._port;
		this->_errorPages = other._errorPages;
		this->_maxBodySize = other._maxBodySize;
		this->_root = other._root;
		this->_index = other._index;
		this->_locations = other._locations;
		this->_allowedMethods = other._allowedMethods;
		this->_return = other._return;
	}
	return (*this);
}

VirtualHostConfig::VirtualHostConfig(const VirtualHostConfig& other)
{
	if (this != &other)
		*this = other;
}

bool VirtualHostConfig::findLocation( std::string& path, Location& loc ) const
{
	for(size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i].getPath() == path)
		{
			loc = _locations[i];
			return (true);
		}
	}
	return (false);
}

std::string	VirtualHostConfig::getCgiInterpreterPath( std::string& filename, std::string& endpoint )
{

}

std::pair<int, std::string> VirtualHostConfig::getReturn( std::string& endpoint )
{
	Location local;

	if (findLocation(endpoint, local))
	{
		if (local.getReturn().first != 0)
			return (local.getReturn());
	}
	return (_return);
}

std::string	VirtualHostConfig::getErrorPage( int error, std::string& endpoint )
{

}

std::string	VirtualHostConfig::getCgiScriptPath( std::string& filename, std::string& endpoint )
{

}

std::string	VirtualHostConfig::getFullPath( std::string& filename, std::string& endpoint)
{

}

bool		VirtualHostConfig::isMethodAllowed( std::string& method, std::string& endpoint )
{
	Location	local;

	if (findLocation(endpoint, local))
	{
		return (std::find(local.getAllowedMethods().begin(),
			local.getAllowedMethods().end(), method)
			!= local.getAllowedMethods().end());
	}
	return (std::find(_allowedMethods.begin(), _allowedMethods.end(), method)
		!= _allowedMethods.end());
}

bool		VirtualHostConfig::shouldIndex( std::string& endpoint )
{
	Location local;

	if (findLocation(endpoint, local))
	{
		return (!local.getIndex().empty());
	}
	return (false);
}

bool		VirtualHostConfig::shouldAutoindex( std::string& endpoint )
{
	Location local;

	if (findLocation(endpoint, local))
	{
		return (local.getAutoindex());
	}
	return (false);
}

bool		VirtualHostConfig::shouldRedirect( std::string& endpoint )
{

}

std::ostream&	operator<<( std::ostream& out, const VirtualHostConfig& vhc )
{
	out << "  Server Names: ";
	for (std::size_t i = 0; i < vhc.getServerNames().size(); ++i)
		out << vhc.getServerNames()[i] << " ";
	out << "\n";

	out << "  Listen (IP:Port): ";
	for (std::size_t i = 0; i < vhc.getVecHostIp().size(); ++i)
		out << vhc.getHostIp(i) << ":" << vhc.getPort(i) << " ";
	out << "\n";

	out << "  Root: " << vhc.getRoot() << "\n";
	out << "  Max Body Size: " << vhc.getMaxBodySize() << "\n";

	out << "  Index: ";
	for (std::size_t i = 0; i < vhc.getIndex().size(); ++i)
		out << vhc.getIndex()[i] << " ";
	out << "\n";

	out << "  Allowed Methods: ";
	for (std::size_t i = 0; i < vhc.getAllowedMethods().size(); ++i)
		out << vhc.getAllowedMethods()[i] << " ";
	out << "\n";

	out << "  Error Pages:\n";
	std::map<int, std::string> ep = vhc.getErrorPages();
	for (std::map<int, std::string>::const_iterator it = ep.begin(); it != ep.end(); ++it)
		out << "    " << it->first << " -> " << it->second << "\n";

	out << "  Locations:\n";
	for (std::size_t i = 0; i < vhc.getLocation().size(); ++i)
		out << vhc.getLocation()[i] << "\n";

	out << "  Return:\n";
	if (vhc.getReturn().first != 0)
	{
		out <<  "    Return code: " << vhc.getReturn().first << "\n";
		out <<  "    Return URL: " << vhc.getReturn().second << "\n";
	}
	return out;
}
