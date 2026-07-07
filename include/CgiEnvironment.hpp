/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiEnvironment.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 08:36:47 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/07 16:56:51 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIENVIRONMENT_HPP
# define CGIENVIRONMENT_HPP

class CgiEnvironment
{
	private:
		// to be added Danilo
	public:
		CgiEnvironment();
		~CgiEnvironment();
		CgiEnvironment(const CgiEnvironment& other);
		CgiEnvironment& operator=(const CgiEnvironment& other);
};

#endif
