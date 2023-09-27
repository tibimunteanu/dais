### Synchronization refers to "serialization" and "mutual exclusion".

### SEMAPHORE:

- int value initialized to a number (Semaphore(0) means a semaphore initialized to 0)
- wait() decrements the value and blocks if the value is negative
- signal() increments the value and unblocks a waiting thread

###### Signaling

```
[sem = Semaphore(0)]
Thread A
{
    statement a1
    sem.signal()
}
Thread B
{
    sem.wait()
    statement b1
}
```

###### Rendezvous

```
[aArrived = Semaphore(0)]
[bArrived = Semaphore(0)]
Thread A
{
    statement a1
    aArrived.signal()
    bArrived.wait()
    statement a2
}
Thread B
{
    statement b1
    bArrived.signal()
    aArrived.wait()
    statement b2
}
```

###### Deadlock (BAD)

```
[aArrived = Semaphore(0)]
[bArrived = Semaphore(0)]
Thread A
{
    statement a1
    bArrived.wait()
    aArrived.signal()
    statement a2
}
Thread B
{
    statement b1
    aArrived.wait()
    bArrived.signal()
    statement b2
}
```

### MUTEX:

```
[mutex = Semaphore(1)]
ThreadX
{
    mutex.wait()
        // only 1 thread at a time
        CRITICAL SECTION
    mutex.signal()
}
```

### MULTIPLEX:

```
[mutex = Semaphore(n)]
ThreadX
{
    mutex.wait()
        // only n threads at a time
        CRITICAL SECTION
    mutex.signal()
}
```

### BARRIER:

```
[mutex = Semaphore(1)]
[barrier = Semaphore(0)]
ThreadX
{
    RENDEZVOUS

    mutex.wait()
        count = count + 1
        if count == n: barrier.signal()
    mutex.signal()

    barrier.wait()
    barrier.signal()

    CRITICAL SECTION
}
```

- a wait() followed by a signal() is called a "turnstile", because it allows one
  thread to pass at a time and can be locked to bar all threads
- the barrier will end up with a value of 1 because the last thread to arrive will signal twice
- in order to have a reusable barrier, we would have to do a final decrement
  after the last thread completes the critical section:

```
[mutex = Semaphore(1)]
[turnstile = Semaphore(0)]
ThreadX
{
    RENDEZVOUS

    mutex.wait()
        count += 1
        if count == n: turnstile.signal()
    mutex.signal()

    turnstile.wait()
    turnstile.signal()

    CRITICAL SECTION

    mutex.wait()
        count -= 1
        if count == 0: turnstile.wait()
    mutex.signal()
}
```

- if this barrier is to be used in a loop, a thread could pass through the
  second mutex, then loop around and pass through the first mutex and the
  turnstile, effectively getting ahead of the other threads by a lap. To solve
  this, we can use a second turnstile:

```
[mutex = Semaphore(1)]
[turnstile = Semaphore(0)]
[turnstile2 = Semaphore(1)]
ThreadX
{
    RENDEZVOUS

    mutex.wait()
        count += 1
        if count == n:
            turnstile2.wait()   // lock the second
            turnstile.signal()  // unlock the first
    mutex.signal()

    turnstile.wait()            // first turnstile
    turnstile.signal()

    CRITICAL SECTION

    mutex.wait()
        count -= 1
        if count == 0:
            turnstile.wait()    // lock the first
            turnstile2.signal() // unlock the second
    mutex.signal()

    turnstile2.wait()           // second turnstile
    turnstile2.signal()
}
```

- this is called a "two-phase barrier" and it is correct because:

  - only the nth thread can lock or unlock the turnstiles;
  - before a thread can unlock the first turnstile, it must lock the second;
  - before a thread can unlock the second turnstile, it must lock the first.

- the solution can be simplified if we have a way to signal a semaphore n times at once:

```
[mutex = Semaphore(1)]
[turnstile = Semaphore(0)]
[turnstile2 = Semaphore(0)]
ThreadX
{
    RENDEZVOUS

    mutex.wait()
        count += 1
        if count == n:
            turnstile.signal(n)  // unlock the first
    mutex.signal()

    turnstile.wait()            // first turnstile

    CRITICAL SECTION

    mutex.wait()
        count -= 1
        if count == 0:
            turnstile2.signal(n) // unlock the second
    mutex.signal()

    turnstile2.wait()           // second turnstile
}
```

- a Barrier object can be encapsulated as follows (C#):

```
public class Barrier
{
    private int n, count;
    private Semaphore mutex, turnstile, turnstile2;

    public Barrier(n)
    {
        this.n = n;
        this.mutex = new Semaphore(1);
        this.turnstile = new Semaphore(0);
        this.turnstile2 = new Semaphore(0);
    }

    public void Phase1()
    {
        mutex.wait();
        {
            count += 1;
            if (count == n)
            {
                turnstile.signal(n);
            }
        }
        mutex.signal();

        turnstile.wait();
    }

    public void Phase2()
    {
        mutex.wait();
        {
            count -= 1;
            if (count == 0)
            {
                turnstile2.signal(n);
            }
        }
        mutex.signal();

        turnstile2.wait();
    }

    public void Wait()
    {
        Phase1();
        Phase2();
    }
}
```

- usage example:

```
var barrier = new Barrier(n); // initialize a new barrier for n threads
barrier.Wait(); // wait for all threads to arrive
```

- or

```
barrier.Phase1();
// do some work
barrier.Phase2();
```

### QUEUE

- consider leaders and followers with the constraint that each leader can invoke
  dance() concurrently with only one follower, and vice versa (exclusive queue)

```
[leaders = followers = 0]
[mutex = Semaphore(1)]
[leaderQueue = Semaphore(0)]
[followerQueue = Semaphore(0)]
[rendezvous = Semaphore(0)]
ThreadLeaders
{
    mutex.wait()
        if followers > 0:
            followers--
            followerQueue.signal()
        else:
            leaders++
            mutex.signal()
            leaderQueue.wait()

    dance()
    rendezvous.wait()
    mutex.signal()
}
ThreadFollowers
{
    mutex.wait()
        if leaders > 0:
            leaders--
            leaderQueue.signal()
        else:
            followers++
            mutex.signal()
            followerQueue.wait()

    dance()
    rendezvous.signal()
}
```

- when a leader arrives, it gets the mutex that protects leaders and followers.
  if there is a follower waiting, the leader decrements followers, signals a follower,
  and then invokes dance, all before releasing the mutex. that guarantees that there
  can be only one follower thread running dance concurrently.
- if there are no followers waiting, the leader has to give up the mutex before
  waiting on the leaderQueue.
- when a follower arrives, it checks for a waiting leader. if there is one, the
  follower decrements leaders, signals a leader, and executes dance, all without
  releasing the mutex. the follower never releases the mutex because we know that one
  of them does, and either one of them can release it. in this case, it's the leader.

-- reached page 67 --
