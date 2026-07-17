/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 15:31:29 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config) {}

Server::~Server() 
{
	for (int i = 0; i < this->_clients.size(); i++)
	{
		Client* curr = this->_clients[i];
		if (curr)
		{
			close(curr->_fd);
			delete(curr);
		}
	}
	for (int j = 0; j < this->_listenFds.size(); j++)
		close(this->_listenFds[j]);
}

Server&	Server::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_configs = other._configs;
		this->_serverMap = other._serverMap;
		this->_listenFds = other._listenFds;
		this->_clientMap = other._clientMap;
		this->_cgiMap = other._cgiMap;
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
		hints.ai_family = AF_INET;
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
		this->_configMap[sockFd] = v;
	}
	serverRunning = 1;
	return (this->_listenFds);
}

void	Server::runServer()
{
	std::vector<std::pair<int, uint32_t>> fds;
	std::vector<int>::iterator listenBegin, listenEnd, it;
	enum FdType fdType;
	int	fdSize;

	while (serverRunning)
	{
		fds = this->_multiplexer.wait();
		fdSize = fds.size();
		for (int j = 0; j < fdSize; j++)
		{
			listenBegin = this->_listenFds.begin();
			listenEnd = this->_listenFds.end();
			it = std::find(listenBegin, listenEnd, fds[j].first);
			fdType = (it != listenEnd) ? SOCKET : CLIENT;
			if (this->_cgiMap.count(fds[j].first) > 0)
				fdType = CGI;
			this->routeServer(fds[j].first, fds[j].second, fdType);
			this->checkTimeouts();
		}
	}
}

void	Server::routeServer(int fd, uint32_t eventType, enum FdType fdType)
{
	int status = 0;

	if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		this->handleError(fd, fdType);
		return ;
	}

	switch (fdType)
	{
		case SOCKET:
			if (eventType & EPOLLIN)
			{
				int newClient = this->createClient(fd);
				if (newClient != -1)
					this->_multiplexer.addFd(newClient, (EPOLLIN | EPOLLRDHUP));		
			}
			break;
		case CLIENT:
		{
			Client* client = this->_clientMap[fd];
			if (!client) break;

			if (eventType & EPOLLIN)
				status = client->processHttpRequest();
			else if (eventType & EPOLLOUT)
				status = client->sendHttpResponse();

			if (status == -1)
			{
				this->destroyClient(fd);
				return ;
			}
			
			enum ClientStatus clientStatus = client->getStatus();
			if (clientStatus == PROCESSING_STATIC_FILE || clientStatus == PROCESSING_CGI)
			{
				this->_multiplexer.removeFd(fd);
				std::vector<FdTask> tasks = client->executeMethod(clientStatus);
				for (int i = 0; i < tasks.size(); i++)
				{
					switch (tasks[i].type)
					{
						case STATIC_FILE_READ:
							this->_staticFileMap[tasks[i].fd] = client;
							this->_multiplexer.addFd(tasks[i].fd, EPOLLIN);
							break;
						case CGI_READ:
							this->_cgiMap[tasks[i].fd] = client;
							this->_multiplexer.addFd(tasks[i].fd, EPOLLIN);
							break;
						case STATIC_FILE_WRITE:
							this->_staticFileMap[tasks[i].fd] = client;
							this->_multiplexer.addFd(tasks[i].fd, EPOLLOUT);
							break;
						case CGI_WRITE:
							this->_cgiMap[tasks[i].fd] = client;
							this->_multiplexer.addFd(tasks[i].fd, EPOLLOUT);
							break;
					}
					client->registerFd(tasks[i].fd);
				}
			}
			break;
		}
		case STATIC_FILE:
		{
			Client* client = this->_staticFileMap[fd];
			if (!client) break;
			
			bool isDone = client->processStaticFile(fd, eventType);
			
			if (isDone)
			{
				this->_multiplexer.removeFd(fd);
				close(fd);
				this->_staticFileMap.erase(fd);
				this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);	
			}
			
			break;
		}
		case CGI:
		{
			Client* client = this->_cgiMap[fd];
			if (!client) break;

			bool isPipeDone = client->processCgi(fd, eventType);
			if (isPipeDone)
			{
				this->_multiplexer.removeFd(fd);
				close(fd);
				this->_cgiMap.erase(fd);
				if (client->getStatus() == PREPARING_RESPONSE)
					this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);	
			}
			break;
		}
	}
}

