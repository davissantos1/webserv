/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 00:30:39 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/29 18:27:09 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <algorithm>
#include "color.hpp"
#include "Client.hpp"
#include "Server.hpp"

Client::Client() {}

Client::~Client() {}

Client::Client(std::string ip, uint16_t port, int fd, std::vector<VirtualHostConfig>* configs):
 _ip(ip), _port(port), _fd(fd), _configs(configs), _keepAlive(false), _redirect(false)
{
	std::stringstream ss;

	ss << fd;
	this->_status = READING_REQUEST;
	this->_session = NULL;
	this->_lastActivity = std::time(NULL);
	Server::printLog("TCP connection " + color::blue + "created" + color::reset + " for FD " + ss.str());
}

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
		this->_keepAlive = other._keepAlive;
		this->_redirect = other._redirect;
	}
	return (*this);
}

enum ClientStatus	Client::processHttpRequest()
{
	enum ClientStatus	clientStatus;
	char tempBuffer[8192];
	HttpRequestParser& parse = this->_httpRequestParser;
	enum RequestStatus	requestStatus = PARSING_REQUEST_LINE;

	this->updateActivity();
	while (true)
	{
		ssize_t bytes = recv(this->_fd, tempBuffer, sizeof(tempBuffer), 0);
		if (bytes > 0)
		{
			this->_lastActivity = std::time(NULL);
			requestStatus = parse.feed(tempBuffer, bytes);
		}
		else if (bytes == 0)
			return (DISCONNECT);
		else
			break ;
	}
	clientStatus = this->checkRequest(requestStatus);
	return (clientStatus);
}

enum ClientStatus	Client::checkRequest(enum RequestStatus status)
{
	HttpRequestParser& parse = this->_httpRequestParser;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	VirtualHostConfig& conf = this->_virtualHostConfig;

	if (parse.getHeadersDone())
	{
		this->_httpResponseBuilder.reset();
		this->_keepAlive = req.getKeepAlive();
		this->_virtualHostConfig = this->getCurrentConfig(req.getHeader("Host"));
		conf = this->_virtualHostConfig;
		parse.splitRequestUri(conf);
		parse.setMaxBodySize(conf.getMaxBodySize(req.getEndpoint()));
		this->_redirect = conf.shouldRedirect(req.getEndpoint());
		status = parse.feed("", 0);
	}
	switch (status)
	{
		case DONE:
		{
			req.setServerPort(this->_port);
			req.setClientIp(this->_ip);
			this->_httpResponseBuilder.setHttpRequest(&req);
			if (!conf.isMethodAllowed(req.getMethod(), req.getEndpoint()))
			{
				this->setStatusCode(405);
				this->_status = PROCESSING_EXCEPTION;
				return (PROCESSING_EXCEPTION);
			}
			if (this->_redirect)
			{
				std::pair<int, std::string> ret = conf.getReturn(req.getEndpoint());
				this->setStatusCode(ret.first);
				this->_status = PREPARING_RESPONSE;
				return (PREPARING_RESPONSE);
			}
			this->setStatusCode(200);
			if (parse.hasCgi(this->_virtualHostConfig) && req.getMethod() != "DELETE")
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
			this->_keepAlive = false;
			req.setKeepAlive(false);
			this->_httpResponseBuilder.setHttpRequest(&req);
			this->_status = PROCESSING_EXCEPTION;
			return (PROCESSING_EXCEPTION);
		}
		case ERROR_REQUEST_TOO_LARGE:
		{
			this->setStatusCode(413);
			this->_keepAlive = false;
			req.setKeepAlive(false);
			this->_httpResponseBuilder.setHttpRequest(&req);
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
	int bytes, bytesSent, bytesRemaining, headSize, offset;
	HttpResponseBuilder& build = this->_httpResponseBuilder;
	HttpResponse&		rep = build.getHttpResponse();

	this->updateActivity();
	if (this->_status == PREPARING_RESPONSE)
	{
		rep.setReasonPhrase(rep.getStatusCode());
		this->_httpResponseBuilder.buildHeaders(this->_virtualHostConfig);
		this->_httpResponseBuilder.buildResponse();
		this->_httpRequestParser.reset();
		this->_cgiHandler.reset();
		this->_status = WRITING_RESPONSE;
	}
	while (true)
	{
		bytesSent = build.getBytesSent();
		headSize = build.getHttpResponseHeadSize();
		std::string& responseStr = (bytesSent < headSize) ? build.getHttpResponseHead() : build.getHttpResponseBody();
		offset = (bytesSent < headSize) ? bytesSent : (bytesSent - headSize);
		res = responseStr.c_str() + offset;
		bytesRemaining = (bytesSent < headSize) ? responseStr.size() - bytesSent: responseStr.size() - bytesSent + headSize;
		bytes = send(this->_fd, res, bytesRemaining, 0);
		if (bytes > 0)
		{
			this->_httpResponseBuilder.setBytesSent(bytesSent + bytes);
			if (build.getBytesSent() == build.getTotalBytes())
			{
				this->_status = SENT_RESPONSE;
				this->_httpResponseBuilder.reset();
				return (SENT_RESPONSE);
			}
		}
		else
			break;
	}
	return (WRITING_RESPONSE);
}

void	Client::destroyCgi(int fd)
{
	this->_activeFds.erase(std::remove(this->_activeFds.begin(), this->_activeFds.end(), fd), this->_activeFds.end());
}

void	Client::executeStaticFileMethod()
{
	int statusCode = this->getStatusCode();
	VirtualHostConfig& conf = this->_virtualHostConfig;
	StaticFileHandler& stat = this->_staticFileHandler;
	HttpResponseBuilder& build = this->_httpResponseBuilder;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	
	this->updateActivity();
	if (statusCode != 200)
	{
		if(!stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build))
			build.setHardFallback(true);
		this->_status = PREPARING_RESPONSE;
		return ;
	}
	if (conf.shouldIndex(req.getFilename(), req.getEndpoint()))
		this->handleIndex();
	statusCode = 0;
	if (req.getMethod() == "GET" || req.getMethod() == "HEAD")
		stat.handleGet(req, conf, build);
	else if (req.getMethod() == "POST")
	{
		if (req.getBodySize() == 0)
			statusCode = 400;
		else
			stat.handlePost(req, conf, build);
	}
	else if (req.getMethod() == "DELETE")
		stat.handleDelete(req, conf, build);
	statusCode = this->getStatusCode();
	if (statusCode > 299)
	{
		if(!stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build))
			build.setHardFallback(true);
	}
	this->_status = PREPARING_RESPONSE;
}

