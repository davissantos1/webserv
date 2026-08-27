/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/26 20:27:37 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "webserv.hpp"
# include "HttpRequest.hpp"
# include "VirtualHostConfig.hpp"
# include "HttpResponseBuilder.hpp"
# include "CgiEnvironment.hpp"
# include <cerrno>
# include <stdint.h>
# include <unistd.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/epoll.h>

class VirtualHostConfig;
class HttpResponseBuilder;
class HttpRequest;

class CgiHandler
{
	private:
		CgiEnvironment	_cgiEnvironment;
		bool			_readDone;
		bool			_writeDone;
		int				_stat_loc;
		size_t			_bytesWritten;
	public:
		CgiHandler();
		~CgiHandler();
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);

		void											reset();
		bool											processCgi(int fd, uint32_t eventType, HttpResponseBuilder& builder);
		std::pair<int, enum CgiIoType>					handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		std::vector<std::pair<int, enum CgiIoType> >	handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
};

#endif
