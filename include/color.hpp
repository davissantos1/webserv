/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 01:38:27 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/20 14:47:04 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_HPP
# define COLOR_HPP

#include <string>

namespace color
{
	static const std::string red = "\033[31m";
	static const std::string green = "\033[32m";
	static const std::string blue = "\033[34m";
	static const std::string reset = "\033[0m";
}

#endif
