#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define TEST_ASSERT(assert)			\
do {						\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {				\
		printf("PASS\n");		\
	} else	{				\
		printf("FAIL\n");		\
		exit(1);			\
	}					\
} while(0)

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
	int *a = (int*)data;
	int inc = (int)(long)arg;

	*a += inc;

	return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
	int *a = (int*)data;
	int match = (int)(long)arg;

	if (*a == match)
		return 1;

	return 0;
}

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(ptr == &data);
}

void test_queue_dequeue_2times(void)
{
	int data = 3, *ptr ,*ptr_2;
	int data_2 = 5;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
        queue_enqueue(q, &data_2);
	queue_dequeue(q, (void**)&ptr_2);
	

	TEST_ASSERT(ptr_2 == &data_2);
}

void test_queue_dequeue_4times(void)
{
	int data = 3, *ptr ,*ptr_2, *ptr_3,*ptr_4;
	int data_2 = 5;
	int data_3 = 8;
	int data_4 = 10;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
        queue_enqueue(q, &data_2);
	queue_dequeue(q, (void**)&ptr_2);
	queue_enqueue(q, &data_3);
	queue_dequeue(q, (void**)&ptr_3);
        queue_enqueue(q, &data_4);
	queue_enqueue(q, &data_3);
	queue_dequeue(q, (void**)&ptr_4);
	

	TEST_ASSERT(ptr_4 == &data_4);
}



void test_edqueue_NULL_data(void)
{
	void* data = NULL;
	queue_t q;

	fprintf(stderr, "*** TEST edqueue_NULL_data ***\n");

	q = queue_create();
	queue_enqueue(q, data);


	TEST_ASSERT(queue_enqueue(q, data) == -1);
}

void test_edqueue_NULL_queue(void)
{
	int data = 1;
	queue_t q;

	fprintf(stderr, "*** TEST edqueue_NULL_queue ***\n");

	q = queue_create();
	queue_destroy(q);
	queue_enqueue(q, &data);


	TEST_ASSERT(queue_enqueue(q, &data) == -1);
}
void test_dequeue_null_queue(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST dequeue_null_queue ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_destroy(q);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT((queue_dequeue(q, (void**)&ptr)) == -1);
}
void test_dequeue_null_data(void){
	void* data = NULL;
	int *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST dequeue_null_data ***\n");

	q = queue_create();
	queue_enqueue(q, &data);

	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT((queue_dequeue(q, (void**)&ptr)) == -1);
}

void test_dequeue_empty(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST dequeue_empty ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT((queue_dequeue(q, (void**)&ptr)) == -1);
}

void test_delete_not_find(void)
{
	int data = 3, data_2 = 5;
	queue_t q;

	fprintf(stderr, "*** TEST delete_not_find ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_2);
	queue_delete(q, (void*)4);

	TEST_ASSERT(queue_delete(q, (void*)4) == -1);
}

void test_delete_empty(void){
	int data = 3, data_2 = 5, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST delete_empty ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_2);
	queue_dequeue(q,  (void**)&ptr);
	queue_dequeue(q,  (void**)&ptr);
	queue_delete(q, (void*)4);

	TEST_ASSERT(queue_delete(q, (void*)4) == -1);
}

void test_delete_NULL_queue(void){
	int data = 3, data_2 = 5, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST delete_NULL_queue ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_2);
	queue_destroy(q);
	queue_delete(q, (void*)4);

	TEST_ASSERT(queue_delete(q, (void*)4) == -1);
}

void test_delete_data_NULL(void){
	int data = 3;
	void* data_2 = NULL;
	queue_t q;

	fprintf(stderr, "*** TEST delete_data_NULL ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_delete(q, NULL);

	TEST_ASSERT(queue_delete(q, NULL) == -1);
}

void test_destroy_nonempty(void){
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST destroy_nonempty ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_destroy(q);

	TEST_ASSERT(queue_destroy(q) == -1);
}

void test_destroy_NULL(void){
	queue_t q;

	fprintf(stderr, "*** TEST destroy_NULL ***\n");

	q = queue_create();
	queue_destroy(q);
	queue_destroy(q);

	TEST_ASSERT(queue_destroy(q) == -1);
}


void test_iterator(void)
{
	fprintf(stderr, "*** TEST iterator ***\n");
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;
	int *ptr;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Add value '1' to every item of the queue */
	queue_iterate(q, inc_item, (void*)1, NULL);
	TEST_ASSERT(data[0] == 2);

	/* Find and get the item which is equal to value '5' */
	ptr = NULL;
	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(*ptr == 5);
	TEST_ASSERT(ptr == &data[3]);
}

int main(void)
{/*
	test_destroy_NULL();
	test_destroy_nonempty();
	test_queue_simple();
	test_iterator();
	test_edqueue_NULL_data();
	test_dequeue_null_queue();
	test_dequeue_null_data();
	test_dequeue_empty();
	test_create();

        test_edqueue_NULL_queue();
        test_delete_NULL_queue();
	test_delete_not_find();
	test_delete_empty();
	test_delete_data_NULL();   */
	test_queue_dequeue_4times();
	return 0;

}
