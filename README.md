This project has been created as part of the 42 curriculum by jdreissi.

# Codexion

## Description

Codexion is a concurrency simulation modeled on the classic Dining Philosophers
problem. Coders sit in a circle and cycle through **compile → debug → refactor**.
Compiling requires holding two shared USB dongles at once (one on each side).
Dongles are protected by mutexes, subject to a cooldown after release, and
granted fairly according to a configurable scheduler (`fifo` or `edf`). A coder
that fails to start compiling within `time_to_burnout` ms burns out, which ends
the simulation — the simulation also ends once every coder has reached
`number_of_compiles_required` compiles.

## Instructions

Build:
```
make
```

Run:
```
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

Example:
```
./codexion 5 5000 200 200 200 10 100 fifo
```

- All arguments are mandatory positive integers, except `scheduler`, which must
  be exactly `fifo` or `edf`.
- With `number_of_coders 1`, only one dongle exists. Since compiling requires
  two distinct dongles, a single coder can never satisfy that requirement and
  will always burn out — this is the intended behavior for that edge case.

## Resources

- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/) reference on `pthread_create`,
  `pthread_mutex_t`, and `pthread_cond_t`.
- `man` pages for `pthread_mutex_init`, `pthread_cond_wait`,
  `pthread_cond_timedwait`, `pthread_cond_broadcast`, `gettimeofday`.
- Wikipedia — [Dining Philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem), [Coffman's conditions for deadlock](https://en.wikipedia.org/wiki/Deadlock_(computer_science)),
  [Earliest Deadline First scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling).
- AI was used throughout development as a learning resource:
  - get an ovwerview of the project
  - to understand how a thread and a mutex work and how they work together
  - what a pthread_cond is and how it works
  - discuss design principles and tradeoffs


## Blocking cases handled

- **Deadlock prevention:** each coder picks up its two
  dongles in a fixed order based on parity — even-numbered coders take
  left-then-right, odd-numbered coders take right-then-left. This breaks the
  circular-wait condition that would otherwise let every coder hold one
  dongle while waiting on the next, all the way around the ring.
- **Starvation prevention:** each dongle tracks up to two waiting coders (its
  only two possible neighbors) with arrival time and deadline. Under `fifo`,
  the earliest arrival is served first; under `edf`, the coder with the
  earliest burnout deadline is served first. Because every waiter's key is
  fixed at registration while time keeps advancing, no waiter is passed over
  indefinitely.
- **Lost-wakeup prevention:** cooldown expiry is a time-based condition, not
  an event, so nothing signals a waiting coder when a cooldown simply elapses.
  A dedicated wake-up thread periodically re-broadcasts on dongles that are
  free and off cooldown, ensuring waiting coders are never stuck asleep
  waiting for a signal that would otherwise never come.
- **Cooldown handling:** every dongle records `released_at` on release; a
  dongle cannot be re-acquired until `dongle_cooldown` ms have passed.
- **Precise burnout detection:** a dedicated monitor thread polls every
  coder's `last_compile_start` at a short, fixed interval and logs `burned
  out` within the required 10 ms window of the actual burnout time.
- **Log serialization:** all log output goes through a single mutex-protected
  function, so two state messages can never interleave on one line.

## Thread synchronization mechanisms

- **`pthread_mutex_t` per dongle** protects `in_use`, `released_at`, and the
  waiter queue from concurrent access by coder threads.
- **`pthread_mutex_t` per coder** protects `last_compile_start` and
  `number_of_finished_compiles`, which are written by the coder's own thread
  and read concurrently by the monitor thread.
- **`pthread_cond_t` per dongle**, combined with the wake-up thread, lets
  coder threads sleep instead of busy-polling while waiting for a dongle,
  and wake up correctly whether the trigger was a release, a cooldown
  expiring, or the simulation stopping.
- **A global `stop` flag guarded by its own mutex** signals every thread
  (coders, monitor, wake-up thread) to exit cleanly; it is always set and
  read under its own lock, and is never held simultaneously with a dongle
  lock, to avoid a lock-ordering deadlock between threads.
- **A single log mutex** serializes all `printf` calls across every thread.
