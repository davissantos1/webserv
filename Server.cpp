/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/04 15:34:08 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config) {}

Server::~Server()
{

}

Server::Server(const Server& other)
{

}

Server&	Sever::operator=(const Server& other)
{

}

void	Server::createConnection()
{

}

void	Server::destroyConnection()
{

}
