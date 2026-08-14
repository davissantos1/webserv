/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/14 18:01:59 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

# include <cstdlib>
# include <sstream>
# include <string>
# include <map>
# include <ctime>

class Session
{
	private:
		std::string							_sessionId;
		time_t								_lastActivity;
		std::map<std::string, std::string>	_cookies;

	public:
		Session();
		~Session();
		Session(const Session& other);
		Session& operator=(const Session& other);

		std::string&	getSessionId() { return (this->_sessionId); }
		time_t	getLastActivity() { return (this->_lastActivity); }
		std::map<std::string, std::string>	getCookies() { return (this->_cookies); }

		void	setSessionId(std::string sessionId) { this->_sessionId = sessionId; }
		void	setLastActivity(time_t lastActivity) { this->_lastActivity = lastActivity; }
		void	setCookies(std::map<std::string, std::string> cookies) { this->_cookies = cookies; }

		void	extractCookies(std::string cookies);
};

#endif
