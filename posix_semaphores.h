#ifndef POSIX_SEMAPHORES_H
#define POSIX_SEMAPHORES_H 1

#define PHP_POSIX_SEMAPHORES_VERSION "1.0"
#define PHP_POSIX_SEMAPHORES_EXTNAME "posix_semaphores"

PHP_MINIT_FUNCTION(posix_semaphores);
PHP_MSHUTDOWN_FUNCTION(posix_semaphores);

PHP_FUNCTION(posix_sem_open);
PHP_FUNCTION(posix_sem_close);
PHP_FUNCTION(posix_sem_unlink);
PHP_FUNCTION(posix_sem_getvalue);
PHP_FUNCTION(posix_sem_post);
PHP_FUNCTION(posix_sem_wait);
PHP_FUNCTION(posix_sem_trywait);
PHP_FUNCTION(posix_sem_timedwait);
PHP_FUNCTION(posix_sem_error);

extern zend_module_entry posix_semaphores_module_entry;
#define phpext_posix_semaphores_ptr &posix_semaphores_module_entry;

#endif
