# Kernel Synchronization — Race Condition & Mutex

# 1. Why Synchronization Is Needed

When multiple execution contexts access the same shared data, the operations can interfere with each other.

Example:

```c
int counter = 0;

counter++;
```

Although `counter++` looks like one C operation, conceptually it is:

```text
LOAD counter
     ↓
ADD 1
     ↓
STORE counter
```

If two threads perform this at the same time, their operations can interleave.

---

# 2. Race Condition

Example:

```text
Initial counter = 0

Thread A                 Thread B

LOAD counter
→ 0
                        LOAD counter
                        → 0

ADD 1
→ 1
                        ADD 1
                        → 1

STORE 1
                        STORE 1
```

Final result:

```text
counter = 1
```

Expected:

```text
counter = 2
```

One update has been lost.

### Important Point

> A race condition occurs when the correctness of a program depends on the timing/interleaving of concurrent execution.

---

# 3. Critical Section

The code that accesses shared data and must be protected is the **critical section**.

Example:

```c
counter++;
```

Conceptually:

```text
             Shared Resource
                    |
                    ↓
          ┌──────────────────┐
          │ Critical Section │
          │                  │
          │ READ             │
          │ MODIFY           │
          │ WRITE            │
          └──────────────────┘
```

The goal of synchronization is to ensure that incompatible concurrent accesses do not happen simultaneously.

---

# 4. Mutex

A **mutex** provides mutual exclusion.

Basic pattern:

```c
pthread_mutex_lock(&lock);

counter++;

pthread_mutex_unlock(&lock);
```

Conceptually:

```text
Thread A

LOCK
  ↓
critical section
  ↓
UNLOCK
```

While A owns the mutex:

```text
Thread B
   |
   ↓
LOCK
   |
   ↓
WAIT
```

After A releases it:

```text
Thread A
   |
UNLOCK
   |
   ↓
Thread B
   |
LOCK
   |
critical section
   |
UNLOCK
```

### Important Point

> A mutex does not make `counter++` itself atomic. It provides mutual exclusion around the critical section.

---

# 5. Lab — Race Condition vs Mutex

## Source Code

```c
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define ENABLE_MUTEX        1
#define MAX_INCREMENT_COUNT 100000

static int counter = 0;

#if (ENABLE_MUTEX == 1)
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#endif

void *func(void *args)
{
    int i = 0;

    for (i = 0; i < MAX_INCREMENT_COUNT; i++)
    {
#if (ENABLE_MUTEX == 1)
        pthread_mutex_lock(&lock);

        counter++;

        pthread_mutex_unlock(&lock);
#else
        counter++;
#endif
    }

    return NULL;
}

int main()
{
    pthread_t p1, p2;

    pthread_create(&p1, NULL, func, NULL);
    pthread_create(&p2, NULL, func, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Counter Value: %d\n", counter);

    return 0;
}
```

---

## Without Mutex

```text
Thread 1 ───────┐
                ├── counter++ ──┐
Thread 2 ───────┘               │
                                ↓
                         Race Condition
```

## With Mutex

```text
Thread 1 ── LOCK ── counter++ ── UNLOCK
                         |
                         |
Thread 2 ───────────── WAIT
                         |
                         ↓
                       LOCK
                    counter++
                       |
                    UNLOCK
```

---

# 9. C → CPU → ARM64 Connection

This experiment is important for understanding synchronization at the ARM64 level.

The C statement:

```c
counter++;
```

can conceptually become:

```text
LOAD
 ↓
MODIFY
 ↓
STORE
```

Therefore, multiple execution contexts can interfere unless the operation is protected or implemented using an appropriate atomic mechanism.

The learning progression is:

```text
C
 ↓
Shared Data
 ↓
Race Condition
 ↓
Critical Section
 ↓
Mutex
 ↓
Spinlock
 ↓
Atomic Operations
 ↓
ARM64 LDXR / STXR
 ↓
Memory Ordering
```

The ARMv8 architecture provides mechanisms for atomicity and synchronization. These will be studied later at the instruction and memory-model level.

---

# 10. Kernel Connection

The same basic pattern appears in Linux kernel code:

```c
mutex_lock(&my_mutex);

modify_shared_data();

mutex_unlock(&my_mutex);
```

But kernel synchronization is more complicated because Linux has multiple execution contexts and concurrency sources:

```text
Process Context
      +
Interrupt Context
      +
Multiple CPUs
      +
Preemption
      +
Memory Ordering
```

This leads to the need for different synchronization primitives.

---