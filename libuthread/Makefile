# Target library
lib := libuthread.a
o_file :=  uthread.o context.o queue.o preempt.o

CC  := gcc
CFLAGS := -Wall -Wextra -Werror
DEPFLAGS = -MMD -MF $(@:.o=.d)


all: $(lib)

## TODO: Phase 1.1

$(lib): $(o_file)
	ar rcs $@ $<
	ranlib $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -f $(lib) $(o_file)
