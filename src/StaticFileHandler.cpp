/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:58:16 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/03 21:26:39 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler()
{

}

StaticFileHandler::~StaticFileHandler()
{

}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& other)
{
	if (this != &other)
		*this = other;
}

StaticFileHandler&	StaticFileHandler::operator=(const StaticFileHandler& other)
{
	if (this != other)
	{
		// whatever
	}
	return (*this);
}

bool	StaticFileHandler::processStaticFile(int fd, uint32_t eventType, HttpRequestBuilder& builder)
{
	ssize_t bytes;
	char tempBuffer[8192];

	while (true)
	{
		ernno = 0;
		if (eventType & EPOLLIN)
		{
			bytes = read(fd, tempBuffer, sizeof(tempBuffer));
			if (bytes > 0)
				builder.feed(tempBuffer, bytes);
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
			std::string body = builder.getBody();
			bytes = write(fd, body.c_str() + bytesWritten, body.size() - bytesWritten);
			if (bytes > 0)
				builder.settBytesWritten(bytes + bytesWritten);
			else if (bytes == body.size())
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
	std::string path = conf.getFullPath(req.getUri());
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
	if (*statusCode > 0)
		return (fdBundle);
	fdBundle = std::make_pair(fd, STATIC_FILE_READ);
	return (fdBundle);
}

std::vector<<std::pair<int, enum FdIoType> >	StaticFileHandler::handlePost(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{
	int fd, status;
	std::vector<std::pair<int, enum FdIoType> > bundles;
	std::string path = conf.getFullPath(req.getUri());

	errno = 0;
	if (fd = open(path.c_str(), 
}

void	StaticFileHandler::handleDelete(HttpRequest& req, VirtualHostConfig& conf, int* statusCode)
{

}

std::vector<std::pair<int, enum FdIoType> >	StaticFileHandler::handleException(int exception)
{
	int fd;
	std::vector<std::pair<int, enum FdIoType> > tasks;
	switch (exception)
	{
		case 400:
			fd = open("./www/error_pages/400.html", O_RDONLY);
			break;
		case 403:
			fd = open("./www/error_pages/403.html", O_RDONLY);
			break;
		case 404:
			fd = open("./www/error_pages/404.html", O_RDONLY);
			break;
		case 405:
			fd = open("./www/error_pages/405.html", O_RDONLY);
			break;
		case 413:
			fd = open("./www/error_pages/413.html", O_RDONLY);
			break;
		case 500:
			fd = open("./www/error_pages/500.html", O_RDONLY);
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
	std::string path = conf.getFullPath(req.getUri());
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

	if (basePath.empty() || basePath[basePath.size() - 1] != "/")
		basePath += "/";

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

		double kbSize = static_cast<double> info.st_size / 1024;
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
			<<	"</html>\n"

	if (closedir(stream) == -1)
		return (500);
	build.fillResponseBody(html.str());
	return (-1);
}
