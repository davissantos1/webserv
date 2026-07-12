/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHostConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:37:20 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/04 15:49:19 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VirtualHostConfig.hpp"

VirtualHostConfig::VirtualHostConfig() {}

VirtualHostConfig::~VirtualHostConfig() {}

VirtualHostConfig::VirtualHostConfig(const std::string configPath)
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
	}
	return (*this);
}

VirtualHostConfig::VirtualHostConfig(const VirtualHostConfig& other)
{
	if (this != &other)
		*this = other;
}
