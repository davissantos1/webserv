/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/11 19:51:48 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>
# include <map>

class HttpResponse
{
	private:
		int									_statusCode;
		std::string							_version;
		std::string							_reasonPhrase;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);

		int	getStatusCode() { return this->_statusCode }
		std::string	getVersion() { return this->_version }
		std::string	getReasonPhrase() { return this->_reasonPhrase }
		std::map<std::string, std::string> getHeaders() { return this->_headers }
		std::string	getBody() { return this->_body }
		int	getBodySize() { return this->_body.size() }

		void	setVersion(std::string version) { this->_body = body; }
		void	setReasonPhrase(std::string body) { this->_body = body; }
		void	setHeaders(std::string body) { this->_body = body; }
		void	setBody(std::string body) { this->_body = body; }

		void	feedBody(std::string data) { this->_body += data; }
};

#endif
