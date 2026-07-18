/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/12 15:40:53 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Location.hpp"
#include "VirtualHostConfig.hpp"
#include <cstddef>
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
		if (analyseTokens() == true)
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

bool	ConfigParser::analyseTokens( void ) const
{
	return (false);
}

Location	ConfigParser::parseLocation( void ) const
{
	return (Location());
}

VirtualHostConfig	ConfigParser::parseVirtHost( void ) const
{
	return (VirtualHostConfig());
}

void	ConfigParser::mountConfigVec( std::vector<VirtualHostConfig> & configs )
{
	while(curr_token().first == TOKEN_NEWLINE)
	{
		while (curr_token().second == "server")
			configs.push_back(parseVirtHost());
		_pos++;
	}

	if (_tokens.size() == 1)
		std::cerr << "Empty configuration file." << std::endl;

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
