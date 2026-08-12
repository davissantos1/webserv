/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualHostConfig.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:37:28 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/11 15:19:50 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUALHOSTCONFIG_HPP
# define VIRTUALHOSTCONFIG_HPP

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "Location.hpp"

class VirtualHostConfig
{
	private:
		std::vector<std::string>	_serverNames;
		std::vector<std::string>	_hostIp;
		std::vector<int>			_port;
		std::map<int, std::string>	_errorPages;
		std::size_t					_maxBodySize;
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_allowedMethods;
		std::vector<Location>		_locations;
		std::pair<int, std::string>	_return;

		bool findLocation( std::string& path, Location& loc ) const;

	public:
		VirtualHostConfig();
		~VirtualHostConfig();
		VirtualHostConfig(const std::string configPath);
		VirtualHostConfig(const VirtualHostConfig& other);
		VirtualHostConfig&	operator=(const VirtualHostConfig& other);

		const std::vector<std::string>& getServerNames() const { return _serverNames; }
		const std::vector<std::string> & getVecHostIp() const { return _hostIp; }
		const std::vector<int> & getVecPort() const { return _port; }
		const std::string& getHostIp( const int i ) const { return (_hostIp[i]); }
		int getPort( const int i ) const { return (_port[i]); }
		const std::map<int, std::string>& getErrorPages() const { return _errorPages; }
		std::size_t getMaxBodySize() const { return _maxBodySize; }
		const std::string& getRoot() const { return _root; }
		const std::vector<std::string>& getIndex() const { return _index; }
		const std::vector<std::string>& getAllowedMethods() const { return _allowedMethods; }
		const std::vector<Location>& getLocation() const { return _locations; }
		const std::pair<int, std::string> & getReturn( void ) const { return (_return); }

		std::pair<int, std::string> getReturnCode( std::string& endpoint ); // to be done
		std::string	getErrorPage( int error, std::string& endpoint ); // to be done
		std::string	getCgiInterpreterPath( std::string& filename, std::string& endpoint ); // to be done
		std::string	getCgiScriptPath( std::string& filename, std::string& endpoint ); // to be done excolhe 1
		std::string	getFullPath( std::string& filename, std::string& endpoint); // to be done aqui exolhe 1
		bool		isMethodAllowed( std::string& method, std::string& endpoint ); // to be done
		bool		shouldIndex( std::string& endpoint ); // to be done
		bool		shouldAutoindex( std::string& endpoint ); // to be done
		bool		shouldRedirect( std::string& endpoint ); // to be done

		void	setServerNames( const std::vector<std::string> & serverNames ) { _serverNames = serverNames; }
		void	setHostIp( const std::vector<std::string> & hostIp ) { _hostIp = hostIp; }
		void	setPort( const std::vector<int> & port ) { _port = port; }
		void	setErrorPages( const std::map<int, std::string> & errorPages ) { _errorPages = errorPages; }
		void	setMaxBodySize( const std::size_t maxBodySize ) { _maxBodySize = maxBodySize; }
		void	setRoot( const std::string& root ) { _root = root; }
		void	setIndex( const std::vector<std::string> & index ) { _index = index; }
		void	setAllowedMethods( const std::vector<std::string> & allowedMethods ) { _allowedMethods = allowedMethods; }
		void	setLocations( const std::vector<Location> & locations ) { _locations = locations; }
		void	setReturn( const std::pair<int, std::string> & ret ) { _return = ret; };

		void	addServerName( const std::string& name ) { _serverNames.push_back(name); }
		void	addHostIp( const std::string& hostIp ) { _hostIp.push_back(hostIp); }
		void	addPort( const int port ) { _port.push_back(port); }
		void	addErrorPage( const int errorCode, const std::string& errorPath ) { _errorPages[errorCode] = errorPath; }
		void	addIndex( const std::string& index ) { _index.push_back(index); }
		void	addAllowedMethod( const std::string& method ) { _allowedMethods.push_back(method); }
		void	addLocation( const Location& location ) { _locations.push_back(location); }
};

std::ostream&	operator<<( std::ostream& out, const VirtualHostConfig& vhc );

#endif
