/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/15 01:27:15 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"
#include "HttpRequest.hpp"
#include "string_utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <vector>

HttpRequestParser::HttpRequestParser( void ): _requestStatus(PARSING_REQUEST_LINE), _expectedBodyLen(0), _chunked(TO_VERIFY), _state(SIZE), _chunkLen(0) {}

HttpRequestParser::~HttpRequestParser( void ) {}

HttpRequestParser::HttpRequestParser(const HttpRequestParser& other)
{
	if (this != &other)
		*this = other;
}

HttpRequestParser&	HttpRequestParser::operator=(const HttpRequestParser& other)
{
	if (this != &other)
	{
		_requestStatus = other._requestStatus;
		_httpRequest = other._httpRequest;
		_buffer = other._buffer;
		_expectedBodyLen = other._expectedBodyLen;
		_chunked = other._chunked;
		_state = other._state;
		_body = other._body;
		_chunkLen = other._chunkLen;
	}
	return (*this);
}

enum RequestStatus HttpRequestParser::feed( const char* buffer, size_t len )
{
	std::string	tmp;
	std::size_t	i;

	_buffer.append(buffer, len);
	while (_requestStatus != DONE && _requestStatus != ERROR_BAD_REQUEST && _requestStatus != ERROR_REQUEST_TOO_LARGE)
	{
		if (_requestStatus == PARSING_REQUEST_LINE || _requestStatus == PARSING_HEADERS)
		{
			i = _buffer.find("\r\n");
			if (i == std::string::npos)
				break ;

			std::string param = _buffer.substr(0, i);

			if (_requestStatus == PARSING_REQUEST_LINE)
				handleRequestLine(param);
			else
				handleHeaders(param);
			_buffer.erase(0, i + 2);
		}
		else
		{
			handleBody();
			if ((_chunked == CHUNKED && (_state == CONTENT || (_buffer.find("\n\r") == std::string::npos))) || _chunked != CHUNKED)
				break ;
		}
	}
	return (_requestStatus);
}

static std::string cleanUp_uri( const std::string& uri)
{
	return (normalize_str(decode_str(uri)));
}

void	HttpRequestParser::handleRequestLine( const std::string& str )
{
	std::vector<std::string> splited = split(str, ' ');

	if (splited.size() != 3 || (splited[0] != "GET" && splited[0] != "POST" && splited[0] != "DELETE"))
	{
		_requestStatus = ERROR_BAD_REQUEST;
		return ;
	}

	_httpRequest.setMethod(splited[0]);
	_httpRequest.setUri(cleanUp_uri(splited[1]));
	_httpRequest.setVersion(splited[2]);

	if (splited[2] != "HTTP/1.1" && splited[2] != "HTTP/1.0")
	{
		_requestStatus = ERROR_BAD_REQUEST;
		return ;
	}

	splitRequestUri();
	_requestStatus = PARSING_HEADERS;
}

void	HttpRequestParser::handleHeaders( const std::string& str )
{
	std::string	key;
	std::string	value;
	size_t		pos;

	if (str.empty())
	{
		_requestStatus = PARSING_BODY;
		return ;
	}
	pos = str.find(':');
	if (pos == std::string::npos)
	{
		_requestStatus = ERROR_BAD_REQUEST;
		return ;
	}
	key = str.substr(0, pos);
	value = str.substr(pos + 1);
//	strToLower(key); aqui
	trimStr(value);
	_httpRequest.addHeader(key, value);
}


void	HttpRequestParser::handleBody( void )
{
	if (_chunked == TO_VERIFY)
		checkContentProtocol();
	if (_requestStatus == ERROR_BAD_REQUEST || _requestStatus == DONE)
		return ;

	if (_chunked == NOT_CHUNKED)
		handleContent();
	else
		handleChunked();
}

