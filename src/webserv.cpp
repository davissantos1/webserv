/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:08:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/13 18:46:40 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"
#include "Multiplexer.hpp"
#include <iostream>
#include <string>

volatile sig_atomic_t serverRunning = 0;

void	panic(int sig)
{
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT)
			serverRunning = 0;
}

void	registerSignals()
{
	std::signal(SIGINT, panic);
	std::signal(SIGTERM, panic);
	std::signal(SIGQUIT, panic);
	std::signal(SIGPIPE, SIG_IGN);
}

int	main(int ac, char** av)
{
	if (ac > 2)
	{
		std::cerr	<< "Wrong number of arguments!\n"
					<< "Please type in ./webserv or ./webserv <configuration.conf>\n"
					<< std::endl;
		return (1);
	}
	if (ac == 1)
		std::string configPath("/usr/local/etc/webserv.conf");
	else
		std::string configPath(av[1]);
	registerSignals();
	try
	{
		Parser parser;
		Server server(parser.parseConfigFile(configPath));
		std::vector<int> listenFds = server.startServer();
	}
	catch (std::exception& e)
	{
		std::cerr	<< e.what() << std::endl;
		return (1);
	}
	Multiplexer mult;
	mult.eventLoop(&server);
	return (0);
}
