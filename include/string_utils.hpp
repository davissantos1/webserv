/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/09 12:14:51 by vitosant          #+#    #+#             */
/*   Updated: 2026/08/09 12:15:47 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

std::vector<std::string>	split( const std::string& str, const char spliter );
void						trimStr( std::string& src );
