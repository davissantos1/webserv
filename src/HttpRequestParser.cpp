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
#include <string>
#include <vector>

HttpRequestParser::HttpRequestParser( void ): _requestStatus(PARSING_REQUEST_LINE), _expectedBodyLen(0), _chunked(false), _pos(0) {}

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
		_pos = other._pos;
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
		i = _buffer.find("\r\n");
		if (i == std::string::npos)
			break ;
		if (_requestStatus == PARSING_REQUEST_LINE || _requestStatus == PARSING_HEADERS)
		{
			std::string param = _buffer.substr(0, i);

			if (_requestStatus == PARSING_REQUEST_LINE)
				handleRequestLine(param);
			else
				handleHeaders(param);
			_buffer.erase(0, i + 2);
		}
		else
		{
			handleBody(_buffer);
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
	std::vector<std::string> splited = split(str, '\n');


}

void	HttpRequestParser::handleBody( const std::string& str )
{
	(void)str;
	// minha nossa como eu amo parsear, eu sou o maior parseador desse mundo, faço 2050 parseadas num sentada. aqui
	// Lá ele, numa sentada só ainda KKKKKKKK
	// eu tava construindo esse metodo para o HttpRequest e vi que ele faz mais sentido no proprio parser assim que a URI tiver pronta
}

bool	HttpRequestParser::hasCgi()
{
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
