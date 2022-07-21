/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/04 10:11:19 by mamaurai          #+#    #+#             */
/*   Updated: 2022/07/18 11:28:19 by mamaurai         ###   ########.fr       */
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

extern int g_debug_prog_level;

#include "_colors.hpp"
#include "_utils.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Configuration.hpp"
#include "Socket.hpp"
#include "Select.hpp"

extern INLINE_NAMESPACE::Configuration g_config;

#endif /* WEBSERV_HPP */