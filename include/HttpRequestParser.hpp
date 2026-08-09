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

#include "HttpRequest.hpp"
#include <cstddef>

enum RequestStatus {
	PARSING_REQUEST_LINE,
	PARSING_HEADERS,
	PARSING_BODY,
	DONE,
	ERROR
};

// To be added Vitor
class HttpRequestParser
{
	private:
		RequestStatus	_requestStatus;
		HttpRequest		_httpRequest;
		std::string		_buffer;
		std::size_t		_expectedBodyLen;
		bool			_chunked;
		std::size_t		_pos;

		void	handleRequestLine( const std::string& str );
		void	handleHeaders( const std::string& str );
		void	handleBody( const std::string& str );

	public:
		HttpRequestParser( void );
		~HttpRequestParser( void );
		HttpRequestParser( const HttpRequestParser& other );
		HttpRequestParser& operator=( const HttpRequestParser& other );

		enum RequestStatus	getRequestStatus() { return this->_requestStatus; }
		HttpRequest&		getHttpRequest() { return this->_httpRequest; }
		HttpRequest*		getHttpRequestPointer() { return &this->_httpRequest; }

		enum RequestStatus	feed( const char* buffer, size_t size ); // to be done
		bool				hasCgi(); // to be done

};

#endif
