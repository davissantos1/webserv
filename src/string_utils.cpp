/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 12:01:55 by vitosant          #+#    #+#             */
/*   Updated: 2026/08/09 12:01:55 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>
#include <vector>

std::vector<std::string> split( const std::string& str, const char spliter )
{
	std::vector<std::string>	splited;
	std::size_t					i = 0;
	std::size_t					pos = 0;

	while ((i = str.find(spliter, pos)) != std::string::npos)
	{
		splited.push_back(str.substr(pos, i - pos));
		pos = i;
		while (pos < str.length() && str[pos] == spliter)
			pos++;
	}
	splited.push_back(str.substr(pos));
	return (splited);
}

void trimStr( std::string& src )
{
	const std::string&	blank = "\t\n\v\f\r ";
	int					start = 0;
	int					end = 0;
	int					len;

	if (src.empty())
		return ;
	start = src.find_first_not_of(blank);
	end = src.find_last_not_of(blank);
	len = end - start + 1;
	if (start < 0 || end < 0)
		src.clear();
	else
		src.assign(src.substr(start, len));
}
