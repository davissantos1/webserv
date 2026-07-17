/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 01:09:41 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <ctime>
#include <string>
#include <iostream>

class HttpRequest;
class HttpResponse;

enum ClientStatus;
{
	READING_REQUEST,
	PROCESSING_CGI,
	PROCESSING_STATIC_FILE,
	PREPARING_RESPONSE,
	WRITING_RESPONSE,
	DISCONNECT
};

class Client
{
	private:
		std::string			_ip;
		uint16_t			_port;
		int					_fd;
		CgiHandler			_cgi;
		enum ClientStatus	_status;
		VirtualHostConfig	_config;
		HttpRequestParser	_requestParser;
		HttpResponseBuilder	_requestBuilder;
		time_t				_lastActivity;
	public:
		Client();
		~Client();
		Client(const Client& other);
		Client(std::string ip, uint16_t port, int fd);
		Client&	operator=(const Client& other);
		void	processHttpRequest();
		void	processHttpResponse();

		void	methodGET();
		void	methodPOST();
		void	methodDELETE();

		void	getFd() { return this->_fd; }
		void	getStatus() { return this->_status; }
		void	getLastActivity() { return this->_lastActivity; }
		void	setStatus(enum ClientStatus status) { this->_status = status; }

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
