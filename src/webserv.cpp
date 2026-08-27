/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:08:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/27 18:05:30 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
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
	std::string configPath;
	if (ac > 2)
	{
		std::cerr	<< "Wrong number of arguments!\n"
					<< "Please type in ./webserv or ./webserv <configuration.conf>\n"
					<< std::endl;
		return (1);
	}
	if (ac == 1)
		configPath = "/usr/local/etc/webserv.conf";
	else
		configPath = av[1];
	registerSignals();
	try
	{
		ConfigParser configParser;
		Server server(configParser.parse(configPath));
		server.startServer();
		server.runServer();
	}
	catch (std::exception& e)
	{
		std::cerr	<< e.what() << std::endl;
		return (1);
	}
	return (0);
}
