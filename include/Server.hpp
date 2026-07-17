/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 05:29:16 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BACKLOG 20

# include "webserv.hpp"
# include "Client.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <cstring>
# include <algorithm>
# include <sstream>
# include <vector>
# include <map>
# include <iostream>

enum FdType
{
	SOCKET,
	CLIENT,
	STATIC_FILE,
	CGI
};

class VirtualHostConfig;

class	Server
{
	private:
		std::vector<VirtualHostConfig>			_configs;
		std::map<int, VirtualHostConfig>		_configMap;
		std::map<int, Client*>					_clientMap;
		std::map<int, Client*>					_cgiMap;
		std::map<int, Client*>					_staticFileMap;
		std::vector<Client*>					_clients;
		std::vector<int>						_listenFds;
		Multiplexer								_multiplexer;
	public:
		Server();
		~Server();	
		Server(const Server& other);	
		Server&	operator=(const Server& other);
		Server(const std::vector<VirtualHostConfig> config);
		void				runServer()
		std::vector<int>	startServer();
		int					createClient(int sockFd);
		void				destroyClient(int clientFd);
		static void			printLog(const std::string& msg);
		void				routeServer(int fd, uint32_t eventType, enum FdType fdType)
		class ServerException: public std::exception
		{
			private:
				int	_errno;
			public:
				ServerException(int err): _errno(err) {}
				const char* what() const throw { return std::strerror(_errno); }
		};
};

#endif