void	Server::handleError(int fd, enum FdType fdType)
{
	switch (fdType)
	{
		case SOCKET:
			this->_listenFds.erase(std::remove(this->_listenFds.begin(), this->_listenFds.end(), fd));
			close(fd);
			Server::printLog("socket fd removed from connection");
			break;
		case CLIENT:
			this->destroyClient(fd);
			break;
		case STATIC_FILE:
		{
			Client* client = this->_staticFileMap[fd];

			this->_multiplexer.removeFd(fd);
			this->_staticFileMap.erase(fd);
			close(fd);
			client->setStatusCode(500);
			client->setStatus(PREPARING_RESPONSE);
			this->_multiplexer.addFd(client->getFd(), EPOLLOUT);
			Server::printLog("static file presented an error");
			break;
		}
		case CGI:
		{
			Client* client = this->_cgiMap[fd];

			this->_multiplexer.removeFd(fd);
			this->_cgiMap.erase(fd);
			close(fd);
			client->destroyCgi(fd);
			client->setStatusCode(500);
			client->setStatus(PREPARING_RESPONSE);
			this->_multiplexer.addFd(client->getFd(), EPOLLOUT);
			Server::printLog("CGI FD removed due to internal error");
			break;
		}
	}
}

int	Server::createClient(int sockFd)
{
	std::string ipStr;
	struct sockaddr_storage addr;
	struct socklen_t addr_len = sizeof(addr);
	int clientFd, status;
	uint16_t	port = 0;
	uint32_t	ip = 0;

	if ((clientFd = accept(sockFd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		Server::printLog("accept error on client!");
		return (clientFd);
	}
	if (addr.ss_family == AF_INET)
	{
		struct sockaddr_in* addr_in = (struct sockaddr_storage*)&addr;
		port = ntohs(addr_in->sin_port);
		ip = ntohl(addr_in->sin_addr.s_addr);
		std::stringstream ipStream;
		ipStream	<< ((ip >> 24) & 0xFF) << "."
					<< ((ip >> 16) & 0xFF) << "."
					<< ((ip >> 8) & 0xFF) << "."
					<< ((ip & 0xFF));
		ipStr = ipStream.str();
	}
	Client* newClient = new Client(ipStr, port, clientFd, this->_configMap[sockFd]);
	this->_clients.push_back(newClient);
	this->_clientMap[sockFd] = newClient;
	return (clientFd);
}

void	Server::destroyClient(int clientFd)
{
	Client* client = this->_clientMap[clientFd];
	if (!client) return ;

	this->_multiplexer.removeFd(clientFd);
	this->_clientMap.erase(clientFd);
	this->_clients.erase(std::remove(this->_clients.begin(), this->_clients.end(), client), this->_clients.end());
	std::vector<int> activeFds = client->getActiveFds();
	for (size_t i = 0; i < activeFds.size(); i++)
	{
		int fd = activeFds[i];
		this->_multiplexer.removeFd(fd);
		this->_staticFileMap.erase(fd);
		this->_cgiMap.erase(fd);
		close (fd);
	}
	close(clientFd);
	delete (client);
}

void	Server::checkTimeouts()
{
	std::time_t currentTime = std::time(NULL);

	for (int i = 0; i < this->_clients.size(); i++)
	{
		Client* currentClient = this->_clients[i];
		double secondsIdle = std::difftime(currentTime, currentClient->getLastActivity());

		if (secondsIdle > TIMEOUT)
		{
			Server::printLog("client timed out!");
			this->destroyClient(currentClient->getFd());
		}
	}
}

void	Server::printLog(const std::string& msg)
{
	std::cerr	<< "[WEBSERV]: "
				<< msg
				<< std::endl;
}
