/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/30 20:09:43 by davi             ###   ########.fr       */
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

bool	StaticFileHandler::processStaticFile(int fd, uint32_t eventType, HttpRequestBuilder& builder)
{
	ssize_t bytes;
	char tempBuffer[8192];

	while (true)
	{
		ernno = 0;
		if (eventType & EPOLLIN)
		{
			bytes = read(fd, tempBuffer, sizeof(tempBuffer));
			if (bytes > 0)
				builder.feed(tempBuffer, bytes);
			else if (bytes == 0)
			{
				builder.setStatusCode(200);
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				builder.setStatusCode(500);
				return (true);
			}
		}
		else
		{
			int bytesWritten = builder.getBytesWritten();
			std::string body = builder.getBody();
			bytes = write(fd, body.c_str() + bytesWritten, body.size() - bytesWritten);
			if (bytes > 0)
				builder.settBytesWritten(bytes + bytesWritten);
			else if (bytes == body.size())
			{
				builder.setStatusCode(200);
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				builder.setStatusCode(500);
				return (true);
			}
		}
	}
	return (false);
}

std::pair<int, enum FdIoType>	StaticFileHandler::handleGet(HttpRequest& req, int* statusCode)
{
	int fd, status;
	std::pair<int, enum FdIoType> fdBundle;

	fd = open();
	if ((status = fcntl()) == -1)
	{
		*statusCode = 500;
		return (fdBundle);
	}
}

std::vector<<std::pair<int, enum FdIoType> >	StaticFileHandler::handlePost(HttpRequest& req, int* statusCode)
{

}

void	StaticFileHandler::handleDelete(HttpRequest& req, int* statusCode)
{

}

bool	StaticFileHandler::checkPermissions()
{

}
