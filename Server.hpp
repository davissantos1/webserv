/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/06 18:58:00 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# define BACKLOG 20
# define MAX_EVENTS 10
# define _GNU_SOURCE

# include "Config.hpp"
# include "Connection.hpp"
# include <vector>
# include <map>
# include <iostream>
# include <sys/epoll.h>

class	Server
{
	private:
		std::vector<VirtualHostConfig>	_configs;
		std::map<int, Connection*>		_connectionMap;
		std::vector<Connection*>		_connections;
		std::vector<int>				_listenFds;
		int								_epollFd;
		bool							_isRunning;
	public:
		Server();
		~Server();	
		Server(const Server& other);	
		Server(const std::vector<VirtualHostConfig> config);
		Server&	operator=(const Server& other);
		void	startServer();
		void	loopServer();
		void	stopServer();
		class ServerException: std::exception
		{
			private:
				int	_errno;
			public
				ServerException(int err): _errno(err) {}
				const char* what() const throw { return std::strerror(_errno); }
		};
};

#endif
