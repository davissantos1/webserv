/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 09:08:33 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "VirtualHostConfig.hpp"
#include <cstdio>
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

	while (!file.eof())
	{
		getline(file, line);

		ss.str("");
		ss << line;
		getline(ss, token_text, ' ');

		if (token_text.find('#') != std::string::npos)
			std::cout << "tratar # no meio da linha --nao_esquecer" << std::endl;

		if (token_text.length() > 1)
			{}
	}
}
