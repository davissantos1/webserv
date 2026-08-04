/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/04 19:41:05 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <string>
# include <unistd.h>
# include <utility>
# include <vector>

class HttpRequest;
class VirtualHostConfig;

class CgiHandler
{
	public:
		CgiHandler();
		~CgiHandler();
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);

		std::pair<int, enum FdIoType>				handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		std::vector<std::pair<int, enum FdIoType> >	handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		void										handleDelete(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
};

#endif
