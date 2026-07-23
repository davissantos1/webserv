/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/22 21:53:40 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler()
{

}

StaticFileHandler::~StaticFileHandler()
{

}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other)
{
	if (this != &other)
		*this = other;
}

StaticFileHandler&	StaticFileHandler::operator=(const StaticFileHandler& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
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



// to be added Davi
