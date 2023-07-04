CC = gcc
INCLUDES = -I./includes
CFLAGS = -Wall -Wextra -Werror -g3 -MMD $(INCLUDES)
NAME = ft_ping

SRCS := srcs/main.c
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

all: $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re