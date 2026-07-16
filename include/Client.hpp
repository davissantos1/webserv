/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/15 17:11:57 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"
#include <string>
#include <iostream>

class HttpRequest;
class HttpResponse;

enum ClientStatus;
{
	READING_REQUEST,
	EXECUTING_CGI,
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
		enum ClientStatus	_status;
		HttpRequestParser	_requestParser;
		HttpResponseBuilder	_requestBuilder;
		time_t				_lastActivity;
	public:
		Client();
		~Client();
		Client(const Client& other);
		Client(std::string ip, uint16_t port, int fd);
		Client&	operator=(const Client& other);
		void	processHttpRequest(int fd);
		void	processHttpResponse(int fd);
		class	ClientException: std::exception
		{
			private:
				int	_errno;
			public:
				ClientException(int err): _errno(err) {}
				const char* what() const throw { return std::strerror(_errno); }
		};	
};

#endif
