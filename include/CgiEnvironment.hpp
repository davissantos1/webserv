/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 20:11:35 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIENVIRONMENT_HPP
# define CGIENVIRONMENT_HPP

# include "HttpRequest.hpp"
# include <sstream>
# include <cctype>
# include <string>
# include <vector>
# include <map>

class CgiEnvironment
{
	private:
		std::vector<const char *>	_environment;
		
	public:
		CgiEnvironment();
		~CgiEnvironment();
		CgiEnvironment(const CgiEnvironment& other);
		CgiEnvironment& operator=(const CgiEnvironment& other);

		void				startEnvironment(HttpRequest& req);
		std::vector<const char *>	getEnvironment() { return this->_environment; }
};

#endif
