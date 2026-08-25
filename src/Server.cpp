/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 20:36:26 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/25 03:21:23 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "color.hpp"
#include "Server.hpp"

Server::Server(): _currAddr(NULL) {}

Server::Server(const std::vector<VirtualHostConfig> config): _configs(config), _currAddr(NULL) {}

Server::~Server() 
{
	Server::printLog("Destroying server...");
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
		if ((status = setsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) == -1)
			throw (ServerException(errno));
		if ((status = bind(sockFd, res->ai_addr, res->ai_addrlen)) == -1)
			throw (ServerException(errno));
		if ((status = listen(sockFd, BACKLOG)) == -1)
			throw (ServerException(errno));
		if ((status = fcntl(sockFd, F_SETFL, O_NONBLOCK)) == -1)
			throw (ServerException(errno));
		this->_multiplexer.addFd(sockFd, (EPOLLIN | EPOLLRDHUP));
		freeaddrinfo(this->_currAddr);
		this->_currAddr = res = NULL;
		Server::printLog("Listening on " + color::green + it->first + ":" + it->second + color::reset);
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
			this->routeServer(fds[j].first, fds[j].second, fdType);
		}
		this->checkTimeouts();
	}
}

void	Server::routeServer(int fd, uint32_t eventType, enum FdType fdType)
{
	enum ClientStatus	status;

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
			if ((eventType & EPOLLOUT) && (eventType & (EPOLLERR | EPOLLRDHUP | EPOLLHUP)))
				break;

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
			if (status == PROCESSING_STATIC_FILE || status == PROCESSING_EXCEPTION)
			{
				this->_multiplexer.removeFd(client->getFd());
				if (status == PROCESSING_STATIC_FILE)
					this->handleSession(client);
				client->executeStaticFileMethod();
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
			if (status == PREPARING_RESPONSE && !client->getRedirect())
				this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);
			if (status == PREPARING_RESPONSE && client->getRedirect())
				this->_multiplexer.modifyFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);
			if (status == SENT_RESPONSE)
			{
				if (client->getKeepAlive())
				{
					this->_multiplexer.removeFd(client->getFd());
					this->_multiplexer.addFd(client->getFd(), EPOLLIN | EPOLLRDHUP);
					client->reset();
				}
				else
					this->destroyClient(client->getFd());
			}
			break;
		}
		case CGI:
		{
			if ((eventType & EPOLLOUT) && (eventType & (EPOLLERR | EPOLLRDHUP | EPOLLHUP)))
				break;
			Client* client = this->_cgiMap[fd];
			if (!client) break;
			CgiHandler& cgi = client->getCgiHandler();
			HttpResponseBuilder& builder = client->getHttpResponseBuilder();

			bool isPipeDone = cgi.processCgi(fd, eventType, builder);
			if (isPipeDone)
				this->handleProcessedFile(client, builder.getStatusCode());
			client->updateActivity();
			break;
		}
	}
    if (eventType & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
		this->handleError(fd, fdType);
}

void	Server::handleProcessedFile(Client* client, int statusCode)
{
	HttpRequest& req = client->getHttpRequest();
	std::vector<std::pair<int, enum CgiIoType> > errorFd;
	StaticFileHandler& stat = client->getStaticFileHandler();
	VirtualHostConfig& conf = client->getVirtualHostConfig();
	std::vector<int> activeFds = client->getActiveFds();
	HttpResponseBuilder& build = client->getHttpResponseBuilder();

	for (size_t i = 0; i < activeFds.size(); i++)
	{
		this->_multiplexer.removeFd(activeFds[i]);
		this->_cgiMap.erase(activeFds[i]);
	}
	client->destroyActiveFds();
	client->setStatus(PREPARING_RESPONSE);
	if (statusCode > 299)
		stat.handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build); 
	else
		build.setHttpResponseBody(build.getCgiParser().getBody());
	this->_multiplexer.addFd(client->getFd(), EPOLLOUT | EPOLLRDHUP);
}

void	Server::handleError(int fd, enum FdType fdType)
{
	switch (fdType)
	{
		case SOCKET:
			this->_listenFds.erase(std::remove(this->_listenFds.begin(), this->_listenFds.end(), fd));
			close(fd);
			Server::printLog("Socket fd removed from connection");
			break;
		case CLIENT:
			this->destroyClient(fd);
			break;
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
	int clientFd, status;
	uint16_t	port = 0;
	uint32_t	ip = 0;

	if ((clientFd = accept(sockFd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		Server::printLog("Accept error on client!");
		return (clientFd);
	}
	if ((status = fcntl(clientFd, F_SETFL, O_NONBLOCK)) == -1)
	{
		Server::printLog("Client creation error!");
		this->destroyClient(clientFd);
		return (-1);
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
	std::stringstream ss;
	Client* client = this->_clientMap[clientFd];
	if (!client) return ;

	ss << clientFd;
	this->_multiplexer.removeFd(clientFd);
	this->_clientMap.erase(clientFd);
	this->_sessionClientMap.erase(client->getSession());
	this->_clients.erase(std::remove(this->_clients.begin(), this->_clients.end(), client), this->_clients.end());
	std::vector<int> activeFds = client->getActiveFds();
	for (size_t i = 0; i < activeFds.size(); i++)
	{
		int fd = activeFds[i];
		this->_cgiMap.erase(fd);
		this->_staticFileMap.erase(fd);
		if (fd > 0)
			close (fd);
	}
	close(clientFd);
	delete (client);
	Server::printLog("TCP connection " + color::red + "destroyed" + color::reset + " on FD " + ss.str());
}

void	Server::checkTimeouts()
{
	std::stringstream ss;
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
			ss << currentClient->getFd();
			Server::printLog("Client FD " + ss.str() +  color::red + " timed " + color::reset + "out");
			this->destroyClient(currentClient->getFd());
			ss.str("");
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
			Server::printLog("Session ID " + currentSession->getSessionId() +  color::red + " timed " + color::reset + "out");
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
	std::stringstream ss;
	HttpRequest& req = client->getHttpRequest();
	HttpResponseBuilder& build = client->getHttpResponseBuilder();
	std::string sessionId = client->findSessionId();

	ss << client->getFd();
	if (sessionId.empty() || !this->_sessionMap.count(sessionId))
	{
		std::string newCookie;
		Session* newSession = new Session;
		std::string cookieId = newSession->getSessionId();
		newCookie = "session_id=" + cookieId + "; Max-Age=1800; Path=/";

		this->_sessionMap[newSession->getSessionId()] = newSession;
		this->_sessionClientMap[newSession] = client;
		build.addHeader("Set-Cookie", newCookie);
		client->setSession(newSession);
		Server::printLog("Cookie created " + color::green + cookieId + color::reset + " for FD " + ss.str());
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
