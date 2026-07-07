/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 23:45:07 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 08:50:58 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

enum Status;
{
	REQUEST_PROCESSING,
	RESPONSE_PROCESSING,
	CGI_PROCESSING,
	REQUEST_READY,
	RESPONSE_READY,
	CGI_READY
};

class Client
{
	private:
		enum Status;
		int	clientFd;
	public:
		Client();
		~Client();
		Client(const Client& other);
		Client&	operator=(const Client& other);
};

#endif
