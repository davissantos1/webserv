/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/06 15:09:47 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(std::string ip, uint16_t port, int fd, std::vector<VirtualHostConfig>& configs):
 _ip(ip), _port(port), _fd(fd), _configs(configs) {}

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
		this->_configs = other._configs;
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

void	Client::checkRequest(enum RequestStatus status, std::vector<VirtualHostConfig>& configs)
{
	int status = 0;
	switch (status)
	{
		case REQUEST_READY:
		{
			HttpRequest& req = this->_httpRequestParser.getHttpRequest();
			this->_virtualHostConfig = this->getCurrentConfig(req.getHeader("Host"));
			VirtualHostConfig& conf = this->_virtualHostConfig;

			if (!conf.isMethodAllowed(req.getMethod(), req.getUri()))
			{
				this->setStatusCode(405);
				this->_status = PROCESSING_EXCEPTION;
				return ;
			}
			if ((status = conf.shouldRedirect(req.getLocation())) > 0)
			{
				this->setStatusCode(status);
				this->_status = PREPARING_RESPONSE;
				return ;
			}
			if (parse.hasCgi() && req.getMethod() != "DELETE")
				this->_status = PROCESSING_CGI;
			else
				this->_status = PROCESSING_STATIC_FILE;
			this->setStatusCode(200);
			this->_httpResponseBuilder.setHttpRequest(&req);
			break;
		}
		case REQUEST_PARSE_ERROR:
		{
			this->setStatusCode(400);
			this->_status = PROCESSING_EXCEPTION;
			break;
		}
		case REQUEST_TOO_LARGE:
		{
			this->setStatusCode(413);
			this->_status = PROCESSING_EXCEPTION;
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
	int statusCode = this->getStatusCode();
	enum ClientStatus status = this->_status;
	std::vector<std::pair<int, enum FdIoType> > tasks;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	VirtualHostConfig& conf = this->_virtualHostConfig;
	StaticFileHandler& stat = this->_staticFileHandler;
	CgiHandler& cgi = this->_cgiHandler;

	if ( statusCode != 200)
		return (stat->handleException(statusCode, conf.getErrorPage(statusCode)));
	if (this->_virtualHostConfig.shouldIndex(req.getUri()))
		this->handleIndex();
	statusCode = 0;
	if (req.getMethod() == "GET")
	{
		if (status == PROCESSING_STATIC_FILE)
			std::pair<int, enum FdIoType> task = stat.handleGet(req, conf, &statusCode);
		else
			std::pair<int, enum FdIoType> task = cgi.handleGet(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.push_back(task);
	}
	else if (req.getMethod() == "POST")
	{
		if (status == PROCESSING_STATIC_FILE)
			std::vector<std::pair<int, enum FdIoType> > postFds = stat.handlePost(req, conf, &statusCode);
		else
			std::vector<std::pair<int, enum FdIoType> > postFds = cgi.handlePost(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.insert(tasks.begin(), postFds.begin(), postFds.end());
	}
	else if (req.getMethod() == "DELETE" && status == PROCESSING_STATIC_FILE)
		stat.handleDelete(req, conf, &statusCode);
	if (statusCode > 0)
		this->setStatusCode(status);
	if (statusCode == -1)
	{
		statusCode = stat->handleAutoindex(this->_httpResponseBuilder);
		if (statusCode == -1)
			this->_status = PREPARING_RESPONSE;
	}
	if (statusCode != 200 && statusCode != -1)
		return (stat->handleException(statusCode, conf.getErrorPage(statusCode)));
	return (tasks);
}	

void	Client::destroyActiveFds()
{
	for (int i = 0; i < this->_activeFds.size(); i++)
	{
		if (this->_activeFds[i] > 2)
			close(this->_activeFds[i]);
	}
	this->_activeFds.clear();
}

VirtualHostConfig	Client::getCurrentConfig(std::string host)
{
	for (int i = 0; i < this->_configs.size(); i++)
	{
		std::vector<std::string> serverNames = this->_configs[i].getServerNames();
		for (int j = 0; j < serverNames.size(); j++)
		{
			if (serverNames[j] == host)
				return (serverNames[j]);	
		}
	}
	return (serverNames[0]);
}

void	Client::handleIndex()
{
	int status;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	std::vector<std::string> index = this->_virtualHostConfig.getIndex();
	std::string basePath = this->_virtualHostConfig.getFullPath(req.getUri());
	
	for (int i = 0; i < index.size(); i++)
	{
		std::string path = basePath + index[i];
		status = access(path.c_str(), F_OK);
		if (status == 0)
		{
			std::string uri = index[i];
			if (uri[0] != "/")
				uri = "/" + uri;
			req.setUri(uri);
			if (this->_httpRequestParser.hasCgi())
				this->_status = PROCESSING_CGI;
			else
				this->_status = PROCESSING_STATIC_FILE;
		}
	}
}
