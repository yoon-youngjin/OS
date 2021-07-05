#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_BUF		1
#define MAX_LOOP 	32

int buffer[MAX_BUF];
int counter = 0;
int in = -1, out = -1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER; //full
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER; // empty

void *producer (void *arg)
{
    int i,j;
    unsigned long data;

    for (i =0; i < MAX_LOOP; i++)  {

        // Working for a moment
	srand(time(NULL));
	for(j=0; j < rand()%0xFFFFFFFF; j++);

	// Assign a new data
	data = (unsigned long)j;

        pthread_mutex_lock(&mutex);	

        // Wait until the consumer dequeues a data
        if (counter == MAX_BUF){
            printf("prod. full!\n");
            pthread_cond_wait(&buffer_has_space, &mutex);
        }

        // Enqueue the new data
        in++; in %= MAX_BUF;
        buffer[in] = (int)data;

        counter++; 

        printf("in:  %d, counter: %d, data: %ld\n", in, counter, data);

        // Signal to the consumer
        pthread_cond_signal(&buffer_has_data); 	

        pthread_mutex_unlock(&mutex);
   }	
}    

void *consumer (void *arg) 
{
    int i,j;
    unsigned long data;

    for (i =0; i < MAX_LOOP; i++)  {

        pthread_mutex_lock(&mutex);	

        // Wait until the producer enqueues a data
        if (counter == 0){
            printf("cons. empty!\n");
            pthread_cond_wait(&buffer_has_data, &mutex);
        }

        // Dequeue a data
        out++; out %= MAX_BUF;
        data = (unsigned long)buffer[out];

        counter--; 

        printf("out: %d, counter: %d, data: %ld\n", out, counter, data);

        // Signal to the producer
        pthread_cond_signal(&buffer_has_space); 	

        pthread_mutex_unlock(&mutex);

        // Working for a moment with the data
	srand(time(NULL));
	for(j=0; j < rand()%0xFFFFFFFF; j++);
    }	
}   

int main(void)
{
    int i; 	

    pthread_t threads[2];

    pthread_create(&threads[0], NULL, &producer, NULL);

    pthread_create(&threads[1], NULL, &consumer, NULL);

    for (i=0; i < 2; i++) 
        pthread_join(threads[i], NULL);

    return 0;
}
