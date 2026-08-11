/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/10 19:07:08 by davi             ###   ########.fr       */
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
		void			setStatusCode(int statusCode) { this->_httpResponse->setStatusCode(statusCode); }
		void			setHttpRequest(HttpRequest* httpRequest) { this->_httpRequest = httpRequest; };
		void			setHttpResponseBody(std::string body) { this->_httpResponse.setBody(body); }
		
		void			buildResponse(); // to be done
		void			feedCgi(char *buffer, int size); // to be done
		void			feedStaticFile(char *buffer, int size); // to be done
};

#endif
