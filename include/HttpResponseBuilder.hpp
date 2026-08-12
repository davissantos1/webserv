/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/11 22:18:42 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEBUILDER_HPP
# define HTTPRESPONSEBUILDER_HPP

# include <string>
# include "CgiParser.hpp"
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"

class HttpResponseBuilder
{
	private:
		int				_bytesWritten;
		int				_totalBytes;
		int				_bytesSent;
		HttpResponse	_httpResponse;
		HttpRequest*	_httpRequest;
		CgiParser		_cgiParser;
		std::string		_httpResponseHead;
		
	public:
		HttpResponseBuilder();
		~HttpResponseBuilder();
		HttpResponseBuilder(const HttpResponseBuilder& other);
		HttpResponseBuilder& operator=(const HttpResponseBuilder& other);

		int				getStatusCode() { return this->_httpResponse.getStatusCode(); }
		int				getBytesWritten() { return this->_bytesWritten; }
		HttpResponse&	getHttpResponse() { return this->_httpResponse; }
		std::string*	getHttpRequestBody() { return this->_httpRequest->getBody(); }
		std::string&	getHttpResponseHead() { return this->_httpResponseHead; }
		std::string&	getHttpResponseBody() { return this->_httpResponse.getBody(); }
		int				getHttpResponseHeadSize() { return this->_httpResponseHead.size(); }

		void			setBytesWritten(int bytesWritten) { this->_bytesWritten = bytesWritten; }
		void			setStatusCode(int statusCode) { this->_httpResponse.setStatusCode(statusCode); }
		void			setHttpRequest(HttpRequest* httpRequest) { this->_httpRequest = httpRequest; };
		void			setHttpResponseBody(std::string body) { this->_httpResponse.setBody(body); }
		
		void			buildResponse();
		void			feedCgi(char *buffer, int size);
		void			feedStaticFile(char *buffer, int size);
};

#endif
