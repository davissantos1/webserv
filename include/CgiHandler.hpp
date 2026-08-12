/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/12 14:46:06 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "webserv.hpp"
# include "CgiEnvironment.hpp"
# include <stdint.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/wait.h>

class VirtualHostConfig;
class HttpResponseBuilder;
class HttpRequest;

class CgiHandler
{
	private:
		CgiEnvironment	_cgiEnvironment;
		int				_stat_loc;
	public:
		CgiHandler();
		~CgiHandler();
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);

		bool										processCgi(int fd, uint32_t eventType, HttpResponseBuilder& builder);
		std::pair<int, enum FdIoType>				handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		std::vector<std::pair<int, enum FdIoType> >	handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
};

#endif
