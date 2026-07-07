/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 15:28:27 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder()
{

}

HttpResponseBuilder::~HttpResponseBuilder()
{

}

HttpResponseBuilder::HttpResponseBuilder(const HttpResponseBuilder& other)
{
	if (this != &other)
		*this = other;
}

HttpResponseBuilder&	HttpResponseBuilder::operator=(const HttpResponseBuilder& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}
