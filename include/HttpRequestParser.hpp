/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/18 12:04:25 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

// deal with the connection and the transfer-encoding header
// This parser will be fed a bit at a time and have an internal state
// I need to know specific states from feed to send an error if the request is malformed or filled with errors
// I'm assuming an enum for each state, tell me if you change it, please
// This project is draining my soul
// Please include PARSING_ERROR and REQUEST_TOO_LARGE statuses for error handling, I need that.

#include "HttpRequest.hpp"
#include <cstddef>


enum RequestStatus {
	PARSING_REQUEST_LINE,
	PARSING_HEADERS,
	PARSING_BODY,
	DONE,
	ERROR_BAD_REQUEST,
	ERROR_REQUEST_TOO_LARGE
};

class HttpRequestParser
{

	enum	e_bodyProtocol
	{
		TO_VERIFY,
		CHUNKED,
		NOT_CHUNKED,
	};

	enum	e_chunckState
	{
		SIZE,
		CONTENT
	};

	private:
		RequestStatus	_requestStatus;
		HttpRequest		_httpRequest;
		std::string		_buffer;
		bool			_heardersDone;
		std::size_t		_expectedBodyLen;
		std::size_t		_maxBodySize;
		e_bodyProtocol	_bodyProtocol;

		//Just to chunked protocol
		e_chunckState	_chunckState;
		size_t			_chunkLen;

		void	handleRequestLine( const std::string& str );
		void	handleHeaders( const std::string& str );
		void	handleBody( void );

		void	checkContentProtocol( void );
		void	handleChunked( void );
		void	handleContent( void );
		void	splitRequestUri( void );

		void	checkHeaders( void );

	public:
		HttpRequestParser( void );
		~HttpRequestParser( void );
		HttpRequestParser( const HttpRequestParser& other );
		HttpRequestParser& operator=( const HttpRequestParser& other );

		enum RequestStatus	getRequestStatus() { return this->_requestStatus; }
		HttpRequest&		getHttpRequest() { return this->_httpRequest; }
		HttpRequest*		getHttpRequestPointer() { return &this->_httpRequest; }
		std::size_t			getMaxBodySize() { return this->_maxBodySize; }
		bool				getHeadersDone( void ) const { return _heardersDone; };

		void				setHeadersDone( bool headersDone ) { _heardersDone = headersDone; };
		void				setMaxBodySize(std::size_t maxBodySize) { this->_maxBodySize = maxBodySize; }

		enum RequestStatus	feed( const char* buffer, size_t size );
		bool				hasCgi( void );
		void				cleanHttpRequest() { this->_httpRequest.reset(); }
		void				reset( void );
};

#endif
