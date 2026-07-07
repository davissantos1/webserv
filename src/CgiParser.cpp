/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 15:28:28 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiParser.hpp"

CgiParser::CgiParser()
{

}

CgiParser::~CgiParser()
{

}

CgiParser::CgiParser(const CgiParser& other)
{
	if (this != &other)
		*this = other;
}

CgiParser&	CgiParser::operator=(const CgiParser& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Vitor
