#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 4
#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

static void display_sched_attr(int policy, struct sched_param *param) {
  printf("    policy=%s, priority=%d\n",
         (policy == SCHED_FIFO)    ? "SCHED_FIFO"
         : (policy == SCHED_RR)    ? "SCHED_RR"
         : (policy == SCHED_OTHER) ? "SCHED_OTHER"
                                   : "???",
         param->sched_priority);
}

static void display_thread_sched_attr(char *msg) {
  int policy, s;
  struct sched_param param;

  s = pthread_getschedparam(pthread_self(), &policy, &param);
  if (s != 0)
    handle_error_en(s, "pthread_getschedparam");

  printf("%s\n", msg);
  display_sched_attr(policy, &param);
}

void *func(void *arg) {

  pthread_t id = pthread_self();
  int thread_num = *(int *)arg;
  printf("[] Thread %d started with thread id %lu\n", thread_num,
         (unsigned long)id);

  for (int i = 0; i < 30; i++) {
    int j = 0;
    while (j < 10000) {
      j++;
    }

    if (i % 15 == 0)
      printf("<> In thread %d doing some work\n", thread_num);
  }

  printf("[] Thread %d ended with thread id %lu\n", thread_num,
         (unsigned long)id);
  pthread_exit(NULL);
}

int main() {

  // cpu_set_t set;
  // CPU_ZERO(&set);                                // clear cpu mask
  // CPU_SET(0, &set);                              // set cpu 0
  // sched_setaffinity(0, sizeof(cpu_set_t), &set); // 0 is the calling process

  pthread_t ptids[NUM_THREADS];
  int args[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++)
    args[i] = i + 1;

  int s;
  int policy = SCHED_RR;
  struct sched_param param = (struct sched_param){.sched_priority = 99};
  pthread_attr_t attr;

  s = pthread_setschedparam(pthread_self(), policy, &param);
  if (s != 0) {
    handle_error_en(s, "pthread_setschedparam");
  }
  
  s = pthread_attr_init(&attr);
  if (s != 0) {
    handle_error_en(s, "pthread_attr_init");
  }

  s = pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
  if (s != 0) {
    handle_error_en(s, "pthread_attr_setinheritsched");
  }
  // pthread_attr_setschedpolicy(&attr, policy); // not needed since inherited
  // pthread_attr_setschedparam(&attr, &param); // same

  // Creating a new thread
  for (int i = 0; i < NUM_THREADS; i++) {
    s = pthread_create(&ptids[i], &attr, &func, &args[i]);
    if (s != 0) {
      handle_error_en(s, "pthread_create");
    }
  }

  // Waiting for the created thread to terminate
  pthread_attr_destroy(&attr);
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(ptids[i], NULL);
  }

  return 0;
}
