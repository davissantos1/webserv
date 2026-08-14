/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 19:25:20 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include "webserv.hpp"
# include "HttpRequest.hpp"
# include "HttpResponseBuilder.hpp"
# include "VirtualHostConfig.hpp"
# include <string>
# include <stdint.h>
# include <utility>
# include <vector>
# include <cerrno>
# include <ctime>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <dirent.h>
# include <cstdio>

class StaticFileHandler
{
	public:
		StaticFileHandler();
		~StaticFileHandler();
		StaticFileHandler(const StaticFileHandler& other);
		StaticFileHandler& operator=(const StaticFileHandler& other);

		std::vector<std::pair<int, enum FdIoType> >		handleException(int exception, std::string path);
		std::pair<int, enum FdIoType>					handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		std::vector<std::pair<int, enum FdIoType> >	handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		void											handleDelete(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);	
		bool											processStaticFile(int fd, uint32_t eventType, HttpResponseBuilder& builder);
		int												handleAutoindex(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);
};

#endif
