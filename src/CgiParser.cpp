/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/28 05:12:12 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_utils.hpp"
#include "CgiParser.hpp"

CgiParser::CgiParser() 
{
	this->_status = BUILDING_HEADERS;
}

CgiParser::~CgiParser() {}

CgiParser::CgiParser(const CgiParser& other)
{
	if (this != &other)
		*this = other;
}

CgiParser&	CgiParser::operator=(const CgiParser& other)
{
	if (this != &other)
	{
		this->_status = other._status;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_buffer = other._buffer;
	}
	return (*this);
}

bool	CgiParser::isCgiDone()
{
	if (this->_status == READY)
		return (true);
	return (false);
}

void	CgiParser::feed(char *buffer, int size)
{
	size_t found, breakPoint;
	this->_buffer.append(buffer, size);

	if (this->_status == BUILDING_BODY)
	{
		this->_body += this->_buffer;
		this->_buffer.clear();
		return ;
	}

	breakPoint = this->_buffer.find("\r\n\r\n");
	if (breakPoint == std::string::npos && this->_status == BUILDING_HEADERS)
		return;

	while (true)
	{
		found = this->_buffer.find("\r\n");

		if (found == breakPoint)
		{
			std::string headerStr = this->_buffer.substr(0, found);
			std::vector<std::string> header = split(headerStr, ':');
			trimStr(header[0]);
			trimStr(header[1]);
			this->_headers[header[0]] = header[1];
			this->_buffer.erase(0, breakPoint + 4);
			this->_status = BUILDING_BODY;
			break;
		}
		if (this->_buffer.substr(0, 2) == "\r\n")
		{
			this->_buffer.erase(0, 2);
			this->_status = BUILDING_BODY;
			break;
		}
		std::string headerStr = this->_buffer.substr(0, found);
		std::vector<std::string> header = split(headerStr, ':');
		trimStr(header[0]);
		trimStr(header[1]);
		this->_headers[header[0]] = header[1];
		this->_buffer.erase(0, breakPoint + 2);
	}
	if (this->_status == BUILDING_BODY)
	{
		this->_body += this->_buffer;
		this->_buffer.clear();
	}
}

void	CgiParser::reset()
{
	this->_status = BUILDING_HEADERS;
	this->_headers.clear();
	this->_body.clear();
	this->_buffer.clear();
}
