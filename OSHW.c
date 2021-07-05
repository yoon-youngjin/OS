#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<semaphore.h>

#define MAX_BUF 32
#define MAX_LOOP 200

int buffer[MAX_BUF];
int counter = 0;
int in = -1, out = -1;

void *producer(void *);
void *consumer(void *);

sem_t full; // pthread_cond_t ->full
sem_t empty; // pthread_cond_t -> empty
sem_t mutex; // pthread_mutex_t

int main()
{
	int i;
	pthread_t p_threads[5];
	pthread_t c_threads[5];


// 세마포어 변수 초기화
	sem_init(&full,0,0);
	sem_init(&empty,0,MAX_BUF);
	sem_init(&mutex,0,1);


	for(i=0;i<5;i++) {
//생산자 스레드 생성
		pthread_create(&p_threads[i],NULL,producer,(void*)(long)i);
//소비자 스레드 생성
		pthread_create(&c_threads[i],NULL,consumer,(void*)(long)i);
	}
	for(i=0;i<5;i++) {
		pthread_join(p_threads[i],NULL);
		pthread_join(c_threads[i],NULL);
	}




	sem_destroy(&full);
	sem_destroy(&empty);
	sem_destroy(&mutex);
	
	return 0;


}



void *producer(void *arg)
{
	int i;
	long id = (long)arg;
	unsigned long data;

	for(i =0; i < MAX_LOOP; i++) {

	    	srand(time(NULL));
//semaphore값을 감소 , 값이 음수가 될 경우 sem_wait()을 호출한 프로세스는 대기상태
		sem_wait(&empty); // pthread_cond_wait(&buffer_has_space,&mutex);
		

//<---------------------------------cs----------------------------------->
		sem_wait(&mutex); // pthread_mutex_lock(&mutex);
		
		printf("producer%ld cs in \n", id);
		usleep(100);

		in++;
		in %= MAX_BUF;

		buffer[in] = rand()%100;
		data = buffer[in];
	
		counter++;

		printf("in: %d, counter: %d,data: %ld\n",in,counter,data);
		usleep(100);
//<----------------------------------cs----------------------------------->

//semaphore값을 증가, 값이 양수가 아닌 경우,sem_wait()에 의해 대기상태의 프로세스 wakeup

		sem_post(&mutex); // pthread_mutex_unlock(&mutex);
		printf("producer%ld cs out \n ",id);
		sem_post(&full); // pthread_cond_signal(&buffer_has_data);



	}
	
}

void *consumer (void *arg)
{
	int i;
	long id = (long)arg;
	unsigned long data;

	for(i = 0;i<MAX_LOOP;i++) {
		

		sem_wait(&full);// pthread_cond_signal(&buffer_has_data)

//<---------------------------------cs----------------------------------->
		sem_wait(&mutex);// pthread_mutex_lock(&mutex)

		printf("consumer%ld cs in \n",id);
		
		usleep(100);


		out++; out %= MAX_BUF;
		data = (unsigned long)buffer[out];

		counter--;

		printf("out: %d, counter: %d, data: %ld\n",out, counter,data);
		usleep(100);
//<---------------------------------cs------------------------------------>
//
		sem_post(&mutex); // pthread_mutex_unlock(&mutex)
		printf("consumer%ld cs out \n",id);
		sem_post(&empty);// pthread_cond_wait(&buffer_has_space,&mutex)

	
	}
}


