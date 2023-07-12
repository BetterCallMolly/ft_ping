CC = gcc
INCLUDES = -I./includes
CFLAGS = -Wall -Wextra -Werror -g3 -MMD $(INCLUDES) -fshort-enums -fsanitize=address
NAME = ft_ping

SRCS := srcs/main.c \
		srcs/icmp_checksum.c \
		srcs/icmp.c \
		srcs/get_identifier.c \
		srcs/get_timestamp.c \
		srcs/generate_data.c \

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

scan-build: clean
	scan-build make

.PHONY: all clean fclean re