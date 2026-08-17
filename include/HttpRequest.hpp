/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/17 15:35:09 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <cstddef>
# include <string>
# include <stdint.h>
# include <map>

class HttpRequest
{
	private:
		std::string							_serverName;
		uint16_t							_serverPort;
		std::string							_clientIp;
		std::string							_filename;
		std::string							_endpoint;
		std::string 						_method;
		std::string							_query;
		std::string 						_uri;
		std::string 						_version;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
		bool								_keepAlive;
	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		const std::string&	getMethod( void ) const { return this->_method; }
		const std::string&	getVersion( void ) const { return this->_version; }
		const std::string&	getUri( void ) const { return this->_uri; }
		const std::string*	getBody( void ) { return &(this->_body); }
		const std::string&	getQuery( void ) const { return this->_query; }
		const std::string&	getServerName( void ) const { return this->_serverName; }
		uint16_t			getServerPort( void ) const { return this->_serverPort; }
		const std::string&	getClientIp( void ) const { return this->_clientIp; }
		std::size_t			getBodySize( void ) const { return this->_body.size(); }
		const std::string&	getFilename( void ) const { return this->_filename; }
		const std::string&	getEndpoint( void ) const { return this->_endpoint; }
		const std::map<std::string, std::string> getHeaders( void ) const { return this->_headers; }

		void	setMethod(std::string method) { this->_method = method; }
		void	setUri(std::string uri) { this->_uri = uri; }
		void	setVersion(std::string version) { this->_version = version; }
		void	setHeaders(std::map<std::string, std::string> headers) { this->_headers = headers; }
		void	setBody(std::string body) { this->_body = body; }
		void	setServerName(std::string serverName) { this->_serverName = serverName; }
		void	setServerPort(uint16_t serverPort) { this->_serverPort = serverPort; }
		void	setClientIp(std::string clientIp) { this->_clientIp = clientIp; }
		void	setKeepAlive(bool keepAlive) { this->_keepAlive = keepAlive; }
		void	setQuery(std::string query) { this->_query = query; }
		void	setFilename(std::string filename) { this->_filename = filename; }
		void	setEndpoint(std::string endpoint) { this->_endpoint = endpoint; }

		void	addHeader( const std::string& key, const std::string& value ) { _headers[key] = value; }
		void	appendBody( const std::string& str, const size_t len ) { _body.append(str, len); }
		void	appendBody( const char *str, const size_t len ) { _body.append(str, len); }

		std::string			getMimeType();
		std::string 		getHeader(std::string header);
		std::string			extractSessionId();
		void				reset();
};

std::ostream& operator<<( std::ostream& os, const HttpRequest& req );

#endif
