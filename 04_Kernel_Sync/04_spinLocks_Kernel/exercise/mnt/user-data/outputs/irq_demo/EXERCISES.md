# irq_demo Exercises

## Setup A — QEMU / no hardware (fallback mode)

```
make
sudo insmod irq_demo.ko
dmesg -w &
cat /proc/irq_demo        # blocks up to ~5s, then prints events=N mode=hrtimer-fallback
```
You should see, every ~5s in dmesg, a "top half" line followed ~50ms
later by a "bottom half" line from a `kworker` thread. Confirm the top
half log shows `in_hardirq=1` and the bottom half shows `in_interrupt=0`
— that's the context switch you're here to internalize.

## Setup B — BeagleBone Black, real GPIO interrupt (loopback wire)

This gets you a genuine hardware-triggered IRQ without needing any
external component beyond one jumper wire.

1. Pick two free GPIO pins on the P9 header, e.g. **P9_12** (as output)
   and **P9_15** (as input, will be your IRQ source). Check your
   device tree / `config-pin` support for your kernel; on many BBB
   images:
   ```
   config-pin P9_12 gpio
   config-pin P9_15 gpio
   ```
2. Find their gpiochip-relative line numbers and export the input pin
   as an interrupt source. On modern kernels, the cleanest way to get
   an IRQ number for a bare GPIO line without writing your own gpiod
   consumer code is via `gpio-keys` in an overlay, or by using the
   `libgpiod` tool to find the line and cross-referencing
   `/sys/kernel/debug/gpio` for the global GPIO number, then:
   ```
   echo <gpio_num> > /sys/class/gpio/export
   echo in > /sys/class/gpio/gpioN/direction
   echo both > /sys/class/gpio/gpioN/edge
   cat /sys/kernel/debug/interrupts | grep gpio   # or check /proc/interrupts after insmod
   ```
   The exact GPIO/IRQ numbers depend on your device tree overlay --
   this is intentionally left for you to derive from your board's
   pinmux tables, matching your existing habit of deriving internals
   rather than being handed them.
3. Physically jumper P9_12 to P9_15.
4. Load the module against that IRQ number:
   ```
   sudo insmod irq_demo.ko irq_num=<N>
   ```
5. Toggle the output pin from userspace to fire the interrupt:
   ```
   echo out > /sys/class/gpio/gpioM/direction   # M = P9_12's gpio number
   echo 1 > /sys/class/gpio/gpioM/value
   echo 0 > /sys/class/gpio/gpioM/value
   ```
   Each rising edge should produce one top-half/bottom-half pair in
   dmesg, exactly like the fallback timer did, except now `mode=real-irq`
   in `/proc/irq_demo` and the top half is genuinely running because
   hardware asserted a line, not because a timer fired.

## Exercise 1 — sleeping in the hard ISR

Temporarily move `msleep(50);` from `do_bottom_half_work()` into
`do_hard_isr_work()`, rebuild, reload (fallback mode is fine for this
one — it genuinely runs in hardirq context). Trigger an event and
watch dmesg for a `BUG: sleeping function called from invalid context`
splat (requires `CONFIG_DEBUG_ATOMIC_SLEEP=y`, on by default on most
distro/debug kernels). Explain, in terms of `preempt_count()`, why the
kernel can detect this at runtime rather than just corrupting state
silently.

## Exercise 2 — drop IRQF_ONESHOT (real hardware only)

Remove `IRQF_ONESHOT` from the `request_threaded_irq()` flags. On a
level-triggered or bouncy line this can let the hard ISR re-fire
before the threaded handler has finished processing the previous
event, since the line isn't kept masked between them. Trigger several
rapid edges (a quick double-toggle of the output pin) and see if you
can produce overlapping bottom-half runs — add a `static atomic_t
in_bottom_half` guard with `WARN_ON()` if it's already nonzero to
detect this without relying on subtle interleavings in dmesg.

## Exercise 3 — disable_irq() self-deadlock

Add a line `disable_irq(irq_num);` at the top of `threaded_isr()` (real
hardware mode). `disable_irq()` blocks until any in-flight *hard*
handler for that IRQ has completed — but since your threaded handler
is a companion of that same hard handler and the two coordinate via
`IRQF_ONESHOT` masking, work out on paper (don't necessarily hang your
board) why this is at minimum redundant and in other calling patterns
can deadlock. Fix it by using `disable_irq_nosync()` instead, and
explain the difference in what each guarantees to the caller.

## Exercise 4 — free before synchronize race

Add a heap-allocated `struct irq_demo_priv *priv` passed as `dev_id`
instead of `NULL`, with a field written by `do_hard_isr_work()`. In
`irq_demo_exit()`, `kfree(priv)` **before** calling `free_irq()`.
Reload/unload rapidly while triggering events from another shell to
try to hit the window where the ISR is mid-execution against freed
memory (KASAN or `CONFIG_DEBUG_SLAB` will make this much easier to
catch than plain dmesg). Then fix the ordering and explain why
`free_irq()` already gives you a synchronization guarantee that makes
a separate explicit `synchronize_irq()` call unnecessary here — but
would NOT be unnecessary if some *other* code path outside this
handler also touched `priv` after unregistration.

## Exercise 5 — AB-BA between irq_lock and a bottom-half mutex

Add a `struct mutex config_lock` and have `do_bottom_half_work()` take
it, then, while still holding it, call a helper that takes
`irq_lock` (via `spin_lock_irqsave`). Separately, write a `/proc`
write handler for "reconfigure" that takes `irq_lock` first and then
tries to take `config_lock` while still holding it. Trigger both paths
concurrently (one via the timer/hardware firing, one via writing to
your new proc entry in a tight loop) and use `lockdep` to catch the
inversion before it ever actually deadlocks. Cross-reference: is this
the same lockdep coverage gap you noted for semaphores, or does
lockdep track mutex-vs-spinlock ordering fine? Confirm empirically.
