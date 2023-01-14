#ifndef CMPMC_H
#define CMPMC_H

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHELINE_SIZE 64

typedef struct {
  _Atomic size_t seq;
  void *data;
} cell_t;

typedef char cacheline_pad_t[CACHELINE_SIZE];

typedef struct {
  cacheline_pad_t pad0_;
  cell_t *const buf;
  size_t const buf_mask;
  cacheline_pad_t pad1;
  _Atomic size_t enqueue_pos;
  cacheline_pad_t pad2;
  _Atomic size_t dequeue_pos;
  cacheline_pad_t pad3;
} mpmc_bounded_queue_t;

void init_mpmc_queue(mpmc_bounded_queue_t *q, size_t buf_size);
void destroy_mpmc_queue(mpmc_bounded_queue_t *q);
int mpmc_enqueue(mpmc_bounded_queue_t *q, void *const data);
void *mpmc_dequeue(mpmc_bounded_queue_t *q);

#endif
