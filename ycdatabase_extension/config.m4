dnl ./configure --with-php-config=/usr/bin/php-config
dnl	php_debug_zval_dump(join, 1);
PHP_ARG_ENABLE(ycdatabase, whether to enable ycdatabase support, [ --enable-ycdatabase Enable Cat Client support])

ycdatabase_source_file="ycdatabase.c \
				   src/memory.c \
				   src/operator.c \
					 src/string.c \
					 src/array.c \
					 src/object.c \
					 src/net.c \
					 src/db/ycdb.c"

if test "$PHP_YCDATABASE" = "yes"; then
    AC_DEFINE(HAVE_YCDATABASE, 1, [Whether you have Ycdatabase])
    PHP_NEW_EXTENSION(ycdatabase, $ycdatabase_source_file, $ext_shared)
fi
