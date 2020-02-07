# Target library
lib := libuthread.a
CC  := gcc
CFLAGS := -Wall -Wextra -Werror
DEPFLAGS = -MMD -MF $(@:.o=.d)


all: $(lib)

## TODO: Phase 1.1

$(lib): uthread.o context.o queue.o preempt.o
	ar rcs $@ $<
	ranlib $@

preempt.o: preempt.c preempt.c preempt.h uthread.h
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS) 

uthread.o: uthread.c uthread.h context.h preempt.h queue.h
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

context.o: context.c context.h preempt.h uthread.h
	
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -f $(lib) *.o
