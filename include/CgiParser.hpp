/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/10 14:17:04 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPARSER_HPP
# define CGIPARSER_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <map>

enum CgiParserStatus
{
	BUILDING_HEADERS,
	BUILDING_BODY,
	READY
};

class CgiParser
{
	private:
		int									_bodySize;
		enum CgiParserStatus				_status;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
		std::string							_buffer;
	public:
		CgiParser();
		~CgiParser();
		CgiParser(const CgiParser& other);
		CgiParser& operator=(const CgiParser& other);

		std::map<std::string, std::string>	getHeaders() { return this->_headers; }
		std::string	getBody() { return this->_body; }

		bool	isCgiDone();
		void	feed(char *buffer, int size);
};

#endif
