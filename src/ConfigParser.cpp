/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/24 08:41:18 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "VirtualHostConfig.hpp"
#include <bits/types/error_t.h>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
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
	_parseServer["root"]					= &ConfigParser::handleRoot;
	_parseServer["server_name"]				= &ConfigParser::handleServerName;
	_parseServer["index"]					= &ConfigParser::handleIndex;
	_parseServer["allow_methods"]			= &ConfigParser::handleAllowedMethods;

	_parseLocation["root"]			= &ConfigParser::handleLocationRoot;
	_parseLocation["index"]			= &ConfigParser::handleLocationIndex;
	_parseLocation["allow_methods"]	= &ConfigParser::handleLocationAllowedMethods;
	_parseLocation["autoindex"]		= &ConfigParser::handleLocationAutoindex;
	_parseLocation["upload_enable"]	= &ConfigParser::handleLocationUploadEnable;
	_parseLocation["upload_store"]	= &ConfigParser::handleLocationUploadStore;
	_parseLocation["cgi_extension"]	= &ConfigParser::handleLocationCgiExtension;
	_parseLocation["cgi_path"]		= &ConfigParser::handleLocationCgiPath;
}

ConfigParser::~ConfigParser( void ) {}

ConfigParser::ConfigParser( const ConfigParser& other ): _tokens(other._tokens), _filePath(other._filePath), _pos(other._pos), _flagErr(other._flagErr) {}

ConfigParser&	ConfigParser::operator=(const ConfigParser& other)
{
	if (this != &other)
	{
		_parseServer = other._parseServer;
		_parseLocation = other._parseLocation;
		_tokens = other._tokens;
		_locationPaths = other._locationPaths;
		_filePath = other._filePath;
		_pos = other._pos;
		_flagErr = other._flagErr;
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

		std::size_t last_pos = 0;
		while((comment_pos = line.find('#', last_pos)) != std::string::npos)
		{
			bool	on_quote = false;
			bool	on_dquote = false;

			for (std::size_t k = last_pos; k < comment_pos; k++)
			{
				if (line[k] == '"')
					on_dquote = !on_dquote;
				if (line[k] == 39)
					on_quote = !on_quote;
			}
			if (!on_dquote && !on_quote)
				line.erase(comment_pos);
			last_pos = comment_pos;
		}

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

	advance_token(2);
	_locationPaths.clear();

	while (curr_token().first != TOKEN_R_BRACE && _flagErr == false)
	{
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

	advance_token(1);

	if (!_flagErr)
	{
		if (virtualHost.getAllowedMethods().empty())
		{
			virtualHost.addAllowedMethod("GET");
			virtualHost.addAllowedMethod("POST");
			virtualHost.addAllowedMethod("DELETE");
		}
		if (virtualHost.getVecPort().empty())
		{
			std::cerr << "Error: no listen directive was found to a server." << std::endl;
			_flagErr = true;
		}
		// Conversar com o Davi aqui
		// if (virtualHost.getRoot().empty())
		// {
		// 	std::cerr << "Error: no root directive was found to a server." << std::endl;
		// 	_flagErr = true;
		// }
		// if (virtualHost.getIndex().empty())
		// {
		// 	std::cerr << "Error: no index directive was found to a server." << std::endl;
		// 	_flagErr = true;
		// }
	}
	return (virtualHost);
}

void	ConfigParser::mountConfigVec( std::vector<VirtualHostConfig> & configs )
{
	while(curr_token().first != TOKEN_END)
	{
		if (curr_token().second == "server")
			configs.push_back(parseVirtualHost());
		else
		{
			_flagErr = true;
			break ;
		}
	}
	if (configs.empty() && !_flagErr)
		std::cerr << "Empty configuration file." << std::endl;

	if (_flagErr)
		configs.clear();
}

void	ConfigParser::skip_newline( void )
{
	if (_pos >= _tokens.size())
		return ;
	while (_tokens[_pos].first == TOKEN_NEWLINE)
		_pos++;
}

const std::deque< std::pair<t_file_tokens, std::string> > &	ConfigParser::getTokens( void ) const
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
	skip_newline();
	if (_pos >= _tokens.size())
		return (_tokens.back());
	return(_tokens[_pos]);
}

std::pair<t_file_tokens, std::string> & ConfigParser::next_token( void )
{
	std::size_t i = 1;

	skip_newline();
	if (_pos + 1 >= _tokens.size())
		return (_tokens.back());
	while (_tokens[_pos + i].first == TOKEN_NEWLINE)
		i++;
	return(_tokens[_pos + i]);
}

void	ConfigParser::advance_token( int i )
{
	int j;

	j = 0;
	while (j < i)
	{
		_pos++;
		skip_newline();
		j++;
	}
}

void	ConfigParser::handleLocation( VirtualHostConfig& vec )
{
	Location	local;

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	local.setPath(next_token().second);
	advance_token(2);

	if (curr_token().first != TOKEN_L_BRACE)
	{
		_flagErr = true;
		std::cerr << "Error: location block needs '{' before directives." << std::endl;
		return ;
	}
	advance_token(1);

	while (curr_token().first != TOKEN_R_BRACE && _flagErr == false)
	{
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

	if (!_flagErr)
	{
		std::list<std::string>::iterator iter = _locationPaths.begin();

		while(iter != _locationPaths.end())
		{
			if (*iter == local.getPath())
			{
				_flagErr = true;
				std::cerr << "It looks like there is a duplicate value." << std::endl;
				return ;
			}
			iter++;
		}
		if (local.getAllowedMethods().empty())
		{
			local.addAllowedMethod("GET");
			local.addAllowedMethod("POST");
			local.addAllowedMethod("DELETE");
		}
	}
	advance_token(1);
	_locationPaths.push_back(local.getPath());
	vec.addLocation(local);
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

	std::string	listen_val = next_token().second;
	advance_token(1);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}

	std::size_t	posDoubledots = listen_val.find(':');

	if (posDoubledots == std::string::npos)
	{
		if (listen_val.find('.') != std::string::npos || strstr(listen_val.c_str(), "localhost"))
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
	advance_token(2);
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
	advance_token(1);

	value = curr_token().second;
	errno = 0;
	parsedValue = std::strtol(value.c_str(), &end, 10);
	if (parsedValue <= 0 || std::strlen(end) > 1 || errno)
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
			_flagErr = true;
			scale = static_cast<std::size_t>(-1);
	}

	std::size_t maxBodySize = static_cast<std::size_t>(parsedValue);

	if ((static_cast<std::size_t>(-1) / scale ) < maxBodySize || _flagErr)
	{
		_flagErr = true;
		return ;
	}
	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}
	advance_token(2);
	vec.setMaxBodySize(maxBodySize * scale);
}

