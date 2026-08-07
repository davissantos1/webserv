/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 17:30:46 by davi             ###   ########.fr       */
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

	}
	else if (this->_status == BUILDING_BODY)
	{

	}
	else
	{

	}
}

void	CgiParser::resetParser()
{
	this->_status = BUILDING_HEADERS;
	this->_headers.clear();
	this->_body.clear();
	this->_buffer.clear();
}
