/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/13 16:17:45 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder() {}

HttpResponseBuilder::~HttpResponseBuilder() {}

HttpResponseBuilder::HttpResponseBuilder(const HttpResponseBuilder& other)
{
	if (this != &other)
		*this = other;
}

HttpResponseBuilder&	HttpResponseBuilder::operator=(const HttpResponseBuilder& other)
{
	if (this != other)
	{
		this->_bytesWritten = other._bytesWritten;
		this->_totalBytes = other._totalBytes;
		this->_bytesSent = other._bytesSent;
		this->_httpResponse = other._httpResponse;
		this->_httpRequest = other._httpRequest;
		this->_cgiParser = other._cgiParser;
		this->_httpResponseHead = other._httpResponseHead;
	}
	return (*this);
}

void	HttpResponseBuilder::buildResponse()
{
	std::stringstream ss;
	HttpResponse& res = this->_httpResponse;
	std::map<std::string, std::string>&	headers = this->_httpResponse.getHeaders();
	std::map<std::string, std::string>::iterator it = header.begin();

	ss	<< res.getVersion()	<< " "
		<< res.getStatusCode() << " "
		<< res.getReasonPhrase() << "\r\n";
	for (; it != headers.end(); it++)
	{
		ss	<< it.first << ": "
			<< it.second << "\r\n";
	}
	ss	<< "\r\n";
	this->_httpResponseHead = ss.str();
	this->_totalBytes = this->_httpResponseHead.size() + res.getBodySize();
}

void	HttpResponseBuilder::buildHeaders()
{
	char buffer[100];
	std::stringstream ss;
	time_t	now = std::time(NULL);
	HttpResponse& rep = this->_httpResponse;
	HttpResponse* req = this->_httpRequest;
	int	statusCode = rep.getStatusCode();
	std::string path = req->getEndpoint() + req->getFilename();

	ss << rep.getBodySize();
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
	this->addHeader("Server", "Webserv/1.0");
	this->addHeader("Date", buffer);
	
	if ((statusCode > 299 && statusCode < 400) || statusCode == 201)
		this->addHeader("Location", path);
	if (statusCode == 200)
		this->addHeader("Content-Type", req->getMimeType());
	else
		this->addHeader("Content-Type", "text/html");
	this->addHeader("Content-Length", ss.str());
	this->addHeader("Connection", "keep-alive");
}

void	HttpResponseBuilder::feedCgi(char *buffer, int size)
{
	if (!(this->_cgiParser.isCgiDone()))
		this->_cgiParser.feed(buffer, size);
	else
	{
		HttpResponse& res = this->_httpResponse;
		res.setHeaders(this->_cgiParser.getHeaders());
		res.setBody(this->_cgiParser.getBody());
	}
}

void	HttpResponseBuilder::feedStaticFile(char *buffer, int size)
{
	std::string stringBuffer;
	for (int i = 0; i < size; i++)
		stringBuffer += buffer[i];
	this->_httpResponse.feedBody(stringBuffer);
}

void	HttpResponseBuilder::addHeader(std::string key, std::string value)
{
	std::map<std::string, std::string>& headers = this->_httpResponse.getHeaders();
	
	if (!headers.count(key))
		headers[key] = value;
}

