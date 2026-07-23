/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/22 21:54:49 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ctime>
#include <string>
#include <iostream>

class HttpRequest;
class HttpResponse;
class VirtualHostConfig;
class CgiHandler;
class StaticFileHandler;

enum ClientStatus;
{
	READING_REQUEST,
	PROCESSING_CGI,
	PROCESSING_STATIC_FILE,
	PREPARING_RESPONSE,
	WRITING_RESPONSE,
	DISCONNECT
};

enum FdIoType
{
	STATIC_FILE_READ,
	STATIC_FILE_WRITE,
	CGI_READ,
	CGI_WRITE
};

class Client
{
	private:
		std::string			_ip;
		uint16_t			_port;
		int					_fd;
		enum ClientStatus	_status;
		VirtualHostConfig	_virtualHostConfig;
		HttpRequestParser	_httpRequestParser;
		HttpResponseBuilder	_httpResponseBuilder;
		StaticFileHandler	_staticFileHandler;
		CgiHandler			_cgiHandler;
		std::vector<int>	_activeFds;
		time_t				_lastActivity;
	public:
		Client();
		~Client();
		Client(const Client& other);
		Client(std::string ip, uint16_t port, int fd);
		Client&	operator=(const Client& other);
		int		processHttpRequest();
		int		processHttpResponse();
		void	destroyCgi(int fd);
		std::vector<std::pair<int, enum FdIoType> executeMethod(enum ClientStatus status);

		int						getFd() { return this->_fd; }
		time_t					getLastActivity() { return this->_lastActivity; }
		uint16_t				getPort() { return this->_port; }
		enum ClientStatus		getStatus() { return this->_status; }
		std::vector<int>		getActiveFds() { return this->_activeFds; }
		HttpRequestParser&		getHttpRequestParser() { return this->_httpRequestParser; }
		HttpResponseBuilder&	getHttpResponseBuilder() { return this->_httpResponseBuilder; }
		StaticFileHandler&		getStaticFileHandler() { return this->_staticFileHandler; }
		CgiHandler&				getCgiHandler() { return this->_cgiHandler; }

		void	setStatus(enum ClientStatus status) { this->_status = status; }
		void	setStatusCode(int code) { this->_requestBuilder.setStatusCode(code); }

		void	registerFd(int fd) { this->_activeFds.push_back(fd); }

		class	ClientException: public std::exception
		{
			private:
				int	_errno;
			public:
				ClientException(int err): _errno(err) {}
				const char* what() const throw { return std::strerror(_errno); }
		};	
};

#endif
