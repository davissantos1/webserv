/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 09:06:22 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse()
{

}

HttpResponse::~HttpResponse()
{

}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	if (this != &other)
		*this = other;
}

HttpResponse&	HttpResponse::operator=(const HttpResponse& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}
