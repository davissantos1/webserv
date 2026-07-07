/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 15:51:45 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Multiplexer.hpp"

Multiplexer::Multiplexer()
{

}

Multiplexer::~Multiplexer()
{

}

Multiplexer::Multiplexer(const Multiplexer& other)
{
	if (this != &other)
		*this = other;
}

Multiplexer&	Multiplexer::operator=(const Multiplexer& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}
