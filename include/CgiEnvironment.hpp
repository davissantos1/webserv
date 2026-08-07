/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 14:51:35 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIENVIRONMENT_HPP
# define CGIENVIRONMENT_HPP

# include <string>
# include <vector>
# include <map>

class HttpRequest;

class CgiEnvironment
{
	private:
		std::vector<char *>	_environment;
		
	public:
		CgiEnvironment();
		~CgiEnvironment();
		CgiEnvironment(const CgiEnvironment& other);
		CgiEnvironment& operator=(const CgiEnvironment& other);

		void				startEnvironment(HttpRequest& req);
		std::vector<char *>	getEnvironment() { return this->_environment; }
};

#endif
