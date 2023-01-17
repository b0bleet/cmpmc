# CMPMC
C implementation of Dmitry Vyukov's Bounded MPMC queue.

## API
```c

int cmpmc_init(mpmc_bounded_queue_t *q, size_t buf_size);
void cmpmc_destroy(mpmc_bounded_queue_t *q);
int cmpmc_enq(mpmc_bounded_queue_t *q, void *const data);
void *cmpmc_deq(mpmc_bounded_queue_t *q);

```
