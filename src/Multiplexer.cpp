/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/16 06:20:46 by dasimoes         ###   ########.fr       */
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

std::vector<std::pair<int, uint32_t>>	Multiplexer::wait()
{
	int eventQuantity, currentFd;
	struct epoll_event event, readyEvents[MAX_EVENTS];
	std::vector<std::pair<int, uint32_t>>	fds;
	uint32_t	eventType;

	eventQuantity = epoll_wait(this->_epollFd, readyEvents, MAX_EVENTS, EPOLL_WAIT);
	for (int j = 0; j < eventQuantity; j++)
	{
		currentFd = readyEvents[j].data.fd;
		eventType = readyEvents[j].events;
		std::pair<int, uint32_t> fdPair(currentFd, eventType);
		fds.push_back(fdPair);
	}
	return (fds);
}

void	Multiplexer::addFd(int fd, uint32_t event)
{
	int		status;
	struct	epoll_event ev;

	errno = 0;
	ev.data.fd = fd;
	ev.events = event;
	status = epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, fd, &ev);
	if (status < 0)
		throw (MultiplexerException(errno));
}

void	Multiplexer::removeFd(int fd)
{
	int		status;

	status = epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, fd, NULL);
	if (status < 0)
		throw (MultiplexerException(errno));
}

void	Multiplexer::modifyFd(int fd, uint32_t event)
{
	errno = 0;
	ev.data.fd = fd;
	ev.events = event;
	status = epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, fd, &ev);
	if (status < 0)
		throw (MultiplexerException(errno));
}
