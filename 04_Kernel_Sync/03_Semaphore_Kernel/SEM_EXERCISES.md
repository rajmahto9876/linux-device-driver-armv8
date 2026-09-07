# Spinlock
---

# Q1) Why can't a spinlock be used to protect data shared with a hard IRQ handler if you only use plain spin_lock()/spin_unlock() in process context? Walk through the exact deadlock scenario step by step.

## Q2) Explain why spin_lock_irqsave() needs to save flags rather than just calling local_irq_disable() unconditionally. Give a concrete nesting scenario where this distinction matters.

## Q3) Why is busy-waiting acceptable for spinlocks but not acceptable for mutexes/semaphores? What kernel-level assumption makes it "acceptable" at all?

## Q4) What's the difference between spin_lock_bh() and spin_lock_irqsave()? Give an example of a scenario where using the wrong one would still "work" most of the time but is technically incorrect.

## Q5)Why are Linux spinlocks non-recursive? What design alternative would recursion require, and why does the kernel avoid it?

## Q6)What does lockdep actually track, and why can it warn you about a deadlock that hasn't happened yet?

# Semaphores
---

## Q7) Why does a semaphore have no concept of "ownership" while a mutex does? Give a real driver scenario where this lack of ownership is exactly the feature you need.

## Q8) Explain, step by step, why calling down() (not down_interruptible()) in a syscall path is an operational hazard — what does the user experience, and how would you notice it in production?

## Q9) What's the actual difference between down_interruptible() returning nonzero due to a signal, vs. down_trylock() returning nonzero? When would you choose one API over the other?

## Q10) Why is it unsafe to call down()/down_interruptible() while holding a spinlock? Trace the exact mechanism of what breaks.

## Q11)Describe priority inversion using a semaphore-based example with a low, medium, and high priority task. Why doesn't a plain struct semaphore solve this, and what primitive would?

## Q12) Two threads use two different semaphores (or a semaphore and a mutex) to protect the same shared struct. Why is this broken even if each individual lock is "used correctly" in isolation?

# Coding Problem Statements:

## Spinlock Problems
---

### S1 — Shared Statistics Struct
Write a module with a struct stats { unsigned long rx_packets; unsigned long rx_bytes; } protected by a spinlock. Simulate an IRQ-like context (use a tasklet or hrtimer in atomic context) that updates both fields "atomically" together every 50ms, and a /proc read handler in process context that reads both fields. Deliberately design it so that without the lock, a reader could see rx_packets from tick N but rx_bytes from tick N+1 (a torn read across two fields) — then add the correct locking and explain why the bug is now impossible.

### S2 — Trylock-Based Non-Blocking Stats Reporter
Write a module where a background kthread holds a spinlock for a deliberately "long" critical section (simulate with a tight busy-loop of ~100,000 iterations inside the lock — never use msleep inside a spinlock). Add a /proc read handler that uses spin_trylock() and returns -EBUSY (or a "busy" message) if it can't get the lock immediately, instead of blocking. Verify with concurrent access that you sometimes get the busy path and sometimes get real data.

### S3 — AB-BA Deadlock, On Purpose
Write a module with two spinlocks, lock_a and lock_b, and two /proc entries. Writing to entry 1 triggers a code path that takes lock_a then lock_b. Writing to entry 2 triggers a path that takes lock_b then lock_a. Reproduce the deadlock by triggering both concurrently from two shells, capture the lockdep splat from dmesg, and then fix it by enforcing a single global lock ordering.

### S4 — rwlock_t Conversion
Take a simple shared counter protected by a plain spinlock_t, convert it to rwlock_t, and add a /proc entry that only reads (using read_lock) plus a separate mechanism that writes (using write_lock). Instrument with pr_info() to prove that concurrent reads don't serialize against each other the way writes do.

## Semaphore Problems
---

### M1 — Bounded Resource Pool
Model a pool of N=3 "hardware channels" using a counting semaphore. Write an ioctl() or /proc write that "acquires" a channel (down_interruptible), sleeps briefly to simulate use, then "releases" it (up). Launch 6 concurrent user-space processes (simple shell script with &) trying to acquire channels simultaneously and confirm via dmesg timestamps that only 3 run concurrently while the rest queue.

### M2 — Unbalanced up()/down() Corruption Detector
Write a module that intentionally has a bug: one specific error-handling path calls up() without a matching prior down() having failed correctly (i.e., double-up on an error branch). Add an assertion/pr_err() that checks an invariant (free + filled == POOL_SIZE) after every operation and have it fire when the corruption occurs. Find the bug from the log, not by reading your own code — pretend you didn't write it.

### M3 — Producer/Consumer with Graceful Shutdown
Extend a producer/consumer module (kthread producer + /proc-write consumer) so that rmmod cleanly shuts down the producer thread even if it's currently blocked in down_interruptible() waiting for a free slot. Prove it works by loading the module, filling the pool completely (so the producer is guaranteed blocked), and then unloading — it should not hang.

### M4 — Semaphore Used as a Signal (Completion Preview)
Initialize a semaphore to 0. Have a kthread call down_interruptible() on it and log when it wakes. Have a /proc write handler call up() on it. Test the "signal arrives before wait starts" case: trigger the /proc write before the kthread has even started running (you'll need to introduce an artificial startup delay in the kthread), and confirm the kthread still doesn't miss the signal when it eventually calls down(). Write a short note on why this works — this is the exact property that struct completion formalizes.

## Combined / Stretch Problems

### C1 — Spinlock Protecting a Semaphore's Consumer Path
Design a scenario where a spinlock protects fast bookkeeping (e.g., an in-memory ring buffer index) while a semaphore governs how many items are available to consume — similar to how real network drivers separate "fast path atomic updates" from "sleeping consumer wake-up." Get the layering right: what's under the spinlock, what's under the semaphore, and why sleeping must never happen while the spinlock is held.

### C2 — Diagnose From Symptoms Only
Write two separate broken modules (don't look at them side by side) — one with a spinlock self-deadlock, one with a semaphore double-up bug. Give yourself only the dmesg output and ps -eo pid,stat,comm output as your "bug report" a day later, and diagnose which bug is which and why, using only the symptoms (soft lockup / 100% CPU vs. D-state hang vs. silent count corruption) — this simulates real on-call debugging where you don't have the diff in front of you