/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
<<<<<<< Updated upstream
/*   Updated: 2026/08/16 20:35:25 by dasimoes         ###   ########.fr       */
=======
/*   Updated: 2026/08/16 12:33:42 by dasimoes         ###   ########.fr       */
>>>>>>> Stashed changes
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): _currAddr(NULL) {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config), _currAddr(NULL) {}

Server::~Server() 
{
	std::map<std::string, Session*>::iterator it;

	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		Client* curr = this->_clients[i];
		if (curr)
		{
			close(curr->getFd());
			delete(curr);
		}
	}
	for (size_t j = 0; j < this->_listenFds.size(); j++)
		close(this->_listenFds[j]);
	for (it = this->_sessionMap.begin(); it != this->_sessionMap.end(); it++)
		delete(it->second);
	if (this->_currAddr)
		freeaddrinfo(this->_currAddr);
}

Server&	Server::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_configs = other._configs;
		this->_clientMap = other._clientMap;
		this->_cgiMap = other._cgiMap;
		this->_staticFileMap = other._staticFileMap;
		this->_sessionMap = other._sessionMap;
		this->_sessionClientMap = other._sessionClientMap;
		this->_clients = other._clients;
		this->_listenFds = other._listenFds;
		this->_multiplexer = other._multiplexer;
		this->_currAddr = other._currAddr;
	}
	return (*this);
}

Server::Server(const Server& other)
{
	if (this != &other)
		*this = other;
}

void	Server::startServer()
{
	int opt = 1;
	int sockFd, status;
	struct addrinfo	hints, *res, *p;
	std::set<std::pair<std::string, std::string> > interface;
	std::set<std::pair<std::string, std::string> >::iterator it;

	interface = ConfigParser::extractListen(this->_configs);
	for (it = interface.begin(); it != interface.end(); it++)
	{
		std::memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		if ((status = getaddrinfo(it->first.c_str(), it->second.c_str(), &hints, &res)) != 0)
			throw (ServerException(gai_strerror(status)));
		this->_currAddr = res;
		for (p = res; p != NULL; p = p->ai_next)
		{
			if ((sockFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
				continue;
			break ;
		}
		if (sockFd == -1)
			throw (ServerException(errno));
		this->_listenFds.push_back(sockFd);
		if ((status = setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) == -1)
			throw (ServerException(errno));
		if ((status = setsockopt(sockFd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt))) == -1)
			throw (ServerException(errno));
		if ((status = bind(sockFd, res->ai_addr, res->ai_addrlen)) == -1)
			throw (ServerException(errno));
		if ((status = listen(sockFd, BACKLOG)) == -1)
			throw (ServerException(errno));
		if ((status = fcntl(sockFd, F_SETFL, O_NONBLOCK)) == -1)
			throw (ServerException(errno));
		freeaddrinfo(this->_currAddr);
		this->_currAddr = res = NULL;
	}
	serverRunning = 1;
}

void	Server::runServer()
{
	std::vector<std::pair<int, uint32_t> > fds;
	std::vector<int>::iterator listenBegin, listenEnd, it;
	enum FdType fdType;

	while (serverRunning)
	{
		fds = this->_multiplexer.wait();
		for (size_t j = 0; j < fds.size(); j++)
		{
			listenBegin = this->_listenFds.begin();
			listenEnd = this->_listenFds.end();
			it = std::find(listenBegin, listenEnd, fds[j].first);
			if (it == listenEnd)
			{
				if (this->_clientMap.count(fds[j].first) == 0 &&
				this->_staticFileMap.count(fds[j].first) == 0 &&
				this->_cgiMap.count(fds[j].first) == 0)
					continue;
					
			}
			fdType = (it != listenEnd) ? SOCKET : CLIENT;
			if (this->_cgiMap.count(fds[j].first) > 0)
				fdType = CGI;
			else if (this->_staticFileMap.count(fds[j].first) > 0)
				fdType = STATIC_FILE;
			this->routeServer(fds[j].first, fds[j].second, fdType);
			this->checkTimeouts();
		}
	}
}

void	Server::routeServer(int fd, uint32_t eventType, enum FdType fdType)
{
	enum ClientStatus	status;

	if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
	{
		this->handleError(fd, fdType);
		return ;
	}

	switch (fdType)
	{
		case SOCKET:
		{
			if (eventType & EPOLLIN)
			{
				int newClient = this->createClient(fd);
				if (newClient != -1)
					this->_multiplexer.addFd(newClient, (EPOLLIN | EPOLLRDHUP));		
			}
			break;
		}
		case CLIENT:
		{
			Client* client = this->_clientMap[fd];
			if (!client) break;

			status = client->getStatus();
			if (eventType & EPOLLIN)
				status = client->processHttpRequest();
			else if (eventType & EPOLLOUT)
				status = client->processHttpResponse();

			if (status == DISCONNECT)
			{
				this->destroyClient(fd);
				return ;
			}
			if (status == PROCESSING_STATIC_FILE || status = PROCESSING_EXCEPTION)
			{
				this->_multiplexer.removeFd(client->getFd());
				if (status == PROCESSING_STATIC_FILE)
					this->handleSession(client);
				client->executeStaticMethod();
				client->setStatus(PREPARING_RESPONSE);
			}
			if (status == PROCESSING_CGI)
			{
				this->_multiplexer.removeFd(client->getFd());
				this->handleSession(client);
				std::vector<std::pair<int, enum CgiIoType> > tasks = client->executeCgiMethod();
				for (size_t i = 0; i < tasks.size(); i++)
				{
					switch (tasks[i].second)
					{
						case CGI_READ:
							this->_cgiMap[tasks[i].first] = client;
							this->_multiplexer.addFd(tasks[i].first, EPOLLIN | EPOLLRDHUP);
							break;
						case CGI_WRITE:
							this->_cgiMap[tasks[i].first] = client;
							this->_multiplexer.addFd(tasks[i].first, EPOLLOUT | EPOLLRDHUP);
							break;
					}
					client->registerFd(tasks[i].first);
				}
			}
			status = client->getStatus();
			if (status == PREPARING_RESPONSE)
				this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);
			if (status == SENT_RESPONSE)
			{
				client->setStatus(READING_REQUEST);
				this->_multiplexer.removeFd(client->getFd());
				this->_multiplexer.addFd(client->getFd(), EPOLLIN | EPOLLRDHUP);
			}
			break;
		}
		case CGI:
		{
			Client* client = this->_cgiMap[fd];
			if (!client) break;
			CgiHandler& cgi = client->getCgiHandler();
			HttpResponseBuilder& builder = client->getHttpResponseBuilder();

			bool isPipeDone = cgi.processCgi(fd, eventType, builder);
			if (isPipeDone)
				this->handleProcessedFile(client, builder.getStatusCode(), CGI);
			client->setLastActivity(std::time(NULL));
			break;
		}
	}
}

