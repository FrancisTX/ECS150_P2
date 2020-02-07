## ECS150 Project 2 Report   
### queue API
In the queue API, we need to implement the queue using our own data structure. 
We use linked list as our basic data structure. Basically, the structure are 
parts. The first parts is the `Node`, which include the `void* data` and 
`struct Node* next;`. The second part is as the queue itself, we have 
`struct queue`, which includes `struct Node* head`, `struct Node* tail` and
`cur_size`. Here, we really need a `struct Node* tail` to keep track of our 
end of the queue, because all the function, except the `queue_delete` and 
`queue_iterate`, should be `O(1)`. If we have `head` and `tail`, we can easily 
finish the `queue_dequeue` and `queue_enqueue` in `O(1)` time complexity.

##### `queue_create`
Here, we need to create the queue, which means we need to initialize the 
linked list. Therefore, we use `malloc` to allocate a space for linked list 
and make it head and tail points to `NULL`. Also, we need to make `tail` 
equal to `head`, because they points to the same address when there is no 
elements in the queue.

##### `queue_destroy`
Here, we just try to make `head` and `tail` points to `NULL`, then we will 
destroy all the queue element in the linked list. Next, it is free the `mlloc`
and set the size of queue to 0. 

##### `queue_enqueue`
For the enqueue, we want to put the new elements at the end of linked list. 
We have the `struct Node* tail` and we just make the `tail` points to the 
new element and the make this new element points to the `tail` to be a new 
tail. Then, we add the size by 1.

##### `queue_dequeue`
For the dequeue, we should move the oldest element because queue is `FIFO`.
Here, it is also easy to do, because we have `struct Node* head` and it is 
the oldest element we put into the queue. We just ignore this node in the 
queue and make the new head points to the original head's next. Then, we 
decrease the size by 1.

##### `queue_delete`
This is a little bit completed if we use linked list, because we need to 
traversal the whole queue and find where the node we want to delete is. 
Also, we need to get the previous node and next node of the node we want 
to delete. Finally, we link the previous node and the next node.

##### `queue_iterate`
We use a for loop to traversal the entire queue and for each element in queue
we past it as arguments to `queue_func_t func()`. Its return value will store
as data and pass it to the queue's current element.


### uthread API
##### Global Variables
We use three queues to store ready, blocked, and zombie threads. 
`running_thread` stores current running thread and `self_tid` stores current 
`TID`.

##### `uthread_yield`
This function yields the current running thread and execute the next ready 
thread.
First, we need to check if there exist threads in ready state. If no, 
continue to current thread. Else, we yield current thread and block it if it 
is the main thread or put it into ready queue otherwise. Then, we take out the 
first thread in the ready queue and execute it.

##### `uthread_self`
This function just return the `TID` of current running thread. We use a global 
variable `self_tid` to store and update the current `TID`.

##### `uthread_create`
This function creates a new thread and put it into ready queue. If we are 
creating the first thread, we need initialization with function 
`uthread_init()`. In this function, we set current running thread to be 
the main thread and create space for queues that store ready, blocked, 
and zombie threads. After initialization for the first thread, new threads 
are directly created and stored into ready queue. We need to create space 
for the stack and context of the thread with `malloc()`. Finally, we 
return the created thread's `TID`.

##### `uthread_exit`
This function exit from current running thread. If the main thread calls 
it, all the threads are completed and collected; thus, we free all memory 
and exit. Otherwise, we put the current running thread into zombie queue 
waiting to be collected. Then, we check whether there exist threads in ready 
state. If so, take out from ready queue and execute it. If the ready queue is
empty, we need to check blocked threads. If there exists blocked threads, 
take out from blocked queue and execute it.

##### `uthread_join`
This function joins a thread at given `TID`. Before we join the thread, we
need to make sure that main thread cannot be joined. We build a callback 
function `if_in_queue`, which is called by `queue_iterate()` and checks
whether the thread to be joined is in the queue. `if_in_queue` simply
compares current thread with the target thread. If we find the target thread
in ready queue, block current thread and execute the next ready thread. If 
it is in zombie queue, collect it with given return value and free memory 
of stack. If it is not in ready queue or zombie queue, we can not join.


### Preemption
For this part, we need to implement the preemption. We have two modes here,
enable and disable. We can use the preemption or not. In "enabled mode",
We can force some thread to yield to next thread by setting a timer and 
protect others thread in this way.

#### preempt_start
Here, we use `sigaction` to throw signal when a thread is running longer 
than 100HZ, and also we use `settimer()` to set a timer to control the 
time interval. If the timer is out of time, we just send a signal.

#### preempt_{enable,disable}
We need to enable or disable the preemption. What we are trying to do is 
to use `sigpromask` to set the signal block or unblock with `SIG_BLOCK` 
and `SIG_UNBLOCK`. When the preemption is enabled, we unblock the signal,
which means if the timer runs out of the time, the signal will interrupt 
the thread and make it run `uthread_yield`. For the disabled preemption, 
we block the signal. That means the timer runs out of the time and want 
to throw the signal, the signal will be blocked.






