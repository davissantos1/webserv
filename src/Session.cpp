/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 15:52:05 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"

Session::Session()
{

}

Session::~Session()
{

}

Session::Session(const Session& other)
{
	if (this != &other)
		*this = other;
}

Session&	Session::operator=(const Session& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

// to be added Danilo
