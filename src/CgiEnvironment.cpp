/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 16:57:14 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiEnvironment.hpp"

CgiEnvironment::CgiEnvironment()
{

}

CgiEnvironment::~CgiEnvironment()
{

}

CgiEnvironment::CgiEnvironment(const CgiEnvironment& other)
{
	if (this != &other)
		*this = other;
}

CgiEnvironment&	CgiEnvironment::operator=(const CgiEnvironment& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Danilo
