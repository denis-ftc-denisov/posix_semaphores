# posix_semaphores - POSIX semaphores extension for PHP

## Introduction

This a thin wrapper around [POSIX semaphores API](http://man7.org/linux/man-pages/man7/sem_overview.7.html). 
There is built-in PHP extension for [semaphores and other IPC](https://www.php.net/manual/en/book.sem.php), 
but it supports only System V semaphores, when POSIX version has much more convenient API.

Also System V semaphore support in PHP does not match exactly with C API (in fact it constructs three semaphores
in set under the hood and uses them for initialization and other bookkeeping tasks). That's very convenient
when one uses semaphores solely from PHP code, but prevents some mixed use cases when one wants to sync PHP code
and other code with semaphores. 

For example it's not possible to create locked semaphore in C++ application, wait for it in PHP (with locking 
of course) and then unlock in C++ application (this case could be used for shared memory IPC syncronization, for instance).

So the goal here is to make C API wrapper as thin as possible to allow simultaneous usage of semaphores in 
applications written in PHP or other programming languages.

## Installation

Make sure you have `php-devel` package installed in your system. You need to be able to build PHP extensions.

After that run commands:
```
phpize
./configure --enable-posix_semaphores
make
```

If everything goes well, you will have file `./modules/posix_semaphores.so`. The last step is to include this file
into your `php.ini`. Add line `extension=/<PATH>/posix_semaphores.so` (replace `<PATH>` with actual path to this
extension) and you're done. Don't forget to restart php-fpm or apache if you're using PHP from there.

## Reference

Extension consists of several functions which are clearly mapped to POSIX semaphores API. 

### posix_sem_open($name, $flags, $mode = 0666, $value = 0)

This is equivalent of [sem_open](http://man7.org/linux/man-pages/man3/sem_open.3.html). 

Parameters:
* $name - name of semaphore to open,
* $flags - opening flags (such as O_CREAT or O_EXCL),
* $mode - opening mode (default value is 0666),
* $value - initial value for semaphore (default is 0).

Function returns semaphore resource on success or `null` on failure.

### posix_sem_close($semaphore)

Function closes semaphore resource (in fact it's just a proxy to [sem_close](http://man7.org/linux/man-pages/man3/sem_close.3.html)).

Parameters:
* $semaphore - semaphore resource acquired via `posix_sem_open`.

Returns `true` on success, `false` otherwise.

### posix_sem_unlink($name)

Unlinks named semaphore from system. The semaphore name is removed immediately. The semaphore itself is removed when 
all processed having the semaphore opened, close it.

Parameters:
* $name - name of semaphore to remove.

Returns `true` on success, `false` otherwise.

### posix_sem_post($semaphore)

Unlocks semaphore. If the semaphore's value consequently becomes greater than zero, then
another process or thread blocked in a sem_wait call will be woken
up and proceed to lock the semaphore.

Parameters:
* $semaphore - semaphore resource to unlock (acquired via `posix_sem_open`).

Returns `true` on success, `false` otherwise.


### posix_sem_wait($semaphore)

Locks a semaphore. If
the semaphore's value is greater than zero, then the decrement
proceeds, and the function returns, immediately.  If the semaphore
currently has the value zero, then the call blocks until either it
becomes possible to perform the decrement (i.e., the semaphore value
rises above zero), or a signal handler interrupts the call.

Parameters:
* $semaphore - semaphore resource to unlock (acquired via `posix_sem_open`).

Returns `true` on success, `false` otherwise.

### posix_sem_trywait($semaphore)

The same as `posix_sem_wait`, except that if the decrement
cannot be immediately performed, then call returns an error (last error
set to EAGAIN) instead of blocking.

### posix_sem_timedwait($semaphore, $timeout_sec, $timeout_nsec = 0)

Tries to lock a semaphore (as `posix_sem_wait`), but if lock could not 
be performed in a specified timeout, returns and sets last error to ETIMEDOUT.

Parameters:
* $semaphore - semaphore resource to unlock (acquired via `posix_sem_open`).
* $timeout_sec - part of the timeout in seconds.
* $timeout_nsec - part of the timeout in nanoseconds.

Total timeout is computed as `$timeout_sec * 1000000000 + $timeout_nsec` and is
measured in nanoseconds.

### posix_sem_getvalue($semaphore)

Gets value of specified semaphore.

Parameters:
* $semaphore - semaphore resource which value to get (acquired via `posix_sem_open`).

Returns semaphore value on success and `null` on error.

Please note that underlying function `sem_getvalue` can behave differently according to
POSIX specification, so please read your OS documentation on that.

### posix_sem_error()

Returns value of last error occured during call of another semaphore-related function.

## Notes for PHP users

All semaphore resources are closed automatically when PHP exits. But underlying semaphores are not unlinked by default. You 
should use `register_shutdown_function` to unlink all semaphores which you don't need after program exit. 
Alternative way for that is to manage semaphore creation and deletion outside of PHP.
