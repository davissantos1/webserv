/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/30 14:07:05 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATICFILEHANDLER_HPP
# define STATICFILEHANDLER_HPP

# include <utility>
# include <vector>

class HttpRequest;
class HttpResponseBuilder;

class StaticFileHandler
{
	public:
		StaticFileHandler();
		~StaticFileHandler();
		StaticFileHandler(const StaticFileHandler& other);
		StaticFileHandler& operator=(const StaticFileHandler& other);

		std::pair<int, enum FdIoType>					handleGet(HttpRequest& req, int* statusCode);
		std::vector<<std::pair<int, enum FdIoType> >	handlePost(HttpRequest& req, int* statusCode);
		void											handleDelete(HttpRequest& req, int* statusCode);	
		bool											processStaticFile(int fd, uint32_t eventType, HttpResponseBuilder& builder);
};

#endif
