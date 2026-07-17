/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 05:48:12 by dasimoes         ###   ########.fr       */
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
		this->_status = PROCESSING_STATIC_FILE;
	if (parse.isRequestReady() && parse.hasCgi())
		this->_status = PROCESSING_CGI;
	return (0);
}

int		Client::processStaticFile()
{

}

int		Client::processCgi()
{

}

std::pair<int, uint32_t>	Client::executeMethod()
{
	int fd, status;
	std::pair<int, uint32_t> exec;
	HttpRequest& req = this->_httpRequestParser.getRequest();

	if (!req.hasCgi())
	{
		if (req.getMethod() == "GET")
		{
			fd = open(req.getUri(), O_RDONLY | O_NONBLOCK);
			if (fd == -1)
				throw (ClientException(errno));
			exec = std::make_pair(fd, EPOLLIN);
		}	
		else if (req.getMethod() == "POST")
		{
			fd = open(req.getUri(), O_WRONLY | O_CREAT | O_NONBLOCK);
			if (fd == -1)
				throw (ClientException(errno));
			exec = std::make_pair(fd, EPOLLOUT);
		}
		else if (req.getMethod() == "DELETE")
		{
			status = std::remove(req.getUri());
			if (!status)
				this->processHttpResponse(SUCCESS);
			
		}
	}
	else
	{

	}
	return (exec);
}

void	Client::sendHttpResponse()
{
	HttpRequest& req = this->_httpRequestParser.getRequest();
	HttpResponseBuilder& build = this->_httpResponseBuilder;

	if (!build.isResponseReady())
		return ;
	// add loop code to send() everything in chunks
}
