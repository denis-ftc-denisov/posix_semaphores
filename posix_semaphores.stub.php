<?php

function posix_sem_open(string $name, int $flags, int $mode, int $value) : resource|null {}
function posix_sem_close(resource $semaphore) : boolean {}
function posix_sem_unlink(resource $semaphore) : boolean {}
function posix_sem_post(resource $semaphore) : boolean {}
function posix_sem_wait(resource $semaphore) : boolean {}
function posix_sem_trywait(resource $semaphore) : boolean {}
function posix_sem_timedwait(resource $semaphore, int $timeout_sec, int $timeout_nsec = 0) : boolean {}
function posix_sem_getvalue(resource $semaphore) : int|null {}
function posix_sem_error() : int {} 