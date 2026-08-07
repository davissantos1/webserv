/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 16:08:59 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiEnvironment.hpp"

CgiEnvironment::CgiEnvironment() {}

CgiEnvironment::~CgiEnvironment() {}

CgiEnvironment::CgiEnvironment(const CgiEnvironment& other)
{
	if (this != &other)
		*this = other;
}

CgiEnvironment&	CgiEnvironment::operator=(const CgiEnvironment& other)
{
	if (this != &other)
		this->_environment = other._environment;
	return (*this);
}

void	CgiEnvironment::startEnvironment(HttpRequest& req)
{
	std::map<std::string, std::string> headers = req.getHeaders();
	std::map<std::string, std::string>::iterator it = headers.begin();
	this->_environment.push_back("SERVER_NAME=" + req.getServerName());
	this->_environment.push_back("SERVER_PORT=" + req.getServerPort());
	this->_environment.push_back("REMOTE_ADDR=" + req.getClientIp());
	this->_environment.push_back("REQUEST_METHOD=" + req.getMethod());
	this->_environment.push_back("QUERY_STRING=" + req.getQueryString());
	this->_environment.push_back("CONTENT_LENGTH=" + req.getHeader("Content-Length"));
	this->_environment.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
	this->_environment.push_back("SCRIPT_FILENAME=" + req.getMethod());
	this->_environment.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->_environment.push_back("REDIRECT_STATUS=200");

	for (; it != headers.end(); i++)
	{
		std::string key = it->first;
		std::string value = it->second;
		std::string env = "HTTP_";

		if (key == "Content-type" || key == "Content-Length")
			continue;
		for (int i = 0; i < key.size(); i++)
		{
			if (key[i] == "-")
				env += "_";
			else
				env += std::toupper(key[i]);
		}
		this->_environment.push_back(env + "=" + value);
	}
	this->_environment.push_back(NULL);
}
