/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/26 23:07:01 by dasimoes         ###   ########.fr       */
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
#include <sstream>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <sys/stat.h>
#include <utility>
#include <vector>

ConfigParser::ConfigParser( void )
{
	_pos = 0;

	_parseServer["location"]				= &ConfigParser::handleLocation;
	_parseServer["listen"]					= &ConfigParser::handleListen;
	_parseServer["client_max_body_size"]	= &ConfigParser::handleClientMaxBodySize;
	_parseServer["error_page"]				= &ConfigParser::handleErrorPage;
	_parseServer["root"]					= &ConfigParser::handleRoot;
	_parseServer["server_name"]				= &ConfigParser::handleServerName;
	_parseServer["index"]					= &ConfigParser::handleIndex;
	_parseServer["allow_methods"]			= &ConfigParser::handleAllowedMethods;
	_parseServer["return"]					= &ConfigParser::handleReturn;
	_parseServer["upload_path"]				= &ConfigParser::handleUploadPath;

	_parseLocation["root"]			= &ConfigParser::handleLocationRoot;
	_parseLocation["index"]			= &ConfigParser::handleLocationIndex;
	_parseLocation["allow_methods"]	= &ConfigParser::handleLocationAllowedMethods;
	_parseLocation["error_page"]	= &ConfigParser::handleLocationErrorPage;
	_parseLocation["autoindex"]		= &ConfigParser::handleLocationAutoindex;
	_parseLocation["upload_path"]	= &ConfigParser::handleLocationUploadPath;
	_parseLocation["cgi_extension"]	= &ConfigParser::handleLocationCgiExtension;
	_parseLocation["cgi_path"]		= &ConfigParser::handleLocationCgiPath;
	_parseLocation["return"]		= &ConfigParser::handleLocationReturn;
}

ConfigParser::~ConfigParser( void ) {}

ConfigParser::ConfigParser( const ConfigParser& other )
{
	*this = other;
}

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
		if (is_dir(path.c_str()))
			throw std::runtime_error("Error: invalid configuration file.");
		makeTokens(file);
		mountConfigVec(configs);
		file.close();
	}
	else
		throw std::runtime_error("Error: Couldn't read configuration file.");
	return (configs);
}

bool	ConfigParser::is_dir( const char* path )
{
	struct stat	file;

	bzero(&file, sizeof(file));
	if (stat(path, &file) != 0)
		throw std::runtime_error("Error: stat() syscall error.");
	return ((file.st_mode & S_IFMT) == S_IFDIR);
}

void	ConfigParser::makeTokens( std::ifstream& file )
{
	std::string	line;
	std::string	token_text;
	bool		in_word = false;
	char		quote = '\0';

	token_text.reserve(256);
	while (std::getline(file, line))
	{
		for (std::size_t i = 0; i < line.length(); i++)
		{
			char	c = line[i];

			if (quote != '\0')
			{
				if (c == quote)
				{
					if (token_text.empty())
						throw std::runtime_error("Error: empty content inside quotes.");
					else
						quote = '\0';
				}
				else
					token_text += c;
			}
			else
			{
				switch (c)
				{
					case '\'':
					case '"':
						quote = c;
						in_word = true;
						break ;

					case '#':
						i = line.length();
						break ;

					case ' ':
					case '\t':
					case '\r':
					case '\v':
					case '\f':
						if (in_word)
						{
							_tokens.push_back(std::make_pair(TOKEN_WORD, token_text));
							token_text.clear();
							in_word = false;
						}
						break ;

					case '{':
					case '}':
					case ';':
						if (in_word)
						{
							_tokens.push_back(std::make_pair(TOKEN_WORD, token_text));
							token_text.clear();
							in_word = false;
						}
						if (c == '{')
							_tokens.push_back(std::make_pair(TOKEN_L_BRACE, "{"));
						else if (c == '}')
							_tokens.push_back(std::make_pair(TOKEN_R_BRACE, "}"));
						else
							_tokens.push_back(std::make_pair(TOKEN_SEMICOLON, ";"));
						break ;

					default:
						token_text += c;
						in_word = true;
						break ;
				}
			}
		}

		if (quote != '\0')
			throw_exception("Syntax error: Unclosed quote detected.");

		if (in_word)
		{
			_tokens.push_back(std::make_pair(TOKEN_WORD, token_text));
			token_text.clear();
			in_word = false;
		}
		_tokens.push_back(std::make_pair(TOKEN_NEWLINE, "\n"));
	}
	_tokens.push_back(std::make_pair(TOKEN_END, ""));
}

