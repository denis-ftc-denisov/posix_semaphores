PHP_ARG_ENABLE(posix_semaphores, whether to enable POSIX semaphores support,
[ --enable-posix-semaphores   Enable POSIX semaphores support])

if test "$PHP_POSIX_SEMAPHORES" = "yes"; then
  AC_DEFINE(HAVE_POSIX_SEMAPHORES, 1, [Whether you have POSIX semaphores])
    PHP_NEW_EXTENSION(posix_semaphores, posix_semaphores.c, $ext_shared)
fi