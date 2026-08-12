/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 16:33:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/11 21:45:16 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal>

# define TIMEOUT 15.0
# define EPOLL_WAIT 1000

enum FdIoType
{
	STATIC_FILE_READ,
	STATIC_FILE_WRITE,
	CGI_READ,
	CGI_WRITE
};

extern volatile sig_atomic_t serverRunning;

#endif
