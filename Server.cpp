/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/06 18:25:37 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config) {}

Server::~Server() {}

Server&	Sever::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_configs = other._configs;
		this->_listenFds = other._listenFds;
		this->_connectionMap = other._connectionMap;
		this->_connections = other._connections;
		this->_isRunning = other._isRunning;
	}
	return (*this);
}

Server::Server(const Server& other)
{
	if (this != &other)
		*this = other;
}

void	Server::startServer()
{
	int sockFd, status;
	int sz = this->_configs.size();
	struct epoll_event event;
	struct addrinfo	hints, *res, *p;

	event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	if ((this->_epollFd = epoll_create(1)) == -1)
		throw (ServerException(errno));
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
		event.data.fd = this->_listenFds[i];
		if ((status = epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_listenFds[i], &event)) == -1)
			throw (ServerException(errno));
	}
	this->_isRunning = true;
}

void	Server::stopServer()
{
	for (int i = 0; this->_connections[i]; i++)
	{
		Connection* curr = this->_connections[i];
		if (curr)
		{
			close(curr->_connectionFd);
			delete(curr);
		}
	}
	for (int j = 0; j < this->_listenFds.size(); j++)
		close(this->_listenFds[i]);
}

void	Server::loopServer()
{
	int status, event_quantity, current_fd;
	struct epoll_event event, ready_events[MAX_EVENTS];
	std::vector<int>::iterator listenBegin, listenEnd, it;
	uint32_t event_type

	while (true)
	{
		event_quantity = epoll_wait(this->_epollFd, ready_events, MAX_EVENTS, -1);
		for (int j = 0; j < event_quantity; j++)
		{
			currentFd = ready_events[j].data.fd;
			eventType = ready_events[j].events;
			listenBegin = this->_listenFds.begin();
			listenEnd = this->_listenFds.end();
			it = std::find(listenBegin, listenEnd, ready_events[j].data.fd);
			if (eventType & EPOLLIN)
			{
				if (it != listenEnd)
					this->createConnection(currentFd);
				else
					this->processHttpRequest();
			}
			if (eventType & EPOLLOUT)
			{
				if (it == listenEnd)
					this->processHttpResponse();

			}
			if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
			{
				if (it != listenEnd)
				{
					std::remove(this->_listenFds.begin(), this->_listenFds.end(), currentFd);
					close(currentFd);
					Server::printLog("socket fd removed from connection");
				}
				else
					this->destroyConnection(currentFd);
			}
		}
	}
}

void	Server::createConnection(int sockFd)
{
	struct epoll_event event;
	struct sockaddr_storage addr;
	struct socklen_t addr_len = sizeof(addr);
	int connectionFd, status;

	event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
	if ((connectionFd = accept(event.data.fd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		Server::printLog("accept error on client!");
		return ;
	}
	event.data.fd = connectionFd;
	if ((status = epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, connectionFd, &event)) == -1)
	{
		Server::printLog("epoll_ctl error on client!");
		close(connectionFd);
		return ;
	}
	Connection* newConnection = new Connection(connectionFd);
	this->_connections.push_back(newConnection);
	this->_connectionMap[sockFd] = newConnection;
}

void	Server::destroyConnection(int connectionFd)
{
	Connection* conn = this->_connectionMap[connectionFd];

	this->_connectionMap.erase(connectionFd);
	std::remove(this->_connections.begin(), this->connections.end(), conn); 
	close(connectionFd);
	delete(conn);
}

void	Server::printLog(std::string& msg)
{
	std::cerr	<< "[WEBSERV]: "
				<< msg
				<< std::endl;
}

void	Server::processRequest()
{

}

void	Server::processResponse()
{

}

