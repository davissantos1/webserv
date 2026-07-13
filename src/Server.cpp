/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 18:42:51 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config) {}

Server::~Server() 
{
	for (int i = 0; this->_clients[i]; i++)
	{
		Client* curr = this->_clients[i];
		if (curr)
		{
			close(curr->_fd);
			delete(curr);
		}
	}
	for (int j = 0; j < this->_listenFds.size(); j++)
		close(this->_listenFds[i]);
}

Server&	Sever::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_configs = other._configs;
		this->_listenFds = other._listenFds;
		this->_clientMap = other._clientMap;
		this->_clients = other._clients;
	}
	return (*this);
}

Server::Server(const Server& other)
{
	if (this != &other)
		*this = other;
}

std::vector<int> Server::startServer()
{
	int sockFd, status;
	int sz = this->_configs.size();
	struct addrinfo	hints, *res, *p;

	for (int i = 0; i < sz; i++)
	{
		std::memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		VirtualHostConfig v = this->_configs[i];
		if ((status = getaddrinfo(v.getHostIp().c_str(), "http", &hints, &res)) == -1)
			throw (ServerException(errno));
		for (p = res; p != NULL; p = p->ai_next)
		{
			if ((sockFd = socket(v.getHostIp().c_str(), SOCK_STREAM, "http")) == -1)
				continue;
			break ;
		}
		if (sockFd == -1)
			throw (ServerException(errno));
		this->_listenFds.push_back(sockFd);
		if ((status = bind(sockFd, res->ai_address, res->ai_addrlen)) == -1)
			throw (ServerException(errno));
		if ((status = listen(sockFd, BACKLOG) == -1)
			throw (ServerException(errno));
		if ((status = fcntl(sockFd, F_SETFL, O_NONBLOCK)) == -1)
			throw (ServerException(errno));
	}
	serverRunning = 1;
	return (this->_listenFds);
}

int Server::createClient(int sockFd)
{
	struct sockaddr_storage addr;
	struct socklen_t addr_len = sizeof(addr);
	int clientFd, status;

	if ((clientFd = accept(sockFd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		Server::printLog("accept error on client!");
		return ;
	}
	Client* newClient = new Client(clientFd);
	this->_clients.push_back(newClient);
	this->_clientMap[sockFd] = newClient;
	return (clientFd);
}

void	Server::destroyClient(int clientFd)
{
	Client* client = this->_clientMap[clientFd];

	this->_clientMap.erase(clientFd);
	std::remove(this->_clients.begin(), this->clients.end(), conn); 
	close(clientFd);
	delete(client);
}

void	Server::printLog(const std::string& msg)
{
	std::cerr	<< "[WEBSERV]: "
				<< msg
				<< std::endl;
}
