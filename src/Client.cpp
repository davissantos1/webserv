/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/22 21:58:11 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(std::string ip, uint16_t port, int fd, VirtualHostConfig config):
 _ip(ip), _port(port), _fd(fd), _virtualHostConfig(config) {}

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
		this->_config = other._config;
		this->_status = other._status;
		this->_responseBuilder = other._responseBuilder;
		this->_requestParser = other._requestParser;
		this->_lastActivity = other._lastActivity;
		this->_activeFds = other._activeFds;
		this->_cgiHandler = other._cgiHandler;
		this->_staticFileHandler = other._staticFileHandler;
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
		ssize_t bytes = recv(fd, tempBuffer, sizeof(tempBuffer), NULL);
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
			return (-1);
		}
	}
	if (parse.isRequestReady() && !parse.hasCgi())
		this->_status = PROCESSING_STATIC_FILE;
	if (parse.isRequestReady() && parse.hasCgi())
		this->_status = PROCESSING_CGI;
	return (0);
}

int	Client::processHttpResponse()
{
	//to be implemented
}

void	Client::destroyCgi(int fd)
{
	this->_cgiHandler.destroyCgi(fd);
	this->_activeFds.erase(std::remove(this->_activeFds.begin(). this->_activeFds.end(), fd). this->_activeFds.end());
}

std::vector<FdTask>	Client::executeMethod(enum ClientStatus status)
{
	std::vector<FdTask> tasks;
	HttpRequest& req = this->_httpRequestParser.getRequest();
	StaticFileHandler& stat = this->_staticFileHandler;
	CgiHandler& cgi = this->_cgiHandler;

	if (req.getMethod() == "GET")
	{
		if (status == PROCESSING_STATIC_FILE)
			tasks.push_back(stat.handleGet(req.getUri()));
		else
			tasks.push_back(cgi.handleGet(req.getUri()));
	}
	else if (req.getMethod() == "POST")
	{
		if (status == PROCESSING_STATIC_FILE)
			tasks.push_back(stat.handlePost(req.getUri()));
		else
		{
			std::vector<FdTask> postFds = cgi.handlePost();
			tasks.insert(tasks.begin(), postFds.begin(), postFds.end());
		}
	}
	else if (req.getMethod() == "DELETE")
	{
		if (status == PROCESSING_STATIC_FILE)
			stat.handleDelete(req.getUri());
		else
			tasks.push_back(cgi.handleDelete(req.getUri()));
	}
	return (tasks);
}