void	Server::handleProcessedFile(Client* client, int statusCode, enum FdType type)
{
	HttpRequest& req = client->getHttpRequest();
	std::vector<std::pair<int, enum CgiIoType> > errorFd;
	StaticFileHandler& stat = client->getStaticFileHandler();
	VirtualHostConfig& conf = client->getVirtualHostConfig();
	std::vector<int> activeFds = client->getActiveFds();
	for (size_t i = 0; i < activeFds.size(); i++)
	{
		this->_multiplexer.removeFd(activeFds[i]);
		if (type == CGI)
			this->_cgiMap.erase(activeFds[i]);
		else
			this->_staticFileMap.erase(activeFds[i]);
	}
	client->destroyActiveFds();
	if (statusCode > 299)
	{
		client->setStatus(PROCESSING_STATIC_FILE);
		errorFd = stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint())); 
		this->_multiplexer.addFd(errorFd[0].first, EPOLLIN | EPOLLRDHUP);
		this->_staticFileMap[errorFd[0].first] = client;
		client->registerFd(errorFd[0].first);
	}	
	else
	{
		client->setStatus(PREPARING_RESPONSE);
		this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);
	}
}

void	Server::handleError(int fd, enum FdType fdType)
{
	switch (fdType)
	{
		case SOCKET:
			this->_listenFds.erase(std::remove(this->_listenFds.begin(), this->_listenFds.end(), fd));
			close(fd);
			Server::printLog("socket fd removed from connection");
			break;
		case CLIENT:
			this->destroyClient(fd);
			break;
		case STATIC_FILE:
		{
			Client* client = this->_staticFileMap[fd];

			this->_multiplexer.removeFd(fd);
			this->_staticFileMap.erase(fd);
			close(fd);
			client->setStatusCode(500);
			client->setStatus(PREPARING_RESPONSE);
			this->_multiplexer.addFd(client->getFd(), EPOLLOUT);
			Server::printLog("static file presented an error");
			break;
		}
		case CGI:
		{
			Client* client = this->_cgiMap[fd];

			this->_multiplexer.removeFd(fd);
			this->_cgiMap.erase(fd);
			close(fd);
			client->destroyCgi(fd);
			client->setStatusCode(500);
			client->setStatus(PREPARING_RESPONSE);
			this->_multiplexer.addFd(client->getFd(), EPOLLOUT);
			Server::printLog("CGI FD removed due to internal error");
			break;
		}
	}
}

