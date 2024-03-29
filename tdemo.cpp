#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t ntid;

void printids(const char *s)
{
	pid_t pid;
	pthread_t tid;
	pid=getpid();
	tid=pthread_self();
	printf("%s pid %u tid %u (0x%x) \n",s,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid);

}

void * thr_fn(void *arg)
{
	printids("new thread: ");
	return ((void *)0);
}

int main()
{
   int err;
   err=pthread_create(&ntid,NULL,thr_fn,NULL);
   if(err!=0)
     printf("\n Can't create threads\n");
   printids("main thread: ");
   sleep(1);
   _exit(0);
	return 0;
}