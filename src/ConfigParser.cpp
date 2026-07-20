/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/19 17:10:40 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "VirtualHostConfig.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

ConfigParser::ConfigParser( void )
{
	_pos = 0;
	_flagErr = false;

	_parseServer["location"]				= &ConfigParser::handleLocation;
	_parseServer["listen"]					= &ConfigParser::handleListen;
	_parseServer["client_max_body_size"]	= &ConfigParser::handleClientMaxBodySize;
	_parseServer["error_page"]				= &ConfigParser::handleErrorPage;

	_parseLocation["root"]			= &ConfigParser::handleLocationRoot;
	_parseLocation["index"]			= &ConfigParser::handleLocationIndex;
	_parseLocation["allow_methods"]	= &ConfigParser::handleLocationAllowedMethods;
	_parseLocation["autoindex"]		= &ConfigParser::handleLocationAutoindex;
}

ConfigParser::~ConfigParser( void ) {}

ConfigParser::ConfigParser(const ConfigParser& other): _tokens(other._tokens), _filePath(other._filePath), _pos(other._pos), _flagErr(other._flagErr){}

ConfigParser&	ConfigParser::operator=(const ConfigParser& other)
{
	if (this != &other)
	{
		_tokens = other._tokens;
		_pos = other._pos;
		_filePath = other._filePath;
	}
	return (*this);
}

std::vector<VirtualHostConfig> ConfigParser::parse( const std::string &path )
{
	std::vector<VirtualHostConfig>	configs;
	std::ifstream					file;

	file.open(path.c_str());
	if (file.is_open() == true)
	{
		makeTokens(file);
		mountConfigVec(configs);
		file.close();
	}
	else
		std::cerr << "Error: Couldn't read " << path << " file." << std::endl;
	return (configs);
}

static std::pair<t_file_tokens, std::string> makePair( t_file_tokens type, std::string text )
{
	std::pair<t_file_tokens, std::string>	token;

	token.first = type;
	token.second = text;
	return (token);
}

void	ConfigParser::makeTokens( std::ifstream& file )
{
	std::string line;
	std::size_t	pos;
	std::size_t	comment_pos;
	std::string	token_text;

	while (file.eof() == false)
	{
		pos = 0;
		getline(file, line);

		comment_pos = line.find('#');
		if (comment_pos != std::string::npos)
			line.erase(comment_pos);

		while (pos < line.length())
		{
			if (line[pos] == ' ' || (line[pos] >= 9 && line[pos] <= 13))
			{
				pos++;
				continue ;
			}

			switch (line[pos])
			{
				case '{':
					_tokens.push_back(makePair(TOKEN_L_BRACE, "{"));
					pos++;
					break ;
				case '}':
					_tokens.push_back(makePair(TOKEN_R_BRACE, "}"));
					pos++;
					break ;
				case ';':
					_tokens.push_back(makePair(TOKEN_SEMICOLON, ";"));
					pos++;
					break ;
				default :
					std::size_t	i = pos;
					while (i < line.length() && !std::strchr("{}; \t\n\r", line[i]))
					{
						token_text += line[i];
						i++;
					}
					_tokens.push_back(makePair(TOKEN_WORD, token_text));
					token_text.clear();
					pos = i;
			}
		}
		_tokens.push_back(makePair(TOKEN_NEWLINE, "\n"));
	}
	_tokens.push_back(makePair(TOKEN_END, ""));
}

VirtualHostConfig	ConfigParser::parseVirtualHost( void )
{
	VirtualHostConfig	virtualHost;

	if (next_token().first != TOKEN_L_BRACE)
	{
		_flagErr = true;
		return (virtualHost);
	}

	_pos += 2;

	while (curr_token().first != TOKEN_R_BRACE && _flagErr == false)
	{
		while (curr_token().first == TOKEN_NEWLINE)
			_pos++;

		if (curr_token().first == TOKEN_WORD)
		{
			std::map<std::string, ParseServer>::iterator iter = _parseServer.find(curr_token().second);

			if (iter != _parseServer.end())
				(this->*(iter->second))(virtualHost);
			else
				_flagErr = true;
		}
		else
			_flagErr = true;
	}

	return (virtualHost);
}

void	ConfigParser::mountConfigVec( std::vector<VirtualHostConfig> & configs )
{
	while(curr_token().first == TOKEN_NEWLINE)
	{
		while (curr_token().second == "server")
			configs.push_back(parseVirtualHost());
		_pos++;
	}

	if (_tokens.size() == 1)
		std::cerr << "Empty configuration file." << std::endl;

	if (_flagErr)
		configs.clear();
}

const std::deque< std::pair<t_file_tokens, std::string> >&	ConfigParser::getTokens( void ) const
{
		return (_tokens);
}

std::ostream&	operator<<( std::ostream& out, const ConfigParser & tokens )
{
	std::size_t	i = 0;
	std::size_t	size = tokens.getTokens().size();

	while (i < size)
	{
		out << tokens.getTokens()[i].second << ' ';
		i++;
	}
	return (out);
}

std::pair<t_file_tokens, std::string> & ConfigParser::curr_token( void )
{
	if (_pos > _tokens.size())
		return (_tokens.back());
	return(_tokens[_pos]);
}

