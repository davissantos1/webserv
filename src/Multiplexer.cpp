/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/14 13:30:47 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Multiplexer.hpp"

Multiplexer::~Multiplexer() {}

Multiplexer::Multiplexer()
{
	if ((this->_epollFd = epoll_create(1)) == -1)
		throw (MultiplexerException(errno));
}

Multiplexer::Multiplexer(const Multiplexer& other)
{
	if (this != &other)
		*this = other;
}

Multiplexer&	Multiplexer::operator=(const Multiplexer& other)
{
	if (this != &other)
		this->_epollFd = other._epollFd;
	return (*this);
}

void	Multiplexer::eventLoop(Server* server)
{
	int eventQuantity, currentFd;
	struct epoll_event event, readyEvents[MAXEVENTS];
	std::vector<int>::iterator listenBegin, listenEnd, it;
	enum FdType fdType;

	while (serverRunning)
	{
		eventQuantity = epoll_wait(this->_epollFd, readyEvents, MAX_EVENTS, -1);
		for (int j = 0; j < eventQuantity; j++)
		{
			currentFd = readyEvents[j].data.fd;
			listenBegin = server->_listenFds.begin();
			listenEnd = server->_listenFds.end();
			it = std::find(listenBegin, listenEnd, readyEvents[j].data.fd);
			fdType = (it != listenEnd) ? SOCKET : CLIENT;
			this->routeEvent(currentFd, readyEvents[j].events, fdType, server);
		}
	}
}

void	Multiplexer::routeEvent(int currentFd, uint32_t eventType, enum FdType fdType, Server& server)
{
	int acceptFd;
	Client* client = (fdType == CLIENT) ? server._clientMap[currentFd] : nullptr;
	if (eventType & EPOLLIN)
	{
		if (fdType == SOCKET)
		{
			acceptFd = server->createClient(currentFd);
			this->manageFd(acceptFd, EPOLL_CTL_ADD, EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP);
		}
		else if (fdType == CLIENT)
			client->processHttpRequest(currentFd);
	}
	if ((eventType & EPOLLOUT) && fdType == CLIENT)
		client->processHttpResponse(currentFd);
	if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		if (fdType == SOCKET)
		{
			std::remove(server->_listenFds.begin(), server->_listenFds.end(), currentFd);
			close(currentFd);
			Server::printLog("socket fd removed from connection");
		}
		else if (fdType == CLIENT)
			server->destroyClient(currentFd);
	}
}

void	Multiplexer::manageFd(int fd, int op, uint32_t event)
{
	int		status;
	struct	epoll_event ev;

	errno = 0;
	ev.events = event;
	if (op == EPOLL_CTL_ADD || op == EPOLL_CTL_MOD || op == EPOLL_CTL_DEL)
		status = epoll_ctl(this->_epollFd, op, fd, &event);
	if (status < 0)
		throw (MultiplexerException(errno));
}
