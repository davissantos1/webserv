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
#include <cstdlib>
#include <string>
#include <vector>

std::vector<std::string>	split( const std::string& str, const char spliter )
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

void	trimStr( std::string& src )
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

std::string	decode_str( const std::string& str )
{
	std::string ret;
	std::size_t	k = str.length();
	std::string	set;

	ret.reserve(k);
	set.reserve(2);
	for(size_t i = 0; i < k; i++)
	{
		if (str[i] == '%' && i + 2 < k)
		{
			char	*end;

			set[0] = str[i + 1];
			set[1] = str[i + 2];
			ret += static_cast<char>(strtol(set.c_str(), &end, 16));
		}
		else if (str[i] == '+')
			ret += ' ';
		else
			ret += str[i];
	}
	return (ret);
}
