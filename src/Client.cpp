/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/16 06:07:05 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(std::string ip, uint16_t port, int fd, VirtualHostConfig config):
 _ip(ip), _port(port), _fd(fd), _config(config) {}

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

int	Client::processHttpRequest()
{
	char tempBuffer[8192];
	HttpRequestParser& parse = this->_httpRequestParser;

	while (true)
	{
		ernno = 0;
		ssize_t bytes = recv(fd, tempBuffer, sizeof(tempBuffer));
		if (bytes > 0)
		{
			this->_lastActivity = std::time(NULL);
			parse.feed(tempBuffer);
		}
		else if (bytes == 0)
			return (-1);
		else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break
			throw (ClientException(errno));
		}
	}
	if (parse.isRequestReady() && !parse.hasCgi())
		this->_status = PREPARING_RESPONSE;
	if (parse.isRequestReady() && parse.hasCgi())
		this->_status = EXECUTING_CGI;
	return (0);
}

void	Client::processHttpResponse()
{
	HttpRequest& req = this->_httpRequestParser.getRequest();
	HttpResponseBuilder& build = this->_httpResponseBuilder;

	
}
