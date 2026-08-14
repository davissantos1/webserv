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
#include <map>
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
		this->_uploadPath = other._uploadPath;
	}
	return (*this);
}

VirtualHostConfig::VirtualHostConfig(const VirtualHostConfig& other)
{
	if (this != &other)
		*this = other;
}

bool VirtualHostConfig::findLocation( const std::string& path, Location& loc ) const
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


static bool checkFileExtension( const std::string& filename )
{
	size_t		i;
	std::string	extension;

	i = filename.rfind('.');
	extension = filename.substr(i);
	return (extension == ".py" || extension == ".php");
}


// Essa daqui vai retornar "" se não achar o endpoint ou a extensão do arquivo não for válida
std::string	VirtualHostConfig::getCgiInterpreterPath( const std::string& filename, const std::string& endpoint ) const
{
	Location local;

	if (findLocation(endpoint, local))
	{
		if (checkFileExtension(filename))
		{
			if (local.getPath().empty())
			{
				if (local.getCgiExtension() == ".py")
					return ("/usr/bin/python3");
				else
					return ("/usr/bin/php");
			}
			else
				return (local.getCgiPath());
		}
	}
	return (std::string());
}

std::pair<int, std::string> VirtualHostConfig::getReturn( const std::string& endpoint ) const
{
	Location local;

	if (findLocation(endpoint, local))
	{
		if (local.getReturn().first != 0)
			return (local.getReturn());
	}
	return (_return);
}

std::string	VirtualHostConfig::getErrorPage( const int error, const std::string& endpoint ) const
{
	Location local;
	std::map<int, std::string>::const_iterator it;

	if (findLocation(endpoint, local))
	{
		if (!_errorPages.empty())
		{
			it = local.getErrorPages().find(error);
			if (it != _errorPages.end())
				return (it->second);
		}
	}

	it = getErrorPages().find(error);
	if (it != getErrorPages().end())
		return (it->second);
	return (std::string());
}


std::string	VirtualHostConfig::getFullPath( const std::string& filename, const std::string& endpoint ) const
{
	Location	local;

	if (!checkFileExtension(filename))
		return (std::string());
	if (findLocation(endpoint, local))
	{
		if (!local.getRoot().empty())
			return (local.getRoot() + filename);
	}
	return (getRoot() + filename);
}

bool		VirtualHostConfig::isMethodAllowed( const std::string& method, const std::string& endpoint ) const
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

bool		VirtualHostConfig::shouldIndex( const std::string& endpoint ) const
{
	Location local;

	if (findLocation(endpoint, local))
	{
		return (!local.getIndex().empty());
	}
	return (!getIndex().empty());
}

bool		VirtualHostConfig::shouldAutoindex( const std::string& endpoint ) const
{
	Location local;

	return (findLocation(endpoint, local) && local.getAutoindex());
}

bool		VirtualHostConfig::shouldRedirect( const std::string& endpoint ) const
{
	Location local;

	if (findLocation(endpoint, local))
	{
		if (local.getReturn().first != 0)
			return (true);
	}
	return (getReturn().first != 0);
}

std::string	VirtualHostConfig::getUploadPath( const std::string& endpoint ) const
{
	Location	local;

	if (findLocation(endpoint, local))
	{
		if (local.getUploadPath().empty())
			return (local.getUploadPath());
	}
	return (getUploadPath());
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

	if (!vhc.getUploadPath().empty())
		out << "    Upload Path: " << vhc.getUploadPath() << "\n";

	return out;
}
