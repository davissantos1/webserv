/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/12 23:52:03 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "Session.hpp"
# include "CgiHandler.hpp"
# include "StaticFileHandler.hpp"
# include "HttpResponseBuilder.hpp"
# include "HttpRequestParser.hpp"
# include "VirtualHostConfig.hpp"
# include <sys/types.h>
# include <sys/socket.h>
# include <utility>
# include <ctime>
# include <cstring>
# include <cerrno>
# include <stdint.h>
# include <vector>
# include <string>
# include <iostream>
# include <algorithm>

class HttpRequest;

enum ClientStatus
{
	READING_REQUEST,
	PROCESSING_CGI,
	PROCESSING_EXCEPTION,
	PROCESSING_STATIC_FILE,
	PREPARING_RESPONSE,
	WRITING_RESPONSE,
	SENT_RESPONSE,
	DISCONNECT
};

class Client
{
	private:
		std::string						_ip;
		uint16_t						_port;
		int								_fd;
		enum ClientStatus				_status;
		VirtualHostConfig				_virtualHostConfig;
		HttpRequestParser				_httpRequestParser;
		HttpResponseBuilder				_httpResponseBuilder;
		StaticFileHandler				_staticFileHandler;
		CgiHandler						_cgiHandler;
		Session*						_session;
		time_t							_lastActivity;
		std::vector<int>				_activeFds;
		std::vector<VirtualHostConfig>*	_configs;
	public:
		Client();
		~Client();
		Client(const Client& other);
		Client(std::string ip, uint16_t port, int fd, std::vector<VirtualHostConfig>* configs);
		Client&	operator=(const Client& other);
		void				destroyCgi(int fd);
		enum ClientStatus	processHttpRequest();
		enum ClientStatus	processHttpResponse();
		enum ClientStatus	checkRequest(enum RequestStatus status);
		std::vector<std::pair<int, enum FdIoType> > executeMethod();

		int						getFd() { return this->_fd; }
		time_t					getLastActivity() { return this->_lastActivity; }
		uint16_t				getPort() { return this->_port; }
		enum ClientStatus		getStatus() { return this->_status; }
		std::vector<int>		getActiveFds() { return this->_activeFds; }
		HttpRequestParser&		getHttpRequestParser() { return this->_httpRequestParser; }
		HttpResponseBuilder&	getHttpResponseBuilder() { return this->_httpResponseBuilder; }
		StaticFileHandler&		getStaticFileHandler() { return this->_staticFileHandler; }
		CgiHandler&				getCgiHandler() { return this->_cgiHandler; }
		int						getStatusCode() { return this->_httpResponseBuilder.getStatusCode(); }
		VirtualHostConfig		getCurrentConfig(std::string host);
		Session*				getSession() { return this->_session; }
		HttpRequest&			getHttpRequest() { return this->_httpRequestParser.getHttpRequest(); }

		void	setSession(Session* session) { this->_session = session; }
		void	setStatus(enum ClientStatus status) { this->_status = status; }
		void	setStatusCode(int code) { this->_httpResponseBuilder.setStatusCode(code); }
		void	setLastActivity(time_t time) { this->_lastActivity = time; }

		std::string		findSessionId();
		void			destroyActiveFds();
		void			handleIndex();
		void			registerFd(int fd) { this->_activeFds.push_back(fd); }

		class	ClientException: public std::exception
		{
			private:
				int	_errno;
			public:
				ClientException(int err): _errno(err) {}
				virtual ~ClientException() throw() {}
				const char* what() const throw() { return std::strerror(_errno); }
		};
};

#endif
