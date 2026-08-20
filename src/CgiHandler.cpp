/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/19 22:43:40 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler(): _stat_loc(0) {}

CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler(const CgiHandler& other)
{
	if (this != &other)
		*this = other;
}

CgiHandler&	CgiHandler::operator=(const CgiHandler& other)
{
	if (this != &other)
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
		int bytes = 0;
		if (eventType & EPOLLIN)
		{
			char tmp[8192];
			bytes = read(fd, tmp, 8192); 	
			if (bytes >= 0)
			{
				if (bytes > 0)
					builder.feedCgi(tmp, bytes);
				else
					return (true);
			}
		}
		else if (eventType & EPOLLOUT)
		{
			int bytesWritten = builder.getBytesWritten();
			const std::string* body = builder.getHttpRequestBody();
			bytes  = write(fd, body->c_str() + bytesWritten, body->size() - bytesWritten);
			if (bytes >= 0)
			{
				if (bytes == 0)
				{
					CgiParser& parser = builder.getCgiParser();
					builder.setStatusCode(200);
					builder.setHttpResponseHeaders(parser.getHeaders());
					builder.setHttpResponseBody(parser.getBody());
					parser.finish();
					return (true);
				}
				builder.setBytesWritten(bytesWritten + bytes);
			}
		}
		if (bytes < 0)
			return (true);
	}
	return (false);
}

std::pair<int, enum CgiIoType>	CgiHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fds[2];
	pid_t	pid;
	std::pair<int, enum CgiIoType> bundle;

	if ((pipe(fds) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundle);
	}
	this->_cgiEnvironment.startEnvironment(req);
	if (pid == 0)
	{
		std::string interpreterPath = conf.getCgiInterpreterPath(req.getFilename(), req.getEndpoint());
		std::string scriptPath = conf.getFullPath(req.getFilename(), req.getEndpoint());
		std::vector<const char *>env = this->_cgiEnvironment.getEnvironment();
		const char*	args[] = 
		{
			interpreterPath.c_str(),
			scriptPath.c_str(),
			NULL
		};
		if (dup2(fds[1], STDOUT_FILENO) < 0)
		{
			*statusCode = 500;
			return (bundle);
		}
		if ((close(fds[0]) < 0) || (close(fds[1]) < 0))
		{
			*statusCode = 500;
			return (bundle);
		}
		execve(interpreterPath.c_str(), const_cast<char **>(args), const_cast<char**>(&env[0]));
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
		kill(pid, SIGTERM);
		return (bundle);
	}
	*statusCode = 200;
	bundle = std::make_pair(fds[0], CGI_READ);
	return (bundle);
}

std::vector<std::pair<int, enum CgiIoType> >	CgiHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	pid_t	pid;
	int pipe_in[2];
	int pipe_out[2];
	std::vector<std::pair<int, enum CgiIoType> > bundles;

	if ((pipe(pipe_in) < 0) || (pipe(pipe_out) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundles);
	}
	this->_cgiEnvironment.startEnvironment(req);
	if (pid == 0)
	{
		std::string interpreterPath = conf.getCgiInterpreterPath(req.getFilename(), req.getEndpoint());
		std::string scriptPath = conf.getFullPath(req.getFilename(), req.getEndpoint());
		std::vector<const char *>env = this->_cgiEnvironment.getEnvironment();
		const char*	args[] = 
		{
			interpreterPath.c_str(),
			scriptPath.c_str(),
			NULL
		};
		if ((close(pipe_in[1]) < 0) || (close(pipe_out[0]) < 0))
		{
			*statusCode = 500;
			return (bundles);
		}
		if ((dup2(pipe_in[0], STDIN_FILENO) < 0) || (dup2(pipe_out[1], STDOUT_FILENO) < 0))
		{
			*statusCode = 500;
			return (bundles);
		}
		if ((close(pipe_in[0]) < 0) || (close(pipe_out[1]) < 0))
		{
			*statusCode = 500;
			return (bundles);
		}
		execve(interpreterPath.c_str(), const_cast<char**>(args), const_cast<char**>(&env[0]));
		*statusCode = 500;
		return (bundles);
	}
	if ((close(pipe_in[0]) < 0) || (close(pipe_out[1]) < 0))
	{
		*statusCode = 500;
		return (bundles);
	}
	if (((waitpid(pid, &this->_stat_loc, WNOHANG)) < 0) || (*statusCode == 500))
	{
		*statusCode = 500;
		kill(pid, SIGTERM);
		return (bundles);
	}
	*statusCode = 200;
	bundles.push_back(std::make_pair(pipe_out[0], CGI_READ));
	bundles.push_back(std::make_pair(pipe_in[1], CGI_WRITE));
	return (bundles);
}
