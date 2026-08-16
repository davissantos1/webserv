/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/15 19:16:13 by dasimoes         ###   ########.fr       */
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

	this->_environment.clear();
	tmp = "SERVER_NAME=" + req.getServerName();
	this->_environment.push_back(tmp);
	ss << req.getServerPort();
	tmp = "SERVER_PORT=" + ss.str();
	this->_environment.push_back(tmp);
	tmp = "REMOTE_ADDR=" + req.getClientIp();
	this->_environment.push_back(tmp);
	tmp = "REQUEST_METHOD=" + req.getMethod();
	this->_environment.push_back(tmp);
	tmp = "QUERY_STRING=" + req.getQuery();
	this->_environment.push_back(tmp);
	tmp = "CONTENT_LENGTH=" + req.getHeader("Content-Length");
	this->_environment.push_back(tmp);
	tmp = "CONTENT_TYPE=" + req.getHeader("Content-Type");
	this->_environment.push_back(tmp);
	tmp = "SCRIPT_FILENAME=" + req.getFilename();
	this->_environment.push_back(tmp);
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
		this->_environment.push_back(env);
	}
}

std::vector<const char *>	CgiEnvironment::getEnvironment()
{
	std::vector<const char *> env;
	for (size_t i = 0; i < this->_environment.size(); i++)
		env.push_back(this->_environment[i].c_str());
	env.push_back(NULL);
	return (env);
}
