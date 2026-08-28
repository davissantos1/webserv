/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 16:33:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/28 01:43:19 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <csignal>

# define TIMEOUT 30.0
# define EPOLL_WAIT 1000

enum CgiIoType
{
	CGI_READ,
	CGI_WRITE
};

enum StaticFileIoType
{
	STATIC_FILE_READ,
	STATIC_FILE_WRITE
};

extern volatile sig_atomic_t serverRunning;

#endif
