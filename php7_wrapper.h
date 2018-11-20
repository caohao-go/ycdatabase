/*
  +----------------------------------------------------------------------+
  | ycdatabase   /   ycdb                                                |
  +----------------------------------------------------------------------+
  | LICENSE: https://github.com/caohao0730/ycdatabase/blob/master/LICENSE|
  +----------------------------------------------------------------------+
  | Author: Cao Hao  <649947921@qq.com>                                  |
  | CreateTime: 2018-11-19                                               |
  +----------------------------------------------------------------------+
*/
#ifndef EXT_YCDATABASE_PHP7_WRAPPER_H_
#define EXT_YCDATABASE_PHP7_WRAPPER_H_

#include "ext/standard/php_http.h"

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define yc_inline inline __attribute__((always_inline))
#else
#define yc_inline inline
#endif
#elif defined(_MSC_VER)
#define yc_inline __forceinline
#else
#define yc_inline inline
#endif


#if PHP_MAJOR_VERSION < 7
typedef zend_rsrc_list_entry zend_resource;
#define YC_RETURN_STRING                      RETURN_STRING
#define YC_Z_ARRVAL_P                         Z_ARRVAL_P
#define IS_TRUE                               1

static inline int yc_zend_hash_find(HashTable *ht, char *k, int len, void **v)
{
    zval **tmp = NULL;
    if (zend_hash_find(ht, k, len, (void **) &tmp) == SUCCESS)
    {
        *v = *tmp;
        return SUCCESS;
    }
    else
    {
        *v = NULL;
        return FAILURE;
    }
}

#define yc_zend_hash_index_find               zend_hash_index_find
#define yc_zend_hash_del                      zend_hash_del
#define yc_zend_hash_update                   zend_hash_update
#define YC_ZVAL_STRINGL                       ZVAL_STRINGL
#define YC_ZEND_FETCH_RESOURCE_NO_RETURN      ZEND_FETCH_RESOURCE_NO_RETURN
#define YC_ZEND_FETCH_RESOURCE                ZEND_FETCH_RESOURCE
#define YC_ZEND_REGISTER_RESOURCE             ZEND_REGISTER_RESOURCE
#define YC_MAKE_STD_ZVAL(p)                   MAKE_STD_ZVAL(p)
#define YC_ALLOC_INIT_ZVAL(p)                 ALLOC_INIT_ZVAL(p)
#define YC_SEPARATE_ZVAL(p)
#define YC_ZVAL_STRING                        ZVAL_STRING
#define YC_RETVAL_STRINGL                     RETVAL_STRINGL
#define yc_smart_str                          smart_str
#define yc_php_var_unserialize                php_var_unserialize
#define yc_zend_is_callable                   zend_is_callable
#define yc_zend_is_callable_ex                zend_is_callable_ex
#define yc_zend_hash_add                      zend_hash_add
#define yc_zend_hash_index_update             zend_hash_index_update
#define yc_call_user_function_ex              call_user_function_ex

static yc_inline int yc_call_user_function_fast(zval *function_name, zend_fcall_info_cache *fci_cache, zval **retval_ptr_ptr, uint32_t param_count, zval ***params TSRMLS_DC)
{
    zend_fcall_info fci;

    fci.size = sizeof(fci);
    fci.function_table = EG(function_table);
    fci.object_ptr = NULL;
    fci.function_name = function_name;
    fci.retval_ptr_ptr = retval_ptr_ptr;
    fci.param_count = param_count;
    fci.params = params;
    fci.no_separation = 0;
    fci.symbol_table = NULL;

    return zend_call_function(&fci, fci_cache TSRMLS_CC);
}

#define yc_copy_to_stack(a, b)
#define YCDATABASE_GET_TSRMLS                     TSRMLS_FETCH_FROM_CTX(yc_thread_ctx ? yc_thread_ctx : NULL)

#define yc_add_assoc_string                   add_assoc_string
#define yc_add_assoc_stringl_ex               add_assoc_stringl_ex
#define yc_add_assoc_stringl                  add_assoc_stringl
#define yc_add_assoc_double_ex                add_assoc_double_ex
#define yc_add_assoc_long_ex                  add_assoc_long_ex
#define yc_add_next_index_stringl             add_next_index_stringl