std::vector<std::pair<int, enum CgiIoType> >	Client::executeCgiMethod()
{
	CgiHandler& cgi = this->_cgiHandler;
	int statusCode = this->getStatusCode();
	std::vector<std::pair<int, enum CgiIoType> > tasks;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	VirtualHostConfig& conf = this->_virtualHostConfig;
	StaticFileHandler& stat = this->_staticFileHandler;
	HttpResponseBuilder& build = this->_httpResponseBuilder;
	std::pair<int, enum CgiIoType> task;
	std::vector<std::pair<int, enum CgiIoType> > postFds;

	this->updateActivity();
	if (statusCode > 299)
	{
		if(!stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build))
			build.setHardFallback(true);
		return (tasks);
	}
	if (conf.shouldIndex(req.getFilename(), req.getEndpoint()))
		this->handleIndex();
	statusCode = 0;
	if (req.getMethod() == "GET" || req.getMethod() == "HEAD")
	{
		task = cgi.handleGet(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.push_back(task);
	}
	else if (req.getMethod() == "POST")
	{
		postFds = cgi.handlePost(req, conf, &statusCode);
		if (statusCode == 200)
			tasks.insert(tasks.begin(), postFds.begin(), postFds.end());
	}
	if (statusCode > 0)
		this->setStatusCode(statusCode);
	if (statusCode > 299)
	{
		if(!stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build))
			build.setHardFallback(true);
		this->_status = PREPARING_RESPONSE;
	}
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
	if (firstServerNames.empty())
		req.setServerName("localhost");
	else
		req.setServerName(firstServerNames[0]);
	return ((*this->_configs)[0]);
}

void	Client::handleIndex()
{
	int status;
	std::string path, uri;
	HttpRequest& req = this->_httpRequestParser.getHttpRequest();
	const std::vector<std::string> index = this->_virtualHostConfig.getIndex(req.getEndpoint());
	std::string basePath = this->_virtualHostConfig.getFullPath(req.getFilename(), req.getEndpoint());
	std::string base = req.getEndpoint() + req.getFilename();

	uri = "";
	if (base.size() > 1 && base[base.size() - 1] == '/')
		base.erase(base.size() - 1);
	if (basePath[basePath.size() -1] != '/')
		basePath += "/";
	for (size_t i = 0; i < index.size(); i++)
	{
		path = basePath + index[i];
		status = access(path.c_str(), F_OK);
		if (status == 0)
		{
			uri = index[i];
			if (uri[0] != '/')
				uri = "/" + uri;
			if (base != "/")
				uri = base + uri;
			req.setUri(uri);
			this->_httpRequestParser.splitRequestUri(this->_virtualHostConfig);
			break ;
		}
	}
	if (uri.empty())
	{
		uri = index[0];
		if (uri[0] != '/')
			uri = "/" + uri;
		if (base != "/")
			uri = base + uri;
		req.setUri(uri);
		this->_httpRequestParser.splitRequestUri(this->_virtualHostConfig);
	}
	if (this->_httpRequestParser.hasCgi(this->_virtualHostConfig))
		this->_status = PROCESSING_CGI;
	else
		this->_status = PROCESSING_STATIC_FILE;
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

void	Client::updateActivity()
{
	Session* session = this->getSession();
	this->setLastActivity(std::time(NULL));
	if (session)
		session->setLastActivity(std::time(NULL));
}

void	Client::registerFd(int fd)
{ 
	if (std::find(_activeFds.begin(), _activeFds.end(), fd) != _activeFds.end())
		return;

	this->_activeFds.push_back(fd); 
}

void	Client::reset()
{
	this->destroyActiveFds();
	this->_status = READING_REQUEST;
	this->_httpRequestParser.reset();
	this->_httpResponseBuilder.reset();
	this->_staticFileHandler.reset();
	this->_cgiHandler.reset();
	this->_session = NULL;
	this->_lastActivity = std::time(NULL);
	this->_keepAlive = false;
	this->_redirect = false;
	this->_httpResponseBuilder.getCgiParser().reset();
}
