/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/07 13:00:31 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIENVIRONMENT_HPP
# define CGIENVIRONMENT_HPP



class CgiEnvironment
{
	private:
		
	public:
		CgiEnvironment();
		~CgiEnvironment();
		CgiEnvironment(const CgiEnvironment& other);
		CgiEnvironment& operator=(const CgiEnvironment& other);

		void			startEnvironment(HttpRequest& req);
		const char**	getEnvironment();
};

#endif
