/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:43 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 18:44:57 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BACKLOG 20

# include <vector>
# include <map>
# include <iostream>

class VirtualHostConfig;
class Client;

class	Server
{
	private:
		std::vector<VirtualHostConfig>	_configs;
		std::map<int, Client*>			_clientMap;
		std::vector<Client*>			_clients;
		std::vector<int>				_listenFds;
	public:
		Server();
		~Server();	
		Server(const Server& other);	
		Server&	operator=(const Server& other);
		Server(const std::vector<VirtualHostConfig> config);
		std::vector<int>	startServer();
		int					createClient(int sockFd);
		void				destroyClient(int clientFd);
		static void			printLog(const std::string& msg);
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
