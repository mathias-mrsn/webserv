/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 10:11:19 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/22 13:48:52 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <cctype>
#include <sys/stat.h>
// #include <bits/stdc++.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "_utils.hpp"

extern int 								g_debug_prog_level;
extern string_vector 					g_header;

#include "_colors.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Configuration.hpp"
#include "Socket.hpp"
#include "Select.hpp"
#include "Request.hpp"
#include "Response.hpp"

extern INLINE_NAMESPACE::Configuration 	g_config;

#endif /* WEBSERV_HPP */