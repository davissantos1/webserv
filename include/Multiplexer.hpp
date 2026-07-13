/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 16:20:46 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

# define MAX_EVENTS 10
# define _GNU_SOURCE

# include <sys/epoll.h> 

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
