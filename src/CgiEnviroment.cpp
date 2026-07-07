/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnviroment.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 09:07:34 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiEnviroment.hpp"

CgiEnviroment::CgiEnviroment()
{

}

CgiEnviroment::~CgiEnviroment()
{

}

CgiEnviroment::CgiEnviroment(const CgiEnviroment& other)
{
	if (this != &other)
		*this = other;
}

CgiEnviroment&	CgiEnviroment::operator=(const CgiEnviroment& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Danilo
