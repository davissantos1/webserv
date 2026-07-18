/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/16 08:05:34 by dasimoes         ###   ########.fr       */
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
		this->_statusCode = other._statusCode;
		this->_version = other._version;
		this->_reasonPhrase = other._reasonPhrase;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return (*this);
}
