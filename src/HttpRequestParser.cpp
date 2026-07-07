/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 09:08:33 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser()
{

}

HttpRequestParser::~HttpRequestParser()
{

}

HttpRequestParser::HttpRequestParser(const HttpRequestParser& other)
{
	if (this != &other)
		*this = other;
}

HttpRequestParser&	HttpRequestParser::operator=(const HttpRequestParser& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Vitor
