/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 10:11:19 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/05 14:11:51 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <vector>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <cctype>

extern int g_debug_prog_level;

#include "_colors.hpp"
#include "_utils.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Configuration.hpp"

extern INLINE_NAMESPACE::Configuration g_config;

#endif /* WEBSERV_HPP */