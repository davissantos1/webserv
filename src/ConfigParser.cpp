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
#include <cstdio>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

ConfigParser::ConfigParser( void ) {}

ConfigParser::~ConfigParser( void ) {}

ConfigParser::ConfigParser(const ConfigParser& other): _tokens(other._tokens), _filePath(other._filePath), _pos(other._pos){}

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
	return token;
}

void	ConfigParser::makeTokens( std::ifstream& file )
{
	std::string			line;
	std::stringstream	ss;
	std::string			token_text;
	std::size_t			pos;
	while (!file.eof())
	{
		getline(file, line);

		pos = line.find('#');
		if (pos != std::string::npos)
			line.erase(pos, line.length() - pos);

		ss.clear();
		ss.str(line);

		while (ss.eof() != false)
		{
			getline(ss, token_text, ' ');

			if (!token_text[0])
				continue ;

			if (token_text.length() > 1)
				_tokens.push_back(makePair(TOKEN_WORD, token_text));
			else
			{
				switch (token_text[0])
				{
					case '{':
						_tokens.push_back(makePair(TOKEN_L_BRACE, token_text));
						break ;
					case '}':
						_tokens.push_back(makePair(TOKEN_R_BRACE, token_text));
						break ;
					case ';':
						_tokens.push_back(makePair(TOKEN_SEMICOLON, token_text));
						break ;
					default:
						_tokens.push_back(makePair(TOKEN_WORD, token_text));
				}
			}
		}
		_tokens.push_back(makePair(TOKEN_NEWLINE, ""));
	}
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
	configs.push_back(parseVirtHost());
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
		out << tokens.getTokens()[i].second << " ----- ";
		i++;
	}
	return (out);
}
