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
	PHP_FE(posix_sem_open, NULL)
	PHP_FE(posix_sem_close, NULL)
	PHP_FE(posix_sem_unlink, NULL)
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

static void php_posix_semaphore_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_posix_semaphore *s = (php_posix_semaphore*)rsrc->ptr;
	if (s) 
	{
		if (s->sem)
		{
			sem_close(s->sem);
			s->sem = NULL;
		}
	}
}

#define PHP_POSIX_SEMAPHORES_RES_NAME "POSIX semaphore"

// list entry identifier
int le_posix_semaphore;

PHP_MINIT_FUNCTION(posix_semaphores)
{
	le_posix_semaphore = zend_register_list_destructors_ex(php_posix_semaphore_dtor, NULL, PHP_POSIX_SEMAPHORES_RES_NAME, module_number);
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
	int name_length;
	long flags;
	long mode = 0666;
	long value = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|ll", &name, &name_length, &flags, &mode, &value) == FAILURE) 
	{
		RETURN_NULL();
	}
	sem_t* sem = sem_open(name, flags, mode, value);
	if (sem == SEM_FAILED)
	{
		printf("sem_open failed, errno = %d flags = %d O_CREAT = %d", errno, flags, O_CREAT);
		RETURN_NULL();
	}
	php_posix_semaphore *semaphore = emalloc(sizeof(php_posix_semaphore));
	semaphore->sem = sem;
	ZEND_REGISTER_RESOURCE(return_value, semaphore, le_posix_semaphore);
}

PHP_FUNCTION(posix_sem_close)
{
	php_posix_semaphore *semaphore;
	zval *zsemaphore;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zsemaphore) == FAILURE)
	{
		RETURN_FALSE;
	}
	
	ZEND_FETCH_RESOURCE(semaphore, php_posix_semaphore*, &zsemaphore, -1, PHP_POSIX_SEMAPHORES_RES_NAME, le_posix_semaphore);
	if (semaphore->sem)
	{
		if (sem_close(semaphore->sem) == -1)
		{
		    RETURN_FALSE;
		}
		semaphore->sem = NULL;
	}
	
	RETURN_TRUE;
}

PHP_FUNCTION(posix_sem_unlink)
{
	char *name;
	int name_length;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_length) == FAILURE) 
	{
		RETURN_NULL();
	}
	if (sem_unlink(name) == -1)
	{
	    RETURN_FALSE;
	}
	RETURN_TRUE;
}