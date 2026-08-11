/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/10 19:07:08 by davi             ###   ########.fr       */
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

void			HttpResponseBuilder::buildResponse()
{
	std::stringstream ss;
	HttpResponse& res = this->_httpResponse;
	std::map<std::string, std::string>&	headers = this->_httpResponse.getHeaders();
	std::map<std::string, std::string>::iterator it = header.begin();

	ss	<< res.getVersion()	<< " "
		<< res.getStatusCode() << " "
		<< res.getReasonPhrase() << "\r\n";
	for (; it != headers.end(); it++)
	{
		ss	<< it.first << ": "
			<< it.second << "\r\n";
	}
	ss	<< "\r\n";
	this->_httpResponseHead = ss.str();
	this->_totalBytes = this->_httpResponseHead.size() + res.getBodySize();
}

void			HttpResponseBuilder::feedCgi(char *buffer, int size)
{

}

void			HttpResponseBuilder::feedStaticFile(char *buffer, int size)
{

}
