/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/01 16:03:56 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 13:41:19 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

_BEGIN_NAMESPACE_WEBSERV

// TODO operator overload "<<"

typedef struct s_location
{
	s_location (void) :
		_methods(0),
		_return(),
		_root(""),
		_index(""),
		_path(""),
		_autoindex(false),
		_cgi(),
		_upload_path()
	{ }

	~s_location (void)
	{ }

	void	_set_methods (std::vector<std::string> &);
	void	_set_path (std::vector<std::string> &);

	uint8_t	_methods;
	std::vector<std::pair<int, std::string> > _return;
	std::string	_root;
	std::string _index;
	std::string _path;
	bool	_autoindex;
	std::string	_cgi;
	std::string _upload_path;

	friend std::ostream & operator<< (std::ostream & o, const struct s_location & l) {
		return (o);
	}
	
	EXCEPTION(InvalidLocationBlock, "configuration_file : invalid location block")

}	t_location;

typedef struct s_function_pair_location {
	void (s_location::*f) (std::vector<std::string> &);
	std::string	str;
}	t_function_pair_location;

_END_NAMESPACE_WEBSERV

#endif /* LOCATION_HPP */