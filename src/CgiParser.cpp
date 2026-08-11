/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/10 14:20:24 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	if (this != other)
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
	for (int i = 0; i < size; i++)
		this->_buffer += buffer[i];
	if (this->_status == BUILDING_HEADERS)
	{
		size_t found = this->_buffer.find("\r\n");
		size_t next_index = found + 2;

		if ((found == std::string::npos) || (next_index == this->_buffer.size()))
			return ;
		if (this->_buffer[next_index] != '\r')
		{
			std::vector<std::string> strs = split(this->_buffer, ':');
			this->_headers[strs[0]] = trimStr(strs[1]);
			this->buffer.erase(0, next_index);
		}
		found = this->_buffer.find("\r\n");
		if (found == 0)
		{
			std::strinstream ss(this->_headers["Content-Size"]);
			this->_status = BUILDING_BODY;
			this->_buffer.erase(0, 2);
			ss >> this->_bodySize;
		}
	}
	else if (this->_status == BUILDING_BODY)
	{
		if (this->_bodySize == this->body.size())
			this->_status = READY;
		this->_body += this->_buffer;
	}
}

void	CgiParser::resetParser()
{
	this->_bodySize = 0;
	this->_status = BUILDING_HEADERS;
	this->_headers.clear();
	this->_body.clear();
	this->_buffer.clear();
}
