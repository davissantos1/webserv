/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/16 08:09:09 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>

class HttpResponse
{
	private:
		int									_statusCode;
		std::string							_version;
		std::string							_reasonPhrase;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);

		int	getStatusCode() { return this->_statusCode }
		std::string	getVersion() { return this->_version }
		std::string	getReasonPhrase() { return this->_reasonPhrase }
		std::map<std::string, std::string> getHeaders() { return this->_headers }
		std::string	getBody() { return this->_body }
};

#endif
