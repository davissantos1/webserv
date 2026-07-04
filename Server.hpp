/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/03 17:25:06 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

#include "Config.hpp"
#include "Connection.hpp"
#include <vector>
#include <map>
#include <iostream>

class	Server
{
	private:
		std::vector<VirtualHostConfig>	_configs;
		std::vector<int>				_listenFds;
		int								_epollFd;
		std::map<int, Connection*>		_connectionMap;
		bool							_isRunning;
	public:
		Server();
		~Server();	
		Server(const Server& other);	
		Server(const std::vector<Config> config);
		Server&	operator=(const Server& other);
		void	startServer();
		void	stopServer();
		// to be continued...
};

#endif
