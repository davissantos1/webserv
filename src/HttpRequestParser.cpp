/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/21 05:05:10 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"
#include "HttpRequest.hpp"
#include "string_utils.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <vector>

HttpRequestParser::HttpRequestParser( void ): _requestStatus(PARSING_REQUEST_LINE), _heardersDone(false), _expectedBodyLen(1), _maxBodySize(0), _bodyProtocol(TO_VERIFY), _chunckState(SIZE), _chunkLen(0)  {}

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
		_bodyProtocol = other._bodyProtocol;
		_chunckState = other._chunckState;
		_chunkLen = other._chunkLen;
		_maxBodySize = other._maxBodySize;
		_heardersDone = other._heardersDone;
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
			if (_heardersDone)
				checkHeaders();
			_buffer.erase(0, i + 2);
		}
		else
		{
			handleBody();
			if (_bodyProtocol != CHUNKED
				|| (_chunckState == SIZE && _buffer.find("\r\n") == std::string::npos)
				|| (_chunckState == CONTENT && _buffer.size() < _chunkLen + 2))
				break ;
		}
	}
	return (_requestStatus);
}

static std::string cleanUp_uri( const std::string& uri)
{
	return (decode_str(uri));
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
		_heardersDone = true;
		return ;
	}
	pos = str.find(':');
	if (pos == std::string::npos)
	{
		_requestStatus = ERROR_BAD_REQUEST;
		return ;
	}
	key = str.substr(0, pos);
	trimStr(key);
	if (key.empty() || _httpRequest.getHeader(key) != "")
	{
		_requestStatus = ERROR_BAD_REQUEST;
		return ;
	}
	value = str.substr(pos + 1);
	trimStr(value);
	_httpRequest.addHeader(key, value);
}


void	HttpRequestParser::handleBody( void )
{
	if (_bodyProtocol == TO_VERIFY)
		checkContentProtocol();
	if (_requestStatus == ERROR_BAD_REQUEST || _requestStatus == DONE)
		return ;

	if (_bodyProtocol == NOT_CHUNKED)
		handleContent();
	else
		handleChunked();
}

void	HttpRequestParser::handleContent( void )
{
	if (_buffer.size() > _expectedBodyLen || (_maxBodySize != 0 && _expectedBodyLen > _maxBodySize))
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
	long	chunkLen;
	char	*end;

	if (_chunckState == SIZE)
	{
		i = _buffer.find("\r\n");
		if (i == std::string::npos)
			return ;
		errno = 0;
		chunkLen = strtol(_buffer.c_str(), &end, 16);
		if (errno || chunkLen < 0 || *end != '\r')
		{
			_requestStatus = ERROR_BAD_REQUEST;
			return ;
		}
		_expectedBodyLen += static_cast<std::size_t>(chunkLen);
		if (_maxBodySize != 0 && _expectedBodyLen > _maxBodySize)
		{
			_requestStatus = ERROR_REQUEST_TOO_LARGE;
			return ;
		}
		_chunkLen = static_cast<std::size_t>(chunkLen);
		_buffer.erase(0, i + 2);
		_chunckState = CONTENT;
	}

	if (_chunckState == CONTENT)
	{
		if (_buffer.size() < _chunkLen + 2)
			return ;
		_httpRequest.appendBody(_buffer.c_str(), _chunkLen);
		if (_chunkLen == 0 && _buffer.size() == 2)
			_requestStatus = DONE;
		_buffer.erase(0, _chunkLen + 2);
		_chunckState = SIZE;
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

void	HttpRequestParser::splitRequestUri(VirtualHostConfig& conf)
{
	const std::vector<Location>& locations = conf.getLocation();
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
			endpoint.erase(slash);
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
			endpoint.erase(slash);
			filename.erase(0, slash + 1);
		}
		else
			filename.clear();
	}
	const Location* best = NULL;
	size_t bestLen = 0;
	for (size_t i = 0; i < locations.size(); i++)
	{
		const std::string& locPath = locations[i].getPath();
		if (endpoint.compare(0, locPath.size(), locPath) == 0)
		{
			bool valid =
				(locPath[locPath.size() - 1] == '/') ||
				(endpoint.size() == locPath.size()) ||
				(endpoint[locPath.size()] == '/');
			if (valid && locPath.size() > bestLen)
			{
				best = &locations[i];
				bestLen = locPath.size();
			}
		}
	}
	if (best)
		this->_httpRequest.setEndpoint(best->getPath());
	this->_httpRequest.setQuery(queryString);
	this->_httpRequest.setFilename(filename);
}

void	HttpRequestParser::checkContentProtocol( void )
{
	char		*err;
	std::string	value;
	long		size;

	value = _httpRequest.getHeader("Content-Length");
	if (value != "")
	{
		errno = 0;
		size = std::strtol(value.c_str(), &err, 10);
		if (errno || *err || size < 0)
		{
			_requestStatus = ERROR_BAD_REQUEST;
			return ;
		}
		_expectedBodyLen = static_cast<size_t>(size);
		_bodyProtocol = NOT_CHUNKED;
	}
	value = _httpRequest.getHeader("Transfer-Encoding");
	if (value != "")
	{
		if (value != "chunked" || _bodyProtocol != TO_VERIFY)
		{
			_requestStatus = ERROR_BAD_REQUEST;
			return ;
		}
		_bodyProtocol = CHUNKED;
	}
	if (_bodyProtocol == TO_VERIFY && _httpRequest.getMethod() == "POST")
		_requestStatus = ERROR_BAD_REQUEST;
	if (_httpRequest.getMethod() != "POST")
		_requestStatus = DONE;
}

void	HttpRequestParser::reset( void )
{
	_requestStatus = PARSING_REQUEST_LINE;
	_httpRequest.reset();
	_expectedBodyLen = 0;
	_bodyProtocol = TO_VERIFY;
	_chunckState = SIZE;
	_chunkLen = 0;
}

void	HttpRequestParser::checkHeaders( void )
{
	const char *requiredFields[] = {"Host", 0};

	for (size_t i = 0; requiredFields[i] != NULL; i++)
	{
		if (_httpRequest.getHeader(requiredFields[i]) == "")
		{
			_requestStatus = ERROR_BAD_REQUEST;
			break ;
		}
	}
}
