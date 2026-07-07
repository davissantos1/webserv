/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHostConfig.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:37:28 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 08:50:59 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALHOSTCONFIG_HPP
# define VIRTUALHOSTCONFIG_HPP

#include <string>
#include <map>
#include <vector>

class Location;

class VirtualHostConfig
{
	private:
		std::vector<std::string>	_serverNames;
		std::string					_hostIp;
		int							_port;
		std::map<int, std::string>	_errorPages;
		std::size_t					_maxBodySize;
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_allowedMethods;
		std::vector<Location>		_locations;

	public:
		VirtualHostConfig();
		~VirtualHostConfig();
		VirtualHostConfig(const std::string configPath);
		VirtualHostConfig(const VirtualHostConfig& other);
		VirtualHostConfig&	operator=(const VirtualHostConfig& other);

		const std::vector<std::string>& getServerNames() const { return _serverNames; }
		const std::string& getHostIp() const { return _hostIp; }
		const int getPort() const { return _port; }
		const std::map<int, std::string>& getErrorPages() const { return _errorPages; }
		const std::size_t getMaxBodySize() const { return _maxBodySize; }
		const std::string& getRoot() const { return _root; }
		const std::vector<std::string>& getIndex() const { return _index; }
		const std::vector<std::string>& getAllowedMethods() const { return _allowedMethods; }
		const std::vector<Location>& getLocation() const { return _locations; }
};

#endif