std::pair<t_file_tokens, std::string> & ConfigParser::next_token( void )
{
	if (_pos + 1 >= _tokens.size())
		return (_tokens.back());
	return(_tokens[_pos + 1]);
}


void	ConfigParser::handleLocation( VirtualHostConfig& vec )
{
	Location	local;

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	local.setRoot(next_token().second);
	_pos += 2;

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	_pos++;

	while (curr_token().first != TOKEN_R_BRACE && _flagErr == false)
	{
		while (curr_token().first == TOKEN_NEWLINE)
			_pos++;

		if (curr_token().first == TOKEN_WORD)
		{
			std::map<std::string, ParseLocation>::iterator iter = _parseLocation.find(curr_token().second);

			if (iter != _parseLocation.end())
				(this->*(iter->second))(local);
			else
				_flagErr = true;
		}
		else
			_flagErr = true;
	}
}

bool	ConfigParser::validatePort( const std::string& port )
{
	int	portNum;

	if (port.empty() || port.length() > 5)
		return (false);
	for (std::size_t i = 0; i < port.length(); ++i)
	{
		if (!std::isdigit(port[i]))
			return (false);
	}
	portNum = std::atoi(port.c_str());
	return (portNum >= 1 && portNum <= 65535);
}

bool	ConfigParser::validateIp( const std::string& ip )
{
	int			dots = 0;
	std::size_t	start = 0;
	std::size_t	end = 0;

	if (ip == "localhost")
		return (true);
	while ((end = ip.find('.', start)) != std::string::npos)
	{
		std::string	part = ip.substr(start, end - start);

		if (part.empty() || part.length() > 3)
			return (false);

		for (std::size_t i = 0; i < part.length(); ++i)
		{
			if (!std::isdigit(part[i]))
				return (false);
		}

		int	num = std::atoi(part.c_str());
		if (num < 0 || num > 255)
			return (false);

		dots++;
		start = end + 1;
	}

	std::string	last_part = ip.substr(start);

	if (last_part.empty() || last_part.length() > 3)
		return (false);

	for (std::size_t i = 0; i < last_part.length(); ++i)
	{
		if (!std::isdigit(last_part[i]))
			return (false);
	}

	int	num = std::atoi(last_part.c_str());

	if (num < 0 || num > 255)
		return (false);

	return (dots == 3);
}

void	ConfigParser::handleListen( VirtualHostConfig& vec )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	std::string	listen_val = curr_token().second;
	_pos++;

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}

	std::size_t	posDoubledots = listen_val.find(':');

	if (posDoubledots == std::string::npos)
	{
		if (listen_val.find('.') != std::string::npos)
		{
			if (validateIp(listen_val))
			{
				vec.addHostIp(listen_val);
				vec.addPort(80);
			}
			else
				_flagErr = true;
		}
		else
		{
			if (validatePort(listen_val))
			{
				char	*err;
				long	port_num = std::strtol(listen_val.c_str(), &err, 10);

				vec.addPort(static_cast<int>(port_num));
				vec.addHostIp("0.0.0.0");
			}
			else
				_flagErr = true;
		}
	}
	else
	{
		std::string ip = listen_val.substr(0, posDoubledots);
		std::string port_str = listen_val.substr(posDoubledots + 1);

		if (validatePort(port_str) && validateIp(ip))
		{
			char *err;
			long port = std::strtol(port_str.c_str(), &err, 10);

			vec.addHostIp(ip);
			vec.addPort(static_cast<int>(port));
		}
		else
			_flagErr = true;
	}
}

void	ConfigParser::handleClientMaxBodySize( VirtualHostConfig& vec )
{
	long		parsedValue;
	std::string	value;
	std::size_t	scale;
	char		*end;

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	_pos++;

	value = curr_token().second;
	parsedValue = std::strtol(value.c_str(), &end, 10);
	if (parsedValue <= 0 || std::strlen(end) > 1)
	{
		_flagErr = true;
		return ;
	}
	switch (*end)
	{
		case '\0':
			scale = 1;
			break ;
		case 'k':
		case 'K':
			scale = 1 << 10;
			break ;
		case 'm':
		case 'M':
			scale = 1 << 20;
			break ;
		case 'g':
		case 'G':
			scale = 1 << 30;
			break ;
		default:
			scale = -1;
	}

	std::size_t maxBodySize = static_cast<std::size_t>(parsedValue);

	if ((static_cast<std::size_t>(-1) / scale ) < maxBodySize)
	{
		_flagErr = true;
		return ;
	}
	if (next_token().first != TOKEN_SEMICOLON)
	{
			_flagErr = true;
			return ;
	}
	_pos++;
	vec.setMaxBodySize(maxBodySize * scale);
}

void	ConfigParser::handleErrorPage( VirtualHostConfig& vec )
{

}

void	ConfigParser::handleLocationRoot( Location& loc )
{

}

void	ConfigParser::handleLocationIndex( Location& loc )
{

}

void	ConfigParser::handleLocationAllowedMethods( Location& loc )
{
	const std::string	allowed[] = {"GET", "DELETE", "POST"};

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}


}

void	ConfigParser::handleLocationAutoindex( Location& loc )
{

}
