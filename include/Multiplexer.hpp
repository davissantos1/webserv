/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 23:51:38 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

# define MAXEVENTS 10
# define _GNU_SOURCE

# include "webserv.hpp"
# include <sys/epoll.h> 
# include <stdint.h> 

enum FdType
{
	SOCKET,
	CLIENT
};

class Server;

class Multiplexer
{
	private:
		int	_epollFd;
	public:
		Multiplexer();
		~Multiplexer();
		Multiplexer(const Multiplexer& other);
		Multiplexer& operator=(const Multiplexer& other);
		void	eventLoop(Server* server);
		class MultiplexerException: std::exception
		{
			private:
				int	_errno;
			public
				MultiplexerException(int err): _errno(err) {}
				const char* what() const throw { return std::strerror(_errno); }
		};
};

#endif
