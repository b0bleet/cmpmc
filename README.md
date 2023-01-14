# CMPMC
C implementation of Dmitry Vyukov's Bounded MPMC queue.

## API
```c

void init_mpmc_queue(mpmc_bounded_queue_t *q, size_t buf_size);
void destroy_mpmc_queue(mpmc_bounded_queue_t *q);
int mpmc_enqueue(mpmc_bounded_queue_t *q, void *const data);
void *mpmc_dequeue(mpmc_bounded_queue_t *q);


```
