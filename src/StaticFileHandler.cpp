/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/21 00:10:43 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler() {}

StaticFileHandler::~StaticFileHandler() {}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other)
{
	(void) other;
}

StaticFileHandler&	StaticFileHandler::operator=(const StaticFileHandler& other)
{
	(void) other;
	return (*this);
}

bool	StaticFileHandler::processStaticFile(int fd, enum StaticFileIoType type, HttpRequest& req, HttpResponseBuilder& build)
{
	int bytes;
	char buffer[8196];

	if (type == STATIC_FILE_READ)
	{
		while ((bytes = read(fd, buffer, sizeof(buffer))))
		{
			if (bytes < 0)
			{
				build.setStatusCode(500);
				return (false);
			}
			else
				build.feedStaticFile(buffer, bytes);
		}
	}
	else if (type == STATIC_FILE_WRITE)
	{
		const char* reqBody = req.getBody()->c_str();
		bytes = write(fd, reqBody, req.getBodySize());
		if (bytes < 0)
		{
			build.setStatusCode(500);
			return (false);
		}
	}
	return (true);
}

void	StaticFileHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build)
{
	struct stat info;
	int fd, statusCode;

	errno = 0;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());
	if (stat(path.c_str(), &info) == -1)
	{
		if (errno == EACCES || errno == EPERM)
			build.setStatusCode(403);
		else if (errno == ENOENT)
			build.setStatusCode(404);
		else
			build.setStatusCode(500);
	}
	if (S_ISDIR(info.st_mode))
	{
		if (conf.shouldAutoindex(req.getEndpoint()))
			this->handleAutoindex(req, conf, build);
		else
			build.setStatusCode(403);
		return ;
	}
	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
	{
		build.setStatusCode(500);
		statusCode = build.getStatusCode();
		if (!this->handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build))
			build.setHardFallback(true);
		return ;
	}
	build.setStatusCode(200);
	if (!this->processStaticFile(fd, STATIC_FILE_READ, req, build))
	{
		close(fd);
		statusCode = build.getStatusCode();
		this->handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build);
	}
}

void	StaticFileHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build)
{
	int fd, statusCode;
	struct stat info;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());
	std::string uploadPath = conf.getUploadPath(req.getEndpoint());

	errno = 0;
	if (uploadPath.empty())
		build.setStatusCode(405);
	if (stat(path.c_str(), &info) == -1)
	{
		if (errno == EACCES || errno == ENOENT)
			build.setStatusCode(403);
		else
			build.setStatusCode(500);
	}
	if (S_ISDIR(info.st_mode))
	{
		std::string filePath = uploadPath + req.getFilename();
		fd = open(filePath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (fd < 0)
		{
			if (errno == EACCES)
				build.setStatusCode(403);
			else
				build.setStatusCode(500);
		}
		else
		{
			build.setStatusCode(200);
			if (!this->processStaticFile(fd, STATIC_FILE_WRITE, req, build))
			{
				close(fd);
				statusCode = build.getStatusCode();
				this->handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build);
			}
			return ;
		}
	}
	build.setStatusCode(403);
	statusCode = build.getStatusCode();
	if (statusCode != 200)
		this->handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build);
}   	
    	
void	StaticFileHandler::handleDelete(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build)
{
	int statusCode;
	struct stat info;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());

	errno = 0;
	if (stat(path.c_str(), &info) == -1)
	{
		if (errno == EACCES || errno == EPERM)
			build.setStatusCode(403);
		else if (errno == ENOENT)
			build.setStatusCode(404);
		else
			build.setStatusCode(500);
	}
	if (S_ISDIR(info.st_mode))
		build.setStatusCode(403);
	errno = 0;
	if(std::remove(path.c_str()) == 0)
		build.setStatusCode(204);
	else
	{
		if (errno == EACCES || errno == EPERM)
			build.setStatusCode(403);
		else if (errno == ENOENT)
			build.setStatusCode(404);
		else
			build.setStatusCode(500);
	}
	statusCode = build.getStatusCode();
	if (statusCode != 204)
		this->handleException(statusCode, conf.getErrorPage(statusCode, req.getEndpoint()), req, build);
}

bool	StaticFileHandler::handleException(int exception, std::string path, HttpRequest& req, HttpResponseBuilder& build)
{
	int fd = -1;
	if (path.empty())
	{
		std::stringstream ss;
		ss << "./www/error_pages/" << exception << ".html";
		path = ss.str();
	}
	switch (exception)
	{
		case 400:
			fd = open(path.c_str(), O_RDONLY);
			break;
		case 403:
			fd = open(path.c_str(), O_RDONLY);
			break;
		case 404:
			fd = open(path.c_str(), O_RDONLY);
			break;
		case 405:
			fd = open(path.c_str(), O_RDONLY);
			break;
		case 413:
			fd = open(path.c_str(), O_RDONLY);
			break;
		case 500:
			fd = open(path.c_str(), O_RDONLY);
			break;
	}
	if (!this->processStaticFile(fd, STATIC_FILE_READ, req, build))
	{
		close(fd);
		build.setHardFallback(true);
		return (false);
	}
	return (true);
}


int	StaticFileHandler::handleAutoindex(HttpRequest& req, VirtualHostConfig& conf, HttpResponseBuilder& build)
{
	struct dirent* curr;
	struct stat info;
	std::ostringstream html;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());
	DIR* stream = opendir(path.c_str());
	std::string basePath = path;

	if (!stream) 
		return (500);

	html	<<	"<html>\n"
			<<		"<head>\n"
			<<			"<meta charset=\"utf-8\"/>\n"
			<<			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n"
			<<			"<title>Index of" << path << "</title>\n"
			<<			"<link rel=\"stylesheet\" href=\"directory.css\">\n"
			<<		"</head>\n"
			<<		"<body lang=\"en\">\n"
			<<			"<main>\n"
			<<				"<section>\n"
			<<				"<header>\n"
			<<					"<h1>Index of" << path << "</h1>\n"
			<<				"</header>\n"
			<<				"<table>\n";

	if (basePath.empty() || basePath[basePath.size() - 1] != '/')
		basePath += '/';

	while ((curr = readdir(stream)) != NULL)
	{
		std::string fileName = curr->d_name;
		bool	isDir = (curr->d_type == DT_DIR);

		if (fileName == "." || fileName == "..")
			continue ;

		std::string entryPath = basePath + fileName;
		if (stat(entryPath.c_str(), &info) != 0)
			return (500);

		if (isDir)
			entryPath += "/";

		std::string displayName = fileName;
		if (isDir)
			displayName += "/";

		double kbSize = static_cast<double>(info.st_size) / 1024;
		html	<< "<tr>\n"
				<< "<td class=\"link\"><a href=\"" << entryPath << "\">" << displayName << ".</a></td>\n"
				<< "<td class=\"date\">" << std::ctime(&info.st_mtime) << "</td>\n"
				<< "<td class=\"size\">" << kbSize << "KB</td>\n"
				<< "<\tr>\n";
	}

	html	<<				"</table>\n"
			<<				"</section>\n"
			<<			"</main>\n"
			<<		"</body>\n"
			<<	"</html>\n";

	if (closedir(stream) == -1)
		return (500);
	build.setHttpResponseBody(html.str());
	return (200);
}