void	ConfigParser::handleErrorPage( VirtualHostConfig& vec )
{
	char				*end;
	long				num;
	std::list<int>		errors;

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	advance_token(1);

	while (true)
	{
		num = std::strtol(curr_token().second.c_str(), &end, 10);
		if (*end != '\0' || curr_token().first != TOKEN_WORD)
			break ;
		if (num < 300 || num > 599)
		{
			_flagErr = true;
			return ;
		}
		errors.push_back(static_cast<int>(num));
		advance_token(1);
	}

	if ( errors.empty() || curr_token().first != TOKEN_WORD || next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}

	std::list<int>::iterator iter = errors.begin();
	while (iter != errors.end())
	{
		if (vec.getErrorPages().find(*iter) == vec.getErrorPages().end())
		{
			vec.addErrorPage(*iter, curr_token().second);
		}
		else
		{
			_flagErr = true;
			std::cerr << "It looks like there is a duplicate value." << std::endl;
			return ;
		}
		iter++;
	}

	advance_token(2);
}

void	ConfigParser::handleRoot( VirtualHostConfig& vec )
{
	if (!vec.getRoot().empty())
	{
		_flagErr = true;
		std::cerr << "Error: duplicate 'root' directive in server block." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	vec.setRoot(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'root' directive takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleServerName( VirtualHostConfig& vec )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		std::cerr << "Error: 'server_name' requires at least one argument." << std::endl;
		return ;
	}
	advance_token(1);

	while (curr_token().first == TOKEN_WORD)
	{
		vec.addServerName(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: missing ';' at the end of 'server_name' directive." << std::endl;
		return ;
	}
	advance_token(1);
}

void	ConfigParser::handleIndex( VirtualHostConfig& vec )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		std::cerr << "Error: 'index' requires at least one argument." << std::endl;
		return ;
	}
	advance_token(1);

	while (curr_token().first == TOKEN_WORD)
	{
		vec.addIndex(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: missing ';' at the end of 'index' directive." << std::endl;
		return ;
	}
	advance_token(1);
}

void	ConfigParser::handleAllowedMethods( VirtualHostConfig& vec )
{
	const char							*allowed[] = {"GET", "DELETE", "POST", 0};
	bool								flag;
	const std::vector<std::string> &	tmp = vec.getAllowedMethods();

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	advance_token(1);
	while (curr_token().first == TOKEN_WORD)
	{
		flag = false;
		for(int i = 0; allowed[i]; i++)
		{
			if (curr_token().second == allowed[i])
			{
				flag = true;
				for(std::size_t j = 0; j < tmp.size(); j++)
				{
					if (tmp[j] == allowed[i])
					{
						_flagErr = true;
						std::cerr << "Error: duplicate directive in allowed_methods." << std::endl;
						return ;
					}
				}
				vec.addAllowedMethod(curr_token().second);
			}
		}
		if (!flag)
		{
			_flagErr = true;
			std::cerr << "Error: invalid directive in allowed_methods." << std::endl;
			return ;
		}
		advance_token(1);
	}
	if (curr_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: missing ';' at the end of 'index' directive." << std::endl;
		return ;
	}
	advance_token(1);
}

void	ConfigParser::handleLocationRoot( Location& loc )
{
	if (!loc.getRoot().empty())
	{
		_flagErr = true;
		std::cerr << "Error: duplicate 'root' directive in location." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	loc.setRoot(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'root' directive takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleLocationIndex( Location& loc )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	while (curr_token().first == TOKEN_WORD)
	{
		loc.addIndex(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);
}

void	ConfigParser::handleLocationAllowedMethods( Location& loc )
{
	const char							*allowed[] = {"GET", "DELETE", "POST", 0};
	bool								flag;
	const std::vector<std::string> &	tmp = loc.getAllowedMethods();

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}

	advance_token(1);
	while (curr_token().first == TOKEN_WORD)
	{
		flag = false;
		for(int i = 0; allowed[i]; i++)
		{
			if (curr_token().second == allowed[i])
			{
				flag = true;
				for(std::size_t j = 0; j < tmp.size(); j++)
				{
					if (tmp[j] == allowed[i])
					{
						_flagErr = true;
						std::cerr << "Error: duplicate directive in allowed_methods." << std::endl;
						return ;
					}
				}
				loc.addAllowedMethod(curr_token().second);
			}
		}
		if (!flag)
		{
			_flagErr = true;
			std::cerr << "Error: invalid directive in allowed_methods." << std::endl;
			return ;
		}
		advance_token(1);
	}
	if (curr_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: missing ';' at the end of 'index' directive." << std::endl;
		return ;
	}
	advance_token(1);
}

void	ConfigParser::handleLocationAutoindex( Location& loc )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	std::string state = curr_token().second;

	if (state == "on")
		loc.setAutoindex(true);
	else if (state == "off")
		loc.setAutoindex(false);
	else
	{
		_flagErr = true;
		std::cerr << "Error: 'autoindex' must be 'on' or 'off'." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'autoindex' takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleLocationUploadEnable( Location& loc )
{
	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	std::string state = curr_token().second;

	if (state == "on")
		loc.setUploadEnable(true);
	else if (state == "off")
		loc.setUploadEnable(false);
	else
	{
		_flagErr = true;
		std::cerr << "Error: 'upload_enable' must be 'on' or 'off'." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleLocationUploadStore( Location& loc )
{
	if (!loc.getUploadStore().empty())
	{
		_flagErr = true;
		std::cerr << "Error: duplicate 'upload_store' directive." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	loc.setUploadStore(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'upload_store' takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleLocationCgiExtension( Location& loc )
{
	if (!loc.getCgiExtension().empty())
	{
		_flagErr = true;
		std::cerr << "Error: duplicate 'cgi_extension' directive." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	loc.setCgiExtension(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'cgi_extension' takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}

void	ConfigParser::handleLocationCgiPath( Location& loc )
{
	if (!loc.getCgiPath().empty())
	{
		_flagErr = true;
		std::cerr << "Error: duplicate 'cgi_path' directive." << std::endl;
		return ;
	}

	if (next_token().first != TOKEN_WORD)
	{
		_flagErr = true;
		return ;
	}
	advance_token(1);

	loc.setCgiPath(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
	{
		_flagErr = true;
		std::cerr << "Error: 'cgi_path' takes exactly one argument." << std::endl;
		return ;
	}
	advance_token(2);
}
