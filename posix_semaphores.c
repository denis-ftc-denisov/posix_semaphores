#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "posix_semaphores.h"

#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

static zend_function_entry posix_semaphores_functions[] = {
	PHP_FE(posix_sem_open, arginfo_posix_sem_open)
	PHP_FE(posix_sem_close, arginfo_posix_sem_close)
	PHP_FE(posix_sem_unlink, arginfo_posix_sem_unlink)
	PHP_FE(posix_sem_getvalue, arginfo_posix_sem_getvalue)
	PHP_FE(posix_sem_post, arginfo_posix_sem_post)
	PHP_FE(posix_sem_wait, arginfo_posix_sem_wait)
	PHP_FE(posix_sem_trywait, arginfo_posix_sem_trywait)
	PHP_FE(posix_sem_timedwait, arginfo_posix_sem_timedwait)
	PHP_FE(posix_sem_error, arginfo_posix_sem_error)
	{NULL, NULL, NULL}
};
    
zend_module_entry posix_semaphores_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_POSIX_SEMAPHORES_EXTNAME,
	posix_semaphores_functions,
	PHP_MINIT(posix_semaphores),
	PHP_MSHUTDOWN(posix_semaphores),
	NULL,
	NULL,
	NULL,
#if ZEND_MODULE_API_NO >= 20010901
	PHP_POSIX_SEMAPHORES_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_POSIX_SEMAPHORES
	ZEND_GET_MODULE(posix_semaphores)
#endif

// Struct to store semaphore pointer. Will be passed as resource to PHP
// and back
typedef struct _php_posix_semaphore {
	sem_t *sem;
} php_posix_semaphore;


static void php_posix_semaphore_dtor(zend_resource *rsrc)
{
	php_posix_semaphore *s = (php_posix_semaphore*)rsrc->ptr;
	if (s) 
	{
		if (s->sem)
		{
			sem_close(s->sem);
			s->sem = NULL;
		}
		efree(s);
	}
}

#define PHP_POSIX_SEMAPHORES_RES_NAME "POSIX semaphore"

// list entry identifier
int le_posix_semaphore;

// error which occured during last function call
int last_errno;

PHP_MINIT_FUNCTION(posix_semaphores)
{
	le_posix_semaphore = zend_register_list_destructors_ex(php_posix_semaphore_dtor, NULL, PHP_POSIX_SEMAPHORES_RES_NAME, module_number);
	last_errno = 0;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(posix_semaphores)
{
	return SUCCESS;
}

/*
* MODULE FUNCTIONS
*/

PHP_FUNCTION(posix_sem_open)
{
	char *name;
	size_t name_length;
	zend_long flags;
	zend_long mode = 0666;
	zend_long value = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|ll", &name, &name_length, &flags, &mode, &value) == FAILURE) 
	{
		RETURN_NULL();
	}
	sem_t* sem = sem_open(name, flags, mode, value);
	if (sem == SEM_FAILED)
	{
		last_errno = errno;
		RETURN_NULL();
	}
	php_posix_semaphore *semaphore = emalloc(sizeof(php_posix_semaphore));
	semaphore->sem = sem;
	ZVAL_RES(return_value, zend_register_resource(semaphore, le_posix_semaphore));
}

PHP_FUNCTION(posix_sem_close)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsemaphore) == FAILURE)
	{
		RETURN_FALSE;
	}
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		if (sem_close(semaphore->sem) == -1)
		{
		    last_errno = errno;
		    RETURN_FALSE;
		}
		semaphore->sem = NULL;
	}
	
	RETURN_TRUE;
}

PHP_FUNCTION(posix_sem_unlink)
{
	char *name;
	size_t name_length;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_length) == FAILURE) 
	{
		RETURN_NULL();
	}
	if (sem_unlink(name) == -1)
	{
		last_errno = errno;
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

PHP_FUNCTION(posix_sem_getvalue)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsemaphore) == FAILURE)
	{
		RETURN_NULL();
	}
	
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		int value;
		if (sem_getvalue(semaphore->sem, &value) == -1)
		{
			last_errno = errno;
			RETURN_NULL();
		}
		else
		{
			RETURN_LONG(value);
		}
	}
	else
	{
		RETURN_NULL();
	}
}

PHP_FUNCTION(posix_sem_post)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsemaphore) == FAILURE)
	{
		RETURN_FALSE;
	}
	
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		if (sem_post(semaphore->sem) == -1)
		{
			last_errno = errno;
			RETURN_FALSE;
		}
		else
		{
			RETURN_TRUE;
		}
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(posix_sem_wait)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsemaphore) == FAILURE)
	{
		RETURN_FALSE;
	}
	
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		if (sem_wait(semaphore->sem) == -1)
		{
			last_errno = errno;
			RETURN_FALSE;
		}
		else
		{
			RETURN_TRUE;
		}
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(posix_sem_trywait)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsemaphore) == FAILURE)
	{
		RETURN_FALSE;
	}
	
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		if (sem_trywait(semaphore->sem) == -1)
		{
			last_errno = errno;
			RETURN_FALSE;
		}
		else
		{
			RETURN_TRUE;
		}
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(posix_sem_timedwait)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	zend_long timeout_sec;
	zend_long timeout_nsec = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|l", &zsemaphore, &timeout_sec, &timeout_nsec) == FAILURE)
	{
		RETURN_FALSE;
	}
	
	semaphore = (php_posix_semaphore*)zend_fetch_resource(Z_RES_P(zsemaphore), PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		struct timespec tm;
		tm.tv_sec = (int)timeout_sec;
		tm.tv_nsec = (int)timeout_nsec;
		if (sem_timedwait(semaphore->sem, &tm) == -1)
		{
			last_errno = errno;
			RETURN_FALSE;
		}
		else
		{
			RETURN_TRUE;
		}
	}
	else
	{
		RETURN_FALSE;
	}
}


PHP_FUNCTION(posix_sem_error)
{
	RETURN_LONG(last_errno);
}
