/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 06:51:16 by dasimoes         ###   ########.fr       */
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

struct FdTask
{
	int fd;
	enum FdIoType type;
};

struct 

class Client
{
	private:
		std::string			_ip;
		uint16_t			_port;
		int					_fd;
		enum ClientStatus	_status;
		VirtualHostConfig	_config;
		HttpRequestParser	_requestParser;
		HttpResponseBuilder	_requestBuilder;
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
		void	processHttpRequest();
		void	processStaticFile(); // to be implemented
		void	processCgi(); // to be implemented
		void	destroyCgi(); // to be implemented
		std::vector<FdTasks> executeMethod(); // to be implemented

		void	getFd() { return this->_fd; }
		void	getStatus() { return this->_status; }
		void	getLastActivity() { return this->_lastActivity; }
		void	getActiveFds() { return this->_activeFds; }
		void	getPort() { return this->_port; }
		void	getIp() { return this->_ip; }

		void	setStatus(enum ClientStatus status) { this->_status = status; }
		void	setStatusCode(int code) { this->_requestBuilder.setStatusCode(code); }

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
