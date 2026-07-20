/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:29:09 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/19 17:10:55 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <vector>

class	Location
{
	private:
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_paths;
		std::vector<std::string>	_allowedMethods;
	public:
		Location();
		~Location();
		Location(const Location& other);
		Location&	operator=(const Location& other);

		void	setRoot( const std::string& root ) { _root = root; }
		void	setIndex( const std::vector<std::string> & index ) { _index = index; }
		void	setPaths( const std::vector<std::string> & paths ) { _paths = paths; }
		void	setAllowedMethods( const std::vector<std::string> & allowedMethods ) { _allowedMethods = allowedMethods; }

		void	addIndex( const std::string& index ) { _index.push_back(index); }
		void	addPath( const std::string& path ) { _allowedMethods.push_back(path); }
		void	addAllowedMethods( const std::string& method ) { _allowedMethods.push_back(method); }
};

#endif
