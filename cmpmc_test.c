#define _GNU_SOURCE
#include <cmpmc.h>
#include <pthread.h>
#include <unistd.h>
#include <x86intrin.h>

static const size_t thread_count = 4;
static const size_t batch_size = 1;
static const size_t iter_count = 2000000;

bool volatile g_start = 0;

void *threadfunc(void *ctx) {
  mpmc_bounded_queue_t *q = ctx;

  srand((unsigned)time(0) + gettid());
  size_t pause = rand() % 1000;

  while (g_start == 0)
    sched_yield();

  for (size_t i = 0; i != pause; i += 1)
    _mm_pause();

  for (int iter = 0; iter != iter_count; ++iter) {
    for (size_t i = 0; i != batch_size; i += 1) {
      while (mpmc_enqueue(q, &i) < 0) {
        sched_yield();
      }
    }
    for (size_t i = 0; i != batch_size; i += 1) {
      while (mpmc_dequeue(q) == NULL) {
        sched_yield();
      }
    }
  }

  return NULL;
}

int mpmc_thread_test() {
  mpmc_bounded_queue_t *q = malloc(sizeof(mpmc_bounded_queue_t));
  init_mpmc_queue(q, 1024);

  pthread_t thread[thread_count];
  for (int i = 0; i < thread_count; i++) {
    pthread_create(&thread[i], NULL, threadfunc, q);
  }

  sleep(1);

  uint64_t start = __rdtsc();
  g_start = 1;

  for (int i = 0; i < thread_count; i++) {
    pthread_join(thread[i], NULL);
  }

  uint64_t end = __rdtsc();
  uint64_t delta = end - start;
  printf("cycles/op=%ld\n",
         delta / (batch_size * iter_count * 2 * thread_count));

  destroy_mpmc_queue(q);
  free(q);

  return 0;
}

int main(int argc, char *argv[]) {
  mpmc_thread_test();
  return 0;
}
