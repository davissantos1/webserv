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

std::vector<std::string>	split( const std::string& str, const char splitter );
std::string					reverse_split( const std::vector<std::string> & vec, const char splitter );
std::string					decode_str( const std::string& str );
std::string					normalize_str( const std::string& raw );
void						trimStr( std::string& src );
