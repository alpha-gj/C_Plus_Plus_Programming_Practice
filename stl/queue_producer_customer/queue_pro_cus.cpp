#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <iostream>

#define MAX_INDEX 10
#define MIN_INDEX 0

using namespace std;

queue<int> Queue1;
pthread_mutex_t QueueMutex; 

typedef enum {
	PUSH,
	POP,
	CHECK,
} QUEUE_HANDLE;

void queueu_handle(QUEUE_HANDLE handle, int val) 
{
	pthread_mutex_lock(&QueueMutex);

	if (handle == PUSH) {

		Queue1.push(val);

	} else if (handle == POP) {

		printf("It will pop is %d\n", Queue1.front());
		Queue1.pop();
		
	}

	pthread_mutex_unlock(&QueueMutex);
}

void customer(void *t)
{
	while(1) {
		/* Check the queue is empty or not */
		if (Queue1.empty()) {
			fprintf(stderr, "Queue is empty\n");
		} else {
			queueu_handle(POP, NULL);
		}
		sleep(1);
	};
}

void producer(void *t)
{
	int input = 0;
	char get_char[10] = {};
	while(1) {
		/* Check the queue is empty or not */
		printf("enter :)\n");
		fgets (get_char, 10, stdin);
		input = atoi(get_char);
		queueu_handle(PUSH, input);
	};
}

int main(void)
{
	pthread_mutex_init(&QueueMutex, NULL);

	pthread_t tid[2];
	pthread_create(&tid[0], NULL, (void *(*)(void *))customer, NULL);
	pthread_create(&tid[1], NULL, (void *(*)(void *))producer, NULL);

	for (int t = 0; t < 2; t++) {
		pthread_join(tid[t], NULL);
	}

	printf("GoodBye\n");
	pthread_mutex_destroy(&QueueMutex);
	pthread_exit(NULL);
	return 0;
}
