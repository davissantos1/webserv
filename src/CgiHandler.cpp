/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/05 13:43:36 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler()
{

}

CgiHandler::~CgiHandler()
{

}

CgiHandler::CgiHandler(const CgiHandler& other)
{
	if (this != &other)
		*this = other;
}

CgiHandler&	CgiHandler::operator=(const CgiHandler& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

std::pair<int, enum FdIoType>				CgiHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fds[2];
	pid_t	pid;
	std::pair<int, enum FdIoType> bundle;

	

}

std::vector<std::pair<int, enum FdIoType> >	CgiHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{

}

