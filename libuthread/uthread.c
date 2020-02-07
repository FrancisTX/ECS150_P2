#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

/* Four states of thread */
enum uthread_state {
    RUNNING,
    READY,
    BLOCKED,
    ZOMBIE
};

/* Struct of thread */
struct uthread {
    uthread_t u_tid;
    enum uthread_state u_state;
    void *stack;
    uthread_ctx_t *u_ctx;
    /* Return value to be collected in join */
    int join_retval;
};

/* The TID assigned to current created thread */
int cur_tid = 0;
/* The TID of current running thread */
int self_tid = 0;
/* Current running thread */
struct uthread* running_thread;

/* Queues store ready, blocked, or zombie thread */
queue_t ready_list = NULL;
queue_t blocked_list = NULL;
queue_t zombie_list = NULL;

/* Yield for the next thread that is ready */
void uthread_yield(void)
{
    struct uthread *temp_uthread;
    temp_uthread = (struct uthread *) malloc(sizeof(struct uthread));
    temp_uthread = running_thread;

    /* If there is no thread to execute, continue to current running thread */
    if (queue_length(ready_list) == 0) {
        return;
    }

    if (running_thread->u_tid == 0) {
        /* If main thread is running, block it */
        running_thread->u_state = BLOCKED;
        queue_enqueue(blocked_list, running_thread);
    } else {
        /* Put thread other than main into ready */
        running_thread->u_state = READY;
        queue_enqueue(ready_list, running_thread);
    }

    /* Take out the thread in the first place in ready */
    queue_dequeue(ready_list,(void**)&running_thread);
    running_thread->u_state = RUNNING;

    /* Update current running TID */
    self_tid = running_thread->u_tid;

    /* Swap to the next thread */
    uthread_ctx_switch((temp_uthread->u_ctx), (running_thread->u_ctx));
    free(temp_uthread);
}

/* Return current working thread's TID */
uthread_t uthread_self(void) {
    return self_tid;
}

/* Initialize before creating the first child thread */
int uthread_init() {
    running_thread = (struct uthread*)malloc(sizeof(struct uthread));
    running_thread->u_state = RUNNING;
    /* TID is set to zero(main thread) */
    running_thread->u_tid = cur_tid;
    /* Initialize stack and context */
    running_thread->stack = uthread_ctx_alloc_stack();
    running_thread->u_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

    /* Fail to initialize context */
    if (uthread_ctx_init(running_thread->u_ctx, running_thread->stack, NULL, NULL) == -1) {
        perror("Failed to initialize");
        return -1;
    }

    /* Initialize queues */
    ready_list = queue_create();
    blocked_list = queue_create();
    zombie_list = queue_create();

    return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
    /* Initialize if no thread has been created */
    if (cur_tid == 0)
        uthread_init();

    struct uthread *new_uthread;
    new_uthread = (struct uthread *) malloc(sizeof(struct uthread));

    /* TID equals TID of previous thread + 1 */
    cur_tid++;
    /* The thread should be in ready */
    new_uthread->u_state = READY;
    new_uthread->u_tid = cur_tid;

    /* Initialize stack and context */
    new_uthread->stack = uthread_ctx_alloc_stack();
    new_uthread->u_ctx = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));

    /* Fail to initialize context */
    if (uthread_ctx_init(new_uthread->u_ctx, new_uthread->stack, func, arg) == -1) {
        perror("Failed to initialize");
        return -1;
    }

    /* Put into ready queue */
    queue_enqueue(ready_list, new_uthread);

    /* Return TID */
    return new_uthread->u_tid;
}

/* Exit from current running thread */
void uthread_exit(int retval) {
    if (self_tid == 0) {
        /* Exit main thread, the program is done */
        queue_destroy(ready_list);
        queue_destroy(blocked_list);
        queue_destroy(zombie_list);
        exit(EXIT_SUCCESS);
    }

    /* Put current thread into zombie queue */
    running_thread->u_state = ZOMBIE;
    running_thread->join_retval = retval;
    queue_enqueue(zombie_list, running_thread);

    if (queue_length(ready_list) != 0) {
        /* If there are threads in ready, run these threads */
        struct uthread *temp_uthread;
        temp_uthread = (struct uthread *) malloc(sizeof(struct uthread));
        temp_uthread = running_thread;

        /* Take out the thread to be run from ready */
        queue_dequeue(ready_list, (void**)&running_thread);
        running_thread->u_state = RUNNING;

        /* Update current running TID */
	    self_tid = running_thread->u_tid;

	    /* Swap to the new thread */
        uthread_ctx_switch(temp_uthread->u_ctx, running_thread->u_ctx);
        free(temp_uthread);
    } else if (queue_length(blocked_list) != 0) {
        /* If there is no thread in ready */
        /* If there are threads in blocked, run these threads */
        struct uthread *temp_uthread;
        temp_uthread = (struct uthread *) malloc(sizeof(struct uthread));
        temp_uthread = running_thread;

        /* Take out the thread to be run from blocked */
        queue_dequeue(blocked_list, (void**)&running_thread);
        running_thread->u_state = RUNNING;

        /* Update current running TID */
        self_tid = running_thread->u_tid;

        /* Swap to the new thread */
        uthread_ctx_switch(temp_uthread->u_ctx, running_thread->u_ctx);
        free(temp_uthread);
    }
}

/* Callback function to check if given thread in the queue */
int if_in_queue(void *cur_node, void *tid) {
    struct uthread* temp_node;

    temp_node =  (struct uthread*)malloc(sizeof(struct uthread));
    temp_node = cur_node;
    return &temp_node->u_tid == tid ? 1 : 0;
}
int uthread_join(uthread_t tid, int *retval)
{
    /*
   while (1) {
        if (queue_length(ready_list) == 0) {
            break;
        }else {
        uthread_yield();
        }
    }
*/
    /* Can't join main thread */
	if (tid == 0)
	    return -1;

	/* Thread to be joined */
    struct uthread *join_uthread;
    /* Stored current running thread */
    struct uthread *temp_uthread;
    struct uthread *cur_uthread;

    /* Initialization */
    temp_uthread = (struct uthread *) malloc(sizeof(struct uthread));
    join_uthread = (struct uthread*)malloc(sizeof(struct uthread));
    cur_uthread = (struct uthread*)malloc(sizeof(struct uthread));
    temp_uthread = running_thread;
    cur_uthread = running_thread;

	if (queue_iterate(ready_list, if_in_queue, &tid, (void**)&(running_thread)) == 0) {
	    /* If thread to join is in ready, put current running thread into blocked */
	    cur_uthread->u_state = BLOCKED;
	    queue_enqueue(blocked_list, cur_uthread);

	    /* Take out the next ready thread */
	    queue_dequeue(ready_list, (void **)&running_thread);
	    running_thread->u_state = RUNNING;

	    /* Update current working TID */
        self_tid = running_thread->u_tid;

        /* Swap to the next thread's context */
        uthread_ctx_switch(temp_uthread->u_ctx, running_thread->u_ctx);
    } else if (queue_iterate(zombie_list, if_in_queue, &tid, (void**)&(join_uthread)) == 0) {
	    /* If thread to join is in zombie, collect it */
	    if (retval != NULL)
	        *retval = join_uthread->join_retval;

	    /* Remove collected thread from zombie queue and free it */
        queue_delete(zombie_list, join_uthread);
        uthread_ctx_destroy_stack((join_uthread->stack));
        free(join_uthread);
    } else {
	    /* Fail to join */
	    return -1;
	}

	return 0;
}
