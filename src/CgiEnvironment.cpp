/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 20:11:15 by dasimoes         ###   ########.fr       */
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
	std::string tmp;
	std::stringstream ss;
	std::map<std::string, std::string> headers = req.getHeaders();
	std::map<std::string, std::string>::iterator it = headers.begin();

	tmp = "SERVER_NAME=" + req.getServerName();
	this->_environment.push_back(tmp.c_str());
	ss << req.getServerPort();
	tmp = "SERVER_PORT=" + ss.str();
	this->_environment.push_back(tmp.c_str());
	tmp = "REMOTE_ADDR=" + req.getClientIp();
	this->_environment.push_back(tmp.c_str());
	tmp = "REQUEST_METHOD=" + req.getMethod();
	this->_environment.push_back(tmp.c_str());
	tmp = "QUERY_STRING=" + req.getQuery();
	this->_environment.push_back(tmp.c_str());
	tmp = "CONTENT_LENGTH=" + req.getHeader("Content-Length");
	this->_environment.push_back(tmp.c_str());
	tmp = "CONTENT_TYPE=" + req.getHeader("Content-Type");
	this->_environment.push_back(tmp.c_str());
	tmp = "SCRIPT_FILENAME=" + req.getFilename();
	this->_environment.push_back(tmp.c_str());
	this->_environment.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->_environment.push_back("REDIRECT_STATUS=200");

	for (; it != headers.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;
		std::string tmp = "HTTP_";

		if (key == "Content-type" || key == "Content-Length")
			continue;
		for (size_t i = 0; i < key.size(); i++)
		{
			if (key[i] == '-')
				tmp += '_';
			else
				tmp += std::toupper(key[i]);
		}
		std::string env = tmp + "=" + value;
		this->_environment.push_back(env.c_str());
	}
	this->_environment.push_back(NULL);
}
