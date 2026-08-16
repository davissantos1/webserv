/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/16 20:29:23 by dasimoes         ###   ########.fr       */
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

		void	handleException(int exception, std::string path);
		void	handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		void	handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);
		void	handleDelete(HttpRequest& req, VirtualHostConfig& conf, int* statusCode);	
		int		handleAutoindex(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);
};

#endif
