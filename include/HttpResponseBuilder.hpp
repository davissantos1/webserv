/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/15 01:01:28 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSEBUILDER_HPP
# define HTTPRESPONSEBUILDER_HPP

# include "CgiParser.hpp"
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include <string>
# include <ctime>

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

		int					getStatusCode() { return this->_httpResponse.getStatusCode(); }
		int					getBytesWritten() { return this->_bytesWritten; }
		CgiParser&			getCgiParser() { return this->_cgiParser; }
		HttpResponse&		getHttpResponse() { return this->_httpResponse; }
		std::string&		getHttpResponseHead() { return this->_httpResponseHead; }
		std::string&		getHttpResponseBody() { return this->_httpResponse.getBody(); }
		int					getHttpResponseHeadSize() { return this->_httpResponseHead.size(); }
		const std::string*	getHttpRequestBody() { return this->_httpRequest->getBody(); }
		int					getBytesSent() { return this->_bytesSent; }
		int					getTotalBytes() { return this->_totalBytes; }

		void				setBytesSent(int bytesSent) { this->_bytesSent = bytesSent; }
		void				setTotalBytes(int totalBytes) { this->_totalBytes = totalBytes; }
		void				setBytesWritten(int bytesWritten) { this->_bytesWritten = bytesWritten; }
		void				setStatusCode(int statusCode) { this->_httpResponse.setStatusCode(statusCode); }
		void				setHttpRequest(HttpRequest* httpRequest) { this->_httpRequest = httpRequest; };
		void				setHttpResponseBody(std::string body) { this->_httpResponse.setBody(body); }
		void				setHttpResponseHeaders(std::map<std::string, std::string> headers) { this->_httpResponse.setHeaders(headers); }
		
		void				buildHeaders();
		void				buildResponse();
		void				feedCgi(char *buffer, int size);
		void				feedStaticFile(char *buffer, int size);
		void				addHeader(std::string key, std::string value);
		void				cleanHttpResponse() { this->_httpResponse.reset(); }
};

#endif
