/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 22:01:31 by dasimoes         ###   ########.fr       */
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

bool	StaticFileHandler::processStaticFile(int fd, uint32_t eventType, HttpResponseBuilder& builder)
{
	ssize_t bytes;
	char tempBuffer[8192];

	while (true)
	{
		errno = 0;
		if (eventType & EPOLLIN)
		{
			bytes = read(fd, tempBuffer, sizeof(tempBuffer));
			if (bytes > 0)
				builder.feedStaticFile(tempBuffer, bytes);
			else if (bytes == 0)
			{
				builder.setStatusCode(200);
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				builder.setStatusCode(500);
				return (true);
			}
		}
		else
		{
			int bytesWritten = builder.getBytesWritten();
			const std::string* body = builder.getHttpRequestBody();
			bytes = write(fd, body->c_str() + bytesWritten, body->size() - bytesWritten);
			if (bytes > 0)
				builder.setBytesWritten(bytes + bytesWritten);
			else if ((size_t)bytes == body->size())
			{
				builder.setStatusCode(200);
				return (true);
			}
			else
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				builder.setStatusCode(500);
				return (true);
			}
		}
	}
	return (false);
}

std::pair<int, enum FdIoType>	StaticFileHandler::handleGet(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fd, status;
	std::pair<int, enum FdIoType> fdBundle;

	errno = 0;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());
	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
	{
		if (errno == ENOENT)
			*statusCode = 404;
		else if (errno == EISDIR)
		{
			if (conf.shouldAutoindex(req.getUri()))
			{
				*statusCode = -1;
				return (fdBundle);
			}
			else
				*statusCode = 403;
		}
		else if (errno == EACCES)
			*statusCode = 403;
		else
			*statusCode = 500;
	}
	if ((status = fcntl(fd, F_SETFL, O_NONBLOCK)) == -1)
		*statusCode = 500;
	if (*statusCode > 299)
		return (fdBundle);
	fdBundle = std::make_pair(fd, STATIC_FILE_READ);
	return (fdBundle);
}

std::vector<std::pair<int, enum FdIoType> >	StaticFileHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fd;
	struct stat info;
	std::vector<std::pair<int, enum FdIoType> > bundles;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());
	std::string uploadPath = conf.getUploadPath(req.getEndpoint());

	errno = 0;
	if (uploadPath.empty())
	{
		*statusCode = 405;
		return (bundles);
	}
	if (stat(path.c_str(), &info) == -1)
	{
		if (errno == EACCES || errno == ENOENT)
			*statusCode = 403;
		else
			*statusCode = 500;
		return (bundles);
	}
	if (S_ISDIR(info.st_mode))
	{
		std::string filePath = uploadPath + req.getFilename();
		fd = open(filePath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (fd < 0)
		{
			if (errno == EACCES)
				*statusCode = 403;
			else
				*statusCode = 500;
			return (bundles);
		}
		bundles.push_back(std::make_pair(fd, STATIC_FILE_WRITE));
		return (bundles);
	}
	*statusCode = 403;
	return (bundles);
}

void	StaticFileHandler::handleDelete(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	struct stat info;
	std::string path = conf.getFullPath(req.getFilename(), req.getEndpoint());

	errno = 0;
	if (stat(path.c_str(), &info) == -1)
	{
		if (errno == EACCES || errno == EPERM)
			*statusCode = 403;
		else if (errno == ENOENT)
			*statusCode = 404;
		else
			*statusCode = 500;
		return ;
	}
	if (S_ISDIR(info.st_mode))
	{
		*statusCode = 403;
		return ;
	}
	errno = 0;
	if(std::remove(path.c_str()) == 0)
		*statusCode = 204;
	else
	{
		if (errno == EACCES || errno == EPERM)
			*statusCode = 403;
		else if (errno == ENOENT)
			*statusCode = 404;
		else
			*statusCode = 500;
	}
}

std::vector<std::pair<int, enum FdIoType> >	StaticFileHandler::handleException(int exception, std::string path)
{
	int fd;
	std::vector<std::pair<int, enum FdIoType> > tasks;
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
	tasks.push_back(std::make_pair(fd, STATIC_FILE_READ));
	return (tasks);
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
	return (-1);
}
