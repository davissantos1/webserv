/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/12 15:40:51 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "Location.hpp"
#include "VirtualHostConfig.hpp"
#include <cstddef>
#include <fstream>
#include <deque>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

typedef enum e_tokens
{
	TOKEN_L_BRACE,
	TOKEN_R_BRACE,
	TOKEN_SEMICOLON,
	TOKEN_NEWLINE,
	TOKEN_WORD,
	TOKEN_END
}	t_file_tokens;

class ConfigParser
{
	private:
		typedef	void (ConfigParser::*ParseServer)(VirtualHostConfig&);
		typedef	void (ConfigParser::*ParseLocation)(Location&);

		std::map<std::string, ParseServer>					_parseServer;
		std::map<std::string, ParseLocation>				_parseLocation;
		std::deque< std::pair<t_file_tokens, std::string> >	_tokens;
		std::string											_filePath;
		std::size_t											_pos;
		bool												_flagErr;

		void				makeTokens( std::ifstream& file );

		VirtualHostConfig	parseVirtualHost( void );
		void				mountConfigVec( std::vector<VirtualHostConfig> & configs );

		std::pair<t_file_tokens, std::string> & curr_token( void );
		std::pair<t_file_tokens, std::string> & next_token( void );

		void	handleLocation( VirtualHostConfig& vec );
		void	handleListen( VirtualHostConfig& vec );
		void	handleClientMaxBodySize( VirtualHostConfig& vec );
		void	handleErrorPage( VirtualHostConfig& vec );

		void	handleLocationRoot( Location& loc );
		void	handleLocationIndex( Location& loc );
		void	handleLocationAllowedMethods( Location& loc );
		void	handleLocationAutoindex( Location& loc );

		bool	validateIp( const std::string& ip );
		bool	validatePort( const std::string& port );

	public:
		ConfigParser( void );
		~ConfigParser( void );
		ConfigParser(const ConfigParser& other);
		ConfigParser& operator=(const ConfigParser& other);

		std::vector<VirtualHostConfig> parse( const std::string& path );
		const std::deque< std::pair<t_file_tokens, std::string> >&	getTokens( void ) const;
};

std::ostream&	operator<<( std::ostream& out, const ConfigParser & tokens );

#endif