int	Server::createClient(int sockFd)
{
	std::string ipStr;
	struct sockaddr_storage addr;
	socklen_t addr_len = sizeof(addr);
	int clientFd;
	uint16_t	port = 0;
	uint32_t	ip = 0;

	if ((clientFd = accept(sockFd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		Server::printLog("accept error on client!");
		return (clientFd);
	}
	if (addr.ss_family == AF_INET)
	{
		struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(&addr);
		port = ntohs(addr_in->sin_port);
		ip = ntohl(addr_in->sin_addr.s_addr);
		std::stringstream ipStream;
		ipStream	<< ((ip >> 24) & 0xFF) << "."
					<< ((ip >> 16) & 0xFF) << "."
					<< ((ip >> 8) & 0xFF) << "."
					<< ((ip & 0xFF));
		ipStr = ipStream.str();
	}
	Client* newClient = new Client(ipStr, port, clientFd, &this->_configs);
	this->_clients.push_back(newClient);
	this->_clientMap[clientFd] = newClient;
	return (clientFd);
}

void	Server::destroyClient(int clientFd)
{
	Client* client = this->_clientMap[clientFd];
	if (!client) return ;

	this->_multiplexer.removeFd(clientFd);
	this->_clientMap.erase(clientFd);
	this->_sessionClientMap.erase(client->getSession());
	this->_clients.erase(std::remove(this->_clients.begin(), this->_clients.end(), client), this->_clients.end());
	std::vector<int> activeFds = client->getActiveFds();
	for (size_t i = 0; i < activeFds.size(); i++)
	{
		int fd = activeFds[i];
		this->_multiplexer.removeFd(fd);
		this->_staticFileMap.erase(fd);
		this->_cgiMap.erase(fd);
		close (fd);
	}
	close(clientFd);
	delete (client);
}

void	Server::checkTimeouts()
{
	std::time_t currentTime = std::time(NULL);
	std::map<std::string, Session*>::iterator it;
	Session* currentSession;
	Client* currentClient;
	double secondsIdle;

	for (size_t i = 0; i < this->_clients.size();)
	{
		currentClient = this->_clients[i];
		secondsIdle = std::difftime(currentTime, currentClient->getLastActivity());

		if (secondsIdle > TIMEOUT)
		{
			Server::printLog("client timed out!");
			this->destroyClient(currentClient->getFd());
		}
		else
			i++;
	}
	currentTime = std::time(NULL);
	for (it = this->_sessionMap.begin(); it != this->_sessionMap.end();)
	{
		currentSession = it->second;
		secondsIdle = std::difftime(currentTime, currentSession->getLastActivity());
		
		if (secondsIdle > SESSION_TIMEOUT)
		{
			Server::printLog("session expired!");
			if (this->_sessionClientMap.count(currentSession) > 0)
			{
				currentClient = this->_sessionClientMap[currentSession];
				if (currentClient != NULL)
					currentClient->setSession(NULL);
				this->_sessionClientMap.erase(currentSession);
			}
			std::map<std::string, Session*>::iterator toDelete = it;
			it++;
			this->_sessionMap.erase(toDelete->first);
			delete (currentSession);
		}
		else
			it++;
	}
}

void	Server::handleSession(Client* client)
{
	HttpRequest& req = client->getHttpRequest();
	HttpResponseBuilder& build = client->getHttpResponseBuilder();
	std::string sessionId = client->findSessionId();

	if (sessionId.empty() || !this->_sessionMap.count(sessionId))
	{
		std::string newCookie;
		Session* newSession = new Session;
		newCookie = "session_id=" + newSession->getSessionId() + "; Max-Age=1800; Path=/";

		this->_sessionMap[newSession->getSessionId()] = newSession;
		this->_sessionClientMap[newSession] = client;
		build.addHeader("Set-Cookie", newCookie);
		client->setSession(newSession);
	}
	else
	{
		Session* currentSession = this->_sessionMap[sessionId];
		client->setSession(currentSession);
		this->_sessionClientMap[currentSession] = client;
		currentSession->extractCookies(req.getHeader("Cookie"));
	}
}

void	Server::printLog(const std::string& msg)
{
	std::cerr	<< "[WEBSERV]: "
				<< msg
				<< std::endl;
}
