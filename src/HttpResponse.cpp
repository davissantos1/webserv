/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/19 21:39:09 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(): _statusCode(0), _version("HTTP/1.1") {}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	if (this != &other)
		*this = other;
}

HttpResponse&	HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		this->_statusCode = other._statusCode;
		this->_version = other._version;
		this->_reasonPhrase = other._reasonPhrase;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return (*this);
}

void	HttpResponse::setReasonPhrase(int statusCode)
{
	this->_statusCode = statusCode;
	switch (statusCode)
	{
		case 200:
			this->_reasonPhrase = "OK";
			break;
		case 201:
			this->_reasonPhrase = "Created";
			break;
		case 204:
			this->_reasonPhrase = "No content";
			break;
		case 301:
			this->_reasonPhrase = "Moved Permanently";
			break;
		case 302:
			this->_reasonPhrase = "Found";
			break;
		case 307:
			this->_reasonPhrase = "Temporary Redirect";
			break;
		case 308:
			this->_reasonPhrase = "Permanent Redirect";
			break;
		case 400:
			this->_reasonPhrase = "Bad Request";
			break;
		case 403:
			this->_reasonPhrase = "Forbidden";
			break;
		case 404:
			this->_reasonPhrase = "Not Found";
			break;
		case 405:
			this->_reasonPhrase = "Method Not Allowed";
			break;
		case 413:
			this->_reasonPhrase = "Payload Too Large";
			break;
		case 500:
			this->_reasonPhrase = "Internal Server Error";
			break;
	}
}

void	HttpResponse::reset()
{
	this->_statusCode = 0;
	this->_version.clear();
	this->_reasonPhrase.clear();
	this->_headers.clear();
	this->_body.clear();
}
