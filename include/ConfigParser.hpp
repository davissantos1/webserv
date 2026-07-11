/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/11 12:15:40 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "Location.hpp"
#include "VirtualHostConfig.hpp"
#include <cstddef>
#include <fstream>
#include <deque>
#include <string>
#include <utility>
#include <vector>

typedef enum e_tokens
{
	TOKEN_L_BRACE,
	TOKEN_R_BRACE,
	TOKEN_HASH,
	TOKEN_SEMICOLON,
	TOKEN_NEWLINE,
	TOKEN_WORD
}	t_file_tokens;

class ConfigParser
{
	private:
		std::deque< std::pair<t_file_tokens, std::string> >	_tokens;
		std::string											_filePath;
		std::size_t											_pos;

		void				makeTokens( std::ifstream& file );
		bool				analyseTokens( void );

		Location			parseLocation( void );
		VirtualHostConfig	parseVirtHost( void );
		void				mountConfigVec( std::vector<VirtualHostConfig> & configs );

	public:
		ConfigParser( void );
		~ConfigParser( void );
		ConfigParser(const ConfigParser& other);
		ConfigParser& operator=(const ConfigParser& other);

		std::vector<VirtualHostConfig> parse( const std::string& path );
};

#endif
