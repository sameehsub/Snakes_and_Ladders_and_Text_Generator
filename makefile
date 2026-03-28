CC = gcc
CFLAGS = -Wall -Wextra -Wvla -std=c99

COMMON_OBJS = markov_chain.o linked_list.o

.PHONY: all clean

all: tweets_generator snakes_and_ladders

tweets_generator: tweets_generator.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

snakes_and_ladders: snakes_and_ladders.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

tweets_generator.o: tweets_generator.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

snakes_and_ladders.o: snakes_and_ladders.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

markov_chain.o: markov_chain.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o tweets_generator snakes_and_ladders
