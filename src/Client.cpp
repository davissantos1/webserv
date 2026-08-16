/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/15 01:05:58 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(std::string ip, uint16_t port, int fd, std::vector<VirtualHostConfig>* configs):
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

enum ClientStatus	Client::processHttpRequest()
{
	enum ClientStatus	clientStatus;
	enum RequestStatus	requestStatus;
	char tempBuffer[8192];
	HttpRequestParser& parse = this->_httpRequestParser;

	while (true)
	{
		errno = 0;
		ssize_t bytes = recv(this->_fd, tempBuffer, sizeof(tempBuffer), 0);
		if (bytes > 0)
		{
			this->_lastActivity = std::time(NULL);
			requestStatus = parse.feed(tempBuffer, bytes);
		}
		else if (bytes == 0)
			return (DISCONNECT);
		else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			return (DISCONNECT);
		}
	}
	clientStatus = this->checkRequest(requestStatus);
	return (clientStatus);
}

enum ClientStatus	Client::checkRequest(enum RequestStatus status)
{
	switch (status)
	{
		case DONE:
		{
			HttpRequestParser& parse = this->_httpRequestParser;
			HttpRequest& req = this->_httpRequestParser.getHttpRequest();
			this->_virtualHostConfig = this->getCurrentConfig(req.getHeader("Host"));
			VirtualHostConfig& conf = this->_virtualHostConfig;
			req.setServerPort(this->_port);
			req.setClientIp(this->_ip);

			if (!conf.isMethodAllowed(req.getMethod(), req.getEndpoint()))
			{
				this->setStatusCode(405);
				this->_status = PROCESSING_EXCEPTION;
				return (PROCESSING_EXCEPTION);
			}
			if (conf.shouldRedirect(req.getEndpoint()))
			{
				std::pair<int, std::string> ret = conf.getReturn(req.getEndpoint());
				this->setStatusCode(ret.first);
				this->_status = PREPARING_RESPONSE;
				return (PREPARING_RESPONSE);
			}
			this->setStatusCode(200);
			this->_httpResponseBuilder.setHttpRequest(&req);
			if (parse.hasCgi() && req.getMethod() != "DELETE")
			{
				this->_status = PROCESSING_CGI;
				return (PROCESSING_CGI);
			}
			else
			{
				this->_status = PROCESSING_STATIC_FILE;
				return (PROCESSING_STATIC_FILE);
			}
		}
		case ERROR_BAD_REQUEST:
		{
			this->setStatusCode(400);
			this->_status = PROCESSING_EXCEPTION;
			return (PROCESSING_EXCEPTION);
		}
		case ERROR_REQUEST_TOO_LARGE:
		{
			this->setStatusCode(413);
			this->_status = PROCESSING_EXCEPTION;
			return (PROCESSING_EXCEPTION);
		}
		default:
			return (this->_status);
	}
}

enum ClientStatus	Client::processHttpResponse()
{
	const char*	res;
	int bytes, bytesSent, bytesRemaining, headSize;
	HttpResponseBuilder& build = this->_httpResponseBuilder;

	if (this->_status == PREPARING_RESPONSE)
	{
		this->_httpResponseBuilder.buildHeaders();
		this->_httpResponseBuilder.buildResponse();
		this->_httpRequestParser.cleanHttpRequest();
		this->_status = WRITING_RESPONSE;
	}
	while (true)
	{
		errno = 0;
		bytesSent = build.getBytesSent();
		headSize = build.getHttpResponseHeadSize();
		bytesRemaining = build.getTotalBytes() - bytesSent;
		std::string& responseStr = (bytesSent < headSize) ? build.getHttpResponseHead() : build.getHttpResponseBody();
		res = responseStr.c_str() + bytesSent;
		bytes = send(this->_fd, res, bytesRemaining, 0);
		if (bytes >= 0)
		{
			this->_httpResponseBuilder.setBytesSent(bytesSent + bytes);
			if (bytes == bytesRemaining)
			{
				this->_status = SENT_RESPONSE;
				this->_httpResponseBuilder.cleanHttpResponse();
				return (SENT_RESPONSE);
			}
		}
		else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			return (DISCONNECT);
		}
	}
	return (WRITING_RESPONSE);
}

void	Client::destroyCgi(int fd)
{
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
	std::pair<int, enum FdIoType> task;
	std::vector<std::pair<int, enum FdIoType> > postFds;

	if (statusCode != 200)
		return (stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint())));
	if (this->_virtualHostConfig.shouldIndex(req.getUri()))
		this->handleIndex();
	statusCode = 0;
	if (req.getMethod() == "GET")
	{
		if (status == PROCESSING_STATIC_FILE)
			task = stat.handleGet(req, conf, &statusCode);
		else
			task = cgi.handleGet(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.push_back(task);
	}
	else if (req.getMethod() == "POST")
	{
		if (status == PROCESSING_STATIC_FILE)
		{
			if (req.getBodySize() == 0)
				statusCode = 400;
			else
				postFds = stat.handlePost(req, conf, &statusCode);
		}
		else
			postFds = cgi.handlePost(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.insert(tasks.begin(), postFds.begin(), postFds.end());
	}
	else if (req.getMethod() == "DELETE" && status == PROCESSING_STATIC_FILE)
		stat.handleDelete(req, conf, &statusCode);
	if (statusCode > 0)
		this->setStatusCode(status);
	if (statusCode == -1)
	{
		statusCode = stat.handleAutoindex(req, conf, this->_httpResponseBuilder);
		if (statusCode == -1)
			this->_status = PREPARING_RESPONSE;
	}
	if (statusCode != 200 && statusCode != -1)
		return (stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint())));
	return (tasks);
}

void	Client::destroyActiveFds()
{
	for (size_t i = 0; i < this->_activeFds.size(); i++)
	{
		if (this->_activeFds[i] > 2)
			close(this->_activeFds[i]);
	}
	this->_activeFds.clear();
}

VirtualHostConfig	Client::getCurrentConfig(std::string host)
{
	const std::vector<std::string>& firstServerNames = (*this->_configs)[0].getServerNames();
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	for (size_t i = 0; i < this->_configs->size(); i++)
	{
		const std::vector<std::string>& serverNames = (*this->_configs)[i].getServerNames();
		for (size_t j = 0; j < serverNames.size(); j++)
		{
			if (serverNames[j] == host)
			{
				req.setServerName(serverNames[j]);
				return ((*this->_configs)[i]);
			}
		}
	}
	req.setServerName(firstServerNames[0]);
	return ((*this->_configs)[0]);
}

void	Client::handleIndex()
{
	int status;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	const std::vector<std::string> index = this->_virtualHostConfig.getIndex();
	const std::string basePath = this->_virtualHostConfig.getFullPath(req.getFilename(), req.getEndpoint());

	for (size_t i = 0; i < index.size(); i++)
	{
		std::string path = basePath + index[i];
		status = access(path.c_str(), F_OK);
		if (status == 0)
		{
			std::string uri = index[i];
			if (uri[0] != '/')
				uri = "/" + uri;
			req.setUri(uri);
			if (this->_httpRequestParser.hasCgi())
				this->_status = PROCESSING_CGI;
			else
				this->_status = PROCESSING_STATIC_FILE;
		}
	}
}

std::string	Client::findSessionId()
{
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	if (!this->_session)
	{
		std::string sessionId = req.extractSessionId();
		if (sessionId.empty())
			return ("");
		return (sessionId);
	}
	return (this->_session->getSessionId());
}
