/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 09:08:33 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler()
{

}

CgiHandler::~CgiHandler()
{

}

CgiHandler::CgiHandler(const CgiHandler& other)
{
	if (this != &other)
		*this = other;
}

CgiHandler&	CgiHandler::operator=(const CgiHandler& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Danilo
