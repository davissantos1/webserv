/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/12 16:20:34 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

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
	if (header.empty() || this->_headers.count(header))
		return ("");
	return (this->_headers[header]);
}
