#include <cmpmc.h>

int cmpmc_init(mpmc_bounded_queue_t *q, size_t buf_size) {
  assert((buf_size >= 2) && ((buf_size & (buf_size - 1)) == 0));

  cell_t *alloc_buf = calloc(buf_size, sizeof(cell_t));
  if (alloc_buf == NULL)
    return -1;
  *(cell_t **)&q->buf = alloc_buf;
  *(size_t *)(&q->buf_mask) = (buf_size - 1);

  for (size_t i = 0; i != buf_size; i += 1)
    atomic_store_explicit(&q->buf[i].seq, i, memory_order_relaxed);

  atomic_store_explicit(&q->enqueue_pos, 0, memory_order_relaxed);
  atomic_store_explicit(&q->dequeue_pos, 0, memory_order_relaxed);
  return 0;
}

void cmpmc_destroy(mpmc_bounded_queue_t *q) {
  if (q->buf)
    free(q->buf);
}

int cmpmc_enq(mpmc_bounded_queue_t *q, void *const data) {
  cell_t *cell;
  size_t pos = atomic_load_explicit(&q->enqueue_pos, memory_order_relaxed);
  for (;;) {
    cell = &q->buf[pos & q->buf_mask];
    size_t seq = atomic_load_explicit(&cell->seq, memory_order_acquire);

    intptr_t dif = (intptr_t)seq - (intptr_t)pos;
    if (dif == 0) {
      if (atomic_compare_exchange_weak_explicit(&q->enqueue_pos, &pos, pos + 1,
                                                memory_order_relaxed,
                                                memory_order_relaxed))
        break;
    } else if (dif < 0) {
      return -1;
    } else {
      pos = atomic_load_explicit(&q->enqueue_pos, memory_order_relaxed);
    }
  }

  cell->data = data;
  atomic_store_explicit(&cell->seq, pos + 1, memory_order_release);
  return 0;
}

void *cmpmc_deq(mpmc_bounded_queue_t *q) {
  cell_t *cell;
  void *data;
  size_t pos = atomic_load_explicit(&q->dequeue_pos, memory_order_relaxed);
  for (;;) {
    cell = &q->buf[pos & q->buf_mask];
    size_t seq = atomic_load_explicit(&cell->seq, memory_order_acquire);
    intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);
    if (dif == 0) {
      if (atomic_compare_exchange_weak_explicit(&q->dequeue_pos, &pos, pos + 1,
                                                memory_order_relaxed,
                                                memory_order_relaxed))
        break;
    } else if (dif < 0) {
      return NULL;
    } else {
      pos = atomic_load_explicit(&q->dequeue_pos, memory_order_relaxed);
    }
  }

  data = cell->data;
  atomic_store_explicit(&cell->seq, pos + q->buf_mask + 1,
                        memory_order_release);
  return data;
}
