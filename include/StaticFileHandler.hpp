/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/21 05:37:00 by dasimoes         ###   ########.fr       */
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
	private:
		int	_fd;
	public:
		StaticFileHandler();
		~StaticFileHandler();
		StaticFileHandler(const StaticFileHandler& other);
		StaticFileHandler& operator=(const StaticFileHandler& other);

		int		getFd() { return this->_fd; } 
		void	setFd(int fd) { this->_fd = fd; }
	
		bool	isDirectory();
		void	handleGet(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);
		void	handlePost(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);
		void	handleDelete(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);	
		int		handleAutoindex(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build);
		bool	handleException(int exception, std::string path, HttpRequest& req, HttpResponseBuilder& build);
		bool	processStaticFile(int fd, enum StaticFileIoType type, HttpRequest& req, HttpResponseBuilder& build);
};

#endif
