/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/01 15:21:20 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <map>

class HttpRequest
{
	private:
		std::string 						_method;
		std::string 						_uri;
		std::string 						_version;
		std::map<std::string, std::string>	_headers;
		std::string 						_body;
		bool								_keepAlive;
	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		std::string	getMethod() { return this->_method }
		std::string	getVersion() { return this->_version }
		std::string	getUri() { return this->_uri }
		std::map<std::string, std::string> getHeaders() { return this->_headers }
		std::string*	getBody() { return &(this->_body) }
		int			getBodySize() { return this->_body.size(); }
};

#endif
