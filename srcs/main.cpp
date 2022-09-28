/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/01 16:28:05 by mamaurai          #+#    #+#             */
/*   Updated: 2022/09/27 14:02:55 by mamaurai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int 								g_debug_prog_level = -1;
INLINE_NAMESPACE::Configuration		g_config;
string_vector						g_header = INLINE_NAMESPACE::init_header();
bool								g_exit = false;

void
quit_program (__attribute__((unused))int signal) {
    g_exit = true;
    DEBUG_1(CNOUT(BRED << std::endl << "Webserv is closing..." << CRESET))
}

static std::string
set_options_ (int ac, char **av) {
	std::vector<std::string> paths;
	for (int i = 1; i < ac; i++) {
		std::string option(av[i], strlen(av[i]));
		
		if (option.find(DEBUG_OPTION) == 0) {
			g_debug_prog_level = atoi(av[1] + strlen(DEBUG_OPTION));
		} else {
			paths.push_back(option);
		}	
	}
	if (g_debug_prog_level < DEBUG_LEVEL_MIN || g_debug_prog_level > DEBUG_LEVEL_MAX || DEBUG_ON == false) {
		g_debug_prog_level = 0;
	}
	if (paths.size() == 1) {
		return paths[0];
	} else if (paths.empty()) {
		std::string return_v (DEFAULT_FILE);
		return (return_v);
	} else {
		std::cerr << INVALID_ARG << std::endl;
		exit (EXIT_FAILURE);
	}
}

int
main (int ac, char **av)
{
	std::string file_name = set_options_(ac, av);
	
	try {
		g_config.lexer(file_name.c_str());
		DEBUG_3(CNOUT(BBLU << "Updating : configuration file has been loaded" << CRESET));
		g_config.parser();
		DEBUG_3(CNOUT(BBLU << "Updating : parsing done, configuration file is correct" << CRESET));
		DEBUG_4(CNOUT(BMAG << "[Configuration file set]" << CRESET));
		DEBUG_4(CNOUT(BWHT << read_file(av[((ac == 2) ? 1 : 2)]) << CRESET));
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	signal(SIGINT, quit_program);
	signal(SIGQUIT, quit_program);

	try {
		INLINE_NAMESPACE::Select select;
		select.setup();
		select.start();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	DEBUG_1(CNOUT(BRED << "Webserv is closed..." << CRESET))

	return (EXIT_SUCCESS);
}
