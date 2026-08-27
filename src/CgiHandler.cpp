/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/26 20:44:30 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler(): _readDone(false), _writeDone(false), _stat_loc(0), _bytesWritten(0) {}

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
		this->_bytesWritten = other._bytesWritten;
		this->_readDone = other._readDone;
		this->_writeDone = other._writeDone;
	}
	return (*this);
}

bool	CgiHandler::processCgi(int fd, uint32_t eventType, HttpResponseBuilder& builder)
{
	HttpResponse&	res = builder.getHttpResponse();
	CgiParser&		parser = builder.getCgiParser();

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
					parser.feed(tmp, bytes);
				else
				{
					this->_readDone = true;
					break ;
				}
			}
		}
		else if (eventType & EPOLLOUT)
		{
			size_t bytesWritten = this->_bytesWritten;
			const std::string* body = builder.getHttpRequestBody();
			bytes  = write(fd, body->c_str() + bytesWritten, body->size() - bytesWritten);
			if (bytes >= 0)
			{
				if (bytes == 0)
					break ;
				this->_bytesWritten = bytesWritten + bytes;
			}
			if (this->_bytesWritten == body->size())
				this->_writeDone = true;
		}
		if (bytes < 0)
			break ;
	}
	if (WIFEXITED(this->_stat_loc) && this->_readDone && this->_writeDone)
	{
		int status = WEXITSTATUS(this->_stat_loc);
		if (status == 0)
		{
			builder.setStatusCode(200);
			builder.setHttpResponseHeaders(parser.getHeaders());
			builder.setHttpResponseBody(parser.getBody());
			res.setHeaders(parser.getHeaders());
			res.setBody(parser.getBody());
		}
		else
			builder.setStatusCode(500);
		return (true);
	}
	return (false);
}

std::pair<int, enum CgiIoType>	CgiHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fds[2];
	pid_t	pid;
	std::pair<int, enum CgiIoType> bundle;
	std::string interpreterPath = conf.getCgiInterpreterPath(req.getFilename(), req.getEndpoint());
	std::string scriptPath = conf.getFullPath(req.getFilename(), req.getEndpoint());

	if (scriptPath.empty() || interpreterPath.empty())
	{
		*statusCode = 404;
		return (bundle);
	}
	if ((pipe(fds) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundle);
	}
	this->_cgiEnvironment.startEnvironment(req);
	if (pid == 0)
	{
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
			_exit(1);
		}
		if ((close(fds[0]) < 0) || (close(fds[1]) < 0))
		{
			*statusCode = 500;
			_exit(1);
		}
		execve(interpreterPath.c_str(), const_cast<char **>(args), const_cast<char**>(&env[0]));
		*statusCode = 500;
		_exit(1);
	}
	else
	{
		if (close(fds[1]) < 0)
		{
			*statusCode = 500;
			kill(pid, SIGKILL);
			return (bundle);
		}
		
	}
	if (((waitpid(pid, &this->_stat_loc, WNOHANG)) < 0) || (*statusCode == 500))
	{
		*statusCode = 500;
		kill(pid, SIGKILL);
		return (bundle);
	}
	this->_writeDone = true;
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
	std::string interpreterPath = conf.getCgiInterpreterPath(req.getFilename(), req.getEndpoint());
	std::string scriptPath = conf.getFullPath(req.getFilename(), req.getEndpoint());

	if (scriptPath.empty() || interpreterPath.empty())
	{
		*statusCode = 404;
		return (bundles);
	}
	if ((pipe(pipe_in) < 0) || (pipe(pipe_out) < 0) || ((pid = fork()) < 0))
	{
		*statusCode = 500;
		return (bundles);
	}
	this->_cgiEnvironment.startEnvironment(req);
	if (pid == 0)
	{
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
			_exit(1);
		}
		if ((dup2(pipe_in[0], STDIN_FILENO) < 0) || (dup2(pipe_out[1], STDOUT_FILENO) < 0))
		{
			*statusCode = 500;
			_exit(1);
		}
		if ((close(pipe_in[0]) < 0) || (close(pipe_out[1]) < 0))
		{
			*statusCode = 500;
			_exit(1);
		}
		execve(interpreterPath.c_str(), const_cast<char**>(args), const_cast<char**>(&env[0]));
		*statusCode = 500;
		_exit(1);
	}
	if ((close(pipe_in[0]) < 0) || (close(pipe_out[1]) < 0))
	{
		*statusCode = 500;
		kill(pid, SIGKILL);
		return (bundles);
	}
	if (((waitpid(pid, &this->_stat_loc, WNOHANG)) < 0) || (*statusCode == 500))
	{
		*statusCode = 500;
		kill(pid, SIGKILL);
		return (bundles);
	}
	*statusCode = 200;
	bundles.push_back(std::make_pair(pipe_out[0], CGI_READ));
	bundles.push_back(std::make_pair(pipe_in[1], CGI_WRITE));
	return (bundles);
}

void	CgiHandler::reset()
{
	this->_cgiEnvironment.reset();
	this->_stat_loc = 0;
	this->_bytesWritten = 0;
	this->_readDone = false;
	this->_writeDone = false;
}
