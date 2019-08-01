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

### posix_sem_wait($semaphore)

### posix_sem_getvalue($semaphore)


## Notes for PHP users

All semaphore resources are closed automatically when PHP exits. But underlying semaphores are not unlinked by default. You 
should use `register_shutdown_function` to unlink all semaphores which you don't need after program exit. 
Alternative way for that is to manage semaphore creation and deletion outside of PHP.
