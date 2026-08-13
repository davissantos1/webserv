/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/13 00:15:40 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

Session::Session()
{
	std::stringstream ss;

	ss << "sid_" << std::time(NULL) << "_" << std::rand();
	this->_sessionId = ss.str();
}

Session::~Session() {}

Session::Session(const Session& other)
{
	if (this != &other)
		*this = other;
}

Session&	Session::operator=(const Session& other)
{
	if (this != other)
	{
		this->_sessionId = other._sessionId;
		this->_lastActivity = other._lastActivity;
		this->_cookies = other._cookies;
	}
	return (*this);
}

void	Session::extractCookies(std::string cookies)
{
	std::string key, value;
	size_t position, cookieStart, cookieEnd;

	position = 0;
	while (true)
	{
		cookieStart = cookies.find('=');
		cookieEnd = cookies.find(';');
		if (cookieEnd == std::string::npos)
			break;
		key = cookies.substr(position, cookieStart - position);
		value = cookies.substr(cookieStart, cookieStart - cookieEnd);
		position = cookieEnd + 1;
		this->_cookies[key] = value;
	}
	key = cookies.substr(position, cookieStart - position);
	value = cookies.substr(cookieStart);
	this->_cookies[key] = value;
}