VirtualHostConfig	ConfigParser::parseVirtualHost( void )
{
	VirtualHostConfig	virtualHost;

	if (next_token().first != TOKEN_L_BRACE)
		throw_exception("Error: expected '{' after server directive.");

	advance_token(2);
	_locationPaths.clear();

	while (curr_token().first != TOKEN_R_BRACE)
	{
		if (curr_token().first == TOKEN_WORD)
		{
			std::map<std::string, ParseServer>::iterator iter = _parseServer.find(curr_token().second);

			if (iter != _parseServer.end())
				(this->*(iter->second))(virtualHost);
			else
				throw_exception("Error: invalid directive in server block.");
		}
		else
			throw_exception("Error: expected unqualified word.");

	}

	advance_token(1);

	if (virtualHost.getAllowedMethods().empty())
	{
		virtualHost.addAllowedMethod("GET");
		virtualHost.addAllowedMethod("POST");
		virtualHost.addAllowedMethod("DELETE");
		virtualHost.addAllowedMethod("HEAD");
	}
	if (virtualHost.getVecPort().empty())
		throw_exception("Error: no 'listen' directive was found in server block.");
	if (virtualHost.getRoot().empty())
			throw_exception("Error: no 'root' directive was found in server block.");
	if (virtualHost.getMaxBodySize() == 0)
		virtualHost.setMaxBodySize(1 << 21);

	_locationPaths.clear();

	return (virtualHost);
}

void	ConfigParser::mountConfigVec( std::vector<VirtualHostConfig> & configs )
{
	while(curr_token().first != TOKEN_END)
		configs.push_back(parseVirtualHost());
	if (configs.empty())
		throw  std::runtime_error("Empty configuration file.");
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
		throw_exception("Error: expected a path after location directive.");

	local.setPath(next_token().second);
	advance_token(2);

	if (curr_token().first != TOKEN_L_BRACE)
		throw_exception("Error: location block needs '{' before directives.");

	advance_token(1);

	while (curr_token().first != TOKEN_R_BRACE)
	{
		if (curr_token().first == TOKEN_WORD)
		{
			std::map<std::string, ParseLocation>::iterator iter = _parseLocation.find(curr_token().second);

			if (iter != _parseLocation.end())
				(this->*(iter->second))(local);
			else
				throw_exception("Error: Invalid directive in Location block.");
		}
		else
			throw_exception("Error: expected word in location block.");
	}

	std::list<std::string>::iterator iter = _locationPaths.begin();

	while(iter != _locationPaths.end())
	{
		if (*iter == local.getPath())
			throw_exception("Error: more then one Location block to the same path.");
		iter++;
	}
	if (local.getAllowedMethods().empty())
	{
		local.addAllowedMethod("GET");
		local.addAllowedMethod("POST");
		local.addAllowedMethod("DELETE");
		local.addAllowedMethod("HEAD");
	}
	if (local.getCgiExtension().empty() != local.getCgiPath().empty())
		throw_exception("Error: both 'cgi_extension' and 'cgi_path' must be assigned.");
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
	if (!vec.getVecHostIp().empty())
		throw_exception("Error: more then one 'listen' directive.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: you must assingn a valid value to 'listen' directive.");

	std::string	listen_val = next_token().second;
	advance_token(1);

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: listen directive must be ended by ';'.");

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
				throw_exception("Error: invalid ip format.");
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
				throw_exception("Error: invalid port format.");
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
			throw_exception("Error: invalid ip-port format.");
	}
	advance_token(2);
}