#define yc_zval_ptr_dtor                      zval_ptr_dtor
#define yc_zend_hash_copy                     zend_hash_copy
#define yc_zval_add_ref                       zval_add_ref
#define yc_zval_dup(val)                      (val)
#define yc_zval_free(val)                     (yc_zval_ptr_dtor(&val))
#define yc_zend_hash_exists                   zend_hash_exists
#define yc_php_array_merge(dest,src)          php_array_merge(dest,src,1 TSRMLS_CC)
#define YC_RETURN_STRINGL                     RETURN_STRINGL
#define YC_RETVAL_STRING                      RETVAL_STRING
#define yc_zend_register_internal_class_ex    zend_register_internal_class_ex

#define yc_zend_call_method_with_0_params     zend_call_method_with_0_params
#define yc_zend_call_method_with_1_params     zend_call_method_with_1_params
#define yc_zend_call_method_with_2_params     zend_call_method_with_2_params

typedef int zend_size_t;

#define YC_HASHTABLE_FOREACH_START(ht, entry)\
    zval **tmp = NULL;\
    for (zend_hash_internal_pointer_reset(ht);\
        zend_hash_has_more_elements(ht) == SUCCESS; \
        zend_hash_move_forward(ht)) {\
        if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
            continue;\
        }\
        entry = *tmp;

#if defined(HASH_KEY_NON_EXISTANT) && !defined(HASH_KEY_NON_EXISTENT)
#define HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT
#endif

