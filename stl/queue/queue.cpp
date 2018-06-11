#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <iostream>

#define MAX_INDEX 10
#define MIN_INDEX 0

using namespace std;

int main(void)
{
	queue<int> Queue1;

	/* Push */
	printf("We push the value in queue from %d to %d\n", MIN_INDEX, MAX_INDEX - 1);
	for (int i = MIN_INDEX; i < MAX_INDEX; i++) {
		Queue1.push(i);
	}

	/* Check the queue is empty or not */
	if (Queue1.empty()) {
		fprintf(stderr, "Queue is empty\n");
		return 0;
	}

	/* Pop */
	int pop_count = 0;
	while (!Queue1.empty()) {
		printf("Queue is %d\n", Queue1.front());
		Queue1.pop();
		pop_count++;
	}

	printf("We pop %d times\n", pop_count);
	
	return 0;
}
