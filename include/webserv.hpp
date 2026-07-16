/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 16:33:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/16 06:20:57 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <csignal>

# define TIMEOUT 15.0
# define EPOLL_WAIT 1000

extern volatile sig_atomic_t serverRunning;

#endif