#define YC_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, entry)\
    zval **tmp = NULL; ulong_t idx;\
    for (zend_hash_internal_pointer_reset(ht); \
            (ktype = zend_hash_get_current_key_ex(ht, &k, &klen, &idx, 0, NULL)) != HASH_KEY_NON_EXISTENT; \
            zend_hash_move_forward(ht)\
        ) { \
    if (zend_hash_get_current_data(ht, (void**)&tmp) == FAILURE) {\
        continue;\
    }\
    entry = *tmp;\
    klen --;
    
#define YC_HASHTABLE_FOREACH_END() }
#define yc_zend_read_property                  zend_read_property
#define yc_zend_hash_get_current_key(a,b,c,d)  zend_hash_get_current_key_ex(a,b,c,d,0,NULL)

static inline int YC_Z_TYPE_P(zval *z)
{
    if (Z_TYPE_P(z) == IS_BOOL)
    {
        if ((uint8_t) Z_BVAL_P(z) == 1)
        {
            return IS_TRUE;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return Z_TYPE_P(z);
    }
}

#define yc_php_var_serialize(a,b,c)         php_var_serialize(a,&b,c)
#define yc_zend_get_executed_filename()     zend_get_executed_filename(TSRMLS_C)
#define IS_TRUE    1
inline int YC_Z_TYPE_P(zval *z);
#define YC_Z_TYPE_PP(z)        YC_Z_TYPE_P(*z)
#define YC_ZVAL_IS_NULL    ZVAL_IS_NULL

#else /*--------------------------------- PHP Version 7 --------------------------------------------*/
#define yc_php_var_serialize                php_var_serialize
typedef size_t zend_size_t;
#define ZEND_SET_SYMBOL(ht,str,arr)         zval_add_ref(arr); zend_hash_str_update(ht, str, sizeof(str)-1, arr);

static yc_inline int Z_BVAL_P(zval *v)
{
    if (Z_TYPE_P(v) == IS_TRUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#define yc_add_assoc_stringl(__arg, __key, __str, __length, __duplicate)   add_assoc_stringl_ex(__arg, __key, strlen(__key), __str, __length)
static yc_inline int yc_add_assoc_stringl_ex(zval *arg, const char *key, size_t key_len, char *str, size_t length, int __duplicate)
{
    return add_assoc_stringl_ex(arg, key, key_len - 1, str, length);
}

#define yc_add_next_index_stringl(arr, str, len, dup)    add_next_index_stringl(arr, str, len)

static yc_inline int yc_add_assoc_long_ex(zval *arg, const char *key, size_t key_len, long value)
{
    return add_assoc_long_ex(arg, key, key_len - 1, value);
}

static yc_inline int yc_add_assoc_double_ex(zval *arg, const char *key, size_t key_len, double value)
{
    return add_assoc_double_ex(arg, key, key_len - 1, value);
}

#define YC_Z_ARRVAL_P(z)                          Z_ARRVAL_P(z)->ht

#define YC_HASHTABLE_FOREACH_START(ht, _val) ZEND_HASH_FOREACH_VAL(ht, _val);  {
#define YC_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, _val) zend_string *_foreach_key;\
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, _foreach_key, _val);\
    if (!_foreach_key) {k = NULL; klen = 0; ktype = 0;}\
    else {k = _foreach_key->val, klen=_foreach_key->len; ktype = 1;} {

#define YC_HASHTABLE_FOREACH_END()                 } ZEND_HASH_FOREACH_END();

#define Z_ARRVAL_PP(s)                             Z_ARRVAL_P(*s)
#define YC_Z_TYPE_P                                Z_TYPE_P
#define YC_Z_TYPE_PP(s)                            YC_Z_TYPE_P(*s)
#define Z_STRVAL_PP(s)                             Z_STRVAL_P(*s)
#define Z_STRLEN_PP(s)                             Z_STRLEN_P(*s)
#define Z_LVAL_PP(v)                               Z_LVAL_P(*v)

#define yc_zval_add_ref(p)   Z_TRY_ADDREF_P(*p)
#define yc_zval_ptr_dtor(p)  zval_ptr_dtor(*p)


#define YC_PHP_MAX_PARAMS_NUM     20

static yc_inline int yc_call_user_function_ex(HashTable *function_table, zval** object_pp, zval *function_name, zval **retval_ptr_ptr, uint32_t param_count, zval ***params, int no_separation, HashTable* ymbol_table TSRMLS_CC)
{
    zval real_params[YC_PHP_MAX_PARAMS_NUM];
    int i = 0;
    for (; i < param_count; i++)
    {
        real_params[i] = **params[i];
    }
    
    zval *object_p = (object_pp == NULL) ? NULL : *object_pp;
    return call_user_function_ex(function_table, object_p, function_name, *retval_ptr_ptr, param_count, real_params, no_separation, NULL);
}

static yc_inline int yc_call_user_function_fast(zval *function_name, zend_fcall_info_cache *fci_cache, zval **retval_ptr_ptr, uint32_t param_count, zval ***params)
{
    zval real_params[YC_PHP_MAX_PARAMS_NUM];
    int i = 0;
    for (; i < param_count; i++)
    {
        real_params[i] = **params[i];
    }

    zval phpng_retval;
    *retval_ptr_ptr = &phpng_retval;

    zend_fcall_info fci;
    fci.size = sizeof(fci);
#if PHP_MINOR_VERSION == 0
    fci.function_table = EG(function_table);
    fci.symbol_table = NULL;
#endif
    fci.object = NULL;
    ZVAL_COPY_VALUE(&fci.function_name, function_name);
    fci.retval = &phpng_retval;
    fci.param_count = param_count;
    fci.params = real_params;
    fci.no_separation = 0;

    return zend_call_function(&fci, fci_cache);
}

#define YC_ZVAL_IS_NULL(z)   	(ZVAL_IS_NULL(z) || Z_TYPE_P(z) == IS_UNDEF)
#define yc_php_var_unserialize(rval, p, max, var_hash)  php_var_unserialize(*rval, p, max, var_hash)
#define YC_MAKE_STD_ZVAL(p)             zval _stack_zval_##p; p = &(_stack_zval_##p)
#define YC_ALLOC_INIT_ZVAL(p)           do{p = (zval *)emalloc(sizeof(zval)); bzero(p, sizeof(zval));}while(0)
#define YC_SEPARATE_ZVAL(p)             zval _##p;\
    memcpy(&_##p, p, sizeof(_##p));\
    p = &_##p
#define YC_RETURN_STRINGL(s, l, dup)    do{RETVAL_STRINGL(s, l); if (dup == 0) efree(s);}while(0);return
#define YC_RETVAL_STRINGL(s, l, dup)    do{RETVAL_STRINGL(s, l); if (dup == 0) efree(s);}while(0)
#define YC_RETVAL_STRING(s, dup)        do{RETVAL_STRING(s); if (dup == 0) efree(s);}while(0)

#define YC_ZEND_FETCH_RESOURCE_NO_RETURN(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)        \
        (rsrc = (rsrc_type) zend_fetch_resource(Z_RES_P(*passed_id), resource_type_name, resource_type))
#define YC_ZEND_REGISTER_RESOURCE(return_value, result, le_result)  ZVAL_RES(return_value,zend_register_resource(result, le_result))

#define YC_RETURN_STRING(val, duplicate)     RETURN_STRING(val)
#define yc_add_assoc_string(array, key, value, duplicate)   add_assoc_string(array, key, value)
#define yc_zend_hash_copy(target,source,pCopyConstructor,tmp,size) zend_hash_copy(target,source,pCopyConstructor)
#define yc_php_array_merge                                          php_array_merge
#define yc_zend_register_internal_class_ex(entry,parent_ptr,str)    zend_register_internal_class_ex(entry,parent_ptr)
#define yc_zend_get_executed_filename()                             zend_get_executed_filename()

#define yc_zend_call_method_with_0_params(obj, ptr, what, method, retval) \
    zval __retval;\
    zend_call_method_with_0_params(*obj, ptr, what, method, &__retval);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define yc_zend_call_method_with_1_params(obj, ptr, what, method, retval, v1)           \
    zval __retval;\
    zend_call_method_with_1_params(*obj, ptr, what, method, &__retval, v1);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define yc_zend_call_method_with_2_params(obj, ptr, what, method, retval, v1, v2)    \
    zval __retval;\
    zend_call_method_with_2_params(*obj, ptr, what, method, &__retval, v1, v2);\
    if (ZVAL_IS_NULL(&__retval)) *(retval) = NULL;\
    else *(retval) = &__retval;

#define YCDATABASE_GET_TSRMLS
#define YC_ZVAL_STRINGL(z, s, l, dup)         ZVAL_STRINGL(z, s, l)
#define YC_ZVAL_STRING(z,s,dup)               ZVAL_STRING(z,s)
#define yc_smart_str                          smart_string
#define zend_get_class_entry                  Z_OBJCE_P
#define yc_copy_to_stack(a, b)                {zval *__tmp = a;\
    a = &b;\
    memcpy(a, __tmp, sizeof(zval));}

static yc_inline zval* yc_zval_dup(zval *val)
{
    zval *dup;
    YC_ALLOC_INIT_ZVAL(dup);
    memcpy(dup, val, sizeof(zval));
    return dup;
}

static yc_inline void yc_zval_free(zval *val)
{
    yc_zval_ptr_dtor(&val);
    efree(val);
}

static yc_inline zval* yc_zend_read_property(zend_class_entry *class_ptr, zval *obj, char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

static yc_inline int yc_zend_is_callable(zval *cb, int a, char **name)
{
    zend_string *key = NULL;
    int ret = zend_is_callable(cb, a, &key);
    char *tmp = estrndup(key->val, key->len);
    zend_string_release(key);
    *name = tmp;
    return ret;
}

static inline int yc_zend_is_callable_ex(zval *callable, zval *object, uint check_flags, char **callable_name, int *callable_name_len, zend_fcall_info_cache *fcc, char **error TSRMLS_DC)
{
    zend_string *key = NULL;
    int ret = zend_is_callable_ex(callable, NULL, check_flags, &key, fcc, error);
    char *tmp = estrndup(key->val, key->len);
    zend_string_release(key);
    *callable_name = tmp;
    return ret;
}

static inline int yc_zend_hash_del(HashTable *ht, char *k, int len)
{
    return zend_hash_str_del(ht, k, len - 1);
}

static inline int yc_zend_hash_add(HashTable *ht, char *k, int len, void *pData, int datasize, void **pDest)
{
    return zend_hash_str_add(ht, k, len - 1, pData) ? SUCCESS : FAILURE;
}

static inline int yc_zend_hash_index_update(HashTable *ht, int key, void *pData, int datasize, void **pDest)
{
    return zend_hash_index_update(ht, key, pData) ? SUCCESS : FAILURE;
}

static inline int yc_zend_hash_update(HashTable *ht, char *k, int len, zval *val, int size, void *ptr)
{
    return zend_hash_str_update(ht, (const char*)k, len -1, val) ? SUCCESS : FAILURE;
}

static inline int yc_zend_hash_find(HashTable *ht, char *k, int len, void **v)
{
		zval *value = zend_hash_str_find(ht, k, len);
    if (value == NULL)
    {
        return FAILURE;
    }
    else
    {
        *v = (void *) value;
        return SUCCESS;
    }
}

static inline int yc_zend_hash_index_find(HashTable *ht, int index, void **pData)
{
		*pData = zend_hash_index_find(ht, index);
    if (*pData == NULL)
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
}

static inline int yc_zend_hash_exists(HashTable *ht, char *k, int len)
{
    zval *value = zend_hash_str_find(ht, k, len - 1);
    if (value == NULL)
    {
        return FAILURE;
    }
    else
    {
        return SUCCESS;
    }
 }

#endif /* PHP Version */

#endif /* EXT_YCDATABASE_PHP7_WRAPPER_H_ */
