/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 16:08:48 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/06 21:10:18 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
	try
	{
		Parser parser;
		Server server(parser.parseConfigFile(configPath));
		server.startServer();
	}
	catch (std::exception& e)
	{
		std::cerr	<< e.what() << std::endl;
		return (1);
	}
	server.loopServer();
	return (0);
}
