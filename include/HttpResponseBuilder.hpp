/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 12:46:44 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEBUILDER_HPP
# define HTTPRESPONSEBUILDER_HPP

# include <string>

class HttpResponse;
class HttpRequest;

class HttpResponseBuilder
{
	private:
		int				_bytesWritten;
		HttpResponse	_httpResponse;
		HttpResponse*	_httpRequest;
		CgiParser		_cgiParser;
		
	public:
		HttpResponseBuilder();
		~HttpResponseBuilder();
		HttpResponseBuilder(const HttpResponseBuilder& other);
		HttpResponseBuilder& operator=(const HttpResponseBuilder& other);

		int				getStatusCode() { return this->_httpResponse.getStatusCode(); }
		int				getBytesWritten() { return this->_bytesWritten; }
		HttpResponse&	getHttpResponse() { return this->_httpResponse; }
		std::string&	getHttpBody() { return this->_httpRequest.getHttpBody(); }

		void			setBytesWritten(int bytesWritten) { this->_bytesWritten = bytesWritten; }
		void			setStatusCode(int statusCode) { this->_httpResponse->setStatusCode(statusCode); }
		void			setHttpRequest(HttpRequest* httpRequest) { this->_httpRequest = httpRequest; };
		
		void			buildResponse(); // to be done
		void			feed(char *buffer, int size); // to be done
		void			fillResponseBody(std::string body); // to be done
};

#endif
