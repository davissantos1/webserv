/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/29 21:45:47 by davi             ###   ########.fr       */
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
	if (this != &other)
	{
		this->_fd = other._fd;
		this->_ip = other._ip;
		this->_port = other._port;
		this->_virtualHostConfig = other._virtualHostConfig;
		this->_status = other._status;
		this->_httpResponseBuilder = other._httpResponseBuilder;
		this->_httpRequestParser = other._httpRequestParser;
		this->_lastActivity = other._lastActivity;
		this->_activeFds = other._activeFds;
		this->_cgiHandler = other._cgiHandler;
		this->_staticFileHandler = other._staticFileHandler;
	}
	return (*this);
}

int	Client::processHttpRequest()
{
	enum RequestStatus status;
	char tempBuffer[8192];
	HttpRequestParser& parse = this->_httpRequestParser;

	while (true)
	{
		errno = 0;
		ssize_t bytes = recv(this->_fd, tempBuffer, sizeof(tempBuffer), 0);
		if (bytes > 0)
		{
			this->_lastActivity = std::time(NULL);
			status = parse.feed(tempBuffer, bytes);
		}
		else if (bytes == 0)
			return (-1);
		else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			return (-1);
		}
	}
	this->checkRequest(status);
	return (0);
}

void	Client::checkRequest(enum RequestStatus status)
{
	int status = 0;
	switch (status)
	{
		case REQUEST_READY:
		{
			VirtualHostConfig& conf = this->_virtualHostConfig;
			HttpRequest& req = this->_httpRequestParser.getRequest();

			if (!conf.isMethodAllowed(req.getMethod(), req.getLocation()))
			{
				this->setStatusCode(405);
				this->_status = PREPARING_RESPONSE;
				return ;
			}
			if ((status = conf.shouldRedirect(req.getLocation())) > 0)
			{
				this->setStatusCode(status);
				this->_status = PREPARING_RESPONSE;
				return ;
			}
			if (parse.hasCgi())
				this->_status = PROCESSING_CGI;
			else
				this->_status = PROCESSING_STATIC_FILE;
			break;
		}
		case REQUEST_PARSE_ERROR:
		{
			this->setStatusCode(400);
			this->_status = PREPARING_RESPONSE;
			break;
		}
		case REQUEST_TOO_LARGE:
		{
			this->setStatusCode(413);
			this->_status = PREPARING_RESPONSE;
			break;
		}
	}
}

int	Client::processHttpResponse()
{
	if (this->_status == PREPARING_RESPONSE)
	{
		this->_httpResponseBuilder.build();
		this->_status = WRITING_RESPONSE;
	}
	std::string&	responseStr = this->_httpResponseBuilder.getHttpResponse();
	ssize_t bytesSent = this->_httpResponseBuilder.getBytesSent();
	ssize_t bytesRemaining = responseStr.size() - bytesSent;
	const char* res = responseStr.c_str() + bytesSent;
	int bytes = send(this->_fd, res, bytesRemaining, 0);
	if (bytes > 0)
	{
		this->_httpResponseBuilder.setBytesSent(bytesSent + bytes);
		if (bytes == bytesRemaining)
		{
			this->_status = READING_REQUEST;
			return (0);
		}
	}
	else if (bytes == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (0);
		return (-1);
	}
	else
		return (-1);
}

void	Client::destroyCgi(int fd)
{
	this->_cgiHandler.destroyCgi(fd);
	this->_activeFds.erase(std::remove(this->_activeFds.begin(), this->_activeFds.end(), fd), this->_activeFds.end());
}

std::vector<std::pair<int, enum FdIoType> >	Client::executeMethod()
{
	int statusCode = 0;
	enum ClientStatus status = this->_status;
	std::vector<std::pair<int, enum FdIoType> > tasks;
	HttpRequest& req = this->_httpRequestParser.getRequest();
	StaticFileHandler& stat = this->_staticFileHandler;
	CgiHandler& cgi = this->_cgiHandler;

	if (req.getMethod() == "GET")
	{
		if (status == PROCESSING_STATIC_FILE)
			std::pair<int, enum FdIoType> task = stat.handleGet(req, &statusCode);
		else
			std::pair<int, enum FdIoType> task = cgi.handleGet(req, &statusCode);
		if (stat.getStatusCode() == 200)
			tasks.push_back(task);
		else
			this->handleMethodException(stat.getStatusCode());
	}
	else if (req.getMethod() == "POST")
	{
		if (status == PROCESSING_STATIC_FILE)
			std::vector<std::pair<int, enum FdIoType> > postFds = stat.handlePost();
		else
			std::vector<std::pair<int, enum FdIoType> > postFds = cgi.handlePost();
		if (stat.getStatusCode() == 200)
			tasks.insert(tasks.begin(), postFds.begin(), postFds.end());
		else
			this->handleMethodException(stat.getStatusCode());
	}
	else if (req.getMethod() == "DELETE")
			stat.handleDelete(req.getUri());
	if (statusCode > 0)
	{
		this->setStatusCode(status);
		this->_status = PREPARING_RESPONSE;
	}
	return (tasks);
}
