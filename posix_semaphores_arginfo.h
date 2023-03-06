/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9e3bcd8b1562df022b0491acc13c50410cec2648 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_posix_sem_open, 0, 4, resource, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_posix_sem_close, 0, 1, boolean, 0)
	ZEND_ARG_OBJ_INFO(0, semaphore, resource, 0)
ZEND_END_ARG_INFO()

#define arginfo_posix_sem_unlink arginfo_posix_sem_close

#define arginfo_posix_sem_post arginfo_posix_sem_close

#define arginfo_posix_sem_wait arginfo_posix_sem_close

#define arginfo_posix_sem_trywait arginfo_posix_sem_close

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_posix_sem_timedwait, 0, 2, boolean, 0)
	ZEND_ARG_OBJ_INFO(0, semaphore, resource, 0)
	ZEND_ARG_TYPE_INFO(0, timeout_sec, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout_nsec, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_sem_getvalue, 0, 1, IS_LONG, 1)
	ZEND_ARG_OBJ_INFO(0, semaphore, resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_posix_sem_error, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()
