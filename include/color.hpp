/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 01:38:27 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/20 01:44:02 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_HPP
# define COLOR_HPP

#include <string>

namespace color
{
	std::string red = "\033[31m";
	std::string green = "\033[32m";
	std::string blue = "\033[34m";
	std::string reset = "\033[0m";
}

#endif
