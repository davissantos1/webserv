/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/17 07:00:10 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler()
{

}

StaticFileHandler::~StaticFileHandler()
{

}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other)
{
	if (this != &other)
		*this = other;
}

StaticFileHandler&	StaticFileHandler::operator=(const StaticFileHandler& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Davi
