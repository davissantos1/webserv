/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/11 22:02:17 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BACKLOG 20

# include "webserv.hpp"
# include "Client.hpp"
# include "Multiplexer.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
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
		struct addrinfo*						_currAddr;
	public:
		Server();
		~Server();	
		Server(const Server& other);	
		Server&	operator=(const Server& other);
		Server(const std::vector<VirtualHostConfig> config);
		void				runServer();
		void				startServer();
		void				checkTimeouts();
		int					createClient(int sockFd);
		void				destroyClient(int clientFd);
		static void			printLog(const std::string& msg);
		void				routeServer(int fd, uint32_t eventType, enum FdType fdType);
		void				handleProcessedFile(Client* client, int statusCode, enum FdType type);
		void				handleError(int fd, enum FdType fdType);
		class ServerException: public std::exception
		{
			private:
				int			_errno;
				const char*	_error;
			public:
				ServerException(int err): _errno(err), _error(NULL) {}
				ServerException(const char* error): _errno(0), _error(error){}
				const char* what() const throw()
				{ 
					if (this->_error)
						return (this->_error);
					return std::strerror(_errno);
				}
		};
};

#endif
