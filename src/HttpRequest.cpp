/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/19 20:52:52 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <ostream>

HttpRequest::HttpRequest(): _serverPort(0), _keepAlive(false) {}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest& other)
{
	if (this != &other)
		*this = other;
}

HttpRequest&	HttpRequest::operator=(const HttpRequest& other)
{
	if (this != &other)
	{
		this->_serverName = other._serverName;
		this->_serverPort = other._serverPort;
		this->_clientIp = other._clientIp;
		this->_filename = other._filename;
		this->_endpoint = other._endpoint;
		this->_method = other._method;
		this->_query = other._query;
		this->_uri = other._uri;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_keepAlive = other._keepAlive;
	}
	return (*this);
}

std::string HttpRequest::getHeader(std::string header)
{
	if (header.empty() || !this->_headers.count(header))
		return ("");
	return (this->_headers[header]);
}

std::string	HttpRequest::extractSessionId()
{
	size_t idStart, idEnd;
	std::string sessionId;

	if (!this->_headers.count("Cookie"))
		return ("");
	sessionId = this->_headers["Cookie"];
	idStart = sessionId.find('=');
	idEnd = sessionId.find(';');
	if (idStart == std::string::npos || idEnd == std::string::npos)
		return ("");
	return (sessionId.substr(idStart, idEnd - idStart));
}

std::string	HttpRequest::getMimeType()
{
	std::string types[] = {"html", "css", "js", "png", "jpeg"};
	std::string mime[] = {"text/html", "text/css", "text/javascript", "image/png", "image/jpeg"};
	std::string type = this->_filename;
	size_t dot = type.find('.');

	type.erase(0, dot);
	for (int i = 0; i < 5; i++)
	{
		if (types[i] == type)
			return (mime[i]);
	}
	return ("text/plain");
}

void	HttpRequest::reset()
{
	this->_serverName.clear();
	this->_serverPort = 0;
	this->_clientIp.clear();
	this->_filename.clear();
	this->_endpoint.clear();
	this->_method.clear();
	this->_query.clear();
	this->_uri.clear();
	this->_version.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_keepAlive = false;
}

std::ostream& operator<<( std::ostream& os, const HttpRequest& req )
{
	os << "========== HTTP REQUEST LOG ==========\n";
	os << "Method:     " << (req.getMethod().empty() ? "N/A" : req.getMethod()) << "\n";
	os << "URI:        " << (req.getUri().empty() ? "N/A" : req.getUri()) << "\n";
	os << "Version:    " << (req.getVersion().empty() ? "N/A" : req.getVersion()) << "\n";
	os << "Endpoint:   " << (req.getEndpoint().empty() ? "N/A" : req.getEndpoint()) << "\n";
	os << "Query:      " << (req.getQuery().empty() ? "N/A" : req.getQuery()) << "\n";
	os << "Filename:   " << (req.getFilename().empty() ? "N/A" : req.getFilename()) << "\n";
	os << "Server:     " << (req.getServerName().empty() ? "N/A" : req.getServerName()) << ":" << req.getServerPort() << "\n";
	os << "Client IP:  " << (req.getClientIp().empty() ? "N/A" : req.getClientIp()) << "\n";

	os << "\n--- Headers ---\n";
	std::map<std::string, std::string> headers = req.getHeaders();
	if (headers.empty())
	{
		os << "(No headers)\n";
	}
	else
	{
		std::map<std::string, std::string>::const_iterator it = headers.begin();
		for (; it != headers.end(); ++it)
		{
			os << it->first << ": " << it->second << "\n";
		}
	}

	os << "\n--- Body ---\n";
	// O const_cast é necessário porque getBody() na sua classe não termina com 'const'
	const std::string* bodyPtr = const_cast<HttpRequest&>(req).getBody();

	if (!bodyPtr || bodyPtr->empty())
		os << "(Empty body)\n";
	else
		os << *bodyPtr << "\n";

	os << "======================================\n";

	return os;
}
