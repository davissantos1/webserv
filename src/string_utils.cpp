/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 12:01:55 by vitosant          #+#    #+#             */
/*   Updated: 2026/08/21 01:05:34 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

std::vector<std::string>	split( const std::string& str, const char splitter )
{
	std::vector<std::string>	splited;
	std::size_t					i = 0;
	std::size_t					pos = 0;

	if (str.empty())
		return (splited);
	while (pos < str.length() && str[pos] == splitter)
		pos++;
	while (pos < str.size() && (i = str.find(splitter, pos)) != std::string::npos)
	{
		splited.push_back(str.substr(pos, i - pos));
		pos = i;
		while (pos < str.length() && str[pos] == splitter)
			pos++;
	}
	if (pos + 1 < str.length())
		splited.push_back(str.substr(pos));
	return (splited);
}

std::string	reverse_split( const std::vector<std::string> & vec, const char splitter )
{
	std::string	result;

	if (vec.empty())
		return (result);
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		result += vec[i];
		if (splitter != '\0' && i != vec.size() - 1)
			result += splitter;
	}
	return (result);
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

std::string normalize_str( const std::string& raw )
{
	std::string					finalStr("/");
	std::vector<std::string>	splited = split(raw, '/');
	std::vector<std::string>	parts;


	if (raw.empty())
		return(raw);
	if (raw == "/" || splited.empty())
		return (finalStr);
	for (size_t i = 0; i < splited.size(); i++)
	{
		if (splited[i] == "..")
		{
			if (parts.size() > 0)
				parts.pop_back();
		}
		else if (splited[i] == ".")
			continue ;
		else
			parts.push_back(splited[i]);
	}
	for (size_t i = 0; i < parts.size(); i++)
		finalStr += parts[i] + "/";
	if (raw.length() > 1 && raw[raw.length() - 1] != '/' && finalStr != "/")
		finalStr.erase(finalStr.length() - 1);
	return (finalStr);
}

void strToLower( std::string& str )
{
	for(size_t i = 0; i < str.size(); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] |= 32;
	}
}
