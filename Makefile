NAME = ircserv

SRCS = main.cpp Server.cpp Client.cpp	Message.cpp

INCS = Server.hpp Client.hpp	Message.cpp

OBJS = $(addprefix objs/, $(SRCS:.cpp=.o))

DEPS = $(wildcard objs/*.d)

CXX = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

objs/%.o:	src/%.cpp
			@ [ -d objs ] || mkdir -p objs
			$(CXX) $(CFLAGS) -MMD -c $< -o $@ -I./include

$(NAME):	$(OBJS)
			$(CXX) -o $@ $^ -fsanitize=address

all:		$(NAME)

clean:
			$(RM)r $(OBJS) $(DEPS) objs

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re

include		$(DEPS)
