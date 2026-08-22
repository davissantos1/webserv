/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/21 02:29:21 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

#include "VirtualHostConfig.hpp"
#include "HttpRequest.hpp"
#include <cstddef>

enum RequestStatus {
	PARSING_REQUEST_LINE = 0,
	PARSING_HEADERS = 1,
	PARSING_BODY = 2,
	DONE = 3,
	ERROR_BAD_REQUEST = 4,
	ERROR_REQUEST_TOO_LARGE = 5
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

		void	processHeaders( void );
		void	checkHeaders( void );
		void	processContentProtocol( void );
		void	processConnection( void );

		void	handleChunked( void );
		void	handleContent( void );

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

		void				splitRequestUri( VirtualHostConfig& conf );
		enum RequestStatus	feed( const char* buffer, size_t size );
		bool				hasCgi( void );
		void				cleanHttpRequest() { this->_httpRequest.reset(); }
		void				reset( void );
};

#endif
