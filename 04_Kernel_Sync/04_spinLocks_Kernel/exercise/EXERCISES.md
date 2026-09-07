# completion_demo Exercises

Build and load first:
```
make
sudo insmod completion_demo.ko
dmesg -w &         # keep this open in another terminal throughout
```

## Warm-up: observe the two patterns

1. Run `cat /proc/completion_demo` immediately after insmod. It should
   block for ~3s then print "device ready". Now run it again — it
   should return instantly. Explain in your own words why the second
   call doesn't block. (Hint: look at what `done` equals after the
   first `complete()`.)

2. Watch dmesg after insmod: four `comp_waiter%ld` threads should log
   that they're blocking on `release_gate`. Run
   `echo 1 > /proc/completion_demo_release` and confirm all four wake
   up together, not one at a time. Now change `complete_all()` to
   `complete()` in `release_write()`, rebuild, reload, and release
   again — confirm only one waiter wakes, and figure out how you'd
   release the rest (hint: you'd need three more `complete()` calls,
   or count via `swait` internals — this is exactly why `complete_all()`
   exists instead of a manual loop).

## Exercise 1 — lost generation via reinit_completion()

Add a second `/proc` write handler that calls `reinit_completion(&init_done)`
followed by re-spawning `worker_thread`. Trigger this *while* a
`cat /proc/completion_demo` is still blocked in `wait_for_completion()`
from a *previous* generation. Observe what happens to that blocked
reader. Now fix it: what invariant must hold before it's safe to call
`reinit_completion()`? Write the fix as a comment above your reinit call.

## Exercise 2 — unkillable completion wait

Change `wait_for_completion()` in `status_read()` to a version that
would make `cat /proc/completion_demo` unkillable if `worker_fn` never
calls `complete()` (e.g. comment out `msleep(3000); complete(&init_done);`
entirely and rebuild). Confirm with:
```
cat /proc/completion_demo &
kill -9 %1
ps -o stat,cmd -p $!
```
The process should sit in `D` state, immune to SIGKILL. Now fix it
using `wait_for_completion_killable()` or `_interruptible()` and confirm
`kill` works. Note which one you chose and why.

## Exercise 3 — timeout as a hung-hardware defense

Replace `wait_for_completion()` in `status_read()` with
`wait_for_completion_timeout(&init_done, msecs_to_jiffies(500))` (shorter
than the worker's 3s init). Confirm the read now returns `-ETIMEDOUT`-style
behavior instead of hanging. This is the pattern real drivers use to
avoid an unresponsive-hardware hang turning into a permanently stuck
process — cross-reference with `CONFIG_DETECT_HUNG_TASK` from your
semaphore module: would that config option have caught the *original*
undetected hang here, or only the killable-D-state one from Exercise 2?

## Exercise 4 — use-after-free race (read-only, do not run destructively)

Sketch (in comments, don't actually crash your VM) a driver-remove
path where:
```
complete(&some_completion);   /* in the ISR/worker */
```
races against:
```
wait_for_completion(&some_completion);
kfree(dev);                   /* in the remove() path, dev embeds the completion */
```
Identify the exact interleaving that produces a use-after-free, and
state the ordering rule that avoids it (hint: which side must fully
finish touching `dev` before the other side is allowed to proceed, and
does `wait_for_completion()` returning guarantee that on its own?).