void	HttpRequestParser::handleContent( void )
{
	if (_buffer.size() > _expectedBodyLen)
	{
		_requestStatus = ERROR_REQUEST_TOO_LARGE;
		return ;
	}

	if (_buffer.size() == _expectedBodyLen)
	{
		_httpRequest.setBody(_buffer);
		_requestStatus = DONE;
	}
}

void	HttpRequestParser::handleChunked( void )
{
	size_t	i;

	if (_state == SIZE)
	{
		i = _buffer.find("\r\n");
		if (i == std::string::npos)
			return ;

		_chunkLen = strtol(_buffer.c_str(), NULL, 16);
		_buffer.erase(0, i + 2);
		_state = CONTENT;
	}

	if (_state == CONTENT)
	{
		if (_buffer.size() < _chunkLen + 2)
			return ;
		_body.append(_buffer.c_str(), _chunkLen);
		_buffer.erase(0, _chunkLen + 2);
		_state = SIZE;
		if (_chunkLen == 0)
		{
			_httpRequest.setBody(_body);
			_chunkLen = 0;
			_body.clear();
			_requestStatus = DONE;
		}
	}
}

bool	HttpRequestParser::hasCgi()
{
	if (_requestStatus == PARSING_HEADERS || _requestStatus == PARSING_BODY)
	{
		size_t dotPos = _httpRequest.getUri().find_last_of(".");

		if (_httpRequest.getUri().find("/cgi-bin/") != std::string::npos)
			return (true);

		if (dotPos != std::string::npos)
		{
			std::string	extensionFile = _httpRequest.getUri().substr(dotPos);

			return (extensionFile == ".py" || extensionFile == ".php");
		}
	}
	return (false);
}

void	HttpRequestParser::splitRequestUri()
{
	std::string	filename, endpoint, queryString;
	std::string uri	= this->_httpRequest.getUri();
	size_t		dot = uri.rfind('.');
	size_t		slash = uri.rfind('/');
	size_t		query = uri.rfind('?');

	filename = endpoint = queryString = uri;
	if (query != std::string::npos)
	{
		filename.erase(query);
		endpoint.erase(query);
		queryString.erase(0, query + 1);
		if (dot != std::string::npos)
		{
			endpoint.erase(slash + 1);
			filename.erase(0, slash + 1);
		}
		else
			filename.clear();
	}
	else
	{
		queryString.clear();
		if (dot != std::string::npos)
		{
			endpoint.erase(slash + 1);
			filename.erase(0, slash + 1);
		}
		else
			filename.clear();
	}
	this->_httpRequest.setQuery(queryString);
	this->_httpRequest.setFilename(filename);
	this->_httpRequest.setEndpoint(endpoint);
}

void	HttpRequestParser::checkContentProtocol( void )
{
	char		*err;
	std::string	value;
	size_t		size;

	value = _httpRequest.getHeader("Content-Length");
	if (value != "")
	{
		size = static_cast<size_t>(std::strtol(value.c_str(), &err, 10));
		if (*err)
		{
			_requestStatus = ERROR_BAD_REQUEST;
			return ;
		}
		_expectedBodyLen = size;
		_chunked = NOT_CHUNKED;
	}
	value = _httpRequest.getHeader("Transfer-Encoding");
	if (value != "")
	{
		if (value != "chunked" || _chunked != TO_VERIFY)
		{
			_requestStatus = ERROR_BAD_REQUEST;
			return ;
		}
		_chunked = CHUNKED;
	}
	if (_chunked == TO_VERIFY && _httpRequest.getMethod() == "POST")
		_requestStatus = ERROR_BAD_REQUEST;
	if (_httpRequest.getMethod() != "POST")
		_requestStatus = DONE;
}

void	HttpRequestParser::reset( void )
{
	_requestStatus = PARSING_REQUEST_LINE;
	_httpRequest.reset();
	_expectedBodyLen = 0;
	_chunked = TO_VERIFY;
	_state = SIZE;
	_chunkLen = 0;
	_body.clear();
}