void	ConfigParser::handleClientMaxBodySize( VirtualHostConfig& vec )
{
	long		parsedValue;
	std::string	value;
	std::size_t	scale = 0;
	char		*end;

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'client_max_body_size' directive needs a valid value.");

	if (vec.getMaxBodySize() != 0)
		throw_exception("Error: 'client_max_body_size' duplicate ditective in server block.");

	advance_token(1);

	value = curr_token().second;
	errno = 0;
	parsedValue = std::strtol(value.c_str(), &end, 10);
	if (parsedValue <= 0 || std::strlen(end) > 1 || errno)
		throw_exception("Error: 'client_max_body_size' directive needs a valid value.");
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
			throw_exception("Error: invalid scale at 'client_max_body_size' directive.");
	}

	std::size_t maxBodySize = static_cast<std::size_t>(parsedValue);

	if ((static_cast<size_t>(-1) / scale) + 1 < maxBodySize)
		throw_exception("Error: too large size at 'client_max_body_size' directive.");

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: missing ';' at the end of 'client_max_body_size' directive.");

	advance_token(2);
	vec.setMaxBodySize(maxBodySize * scale);
}

void	ConfigParser::handleErrorPage( VirtualHostConfig& vec )
{
	char				*end;
	long				num;
	std::list<int>		errors;

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'error_page' directive needs a value.");

	advance_token(1);

	while (true)
	{
		num = std::strtol(curr_token().second.c_str(), &end, 10);
		if (*end != '\0' || curr_token().first != TOKEN_WORD)
			break ;
		if (num < 300 || num > 599)
			throw_exception("Error: invalid error code in 'error_page' directive.");

		errors.push_back(static_cast<int>(num));
		advance_token(1);
	}

	if ( errors.empty() || curr_token().first != TOKEN_WORD || next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: wrong sintaxe for 'error_page' directive.");

	std::list<int>::iterator iter = errors.begin();
	while (iter != errors.end())
	{
		if (vec.getErrorPages().find(*iter) == vec.getErrorPages().end())
			vec.addErrorPage(*iter, curr_token().second);
		else
			throw_exception("Error: it looks like there is a duplicate value in 'error_page' directive.");
		iter++;
	}

	advance_token(2);
}

void	ConfigParser::handleRoot( VirtualHostConfig& vec )
{
	if (!vec.getRoot().empty())
		throw_exception("Error: duplicate 'root' directive in server block.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'root' directive needs a valid value.");

	advance_token(1);

	vec.setRoot(curr_token().second);
	if (next_token().first != TOKEN_SEMICOLON)
		throw  std::runtime_error("Error: missing ';' at the end of 'root' directive.");
	advance_token(2);
}

void	ConfigParser::handleReturn( VirtualHostConfig& vec )
{
	char	*err;
	long	code;

	if (vec.getReturn().first != 0)
		throw_exception("Error: duplicate 'return' directive in server block.");

	advance_token(1);

	if (curr_token().first != TOKEN_WORD || next_token().first != TOKEN_WORD)
		throw_exception("Error: 'return' directive needs two valids arguments.");

	errno = 0;
	code = std::strtol(curr_token().second.c_str(), &err, 10);

	if (errno || *err)
		throw_exception("Error: couldn't convert the return directive code in the server block.");
	if (code < 300 || code > 599)
		throw_exception("Error: invalid code in 'return' directive.");

	vec.setReturn(std::make_pair(static_cast<int>(code), next_token().second));
	advance_token(2);

	if (curr_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'return' takes exactly two argument or is missing ';'.");
	advance_token(1);
}


void	ConfigParser::handleServerName( VirtualHostConfig& vec )
{
	if (next_token().first != TOKEN_WORD)
		throw  std::runtime_error("Error: 'server_name' requires at least one argument.");

	advance_token(1);
	while (curr_token().first == TOKEN_WORD)
	{
		vec.addServerName(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
		throw  std::runtime_error("Error: missing ';' at the end of 'server_name' directive.");
	advance_token(1);
}

void	ConfigParser::handleIndex( VirtualHostConfig& vec )
{
	if (next_token().first != TOKEN_WORD)
		throw  std::runtime_error("Error: 'index' requires at least one argument.");

	advance_token(1);
	while (curr_token().first == TOKEN_WORD)
	{
		vec.addIndex(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
		throw  std::runtime_error("Error: missing ';' at the end of 'index' directive.");
	advance_token(1);
}

void	ConfigParser::handleAllowedMethods( VirtualHostConfig& vec )
{
	const char							*allowed[] = {"GET", "DELETE", "POST", 0};
	bool								flag;
	const std::vector<std::string> &	tmp = vec.getAllowedMethods();

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'allow_methods' directive needs a valid value.");

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
						throw_exception("Error: duplicate atribute in 'allow_methods' directive in server block.");
				}
				vec.addAllowedMethod(curr_token().second);
			}
		}
		if (!flag)
			throw_exception("Error: invalid atribute in 'allow_methods' directive in server block.");
		advance_token(1);
	}
	if (curr_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: missing ';' at the end of 'allow_methods' directive in server block.");
	advance_token(1);
}
void	ConfigParser::handleUploadPath( VirtualHostConfig& vec )
{
	if (!vec.getUploadPath().empty())
		throw_exception("Error: duplicate 'upload_path' directive in server block.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'upload_path' directive requires a valid argument.");

	advance_token(1);

	vec.setUploadPath(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'upload_path' takes exactly one argument or is missing ';'.");

	advance_token(2);
}

void	ConfigParser::handleLocationRoot( Location& loc )
{
	if (!loc.getRoot().empty())
		throw_exception("Error: duplicate 'root' directive in location.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'root' directive requires a valid argument.");

	advance_token(1);

	loc.setRoot(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'root' directive takes exactly one argument or is missing ';'.");

	advance_token(2);
}

void	ConfigParser::handleLocationIndex( Location& loc )
{
	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'index' requires at least one argument in location.");

	advance_token(1);

	while (curr_token().first == TOKEN_WORD)
	{
		loc.addIndex(curr_token().second);
		advance_token(1);
	}

	if (curr_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: missing ';' at the end of 'index' directive in location.");

	advance_token(1);
}

void	ConfigParser::handleLocationAllowedMethods( Location& loc )
{
	const char								*allowed[] = {"GET", "DELETE", "POST", "HEAD", 0};
	bool									flag;
	const std::vector<std::string> &		tmp = loc.getAllowedMethods();

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'allow_methods' directive needs a valid value in location block.");

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
						throw_exception("Error: duplicate attribute in 'allow_methods' directive in location block.");
				}
				loc.addAllowedMethod(curr_token().second);
			}
		}
		if (!flag)
			throw_exception("Error: invalid attribute in 'allow_methods' directive in location block.");

		advance_token(1);
	}
	if (curr_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: missing ';' at the end of 'allow_methods' directive in location block.");

	advance_token(1);
}

void	ConfigParser::handleLocationErrorPage( Location& local )
{
	char				*end;
	long				num;
	std::list<int>		errors;

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'error_page' directive needs a value.");

	advance_token(1);

	while (true)
	{
		num = std::strtol(curr_token().second.c_str(), &end, 10);
		if (*end != '\0' || curr_token().first != TOKEN_WORD)
			break ;
		if (num < 300 || num > 599)
			throw_exception("Error: invalid error code in 'error_page' directive.");

		errors.push_back(static_cast<int>(num));
		advance_token(1);
	}

	if ( errors.empty() || curr_token().first != TOKEN_WORD || next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: wrong sintaxe for 'error_page' directive.");

	std::list<int>::iterator iter = errors.begin();
	while (iter != errors.end())
	{
		if (local.getErrorPages().find(*iter) == local.getErrorPages().end())
			local.addErrorPage(*iter, curr_token().second);
		else
			throw_exception("Error: it looks like there is a duplicate value in 'error_page' directive.");
		iter++;
	}

	advance_token(2);
}

void	ConfigParser::handleLocationAutoindex( Location& loc )
{
	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'autoindex' directive requires a valid argument.");

	advance_token(1);

	std::string state = curr_token().second;

	if (state == "on")
		loc.setAutoindex(true);
	else if (state == "off")
		loc.setAutoindex(false);
	else
		throw_exception("Error: 'autoindex' must be 'on' or 'off'.");

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'autoindex' takes exactly one argument or is missing ';'.");

	advance_token(2);
}

void	ConfigParser::handleLocationUploadPath( Location& loc )
{
	if (!loc.getUploadPath().empty())
		throw_exception("Error: duplicate 'upload_path' directive.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'upload_path' directive requires a valid argument.");

	advance_token(1);

	loc.setUploadPath(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'upload_path' takes exactly one argument or is missing ';'.");

	advance_token(2);
}

void	ConfigParser::handleLocationCgiExtension( Location& loc )
{
	if (!loc.getCgiExtension().empty())
		throw_exception("Error: duplicate 'cgi_extension' directive.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'cgi_extension' directive requires a valid argument.");

	advance_token(1);

	if (curr_token().second != ".py" && curr_token().second != ".php")
		throw_exception("Error: invalid CGI extension, only '.php' and '.py' are accepted.");
	loc.setCgiExtension(curr_token().second);
	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'cgi_extension' takes exactly one argument or is missing ';'.");

	advance_token(2);
}

void	ConfigParser::handleLocationCgiPath( Location& loc )
{
	if (!loc.getCgiPath().empty())
		throw_exception("Error: duplicate 'cgi_path' directive.");

	if (next_token().first != TOKEN_WORD)
		throw_exception("Error: 'cgi_path' directive requires a valid argument.");

	advance_token(1);

	loc.setCgiPath(curr_token().second);

	if (next_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'cgi_path' takes exactly one argument or is missing ';'.");

	advance_token(2);
}


void	ConfigParser::handleLocationReturn( Location& loc )
{
	char	*err;
	long	code;

	if (loc.getReturn().first != 0)
		throw_exception("Error: duplicate 'return' directive in location block.");

	advance_token(1);

	if (curr_token().first != TOKEN_WORD || next_token().first != TOKEN_WORD)
		throw_exception("Error: 'return' directive needs two valids arguments.");

	errno = 0;
	code = std::strtol(curr_token().second.c_str(), &err, 10);

	if (errno || *err)
		throw_exception("Error: couldn't convert the return directive code in the server block.");
	if (code < 300 || code > 599)
		throw_exception("Error: invalid code in 'return' directive.");

	loc.setReturn(std::make_pair(static_cast<int>(code), next_token().second));
	advance_token(2);

	if (curr_token().first != TOKEN_SEMICOLON)
		throw_exception("Error: 'return' takes exactly two argument or is missing ';'.");
	advance_token(1);
}

std::set<std::pair<std::string, std::string> >	ConfigParser::extractListen( std::vector<VirtualHostConfig> & conf )
{
	std::stringstream						ss;
	std::set<std::pair<std::string, std::string> >	listens;
	std::string								ip;
	int										port;


	for (size_t i = 0; i < conf.size(); i++)
	{
		for (size_t j = 0; j < conf[i].getVecHostIp().size(); j++)
		{
			ip = conf[i].getHostIp(j);
			port = conf[i].getPort(j);
			ss << port;
			listens.insert(std::make_pair(ip, ss.str()));
			ss.str("");
			ss.clear();
		}
	}
	return (listens);
}

void	ConfigParser::throw_exception( const char* str )
{
	size_t		i = 0;
	size_t		newline_counter = 1;
	std::string	word = curr_token().second;

	if (word.empty())
		word = "}";
	if (curr_token().first == TOKEN_END)
		newline_counter--;
	while (i <= _pos)
	{
		i++;
		if (_tokens[i].first == TOKEN_NEWLINE)
			newline_counter++;
	}
	std::cerr << "A problem occurred at line " << newline_counter << " near '" << word << "'." << std::endl;
	throw std::runtime_error(str);
}
