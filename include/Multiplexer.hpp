/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplexer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 18:02:49 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

# define MAX_EVENTS 10

# include "webserv.hpp"
# include <sys/epoll.h> 
# include <stdint.h> 
# include <cstring> 
# include <cerrno> 
# include <utility> 
# include <vector> 

class Multiplexer
{
	private:
		int	_epollFd;
	public:
		Multiplexer();
		~Multiplexer();
		Multiplexer(const Multiplexer& other);
		Multiplexer& operator=(const Multiplexer& other);
		void	removeFd(int fd);
		void	addFd(int fd, uint32_t event);
		void	modifyFd(int fd, uint32_t event);
		std::vector<std::pair<int, uint32_t> >	wait();

		class	MultiplexerException: public std::exception
		{
			private:
				int	_errno;
			public:
				MultiplexerException(int err): _errno(err) {}
				virtual ~MultiplexerException() throw() {}
				const char* what() const throw() { return std::strerror(_errno); }
		};
};

#endif
