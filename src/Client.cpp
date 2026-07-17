/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 16:11:37 by dasimoes         ###   ########.fr       */
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

bool	Client::processStaticFile(int fd, uint32_t eventType)
{
	ssize_t bytes;
	char tempBuffer[8192];
	HttpRequest& req = this->_httpRequestParser.getRequest();
	StaticFileHandler&	stat = this->_staticFileHandler;
	HttpRequestBuilder& builder = this->_httpRequestBuilder;

	while (true)
	{
		ernno = 0;
		if (eventType & EPOLLIN)
		{
			bytes = recv(fd, tempBuffer, sizeof(tempBuffer), 0);
			if (bytes > 0)
			{
				this->_lastActivity = std::time(NULL);
				stat.feed(tempBuffer);
			}
			else if (bytes == 0)
			{
				builder.buildResponse(stat.getStatusCode(), stat.getBody());
				this->_status = PREPARING_RESPONSE;
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				return (false);
			}
		}
		else
		{
			bytes = send(fd, req.getBody(), req.getContentSize(), 0);
			if (bytes > 0)
			{
				this->_lastActivity = std::time(NULL);
				stat.feed(tempBuffer);
			}
			else if (bytes == req.getContentSize())
			{
				builder.buildResponse(stat.getStatusCode(), stat.getBody());
				this->_status = PREPARING_RESPONSE;
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				return (false);
			}
		}
	}
	return (true);
}

bool	Client::processCgi(int fd, uint32_t eventType)
{
	ssize_t bytes;
	char tempBuffer[8192];
	HttpRequest& req = this->_httpRequestParser.getRequest();
	CgiHandler&	cgi = this->_cgiHandler;
	HttpRequestBuilder& builder = this->_httpRequestBuilder;

	while (true)
	{
		ernno = 0;
		if (eventType & EPOLLIN)
		{
			bytes = recv(fd, tempBuffer, sizeof(tempBuffer), 0);
			if (bytes > 0)
			{
				this->_lastActivity = std::time(NULL);
				cgi.feed(tempBuffer);
			}
			else if (bytes == 0)
			{
				builder.buildResponse(stat.getStatusCode(), stat.getBody());
				this->_status = PREPARING_RESPONSE;
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				return (false);
			}
		}
		else
		{
			bytes = send(fd, req.getBody(), req.getContentSize(), 0);
			if (bytes > 0)
			{
				this->_lastActivity = std::time(NULL);
				cgi.feed(tempBuffer);
			}
			else if (bytes == req.getContentSize())
			{
				builder.buildResponse(cgi.getStatusCode(), cgi.getBody());
				this->_status = PREPARING_RESPONSE;
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				return (false);
			}
		}
	}
	return (true);
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

void	Client::sendHttpResponse()
{
	HttpRequest& req = this->_httpRequestParser.getRequest();
	HttpResponseBuilder& build = this->_httpResponseBuilder;

	if (!build.isResponseReady())
		return ;
	// add loop code to send() everything in chunks
}
