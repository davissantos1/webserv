/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 13:12:36 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler(const CgiHandler& other)
{
	if (this != &other)
		*this = other;
}

CgiHandler&	CgiHandler::operator=(const CgiHandler& other)
{
	if (this != other)
	{
		this->_cgiEnvironment = other._cgiEnvironment;
		this->_stat_loc = other._stat_loc;
	}
	return (*this);
}

bool	CgiHandler::processCgi(int fd, uint32_t eventType, HttpResponseBuilder& builder)
{
	while (true)
	{
		errno = 0;
		int bytes = 0;
		if (eventType & EPOLLIN)
		{
			char tmp[8192];
			bytes = read(fd, tmp, 8192); 	
			if (bytes >= 0)
			{
				if (bytes > 0)
					builder.feed(tmp, bytes);
				else
					return (true);
			}
		}
		else if (eventType & EPOLLOUT)
		{
			int bytesWritten = builder.getBytesWritten();
			std::string body = builder.getHttpBody();
			bytes  = write(fd, body.c_str() + bytesWritten, body.size() - bytesWritten);
			if (bytes >= 0)
			{
				if (bytes == 0)
				{
					builder.setStatusCode(200);
					return (true);
				}
				builder.setBytesWritten(bytesWritten + bytes);
			}
		}
		if (bytes < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			if (errno == EACCES || errno == EPERM)
				builder.setStatusCode(403);
			else
				builder.setStatusCode(500);
			return (true);
		}
	}
	return (false);
}

std::pair<int, enum FdIoType>	CgiHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fds[2];
	pid_t	pid;
	std::pair<int, enum FdIoType> bundle;

	if ((pipe(fds) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundle);
	}
	this->_cgiEnvironment.startEnviroment(req);
	if (pid == 0)
	{
		std::string interpreterPath = conf.getCgiInterpreterPath(req.getUri());
		std::string scriptPath = conf.getCgiScriptPath(req.getUri());
		const char** env = this->_cgiEnvironment.getEnvironment();
		const char*	args[] = 
		{
			interpreterPath.c_str(),
			scriptPath.c_str(),
			NULL
		};
		if (fcntl(fds[1], F_DUPFD, 1) < 0);
		{
			*statusCode = 500;
			break;
		}
		if ((close(fds[0]) < 0) || (close(fds[1] < 0));
		{
			*statusCode = 500;
			break;
		}
		execve(path.c_str(), args, env);
		*statusCode = 500;
		return (bundle);
	}
	else
	{
		if (close(fds[1]) < 0)
			*statusCode = 500;
	}
	if (((waitpid(pid, &this->_stat_loc, WNOHANG)) < 0) || (*statusCode == 500))
	{
		*statusCode = 500;
		kill(pid, SIGTERM)
		return (bundle);
	}
	*statusCode = 200;
	bundle = std::make_pair(fds[0], CGI_READ);
	return (bundle);
}

std::vector<std::pair<int, enum FdIoType> >	CgiHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fds[2];
	pid_t	pid;
	std::vector<std::pair<int, enum FdIoType> > bundles;

	if ((pipe(fds) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundle);
	}
	this->_cgiEnvironment.startEnviroment(req);
	if (pid == 0)
	{
		std::string interpreterPath = conf.getCgiInterpreterPath(req.getUri());
		std::string scriptPath = conf.getCgiScriptPath(req.getUri());
		const char** env = this->_cgiEnvironment.getEnvironment();
		const char*	args[] = 
		{
			interpreterPath.c_str(),
			scriptPath.c_str(),
			NULL
		};
		if ((fcntl(fds[1], F_DUPFD, 1) < 0) || (fcntl(fds[0], F_DUPFD, 0) < 0))
		{
			*statusCode = 500;
			break;
		}
		if ((close(fds[0]) < 0) || (close(fds[1] < 0)))
		{
			*statusCode = 500;
			break;
		}
		execve(path.c_str(), args, env);
		*statusCode = 500;
		return (bundles);
	}
	if (((waitpid(pid, &this->_stat_loc, WNOHANG)) < 0) || (*statusCode == 500))
	{
		*statusCode = 500;
		kill(pid, SIGTERM)
		return (bundles);
	}
	*statusCode = 200;
	bundles.push_back(std::make_pair(fds[0], CGI_READ));
	bundles.push_back(std::make_pair(fds[1], CGI_WRITE));
	return (bundles);
}
