/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vitosant <vitosant@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 17:29:09 by dasimoes          #+#    #+#             */
/*   Updated: 2026/07/24 14:25:34 by vitosant         ###    ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <map>
#include <string>
#include <utility>
#include <vector>

class Location
{
	private:
		std::string					_path;
		std::string					_root;
		std::vector<std::string>	_index;
		std::vector<std::string>	_allowedMethods;
		std::map<int, std::string>	_errorPages;


		bool						_autoindex;

		bool						_uploadEnable;
		std::string					_uploadStore;

		std::string					_cgiExtension;
		std::string					_cgiPath;

		std::pair<int, std::string>	_return;

	public:
		Location( void );
		~Location( void );
		Location( const Location& other );
		Location&	operator=( const Location& other );

		void	setPath( const std::string& path ) { _path = path; }
		void	setRoot( const std::string& root ) { _root = root; }
		void	setIndex( const std::vector<std::string> & index ) { _index = index; }
		void	setAllowedMethods( const std::vector<std::string> & allowedMethods ) { _allowedMethods = allowedMethods; }
		void	setAutoindex( bool autoindex ) { _autoindex = autoindex; }
		void	setUploadEnable( bool uploadEnable ) { _uploadEnable = uploadEnable; }
		void	setUploadStore( const std::string& uploadStore ) { _uploadStore = uploadStore; }
		void	setCgiExtension( const std::string& cgiExtension ) { _cgiExtension = cgiExtension; }
		void	setCgiPath( const std::string& cgiPath ) { _cgiPath = cgiPath; }
		void	setReturn( const std::pair<int, std::string> & ret ) { _return = ret; } ;
		void	setErrorPage( const std::map<int, std::string> & errorPages ) { _errorPages = errorPages; };

		void	addIndex( const std::string& index ) { _index.push_back(index); }
		void	addAllowedMethod( const std::string& method ) { _allowedMethods.push_back(method); }
		void	addErrorPage( const int index, const std::string& path ) { _errorPages[index] = path; };

		const std::string&					getPath( void ) const { return _path; }
		const std::string&					getRoot( void ) const { return _root; }
		const std::vector<std::string> &	getIndex( void ) const { return _index; }
		const std::vector<std::string> &	getAllowedMethods( void ) const { return _allowedMethods; }
		bool								getAutoindex( void ) const { return _autoindex; }
		bool								getUploadEnable( void ) const { return _uploadEnable; }
		const std::string&					getUploadStore( void ) const { return _uploadStore; }
		const std::string&					getCgiExtension( void ) const { return _cgiExtension; }
		const std::string&					getCgiPath( void ) const { return _cgiPath; }
		const std::pair<int, std::string> &	getReturn( void ) const { return(_return); };
		const std::map<int, std::string> &	getErrorPages( void ) const { return(_errorPages); }
};

std::ostream&	operator<<( std::ostream& out, const Location& loc );

#endif
