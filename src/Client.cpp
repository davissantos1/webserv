/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/15 17:35:47 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::Client(std::string ip, uint16_t port, int fd): _ip(ip), _port(port), _fd(fd) {}

Client::~Client()
{}

Client::Client(const Client& other)
{
	if (this != &other)
		*this = other;
}

Client&	Client::operator=(const Client& other)
{
	if (this != other)
	{
		this->_fd = other._fd;
		this->_ip = other._ip;
		this->_port = other._port;
		this->_status = other._status;
		this->_request = other._request;
		this->_response = other._response;
		this->_responseBuilder = other._responseBuilder;
		this->_requestParser = other._requestParser;
		this->_lastActivity = other._lastActivity;
	}
	return (*this);
}

int	Client::processHttpRequest(int fd)
{
	char tempBuffer[8192];

	while (true)
	{
		ernno = 0;
		ssize_t bytes = recv(fd, tempBuffer, sizeof(tempBuffer));
		if (bytes > 0)
			this->_httpRequestParser.feed(tempBuffer);
		else if (bytes == 0)
			return (-1);
		else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break
			throw (ClientException(errno));
		}
	}
	return (0);
}

void	Client::processHttpResponse(int fd)
{

}
