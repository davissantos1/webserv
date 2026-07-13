/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 18:18:22 by dasimoes         ###   ########.fr       */
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
	int status, event_quantity, current_fd;
	struct epoll_event event, ready_events[MAX_EVENTS];
	std::vector<int>::iterator listenBegin, listenEnd, it;
	uint32_t event_type

	while (server->_isRunning)
	{
		event_quantity = epoll_wait(this->_epollFd, ready_events, MAX_EVENTS, -1);
		for (int j = 0; j < event_quantity; j++)
		{
			currentFd = ready_events[j].data.fd;
			eventType = ready_events[j].events;
			listenBegin = server->_listenFds.begin();
			listenEnd = server->_listenFds.end();
			it = std::find(listenBegin, listenEnd, ready_events[j].data.fd);
			if (eventType & EPOLLIN)
			{
				if (it != listenEnd)
					server->createClient(currentFd);
				else
					server->processHttpRequest();
			}
			if (eventType & EPOLLOUT)
			{
				if (it == listenEnd)
					server->processHttpResponse();

			}
			if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
			{
				if (it != listenEnd)
				{
					std::remove(server->_listenFds.begin(), server->_listenFds.end(), currentFd);
					close(currentFd);
					Server::printLog("socket fd removed from connection");
				}
				else
					server->destroyClient(currentFd);
			}
		}
	}
	server->stopServer();
}

void	Multiplexer::registerFd(int fd, std::uint32_t action)
{

}
