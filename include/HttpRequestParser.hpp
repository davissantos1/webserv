/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/05 18:10:49 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

// deal with the connection and the transfer-encoding header
// This parser will be fed a bit at a time and have an internal state 
// I need to know specific states from feed to send an error if the request is malformed or filled with errors
// I'm assuming an enum for each state, tell me if you change it, please
// This project is draining my soul 

# include <string>

class HttpRequest;

// To be added Vitor
class HttpRequestParser
{
	private:
		enum RequestStatus	_requestStatus;
		HttpRequest			_httpRequest;
	public:
		HttpRequestParser();
		~HttpRequestParser();
		HttpRequestParser(const HttpRequestParser& other);
		HttpRequestParser& operator=(const HttpRequestParser& other);

		enum RequestStatus	getRequestStatus() { return this->_requestStatus; }
		HttpRequest&		getHttpRequest() { return this->_httpRequest; }
		HttpRequest*		getHttpRequestPointer() { return &this->_httpRequest; }

		enum RequestStatus	feed(char* buffer, int size); // to be done
		bool				hasCgi(); // to be done
		
};

#endif
