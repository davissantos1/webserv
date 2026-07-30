/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/29 14:29:26 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include "client.hpp"

class StaticFileHandler
{
	private:
		int	_statusCode;
	public:
		StaticFileHandler();
		~StaticFileHandler();
		StaticFileHandler(const StaticFileHandler& other);
		StaticFileHandler& operator=(const StaticFileHandler& other);

		int		getStatusCode() { return this->_statusCode; }
		void	setStatusCode(statusCode) { this->_statusCode = statusCode; }

		std::pair<int, enum FdIoType>	handleGet(std::string& uri);
		std::pair<int, enum FdIoType>	handlePost(); // add parameters
		void							handleDelete(std::string& uri);	
};

#endif
