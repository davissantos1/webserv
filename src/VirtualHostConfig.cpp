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
#include <ostream>

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
