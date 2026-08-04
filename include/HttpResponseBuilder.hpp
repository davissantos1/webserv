/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/03 17:35:44 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEBUILDER_HPP
# define HTTPRESPONSEBUILDER_HPP

# include <string>

class HttpResponseBuilder
{
	private:
		int				_statusCode;
		int				_httpBodySize;
		int				_writtenBytes;
		HttpResponse	_response;
		std::string		_httpResponse;
		std::string		_httpBody;
		
	public:
		HttpResponseBuilder();
		~HttpResponseBuilder();
		HttpResponseBuilder(const HttpResponseBuilder& other);
		HttpResponseBuilder& operator=(const HttpResponseBuilder& other);

		int				getStatusCode() { return this->_statusCode; }
		HttpResponse&	getResponse() { return this->_response; }
		std::string&	getHttpResponse() { return this->_httpResponse; }

		void			setStatusCode(int statusCode) { this->_statusCode = statusCode; }
		
		void	buildResponse();
		void	feed(char *buffer, int size);
		void	startBuilder(std::string httpBody, int httpBodySize);
		void	fillResponseBody(std::string body);
};

#endif
