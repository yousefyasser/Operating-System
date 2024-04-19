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

static void usage(char *prog_name, char *msg) {
  if (msg != NULL)
    fputs(msg, stderr);

  fprintf(stderr, "Usage: %s [options]\n", prog_name);
  fprintf(stderr, "Options are:\n");
#define fpe(msg) fprintf(stderr, "\t%s", msg) /* Shorter */
  fpe("-p<policy><prio> Set scheduling policy and priority on\n");
  fpe("                 all threads before pthread_create() call\n");
  fpe("                 <policy> can be\n");
  fpe("                     f  SCHED_FIFO\n");
  fpe("                     r  SCHED_RR\n");
  fpe("                     o  SCHED_OTHER\n");
  fpe("                 <prio> is the priority\n");
  fpe("                 For SCHED_OTHER only priority 0 is allowed\n");
  fpe("                 For SCHED_RR/SCHED_FIFO min/max priority is 1/99 \n");
  fpe("                 Example: -pf99\n");
  exit(EXIT_FAILURE);
}

static int get_policy(char p, int *policy) {
  switch (p) {
  case 'f':
    *policy = SCHED_FIFO;
    return 1;
  case 'r':
    *policy = SCHED_RR;
    return 1;
  case 'o':
    *policy = SCHED_OTHER;
    return 1;
  default:
    return 0;
  }
}

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

// none blocking sleep, so that the threads are not preempted
void noop() {
  int i = 0;
  while (i < 10000000) {
    i++;
  }
}

// Function to be executed by the thread
void *thread_start(void *arg) {
  pthread_t id = pthread_self();
  int thread_num = *(int *)arg;
  printf("[] Thread %d started with thread id %lu\n", thread_num,
         (unsigned long)id);

  for (int i = 0; i < 30; i++) {
    noop();
    if (i % 15 == 0)
      printf("<> In thread %d doing some work\n", thread_num);
  }

  printf("[] Thread %d ended with thread id %lu\n", thread_num,
         (unsigned long)id);
  printf("thread %d: %lf\n", thread_num, (double)clock());
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  char *policy_str = NULL;
  int s, opt, policy;
  pthread_attr_t attr;
  int args[NUM_THREADS];
  struct sched_param param;
  pthread_t ptids[NUM_THREADS];

  // Setting the process to run on one core
  cpu_set_t set;
  CPU_ZERO(&set);                                // clear cpu mask
  CPU_SET(0, &set);                              // set cpu 0
  sched_setaffinity(0, sizeof(cpu_set_t), &set); // 0 is the calling process

  // Parsing the command line arguments
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
    case 'p':
      policy_str = optarg;
      break;
    default:
      usage(argv[0], "Unrecognized option\n");
    }
  }

  if (policy_str == NULL) {
    usage(argv[0], "Policy not set\n");
  }

  get_policy(policy_str[0], &policy);
  param.sched_priority = strtol(&policy_str[1], NULL, 0);

  if (policy == SCHED_OTHER && param.sched_priority != 0) {
    usage(argv[0], "For SCHED_OTHER only priority 0 is allowed\n");
  }

  if (policy == SCHED_RR || policy == SCHED_FIFO) {
    if (param.sched_priority < sched_get_priority_min(policy) ||
        param.sched_priority > sched_get_priority_max(policy)) {
      usage(argv[0], "Priority out of range\n");
    }
  }

  // Setting the scheduling policy for main thread
  s = pthread_setschedparam(pthread_self(), policy, &param);
  if (s != 0) {
    handle_error_en(s, "pthread_setschedparam");
  }
  display_thread_sched_attr("* Threads attributes");

  // Setting the scheduling policy for the new threads
  s = pthread_attr_init(&attr);
  if (s != 0) {
    handle_error_en(s, "pthread_attr_init");
  }

  s = pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
  if (s != 0) {
    handle_error_en(s, "pthread_attr_setinheritsched");
  }

  // Creating a new thread
  for (int i = 0; i < NUM_THREADS; i++) {
    args[i] = i + 1;
    printf("thread %d: %lf\n", i + 1, (double)clock());
    s = pthread_create(&ptids[i], &attr, &thread_start, &args[i]);
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
