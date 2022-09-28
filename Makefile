# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mamaurai <mamaurai@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/10 15:51:05 by gmary             #+#    #+#              #
#    Updated: 2022/09/27 14:49:02 by mamaurai         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = 		webserv

SRCS =		./srcs/Configuration.cpp \
			./srcs/Location.cpp \
			./srcs/parsing/checking.cpp \
			./srcs/parsing/parsing.cpp \
			./srcs/Server.cpp \
			./srcs/_init.cpp \
			./srcs/Select.cpp \
			./srcs/Response.cpp \
			./srcs/Socket.cpp \
			./srcs/Header.cpp \
			./srcs/Cgi.cpp \
			./srcs/Request.cpp \
			./srcs/_utils.cpp \
			./srcs/main.cpp \


CC =		c++
CPPFLAGS =	-Ofast
CPPFLAGS = 	-MD  -std=c++98 -Wall -Wextra -Werror -g3
# CPPFLAGS += -g3  -fsanitize=address 
OBJS = 		$(addprefix ${OBJDIR}/,${SRCS:.cpp=.o})
DEP =		$(addprefix ${OBJDIR}/,${SRC:.cpp=.d})
RM =		rm -rf
INCS =		-I ./incs
SYSTEM =	${shell uname -s}

ifeq ($(SYSTEM), Darwin)
	CPPFLAGS += -D_DARWIN_UNLIMITED_SELECT
endif

OBJDIR =	.objs
INCDIR =	incs

ifeq (${SYSTEM}, Darwin)
_END=$'\x1b[0m
_BOLD=$'\x1b[1m
_UNDER=$'\x1b[4m
_REV=$'\x1b[7m
_GREY=$'\x1b[30m
_RED=$'\x1b[31m
_GREEN=$'\x1b[32m
_YELLOW=$'\x1b[33m
_BLUE=$'\x1b[34m
_PURPLE=$'\x1b[35m
_CYAN=$'\x1b[36m
_WHITE=$'\x1b[37m
endif

ifeq (${SYSTEM}, Linux)
_GREY=	$'\033[30m
_RED=	$'\033[31m
_GREEN=	$'\033[32m
_YELLOW=$'\033[33m
_BLUE=	$'\033[34m
_PURPLE=$'\033[35m
_CYAN=	$'\033[36m
_WHITE=	$'\033[37m
_END= $'\033[37m
endif

-include $(DEP)

$(OBJDIR)/%.o: %.cpp
				@printf "%-15s ${_YELLOW}${_BOLD}$<${_END}...\n" "Compiling"
				@mkdir -p $(dir $@) $(DEPDIR) 2> /dev/null
				@${CC} ${CPPFLAGS} -o $@ ${INCS} -c $< 

${NAME}:	${OBJS} ${DEP} 
				@sh .is_file_to_upload.sh
				@printf "%-15s ${_PURPLE}${_BOLD}${NAME}${_END}...\n" "Compiling"
				@${CC} ${CPPFLAGS} -o ${NAME} ${INCS} ${OBJS}
				@printf "\n${_GREEN}${_BOLD}Compilation done !${_END}\n"

all:		${NAME}

clean:
				@printf "%-15s ${_RED}${_BOLD}${NAME} binary files${_END}...\n" "Deleting"
				@${RM} ${OBJDIR} ${OBJS}

fclean:		clean
				@printf "%-15s ${_RED}${_BOLD}${NAME}${_END}...\n" "Deleting"
				@${RM} ${NAME}
				@rm -rf www/Upload/file_to_upload 2> /dev/null

re:			fclean 
			@make all

.PHONY:	all clean fclean re