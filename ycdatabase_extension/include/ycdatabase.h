/*
  +----------------------------------------------------------------------+
  | ycdatabase   /   ycdb                                                |
  +----------------------------------------------------------------------+
  | ycdb is a php database ORM extension for mysql written in c language |
  +----------------------------------------------------------------------+
  | LICENSE: https://github.com/caohao0730/ycdatabase/blob/master/LICENSE|
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/
#ifndef YCDATABASE_H_
#define YCDATABASE_H_

#if defined(HAVE_CONFIG_H) && !defined(COMPILE_DL_YCDATABASE)
#include "config.h"
#endif

#ifdef YC_STATIC_COMPILATION
#include "php_config.h"
#endif

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/un.h>

#include "php7_wrapper.h"
#include "ycdatabase_config.h"
#include "error.h"
#include <ext/standard/php_var.h>

/////////////////////////////// 公共宏定义 //////////////////////////////////
#define YC_OK                  0
#define YC_ERR                -1
#define YC_AGAIN              -2
#define YC_BUSY               -3
#define YC_DONE               -4
#define YC_DECLINED           -5
#define YC_ABORT              -6

#define MAP_ITOA_INT_SIZE 16

#ifndef ulong
#define ulong unsigned long
typedef unsigned long ulong_t;
#endif

//内存分配大小
#define ALIGNED_SIZE_32 32
#define MM_ALIGNED_SIZE_16(size) ((size + Z_L(16) - Z_L(1)) & (~(Z_L(16) - Z_L(1)))) //16位内存对齐
#define MM_ALIGNED_SIZE_32(size) ((size + Z_L(32) - Z_L(1)) & (~(Z_L(32) - Z_L(1)))) //32位内存对齐
#define MM_REAL_SIZE(len) (MM_ALIGNED_SIZE_32(len + 1 + sizeof(size_t)))

/////////////////////// 基础函数 ///////////////////////
#define yc_php_fatal_error(level, fmt_str, ...)   php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
static yc_inline void zval_debug(char* name, zval* p) {
    php_printf("----------%s----------\n", name);
    php_debug_zval_dump(p, 1);
}
static yc_inline void string_debug(char* name, char* str) {
    php_printf("%s：[%s] (string)\n", name, str);
}
static yc_inline void int_debug(char* name, int l) {
    php_printf("%s：[%d] (int)\n", name, l);
}
static yc_inline void float_debug(char* name, float l) {
    php_printf("%s：[%f] (float)\n", name, l);
}
static yc_inline void x_debug(char* name, void* l) {
    php_printf("%s：[%x] (point)\n", name, l);
}
static yc_inline void info_debug(char* name) {
    php_printf("============%s============\n", name);
}
static yc_inline void term_debug(char* term_info) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, term_info);
}


//////////////////// memory.c ///////////////////////////
static yc_inline void yc_emalloc_and_init(void **tmp, size_t size) {
    *tmp = emalloc(size);    //分配空间，并初始化为 0
    memset(*tmp, 0, size);
}

static yc_inline zval* yc_zval_copy(zval * source) {
	zval *copy;
	YC_MAKE_STD_ZVAL(copy);
	*copy = *source;
	zval_copy_ctor(copy);
	return copy;
}

extern char* yc_multi_memcpy(char * source, int n_str, ...);

//以32位对齐分配内存
static yc_inline void yc_string_emalloc_32(char **tmp, int len) {
    size_t real_size = MM_REAL_SIZE(len);
    *tmp = emalloc(real_size);
    memset(*tmp, 0, real_size);
    memcpy(*tmp, &real_size, sizeof(size_t));
    *tmp = *tmp + sizeof(size_t);
}

static yc_inline void yc_string_efree_32(char *tmp) {
    efree(tmp - sizeof(size_t));
}

extern char* yc_multi_memcpy_auto_realloc(char** source, int n_str, ...); //自动扩容

//////////////////// string.c ///////////////////////////
#define yc_is_space(p) (p != '\0' && (isspace(p) || (p) == '\n' || (p) == '\r' || (p) == '\t'))
#define yc_is_string_not_empty(p) (p != NULL && p[0] != '\0')
#define yc_is_string_empty(p) !yc_is_string_not_empty(p)
#define yc_init_stack_string(p, str)  YC_MAKE_STD_ZVAL(p); YC_ZVAL_STRING(p, str, 1);

#if defined(YC_USE_JEMALLOC) || defined(YC_USE_TCMALLOC)
extern yc_inline char* yc_strdup(const char *s);
extern yc_inline char* yc_strndup(const char *s, size_t n);
#else
#define yc_strdup              strdup
#define yc_strndup             strndup
#endif

extern zval* yc_preg_match(const char* regex_str, char* subject_str);
extern int yc_strpos(const char *haystack,const char *needle);
extern char* rtrim(char *str);
extern char* ltrim(char *str);
extern char* trim(char *str);
extern char* ltrim_str(char *str, char *remove);
extern char* rtrim_str(char *str, char *remove);
extern char* yc_itoa(int num,char* str);
extern char* yc_escape(char* sql);
extern char* strreplace(char* original, char const * const pattern, char const * const replacement);
extern void yc_serialize(zval *return_value, zval *var TSRMLS_DC);
extern void yc_unserialize(zval *return_value, zval *var TSRMLS_DC);

/////////////////// array.c ////////////////////////////
extern char* yc_get_string_from_array_index(zval *array, ulong index);
extern HashTable* yc_get_array_keys(zval* p);
extern char* yc_get_array_key_index(zval* p, uint32_t index);
extern zval * yc_get_element_by_hashtable_index(HashTable *ht, int index);
extern zval * php_yc_array_get_value(HashTable *ht, char *key);
static yc_inline void yc_free_hash(HashTable *ht) {
    zend_hash_destroy(ht);    //释放hash
    FREE_HASHTABLE(ht);
}
extern void yc_destroy_hash(HashTable *ht);
extern void yc_destroy_array(zval **array);
extern void yc_clean_hash(HashTable *ht);
extern void yc_clean_array(zval **array);
extern int is_set_array_index(HashTable *ht, int index); //数组下标 index 是否为空
extern char* yc_get_string_from_hashtable_index(HashTable *ht, int index);
extern char* yc_get_string_from_hashtable(HashTable *ht, char* key);
extern char* yc_implode(zval *arr, const char *delim_str, char** result);
extern void yc_array_single_columns(zval** return_single_column_result, zval* data);

/////////////////// operator.c ////////////////////////////
extern int yc_compare_strict_bool(zval *op1, zend_bool op2 TSRMLS_DC);

/////////////////// net.c ////////////////////////////
php_stream* unix_socket_conn(char *servername);

//////////////////// 公共宏定义 /////////////////////
#define YC_IS_FALSE(var)        yc_compare_strict_bool(var, 0 TSRMLS_CC)
#define YC_IS_TRUE(var)         yc_compare_strict_bool(var, 1 TSRMLS_CC)
#define YC_IS_ARRAY(var)   (var != NULL && Z_TYPE_P(var) == IS_ARRAY)
#define YC_IS_NOT_ARRAY(var)   (!YC_IS_ARRAY(var))
#define YC_IS_EMPTY(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL || YC_IS_FALSE(var) || (Z_TYPE_P(var) == IS_STRING && !Z_STRLEN_P(var)) || !zend_is_true(var))
#define YC_IS_NOT_EMPTY(var)   (!YC_IS_EMPTY(var))
#define YC_IS_NULL(var)       (var == NULL || Z_TYPE_P(var) == IS_NULL)
#define YC_IS_NOT_NULL(var)   (!YC_IS_NULL(var))
#define YC_HASHTABLE_IS_NOT_EMPTY(var)		(var != NULL && zend_hash_num_elements(var))
#define YC_HASHTABLE_IS_EMPTY(var)   (!YC_HASHTABLE_IS_NOT_EMPTY(var))

/////////////////// object.c ////////////////////////////
static inline zval* yc_read_init_property(zend_class_entry *scope, zval *object, char *p, size_t pl TSRMLS_DC) {
    zval* property = yc_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    if (property == NULL) {
        zend_update_property_null(scope, object, p, pl);
        return yc_zend_read_property(scope, object, p, pl, 1 TSRMLS_CC);
    } else {
        return property;
    }
}

static inline int yc_call_user_function_ex_fast(zval** object_pp, char* function_name, zval **retval_ptr_ptr, uint32_t param_count, zval ***params) {
    zval* _function_name;
    
    YC_MAKE_STD_ZVAL(_function_name);
    YC_ZVAL_STRING(_function_name, function_name, 1);
    
    if(*retval_ptr_ptr == NULL) YC_ALLOC_INIT_ZVAL(*retval_ptr_ptr);
    
    int ret = yc_call_user_function_ex(EG(function_table), object_pp, _function_name, retval_ptr_ptr, param_count, params, 0, NULL TSRMLS_CC);
    
    yc_zval_ptr_dtor(&_function_name);
    
    return ret;
}

static yc_inline int yc_call_user_function_return_bool_or_unsigned(zval** object_pp, char* function_name, uint32_t param_count, zval ***params) {
	int result = 0;
    zval tmp_point;
    zval* retval = &tmp_point, *_function_name;
	
    YC_MAKE_STD_ZVAL(_function_name);
    YC_ZVAL_STRING(_function_name, function_name, 1);

    int ret = yc_call_user_function_ex(EG(function_table), object_pp, _function_name, &retval, param_count, params, 0, NULL TSRMLS_CC);
    yc_zval_ptr_dtor(&_function_name);
    if (ret == FAILURE) {
    	yc_zval_ptr_dtor(&retval);
    	return FAILURE;
    }

    //返回 true
#if PHP_MAJOR_VERSION < 7
    if (Z_TYPE_P(retval) == IS_BOOL && Z_BVAL_P(retval))
#else
    if (Z_TYPE_P(retval) == IS_TRUE)
#endif
    {
    	result = 1;
    } else if (Z_TYPE_P(retval) == IS_LONG) { //返回无符号整数
        result = Z_LVAL_P(retval);
    }
    
	yc_zval_ptr_dtor(&retval);
	return result;
}

static yc_inline int yc_call_user_function_construct_fast(zval** object_pp, uint32_t param_count, zval ***params) {
    zval tmp_point;
    zval *construct_ret = &tmp_point;
    
    zval* function_name;
    YC_MAKE_STD_ZVAL(function_name);
    YC_ZVAL_STRING(function_name, "__construct", 1);

    int ret = yc_call_user_function_ex(EG(function_table), object_pp, function_name, &construct_ret, param_count, params, 0, NULL TSRMLS_CC);

    yc_zval_ptr_dtor(&function_name);
    
    yc_zval_ptr_dtor(&construct_ret);
    return ret;
}

#endif
