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
#include "php_ycdatabase.h"
#include "ycdb.h"

//类成员函数参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_construct_oo, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, options, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_select_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, join)
ZEND_ARG_INFO(0, columns)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_exec_oo, 0, 0, 1)
ZEND_ARG_INFO(0, query)
ZEND_ARG_ARRAY_INFO(0, map, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_query_oo, 0, 0, 1)
ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_insert_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_ARRAY_INFO(0, cache_info, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_update_oo, 0, 0, 2)
ZEND_ARG_INFO(0, table)
ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdb_delete_oo, 0, 0, 1)
ZEND_ARG_INFO(0, table)
ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()

//类的成员函数
static zend_function_entry ycdb_methods[] = {
    PHP_ME(ycdb, __construct, arginfo_ycdb_construct_oo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(ycdb, __destruct, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(ycdb, errorCode, arginfo_ycdb_construct_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, errorInfo, arginfo_ycdb_construct_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, select, arginfo_ycdb_select_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, exec, arginfo_ycdb_exec_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, query, arginfo_ycdb_query_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, insert, arginfo_ycdb_insert_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, insert_id, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, update, arginfo_ycdb_update_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, delete, arginfo_ycdb_delete_oo, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, initialize, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, begin, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, commit, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC)
    PHP_ME(ycdb, rollback, arginfo_ycdatabase_void, ZEND_ACC_PUBLIC)
    NULL, NULL, NULL
};

//ycdb 类初始化
void ycdb_init() {
    INIT_CLASS_ENTRY(ycdb_ce, "ycdb", ycdb_methods);
    ycdb_ce_ptr = zend_register_internal_class(&ycdb_ce TSRMLS_CC);
    ycdb_ce_ptr->serialize = zend_class_serialize_deny;
    ycdb_ce_ptr->unserialize = zend_class_unserialize_deny;

    zend_declare_property_long(ycdb_ce_ptr, ZEND_STRL("mapkey_index"), 0, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("host"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("username"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("password"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("dbname"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_long(ycdb_ce_ptr, ZEND_STRL("port"), 3306, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_string(ycdb_ce_ptr, ZEND_STRL("charset"), "utf8", ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("option"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("errcode"),ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("errinfo"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("_pdo"), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ycdb_ce_ptr, ZEND_STRL("cache"), ZEND_ACC_PUBLIC TSRMLS_CC);
}

//ycdb构造函数
PHP_METHOD(ycdb, __construct) {
    zval* thisObject = getThis();
    zval *option = NULL;
    zval *v;
    HashTable *vht;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &option) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(option) != IS_ARRAY) {
        RETURN_FALSE;
    }

    vht = Z_ARRVAL_P(option);
    if ((v = php_yc_array_get_value(vht, "host")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(ycdb_ce_ptr, thisObject, ZEND_STRL("host"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "username")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(ycdb_ce_ptr, thisObject, ZEND_STRL("username"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "password")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(ycdb_ce_ptr, thisObject, ZEND_STRL("password"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "dbname")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(ycdb_ce_ptr, thisObject, ZEND_STRL("dbname"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "port")) != NULL) {
        convert_to_long(v);
        zend_update_property_long(ycdb_ce_ptr, thisObject, ZEND_STRL("port"), Z_LVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "charset")) != NULL) {
        convert_to_string(v);
        zend_update_property_string(ycdb_ce_ptr, thisObject, ZEND_STRL("charset"), Z_STRVAL_P(v) TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "cache")) != NULL) {
        zend_update_property(ycdb_ce_ptr, thisObject, ZEND_STRL("cache"), v TSRMLS_DC);
    }

    if ((v = php_yc_array_get_value(vht, "option")) != NULL) {
        zend_update_property(ycdb_ce_ptr, thisObject, ZEND_STRL("option"), v TSRMLS_DC);
    }

    update_error_info(thisObject, "00000", "");
}

PHP_METHOD(ycdb, __destruct) {
}

PHP_METHOD(ycdb, errorCode) {
    zval* errorCode = yc_read_init_property(ycdb_ce_ptr, getThis(), ZEND_STRL("errcode") TSRMLS_CC);
    RETURN_ZVAL(errorCode, 1, 0);
}

PHP_METHOD(ycdb, errorInfo) {
    zval* errorInfo = yc_read_init_property(ycdb_ce_ptr, getThis(), ZEND_STRL("errinfo") TSRMLS_CC);
    RETURN_ZVAL(errorInfo, 1, 0);
}

PHP_METHOD(ycdb, initialize) {
    zval* thisObject = getThis();
    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);

    if (YC_IS_NULL(pdo)) {
        zval *dsn;
        zval **args[4];

        //参数
        zval* host = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("host") TSRMLS_CC);
        zval* username = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("username") TSRMLS_CC);
        zval* password = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("password") TSRMLS_CC);
        zval* dbname = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("dbname") TSRMLS_CC);
        zval* port = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("port") TSRMLS_CC);
        zval* charset = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("charset") TSRMLS_CC);
        zval* option = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("option") TSRMLS_CC);

        yc_sprint_stack_string_128(dsn, "mysql:host=%s;port=%d;dbname=%s", Z_STRVAL_P(host), Z_LVAL_P(port), Z_STRVAL_P(dbname));

        args[0] = &dsn;
        args[1] = &username;
        args[2] = &password;
        args[3] = &option;

        //创建pdo, 连接数据库
        object_init_ex(pdo, get_pdo_ce());

        if (yc_call_user_function_construct_fast(&pdo, 4, args) == FAILURE) {
            yc_zval_ptr_dtor(&dsn);
            yc_php_fatal_error(E_ERROR, "Fail to connect database by PDO");
            RETURN_FALSE;
        }

        if (EG(exception)) {
            yc_zval_ptr_dtor(&dsn);
            RETURN_FALSE;
        }

        yc_zval_ptr_dtor(&dsn);

        zend_update_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo"), pdo TSRMLS_CC);

        //设置字符集
        zval *charset_sql;
        zval** exec_args[1];

        yc_sprint_stack_string_128(charset_sql, "SET NAMES %s", Z_STRVAL_P(charset));
        exec_args[0] = &charset_sql;

        int setret = yc_call_user_function_return_bool_or_unsigned(&pdo, "exec", 1, exec_args);

        if (setret == FAILURE) {
            yc_zval_ptr_dtor(&charset_sql);
            yc_php_fatal_error(E_WARNING, "failed to set database charset.");
            RETURN_FALSE;
        }

        if (EG(exception)) {
            yc_zval_ptr_dtor(&charset_sql);
            RETURN_FALSE;
        }

        yc_zval_ptr_dtor(&charset_sql);
    }

    RETURN_TRUE;
}

//事务开始
PHP_METHOD(ycdb, begin) {
    zval* thisObject = getThis();

    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YC_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    if (yc_call_user_function_return_bool_or_unsigned(&pdo, "beginTransaction", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

//事务提交
PHP_METHOD(ycdb, commit) {
    zval* thisObject = getThis();

    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YC_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    if (yc_call_user_function_return_bool_or_unsigned(&pdo, "commit", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}


//事务回滚
PHP_METHOD(ycdb, rollback) {
    zval* thisObject = getThis();

    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YC_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    if (yc_call_user_function_return_bool_or_unsigned(&pdo, "rollBack", 0, NULL) == 1) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

//执行查询
PHP_METHOD(ycdb, exec) {
    zval* thisObject = getThis();
    zval *query = NULL, *map = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &query, &map) == FAILURE) {
        RETURN_LONG(-1);
    }

    if (Z_TYPE_P(query) != IS_STRING) {
        RETURN_MY_ERROR("Argument 1 passed must be of the type string");
    }

    //初始化错误
    update_error_info(thisObject, "00000", "");

    //判断是否数据库 WRITE 写操作，或者 SELECT 查询
    int is_write = is_write_type(Z_STRVAL_P(query));

    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YC_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    //prepare query
    zval** prepare_args[1];
    zval* statement = NULL;

    prepare_args[0] = &query;

    if (yc_call_user_function_ex_fast(&pdo, "prepare", &statement, 1, prepare_args) == FAILURE) {
        yc_zval_free(statement);
        yc_php_fatal_error(E_ERROR, "failed to prepare query");
        RETURN_LONG(-1);
    }

    if (EG(exception) || YC_IS_NULL(statement)) {
        yc_zval_free(statement);
        RETURN_MY_ERROR("failed to prepare query, pdo is not initialized");
    }

    //bind value
    if (YC_IS_NOT_NULL(map) && Z_TYPE_P(map) == IS_ARRAY) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(map), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        }

        zval** bind_args[3];
        zval *bind_key = NULL, *bind_type = NULL;

        YC_MAKE_STD_ZVAL(bind_key);
        YC_MAKE_STD_ZVAL(bind_type);
        YC_ZVAL_STRING(bind_key, key, 1);

        bind_args[0] = &bind_key;
        bind_args[1] = &value;

        //绑定类型
        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            ZVAL_LONG(bind_type, PDO_PARAM_NULL);
            break;
        case IS_LONG:
            ZVAL_LONG(bind_type, PDO_PARAM_INT);
            break;
        case IS_DOUBLE:
            ZVAL_LONG(bind_type, PDO_PARAM_STR);
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
#else /* PHP Version 7 */
        case IS_TRUE:
        case IS_FALSE:
#endif
            ZVAL_LONG(bind_type, PDO_PARAM_BOOL);
            break;
        case IS_STRING:
            //if (is_numeric_string(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0)) { //当varchar类型的数字 where update为字符串的时候，数据库报错。
            //    ZVAL_LONG(bind_type, PDO_PARAM_INT);
            //} else {
            ZVAL_LONG(bind_type, PDO_PARAM_STR);
            //}
            break;
        default:
            break;
        }

        bind_args[2] = &bind_type;

        int ret = yc_call_user_function_return_bool_or_unsigned(&statement, "bindValue", 3, bind_args);
        yc_zval_ptr_dtor(&bind_key);
        yc_zval_ptr_dtor(&bind_type);

        if (ret == FAILURE) {
            yc_zval_free(statement);
            yc_php_fatal_error(E_ERROR, "failed to bind value");
            RETURN_LONG(-1);
        }

        YC_HASHTABLE_FOREACH_END();
    }

    //execute
    if (yc_call_user_function_return_bool_or_unsigned(&statement, "execute", 0, NULL) == FAILURE) {
        yc_zval_free(statement);
        yc_php_fatal_error(E_ERROR, "failed to execute sql");
        RETURN_LONG(-1);
    }

    if (EG(exception)) {
        yc_zval_free(statement);
        RETURN_MY_ERROR("[exception] failed to execute sql");
    }

    //获取查询 error 信息
    zval* errorCode = NULL;
    zval* errorInfo = NULL;

    yc_call_user_function_ex_fast(&statement, "errorCode", &errorCode, 0, NULL);
    yc_call_user_function_ex_fast(&statement, "errorInfo", &errorInfo, 0, NULL);

    update_pdo_error(thisObject, errorCode, errorInfo);

    if (YC_IS_NULL(errorCode) || strcmp(Z_STRVAL_P(errorCode), "00000") != 0) {
        yc_zval_free(statement);
        yc_zval_free(errorCode);
        yc_zval_free(errorInfo);
        RETURN_LONG(-1);
    }

    yc_zval_free(errorCode);
    yc_zval_free(errorInfo);

    if (is_write) {
        int row_count = yc_call_user_function_return_bool_or_unsigned(&statement, "rowCount", 0, NULL);
        yc_zval_free(statement);
        RETURN_LONG(row_count);
    } else {
        RETVAL_ZVAL(statement, 1, 1);
        efree(statement);
    }
}

//原生查询query
PHP_METHOD(ycdb, query) {
    zval* thisObject = getThis();
    zval *z_sql = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &z_sql) == FAILURE) {
        RETURN_LONG(-1);
    }

    if (Z_TYPE_P(z_sql) != IS_STRING) {
        RETURN_MY_ERROR("Argument 1 passed must be of the type string");
    }

    //执行SQL
    zval *statement = NULL;
    zval **exec_args[1];
    exec_args[0] = &z_sql;

    if (yc_call_user_function_ex_fast(&thisObject, "exec", &statement, 1, exec_args) == FAILURE) {
        yc_zval_ptr_dtor(&z_sql);
        yc_zval_free(statement);
        RETURN_LONG(-1);
    }

    yc_zval_ptr_dtor(&z_sql);

    if (YC_IS_NOT_NULL(statement) && Z_TYPE_P(statement) == IS_OBJECT) { //获取结果
        zval **fetch_args[1];
        zval *result = NULL, *fetch_type = NULL;

        YC_MAKE_STD_ZVAL(fetch_type);
        ZVAL_LONG(fetch_type, PDO_FETCH_ASSOC);

        fetch_args[0] = &fetch_type;

        if (yc_call_user_function_ex_fast(&statement, "fetchAll", &result, 1, fetch_args) == FAILURE) {
            yc_zval_free(statement);
            yc_zval_free(result);
            yc_zval_ptr_dtor(&fetch_type);
            RETURN_LONG(-1);
        }

        yc_zval_free(statement);
        yc_zval_ptr_dtor(&fetch_type);

        RETVAL_ZVAL(result, 1, 1);
        efree(result);
    } else {
        yc_zval_free(statement);
        RETURN_LONG(-1);
    }
}

PHP_METHOD(ycdb, insert_id) {
    zval *thisObject = getThis();

    //初始化错误
    update_error_info(thisObject, "00000", "");

    zval* pdo = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("_pdo") TSRMLS_CC);
    if (YC_IS_NULL(pdo)) {
        RETURN_MY_ERROR("pdo is empty");
    }

    //exec
    zval* insertid = NULL;
    if (yc_call_user_function_ex_fast(&pdo, "lastInsertId", &insertid, 0, NULL) == FAILURE) {
        yc_zval_free(insertid);
        yc_php_fatal_error(E_ERROR, "failed to get lastInsertId");
        RETURN_LONG(-1);
    }

    if (EG(exception) || YC_IS_NULL(insertid)) {
        yc_zval_free(insertid);
        RETURN_MY_ERROR("failed to get lastInsertId, pdo is not initialized");
    }

    RETVAL_ZVAL(insertid, 1, 1);
    efree(insertid);
}

//insert 插入
PHP_METHOD(ycdb, insert) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *data = NULL, *cache_info = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &cache_info) == FAILURE) {
        RETURN_LONG(-1);
    }
	
	//删除缓存
    if(YC_IS_NOT_NULL(cache_info) && YC_IS_ARRAY(cache_info)) {
        zval* cache_obj = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }
    
    //解析data数据
    char *insert_sql, *insert_keys,*insert_value;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    yc_string_emalloc_32(&insert_sql, 0);
    yc_string_emalloc_32(&insert_keys, 0);
    yc_string_emalloc_32(&insert_value, 0);

    YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        yc_string_efree_32(insert_keys);
        yc_string_efree_32(insert_value);
        yc_string_efree_32(insert_sql);
        RETURN_MY_ERROR("input data must be key/value hash, not index array.");
    } else {
        yc_multi_memcpy_auto_realloc(&insert_keys, 3, "`", key, "`,");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            yc_multi_memcpy_auto_realloc(&insert_value, 1, "NULL,");
            break;
        case IS_ARRAY:
            yc_multi_memcpy_auto_realloc(&insert_value, 1, "ARRAY,");
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
            if (Z_BVAL_P(value)) {
                yc_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            } else {
                yc_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            }
            break;
#else /* PHP Version 7 */
        case IS_TRUE:
            yc_multi_memcpy_auto_realloc(&insert_value, 1, "1,");
            break;
        case IS_FALSE:
            yc_multi_memcpy_auto_realloc(&insert_value, 1, "0,");
            break;
#endif
        case IS_LONG:
            yc_itoa(Z_LVAL_P(value), longval);
            yc_multi_memcpy_auto_realloc(&insert_value, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            yc_multi_memcpy_auto_realloc(&insert_value, 2, doubleval, ",");
            break;
        case IS_STRING:
            yc_multi_memcpy_auto_realloc(&insert_value, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    YC_HASHTABLE_FOREACH_END();

    rtrim_str(insert_keys, ",");
    rtrim_str(insert_value, ",");
    yc_multi_memcpy_auto_realloc(&insert_sql, 7, "INSERT INTO `", table, "` (", insert_keys ,") values (", insert_value, ")");
    yc_string_efree_32(insert_keys);
    yc_string_efree_32(insert_value);

    //执行SQL
    zval *result = NULL, *z_sql = NULL;
    zval **exec_args[1];

    YC_MAKE_STD_ZVAL(z_sql);
    YC_ZVAL_STRING(z_sql, insert_sql, 1);
    yc_string_efree_32(insert_sql);
    exec_args[0] = &z_sql;

    if (yc_call_user_function_ex_fast(&thisObject, "exec", &result, 1, exec_args) == FAILURE) {
        yc_zval_ptr_dtor(&z_sql);
        yc_zval_free(result);
        RETURN_LONG(-1);
    }

    yc_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}

//update 更新
PHP_METHOD(ycdb, update) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *data = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|z", &table, &table_len, &data, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //更新语句
    char *update_sql;
    yc_string_emalloc_32(&update_sql, 0);

    char *update_datas;
    char *key;
    zval *value;
    uint32_t key_len;
    int key_type;
    char longval[MAP_ITOA_INT_SIZE], doubleval[32];

    yc_string_emalloc_32(&update_datas, 0);

    YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(data), key, key_len, key_type, value)
    if (HASH_KEY_IS_STRING != key_type) {
        yc_string_efree_32(update_datas);
        yc_string_efree_32(update_sql);
        RETURN_MY_ERROR("input data must be key/value hash, not index array.");
    } else {
        yc_multi_memcpy_auto_realloc(&update_datas, 3, "`", key, "` = ");

        switch (Z_TYPE_P(value)) {
        case IS_NULL:
            yc_multi_memcpy_auto_realloc(&update_datas, 1, "NULL,");
            break;
        case IS_ARRAY:
            yc_multi_memcpy_auto_realloc(&update_datas, 1, "ARRAY,");
            break;
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
        case IS_BOOL:
            if (Z_BVAL_P(value)) {
                yc_multi_memcpy_auto_realloc(&update_datas, 1, "1,");
            } else {
                yc_multi_memcpy_auto_realloc(&update_datas, 1, "0,");
            }
            break;
#else /* PHP Version 7 */
        case IS_TRUE:
            yc_multi_memcpy_auto_realloc(&update_datas, 1, "1,");
            break;
        case IS_FALSE:
            yc_multi_memcpy_auto_realloc(&update_datas, 1, "0,");
            break;
#endif
        case IS_LONG:
            yc_itoa(Z_LVAL_P(value), longval);
            yc_multi_memcpy_auto_realloc(&update_datas, 2, longval, ",");
            break;

        case IS_DOUBLE:
            sprintf(doubleval, "%g", Z_DVAL_P(value));
            yc_multi_memcpy_auto_realloc(&update_datas, 2, doubleval, ",");
            break;
        case IS_STRING:
            yc_multi_memcpy_auto_realloc(&update_datas, 3, "'", Z_STRVAL_P(value), "',");
            break;
        }

    }
    YC_HASHTABLE_FOREACH_END();

    rtrim_str(update_datas, ",");
    yc_multi_memcpy_auto_realloc(&update_sql, 4, "UPDATE `", table, "` SET ", update_datas);
    yc_string_efree_32(update_datas);
    
    //where条件
    zval *map, *cache_info = NULL;
    YC_MAKE_STD_ZVAL(map);
    array_init(map);
		
    where_clause(where, map, & update_sql, &cache_info);
    
    //删除缓存
    if(YC_IS_NOT_NULL(cache_info) && YC_IS_ARRAY(cache_info)) {
        zval* cache_obj = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }

    //执行 SQL 语句
    zval *z_sql = NULL, *result = NULL;
    zval **exec_args[2];

    YC_MAKE_STD_ZVAL(z_sql);
    YC_ZVAL_STRING(z_sql, update_sql, 1);
    yc_string_efree_32(update_sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yc_call_user_function_ex_fast(&thisObject, "exec", &result, 2, exec_args) == FAILURE) {
        yc_zval_ptr_dtor(&map);
        yc_zval_ptr_dtor(&z_sql);
        yc_zval_free(result);
        RETURN_LONG(-1);
    }

    yc_zval_ptr_dtor(&map);
    yc_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}


//delete 删除
PHP_METHOD(ycdb, delete) {
    char *table = NULL;
    zval *thisObject = getThis();
    zend_size_t table_len;
    zval *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &table, &table_len, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //更新语句
    char *delete_sql;
    yc_string_emalloc_32(&delete_sql, 0);
    yc_multi_memcpy_auto_realloc(&delete_sql, 3, "DELETE FROM `", table, "` ");

	//cache 缓存
	zval *cache_info = NULL;
	
    //where条件
    zval *map;
    YC_MAKE_STD_ZVAL(map);
    array_init(map);

    where_clause(where, map, & delete_sql, &cache_info);
    
    //删除缓存
    if(YC_IS_NOT_NULL(cache_info) && YC_IS_ARRAY(cache_info)) {
        zval* cache_obj = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        del_cache(cache_obj, cache_info);
    }

    //执行 SQL 语句
    zval *z_sql = NULL, *result = NULL;
    zval **exec_args[2];

    YC_MAKE_STD_ZVAL(z_sql);
    YC_ZVAL_STRING(z_sql, delete_sql, 1);
    yc_string_efree_32(delete_sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yc_call_user_function_ex_fast(&thisObject, "exec", &result, 2, exec_args) == FAILURE) {
        yc_zval_ptr_dtor(&map);
        yc_zval_ptr_dtor(&z_sql);
        yc_zval_free(result);
        RETURN_LONG(-1);
    }

    yc_zval_ptr_dtor(&map);
    yc_zval_ptr_dtor(&z_sql);
    RETVAL_ZVAL(result, 1, 1);
    efree(result);
}

//select 查询
PHP_METHOD(ycdb, select) {
    char* table = NULL;
    zval* thisObject = getThis();
    zend_size_t table_len;
    zval* join = NULL, *columns = NULL, *where = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|zz", &table, &table_len, &join, &columns, &where) == FAILURE) {
        RETURN_LONG(-1);
    }

    //是否查询单个列，当 where 为空的时候， 判断 join 是不是 "*"，否则判断 columns 是不是 "*"
    int is_single_column = 0;
    if ((YC_IS_NULL(where) && Z_TYPE_P(join) == IS_STRING && strcmp(Z_STRVAL_P(join), "*") != 0 && yc_strpos(Z_STRVAL_P(join), ",") < 0)
            ||(YC_IS_NOT_NULL(where) && Z_TYPE_P(columns) == IS_STRING && strcmp(Z_STRVAL_P(columns), "*") != 0) && yc_strpos(Z_STRVAL_P(join), ",") < 0) {
        is_single_column = 1;
    }

    //查询语句初始化
    char *sql;
    zval *map;
    zval *cache_info = NULL, *cache_obj = NULL, *cache_key = NULL;
    
    YC_MAKE_STD_ZVAL(map);
    array_init(map);
    yc_string_emalloc_32(&sql, 0);
    
    select_context(table, map, join, columns, where, &sql, &cache_info);
    
    //缓存获取数据
    if(YC_IS_NOT_NULL(cache_info) && YC_IS_ARRAY(cache_info)) {
        cache_obj = yc_read_init_property(ycdb_ce_ptr, thisObject, ZEND_STRL("cache") TSRMLS_CC);
        if (YC_IS_NOT_NULL(cache_obj)) {
            cache_key = php_yc_array_get_value(Z_ARRVAL_P(cache_info), "key");
            if(YC_IS_NOT_NULL(cache_key)) {
                zval* cache_ret = get_cache(cache_obj, cache_key);
                
                if(cache_ret != NULL) {
                    RETVAL_ZVAL(cache_ret, 1, 1);
                    efree(cache_ret);
                    yc_string_efree_32(sql);
                    yc_zval_ptr_dtor(&map);
                    return;
                }
            }
        }
    }
    
    //执行SQL
    zval *z_sql = NULL, *statement = NULL;
    zval **exec_args[2];
    
    YC_MAKE_STD_ZVAL(z_sql);
    YC_ZVAL_STRING(z_sql, sql, 1);
    yc_string_efree_32(sql);

    exec_args[0] = &z_sql;
    exec_args[1] = &map;

    if (yc_call_user_function_ex_fast(&thisObject, "exec", &statement, 2, exec_args) == FAILURE) {
        yc_zval_ptr_dtor(&map);
        yc_zval_ptr_dtor(&z_sql);
        yc_zval_free(statement);
        RETURN_LONG(-1);
    }

    yc_zval_ptr_dtor(&map);
    yc_zval_ptr_dtor(&z_sql);

    if (YC_IS_NOT_NULL(statement) && Z_TYPE_P(statement) == IS_OBJECT) { //获取结果
        zval **fetch_args[1];
        zval *result = NULL, *fetch_type = NULL;

        YC_MAKE_STD_ZVAL(fetch_type);
        if (is_single_column == 1) {
            ZVAL_LONG(fetch_type, PDO_FETCH_COLUMN);
        } else {
            ZVAL_LONG(fetch_type, PDO_FETCH_ASSOC);
        }

        fetch_args[0] = &fetch_type;

        if (yc_call_user_function_ex_fast(&statement, "fetchAll", &result, 1, fetch_args) == FAILURE) {
            yc_zval_free(statement);
            yc_zval_free(result);
            yc_zval_ptr_dtor(&fetch_type);
            RETURN_LONG(-1);
        }

        yc_zval_free(statement);
        yc_zval_ptr_dtor(&fetch_type);
        
		//缓存数据
        if(YC_IS_NOT_NULL(cache_key)) {
            zval* cache_expire = php_yc_array_get_value(Z_ARRVAL_P(cache_info), "expire");
            zval* real_expire = NULL;
            if(YC_IS_NULL(cache_expire)) { //默认5分钟/300秒
                YC_MAKE_STD_ZVAL(real_expire);
                ZVAL_LONG(real_expire, 300);
            } else {
                real_expire = cache_expire;
            }
            
            set_cache(cache_obj, cache_key, real_expire, result);
        }
		
        RETVAL_ZVAL(result, 1, 1);
        efree(result);
    } else {
        yc_zval_free(statement);
        RETURN_LONG(-1);
    }
}

int set_cache(zval* cache_obj, zval* cache_key, zval* cache_expire, zval* cache_value) {
    zval* set_string_value;
    YC_MAKE_STD_ZVAL(set_string_value);
    yc_serialize(set_string_value, cache_value);
    
    //set value
    zval **set_args[2];
    set_args[0] = &cache_key;
    set_args[1] = &set_string_value;
    int ret = yc_call_user_function_return_bool_or_unsigned(&cache_obj, "set", 2, set_args);
    yc_zval_ptr_dtor(&set_string_value);
    
    //set expire time
    if(ret == 1) {
        zval **set_args[2];
        set_args[0] = &cache_key;
        set_args[1] = &cache_expire;
        ret = yc_call_user_function_return_bool_or_unsigned(&cache_obj, "expire", 2, set_args);
    }
}

zval* get_cache(zval* cache_obj, zval* cache_key) {
    zval *ret_string = NULL;
    zval **get_args[1];
    get_args[0] = &cache_key;
    
    if (yc_call_user_function_ex_fast(&cache_obj, "get", &ret_string, 1, get_args) == FAILURE) {
        yc_zval_free(ret_string);
        return NULL;
    }
    
    if (Z_TYPE_P(ret_string) != IS_STRING){
        yc_zval_free(ret_string);
        return NULL;
    }
    
    zval *ret_array;
    YC_ALLOC_INIT_ZVAL(ret_array);
    yc_unserialize(ret_array, ret_string);
    yc_zval_free(ret_string);
    
    if (YC_IS_NULL(ret_array) || Z_TYPE_P(ret_array) != IS_ARRAY) {
        yc_zval_free(ret_array);
        return NULL;
    }
    
    return ret_array;
}

void del_cache(zval* cache_obj, zval* cache_info) {
    if (YC_IS_NOT_NULL(cache_obj)) {
        zval* cache_key = php_yc_array_get_value(Z_ARRVAL_P(cache_info), "key");
        if(YC_IS_NOT_NULL(cache_key)) {
            zval **del_args[1];
            del_args[0] = &cache_key;
            yc_call_user_function_return_bool_or_unsigned(&cache_obj, "del", 1, del_args);
        }
    }
}

char *select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql, zval** cache_info) {
    //表别名
    char* table_query;
    yc_string_emalloc_32(&table_query, 0);

    char table_match[MAX_TABLE_SIZE] = {0};
    char alias_match[MAX_TABLE_SIZE] = {0};
    preg_table_match(table, table_match, alias_match);
    if (yc_is_string_not_empty(table_match) && yc_is_string_not_empty(alias_match)) {
        yc_multi_memcpy_auto_realloc(&table_query, 5, "`", table_match, "` AS `", alias_match, "`");
    } else {
        yc_multi_memcpy_auto_realloc(&table_query, 3, "`", table, "`");
    }

    //解析 join
    char* first_join_key = NULL;
    zval* real_where = where;
    zval* real_columns = columns;

    if (YC_IS_ARRAY(join) && (first_join_key = yc_get_array_key_index(join, 0)) != NULL && yc_strpos(first_join_key, "[") == 0) { //第二个参数不为空，而且判断是 join 的情况
        if (yc_is_string_not_empty(alias_match)) {
            handle_join(join, alias_match, &table_query);
        } else {
            handle_join(join, table, &table_query);
        }
    } else {
        if (YC_IS_NULL(where)) { //第四个参数为空时，第二个参数为 column , 第三个参数为 where
            real_columns = join;
            real_where = columns;
        }
    }

    //选择要查询的列
    char* column_query;
    yc_string_emalloc_32(&column_query, 0);

    column_push(real_columns, map, &column_query);

    yc_multi_memcpy_auto_realloc(sql, 4, "SELECT ", column_query, " FROM ", table_query);

    yc_string_efree_32(column_query);
    yc_string_efree_32(table_query);
		
    where_clause(real_where, map, sql, cache_info);
    return *sql;
}

//处理 where 条件
char* where_clause(zval* where, zval* map, char** sql, zval** cache_info) {
    if (YC_IS_EMPTY(where)) {
        return *sql;
    }

    char* group_by_condition = NULL;
    char* having_condition = NULL;
    char* order_condition = NULL;
    char* limit_condition = NULL;

    char* where_condition = NULL;
    yc_string_emalloc_32(&where_condition, 0);

    if (YC_IS_ARRAY(where)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(where), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            if (strcmp(key, "GROUP") == 0) { //分组
                yc_string_emalloc_32(&group_by_condition, 0);
                group_by_implode(value, &group_by_condition);
            } else if (strcmp(key, "HAVING") == 0) { //分组条件
                yc_string_emalloc_32(&having_condition, 0);
                having_implode(value, map, &having_condition);
            } else if (strcmp(key, "ORDER") == 0) { //排序
                yc_string_emalloc_32(&order_condition, 0);
                order_implode(value, &order_condition);
            } else if (strcmp(key, "LIMIT") == 0) { //结果条数
                yc_string_emalloc_32(&limit_condition, 0);
                limit_implode(value, &limit_condition);
            }  else if (strcmp(key, "CACHE") == 0) { //结果条数
            		*cache_info = value;
            } else { // where clause
                where_implode(key, value, map, &where_condition, "AND");
            }
        }
        YC_HASHTABLE_FOREACH_END();

        strreplace(where_condition, "( AND", "(");
        trim(ltrim_str(ltrim(where_condition), "AND"));
        if (where_condition[0] != '\0') {
            yc_multi_memcpy_auto_realloc(sql, 2, " WHERE ", where_condition);
        }
    }

    yc_string_efree_32(where_condition);

    if (group_by_condition != NULL) {
        yc_multi_memcpy_auto_realloc(sql, 2, " GROUP BY ", group_by_condition);
        yc_string_efree_32(group_by_condition);
    }

    if (having_condition != NULL) {
        yc_multi_memcpy_auto_realloc(sql, 2, " HAVING ", having_condition);
        yc_string_efree_32(having_condition);
    }

    if (order_condition != NULL) {
        yc_multi_memcpy_auto_realloc(sql, 2, " ORDER BY ", order_condition);
        yc_string_efree_32(order_condition);
    }

    if (limit_condition != NULL) {
        yc_multi_memcpy_auto_realloc(sql, 2, " LIMIT ", limit_condition);
        yc_string_efree_32(limit_condition);
    }

    return *sql;
}


//where condition
char* where_implode(char* key, zval* value, zval* map, char** where_query, char* connector) {
    //key是否是 AND 或者 OR
    char relation_ship[MAX_OPERATOR_SIZE] = {0};
    preg_and_or_match(key, relation_ship);

    if (Z_TYPE_P(value) == IS_ARRAY && yc_is_string_not_empty(relation_ship)) {
        char* relation_key;
        zval* relation_value;
        uint32_t relation_key_len;
        int relation_key_type;

        char* sub_where_clause = NULL;
        yc_string_emalloc_32(&sub_where_clause, 0);

        yc_multi_memcpy_auto_realloc(where_query, 1, " AND (");

        YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(value), relation_key, relation_key_len, relation_key_type, relation_value)
        if (HASH_KEY_IS_STRING != relation_key_type) {
            continue;
        } else {
            where_implode(relation_key, relation_value, map, &sub_where_clause, relation_ship);
        }
        YC_HASHTABLE_FOREACH_END();

        yc_multi_memcpy_auto_realloc(where_query, 2, sub_where_clause, ")");
        yc_string_efree_32(sub_where_clause);
        return *where_query;
    }

    char column[MAX_TABLE_SIZE] = {0};
    char operator[MAX_OPERATOR_SIZE] = {0};
    preg_operator_match(key, column, operator);

    if (yc_is_string_not_empty(column)) {
        char mapkey[MAP_KEY_SIZE] = {0};

        column_quote(column, column);

        if (yc_is_string_not_empty(operator)) {
            if (strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0) { // >, >=, <, <=
                get_mapkey(mapkey);
                add_map(map, mapkey, value);
                yc_multi_memcpy_auto_realloc(where_query, 6, connector, column, operator, " ", mapkey, " ");
            } else if (strcmp(operator, "!") == 0) { //not equal
                switch (Z_TYPE_P(value)) {
                case IS_NULL:
                    yc_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NOT NULL ");
                    break;
                case IS_ARRAY:
                    yc_multi_memcpy_auto_realloc(where_query, 3, connector, column, "NOT IN (");
                    handle_where_not_in(value, where_query, map);
                    yc_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    break;
                case IS_LONG:
                case IS_STRING:
                case IS_DOUBLE:
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
                case IS_BOOL:
#else /* PHP Version 7 */
                case IS_TRUE:
                case IS_FALSE:
#endif
                    get_mapkey(mapkey);
                    add_map(map, mapkey, value);
                    yc_multi_memcpy_auto_realloc(where_query, 5, connector, column, "!= ", mapkey, " ");
                    break;
                }
            } else if (strcmp(operator, "~") == 0 ||  strcmp(operator, "!~") == 0) { //like
                if (Z_TYPE_P(value) == IS_STRING) {
                    get_mapkey(mapkey);
                    add_map(map, mapkey, value);
                    yc_multi_memcpy_auto_realloc(where_query, 5, connector, column, (strcmp(operator, "~") == 0 ? "LIKE " : "NOT LIKE "), mapkey, " ");
                } else if (Z_TYPE_P(value) == IS_ARRAY) {
                    char* like_connector = yc_get_array_key_index(value, 0);
                    if (like_connector != NULL && (strcmp(like_connector, "AND") == 0 || strcmp(like_connector, "OR") == 0)) {
                        //自定义LIKE连接符
                        zval* connetor_value = php_yc_array_get_value(Z_ARRVAL_P(value), like_connector);

                        if (Z_TYPE_P(connetor_value) == IS_ARRAY) {
                            yc_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                            handle_like_array(connetor_value, where_query, column, operator, map, like_connector);
                            yc_multi_memcpy_auto_realloc(where_query, 1, ") ");
                        }
                    } else { //默认括号内LIKE连接符为 OR
                        yc_multi_memcpy_auto_realloc(where_query, 2, connector, " (");
                        handle_like_array(value, where_query, column, operator, map, "OR");
                        yc_multi_memcpy_auto_realloc(where_query, 1, ") ");
                    }
                }
            } else if (strcmp(operator, "<>") == 0 ||  strcmp(operator, "><") == 0) {
                if (Z_TYPE_P(value) == IS_ARRAY) {
                    zval* between_a = yc_get_element_by_hashtable_index(Z_ARRVAL_P(value), 0);
                    zval* between_b = yc_get_element_by_hashtable_index(Z_ARRVAL_P(value), 1);
                    if (YC_IS_NOT_EMPTY(between_a) && (Z_TYPE_P(between_a) == IS_LONG || Z_TYPE_P(between_a) == IS_STRING)
                            && YC_IS_NOT_EMPTY(between_b) && (Z_TYPE_P(between_b) == IS_LONG || Z_TYPE_P(between_b) == IS_STRING)) {
                        yc_multi_memcpy_auto_realloc(where_query, 2, connector, " ");
                        if (strcmp(operator, "><") == 0) {
                            yc_multi_memcpy_auto_realloc(where_query, 1, "NOT ");
                        }
                        get_mapkey(mapkey);
                        add_map(map, mapkey, between_a);
                        yc_multi_memcpy_auto_realloc(where_query, 5, "(", column, "BETWEEN ", mapkey, " ");
                        get_mapkey(mapkey);
                        add_map(map, mapkey, between_b);
                        yc_multi_memcpy_auto_realloc(where_query, 3, "AND ", mapkey, ") ");
                    }
                }
            }
        } else {
            switch (Z_TYPE_P(value)) {
            case IS_NULL:
                yc_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IS NULL ");
                break;
            case IS_ARRAY:
                yc_multi_memcpy_auto_realloc(where_query, 3, connector, column, "IN (");
                handle_where_not_in(value, where_query, map);
                yc_multi_memcpy_auto_realloc(where_query, 1, ") ");
                break;
            case IS_LONG:
            case IS_STRING:
            case IS_DOUBLE:
#if PHP_MAJOR_VERSION < 7 /* PHP Version 5 */
            case IS_BOOL:
#else /* PHP Version 7 */
            case IS_TRUE:
            case IS_FALSE:
#endif
                get_mapkey(mapkey);
                add_map(map, mapkey, value);
                yc_multi_memcpy_auto_realloc(where_query, 5, connector, column, "= ", mapkey, " ");
                break;
            }
        }
    }

    ltrim_str(*where_query, connector);
    return *where_query;
}

//handle group by
char* group_by_implode(zval* group, char** group_by_condition) {
    if (YC_IS_NOT_EMPTY(group)) {
        if (Z_TYPE_P(group) == IS_STRING) {
            yc_multi_memcpy_auto_realloc(group_by_condition, 1, Z_STRVAL_P(group));
        } else if (Z_TYPE_P(group) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;


            YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(group), key, key_len, key_type, value)
            if (Z_TYPE_P(value) == IS_STRING) {
                yc_multi_memcpy_auto_realloc(group_by_condition, 2, Z_STRVAL_P(value), ",");
            }
            YC_HASHTABLE_FOREACH_END();

            char* tmp = (*group_by_condition) +  strlen(*group_by_condition) - 1;
            if (*tmp == ',') {
                *tmp = '\0';
            }
        }
    }
    return *group_by_condition;
}

//handle having
char* having_implode(zval* having, zval* map, char** having_condition) {

    if (YC_IS_ARRAY(having)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(having), key, key_len, key_type, value)
        if (HASH_KEY_IS_STRING != key_type) {
            continue;
        } else {
            where_implode(key, value, map, having_condition, "AND");
        }
        YC_HASHTABLE_FOREACH_END();
    }

    strreplace(*having_condition, "( AND", "(");
    trim(ltrim_str(ltrim(*having_condition), "AND"));
    return *having_condition;
}

//order by
char* order_implode(zval* order, char** order_condition) {
    if (YC_IS_NOT_EMPTY(order)) {
        if (Z_TYPE_P(order) == IS_STRING) {
            yc_multi_memcpy_auto_realloc(order_condition, 1, Z_STRVAL_P(order));
        } else if (Z_TYPE_P(order) == IS_ARRAY) {
            char* key;
            zval* value;
            uint32_t key_len;
            int key_type;

            YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(order), key, key_len, key_type, value)
            if (HASH_KEY_IS_STRING != key_type) {
                if (Z_TYPE_P(value) == IS_STRING) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(Z_STRVAL_P(value), column);
                    yc_multi_memcpy_auto_realloc(order_condition, 2, column, ",");
                }
            } else {
                if (Z_TYPE_P(value) == IS_STRING && (strcmp(Z_STRVAL_P(value), "ASC") == 0 || strcmp(Z_STRVAL_P(value), "DESC") == 0)) {
                    char column[MAX_TABLE_SIZE] = {0};
                    column_quote(key, column);
                    yc_multi_memcpy_auto_realloc(order_condition, 3, column, Z_STRVAL_P(value), ",");
                }
            }
            YC_HASHTABLE_FOREACH_END();
            rtrim_str(*order_condition, ",");
        }
    }
    return *order_condition;
}

//limit
char* limit_implode(zval* limit, char** limit_condition) {
    if (YC_IS_NOT_EMPTY(limit)) {
        if (Z_TYPE_P(limit) == IS_STRING || Z_TYPE_P(limit) == IS_LONG) {
            convert_to_string(limit);
            if (is_numeric_string(Z_STRVAL_P(limit), Z_STRLEN_P(limit), NULL, NULL, 0)) {
                yc_multi_memcpy_auto_realloc(limit_condition, 1, Z_STRVAL_P(limit));
            }
        } else if (Z_TYPE_P(limit) == IS_ARRAY) {
            zval* offset_val = yc_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 0);
            zval* limit_val = yc_get_element_by_hashtable_index(Z_ARRVAL_P(limit), 1);
            convert_to_string(limit_val);
            convert_to_string(offset_val);

            if (is_numeric_string(Z_STRVAL_P(limit_val), Z_STRLEN_P(limit_val), NULL, NULL, 0)
                    && is_numeric_string(Z_STRVAL_P(offset_val), Z_STRLEN_P(offset_val), NULL, NULL, 0)) {
                yc_multi_memcpy_auto_realloc(limit_condition, 3, Z_STRVAL_P(limit_val), " OFFSET ", Z_STRVAL_P(offset_val));
            }
        }
    }

    return *limit_condition;
}

//like array情况
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;

    char mapkey[MAP_KEY_SIZE] = {0};
    YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(like_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        get_mapkey(mapkey);
        add_map(map, mapkey, value);
        yc_multi_memcpy_auto_realloc(where_query, 5, column, strcmp(operator, "~") == 0 ? "LIKE " : "NOT LIKE ", mapkey, " ", connector);
    }
    YC_HASHTABLE_FOREACH_END();
    rtrim_str(rtrim(*where_query), connector);
    return *where_query;
}

//not in 情况
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map) {
    char * key;
    zval *value;
    uint32_t key_len;
    int key_type;

    char mapkey[MAP_KEY_SIZE] = {0};

    YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(not_in_array), key, key_len, key_type, value)
    if (Z_TYPE_P(value) == IS_STRING || Z_TYPE_P(value) == IS_LONG) {
        get_mapkey(mapkey);
        add_map(map, mapkey, value);
        yc_multi_memcpy_auto_realloc(where_query, 3, " ", mapkey, ",");
    }
    YC_HASHTABLE_FOREACH_END();

    rtrim_str(rtrim(*where_query), ",");
    return *where_query;
}

char* get_mapkey(char* mapkey) {
    memset(mapkey, 0, MAP_KEY_SIZE);
    zval* mapkey_index = zend_read_static_property(ycdb_ce_ptr, ZEND_STRL("mapkey_index"), 1 TSRMLS_DC);
    long l_mapkey_index = Z_LVAL_P(mapkey_index);
    l_mapkey_index = (l_mapkey_index + 1) % 9999;
    zend_update_static_property_long(ycdb_ce_ptr, ZEND_STRL("mapkey_index"), l_mapkey_index TSRMLS_DC);
    sprintf(mapkey, ":param_%d", l_mapkey_index);
    return mapkey;
}

zval* add_map(zval* map, char* mapkey, zval* value) {
    zval *copy;
    YC_MAKE_STD_ZVAL(copy);
    *copy = *value;
    zval_copy_ctor(copy);
    add_assoc_zval(map, mapkey, copy);
    return map;
}

//处理查询列
char* column_push(zval* columns, zval* map, char** column_query) {
    if (YC_IS_EMPTY(columns) || (Z_TYPE_P(columns) == IS_STRING && strcmp(Z_STRVAL_P(columns), "*") == 0)) {
        yc_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }

    if (Z_TYPE_P(columns) == IS_STRING) {
        yc_multi_memcpy_auto_realloc(column_query, 1, Z_STRVAL_P(columns));
        return *column_query;
    } else if (YC_IS_ARRAY(columns)) {
        char * key;
        zval *value;
        uint32_t key_len;
        int key_type;

        YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(columns), key, key_len, key_type, value)
        if (Z_TYPE_P(value) != IS_STRING) {
            continue;
        }

        char match_column[MAX_TABLE_SIZE] = {0};
        char match_alias[MAX_TABLE_SIZE] = {0};
        preg_table_match(Z_STRVAL_P(value), match_column, match_alias);

        if (yc_is_string_not_empty(match_column) && yc_is_string_not_empty(match_alias)) {
            yc_multi_memcpy_auto_realloc(column_query, 4, match_column, " AS `", match_alias, "`,");
        } else {
            yc_multi_memcpy_auto_realloc(column_query, 2, Z_STRVAL_P(value), ",");
        }

        YC_HASHTABLE_FOREACH_END();

        rtrim_str(rtrim(*column_query), ",");
        return *column_query;
    } else {
        yc_multi_memcpy_auto_realloc(column_query, 1, "*");
        return *column_query;
    }
}

//处理join
char* handle_join(zval *join, char *table, char** table_query) {
    char* sub_table;
    zval* relation;
    uint32_t key_len;
    int key_type;

    YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(join), sub_table, key_len, key_type, relation)
    if (HASH_KEY_IS_STRING != key_type) { //非字符串
        continue;
    }

    char join_join[MAX_TABLE_SIZE] = {0};
    char join_table[MAX_TABLE_SIZE] = {0};
    char join_alias[MAX_TABLE_SIZE] = {0};
    preg_join_match(sub_table, join_join, join_table, join_alias);


    if (yc_is_string_not_empty(join_join) && yc_is_string_not_empty(join_table)) {
        yc_multi_memcpy_auto_realloc(table_query, 5, " ", get_join_type(join_join), " JOIN `", join_table, "` ");
        if (yc_is_string_not_empty(join_alias)) {
            yc_multi_memcpy_auto_realloc(table_query, 3, "AS `", join_alias, "` ");
        }

        if (Z_TYPE_P(relation) == IS_STRING) {
            yc_multi_memcpy_auto_realloc(table_query, 3, "USING (`", Z_STRVAL_P(relation), "`) ");
        } else if (Z_TYPE_P(relation) == IS_ARRAY) {
            if (is_set_array_index(Z_ARRVAL_P(relation), 0)) { //relation 为数组
                yc_multi_memcpy_auto_realloc(table_query, 1, "USING (`");
                yc_implode(relation, "`,`", table_query);
                yc_multi_memcpy_auto_realloc(table_query, 1, "`) ");
            } else { //relation 为 Key Hash
                char *key;
                zval *value;

                yc_multi_memcpy_auto_realloc(table_query, 1, "ON ");

                YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(relation), key, key_len, key_type, value)
                if (HASH_KEY_IS_STRING != key_type) { //非字符串
                    continue;
                }

                char* table_column = NULL;
                yc_string_emalloc_32(&table_column, 0);
                if (yc_strpos(key, ".") >= 0) {
                    column_quote(key, table_column);
                } else {
                    yc_multi_memcpy_auto_realloc(&table_column, 5, "`", table, "`.`", key, "`");
                }

                //alias
                if (yc_is_string_not_empty(join_alias)) {
                    yc_multi_memcpy_auto_realloc(table_query, 4, table_column, "=`", join_alias, "`");
                } else {
                    yc_multi_memcpy_auto_realloc(table_query, 4, table_column, "= `", join_table, "`");
                }

                yc_string_efree_32(table_column);

                yc_multi_memcpy_auto_realloc(table_query, 3, ".`", Z_STRVAL_P(value), "` AND");
                YC_HASHTABLE_FOREACH_END();

                rtrim_str(rtrim(*table_query), "AND");
            }
        }

    }
    YC_HASHTABLE_FOREACH_END();


    return *table_query;
}

char* column_quote(char* string, char* table_column) {
    char tmp[MAX_TABLE_SIZE] = {0};

    sprintf(tmp, " `%s` ", string);

    if (strlen(tmp) >= MAX_TABLE_SIZE) {
        yc_php_fatal_error(E_ERROR, "column size is too long, [%s]", string);
    }

    if (yc_strpos(tmp, ".") >= 0) {
        if (strlen(tmp) + 5 >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "column + alias size is too long, [%s]", string);
        }
        strreplace(tmp, ".", "`.`");
    }

    strcpy(table_column, tmp);
    return table_column;
}

char* get_join_type(char* type) {
    if (strcmp(type, "<") == 0) {
        return "LEFT";
    } else if (strcmp(type, ">") == 0) {
        return "RIGHT";
    } else if (strcmp(type, "<>") == 0) {
        return "FULL";
    } else if (strcmp(type, "><") == 0) {
        return "INNER";
    } else {
        return "";
    }
}

//匹配表和别名
int preg_join_match(char* key, char* join, char* table, char* alias) {
    int join_start = -1;
    int join_end = -1;
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    join[0] = '\0';
    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( join_start == -1 && c_key == '[') {
            join_start = i;
        }

        if (table_start == -1 && join_start == -1 && c_key != '[' && !yc_is_space(c_key)) {
            table_start = i;
        }

        if (join_end != -1 && table_start == -1 && !yc_is_space(c_key)) {
            table_start = i;
        }

        if ( join_start != -1 && c_key == ']') {
            join_end = i;
        }

        if (table_start != -1 && c_key == '(') {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "join table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "join alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    if (join_start != -1 && join_end != -1 && join_start < join_end) {
        if (join_end - join_start >= MAX_OPERATOR_SIZE) {
            yc_php_fatal_error(E_ERROR, "join operator size is too long, [%s]", key);
        }

        memset(join, 0, MAX_OPERATOR_SIZE);
        memcpy(join, key + join_start + 1, join_end - join_start - 1);
        if (!(strcmp(join, ">") == 0 || strcmp(join, "<") == 0 || strcmp(join, "<>") == 0 || strcmp(join, "><") == 0)) {
            join[0] = '\0';
        }
    }
    return 1;
}

//匹配表和别名
int preg_table_match(char* key, char* table, char* alias) {
    int table_start = -1;
    int table_end = -1;
    int alias_start = -1;
    int alias_end = -1;

    int key_len = strlen(key);

    table[0] = '\0';
    alias[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( table_start == -1 && !yc_is_space(c_key)) {
            table_start = i;
        }

        if (table_end == -1 && (c_key == '(' || yc_is_space(c_key))) {
            table_end = i;
        }

        if ( alias_start == -1 && c_key == '(') {
            alias_start = i;
        }

        if ( alias_end == -1 && c_key == ')') {
            alias_end = i;
        }
    }

    if (alias_start == -1 || alias_end == -1 || alias_start > alias_end) {
        table_end = key_len;
    }

    if (table_start != -1 && table_end != -1 && table_end > table_start) {
        if (table_end - table_start >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "table size is too long, [%s]", key);
        }

        memset(table, 0, MAX_TABLE_SIZE);
        memcpy(table, key + table_start, table_end - table_start);
    }

    if (alias_start != -1 && alias_end != -1 && alias_end > alias_start) {
        if (alias_end - alias_start >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "alias size is too long, [%s]", key);
        }

        memset(alias, 0, MAX_TABLE_SIZE);
        memcpy(alias, key + alias_start + 1, alias_end - alias_start - 1);
    }

    return 1;
}

//匹配列名和操作符
int preg_operator_match(char* key, char* column, char* operator) {
    int column_start = -1;
    int column_end = -1;
    int column_end_is_space = -1;
    int operator_start = -1;
    int operator_end = -1;

    int key_len = strlen(key);

    column[0] = '\0';
    operator[0] = '\0';

    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];
        if ( column_start == -1 && !yc_is_space(c_key)) {
            column_start = i;
        }

        if (column_end == -1 && (c_key == '[' || yc_is_space(c_key))) {
            column_end = i;
        }

        if (column_end_is_space == -1 && yc_is_space(c_key)) {
            column_end_is_space = i;
        }

        if ( operator_start == -1 && c_key == '[') {
            operator_start = i;
        }

        if ( operator_end == -1 && c_key == ']') {
            operator_end = i;
        }
    }

    if (operator_start == -1 || operator_end == -1 || operator_start > operator_end) {
        column_end = column_end_is_space == -1 ? key_len : column_end_is_space;
    }

    if (column_start != -1 && column_end != -1 && column_end > column_start) {
        if (column_end - column_start - 1 >= MAX_TABLE_SIZE) {
            yc_php_fatal_error(E_ERROR, "column size is too long [%s]", key);
        }

        memset(column, 0, MAX_TABLE_SIZE);
        memcpy(column, key + column_start, column_end - column_start);
    }

    if (operator_start != -1 && operator_end != -1 && operator_start < operator_end) {
        if (operator_end - operator_start - 1 >= MAX_OPERATOR_SIZE) {
            yc_php_fatal_error(E_ERROR, "operator size is too long [%s]", key);
        }

        memset(operator, 0, MAX_OPERATOR_SIZE);
        memcpy(operator, key + operator_start + 1, operator_end - operator_start - 1);
        if (!(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 ||
                strcmp(operator, "!") == 0 || strcmp(operator, "~") == 0 || strcmp(operator, "!~") == 0 || strcmp(operator, "<>") == 0 || strcmp(operator, "><") == 0)) {
            operator[0] = '\0';
        }
    }

    return 1;
}

//匹配是否 AND 或者 OR
int preg_and_or_match(char* key, char* relation) {
    int relation_start = -1;
    int relation_end = -1;

    relation[0] = '\0';

    int key_len = strlen(key);
    if (key_len == 0) {
        return 0;
    }

    int i = -1;
    while (i < key_len) {
        i++;
        char c_key = key[i];

        if ( relation_start == -1 && !yc_is_space(c_key)) {
            relation_start = i;
        }

        if (relation_end == -1 && ( c_key == '#' || yc_is_space(c_key))) {
            relation_end = i;
        }

        if (relation_end == -1 && i == key_len - 1) {
            relation_end = key_len;
        }
    }

    if (relation_start != -1 && relation_end != -1 && relation_end > relation_start && relation_end - relation_start - 1 < MAX_OPERATOR_SIZE) {
        memset(relation, 0, MAX_OPERATOR_SIZE);
        memcpy(relation, key + relation_start, relation_end - relation_start);
        if (strcmp(relation, "AND") != 0 && strcmp(relation, "OR") != 0 && strcmp(relation, "and") != 0 && strcmp(relation, "or") != 0 ) {
            relation[0] = '\0';
        }
    }

    return 1;
}

zend_class_entry* get_pdo_ce() {
    zend_class_entry* ce;
    zend_string* pdo_class_name = zend_string_init("PDO", strlen("PDO"), 0);
    ce = zend_fetch_class(pdo_class_name, ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
    zend_string_free(pdo_class_name);
    return ce;
}

void update_error_info(zval* obj, char* code, char* errmsg) {
    zend_update_property_string(ycdb_ce_ptr, obj, ZEND_STRL("errcode"), code TSRMLS_CC);
    zval* errorinfo = yc_read_init_property(ycdb_ce_ptr, obj, ZEND_STRL("errinfo") TSRMLS_CC);

    if (errorinfo != NULL) {
        zend_update_property_null(ycdb_ce_ptr, obj, ZEND_STRL("errinfo"));
    }

    zval* error_array;
    YC_MAKE_STD_ZVAL(error_array);
    array_init(error_array);

    add_index_string(error_array, 0, code);
    if (strcmp(code, "00000") == 0) {
        add_index_long(error_array, 1, 0);
    } else {
        add_index_long(error_array, 1, -1);
    }
    add_index_string(error_array, 2, errmsg);

    zend_update_property(ycdb_ce_ptr, obj, ZEND_STRL("errinfo"), error_array TSRMLS_CC);
    yc_zval_ptr_dtor(&error_array);
}

void update_pdo_error(zval* obj, zval* errorcode, zval* errorinfo) {
    zend_update_property(ycdb_ce_ptr, obj, ZEND_STRL("errcode"), errorcode TSRMLS_CC);
    zend_update_property(ycdb_ce_ptr, obj, ZEND_STRL("errinfo"), errorinfo TSRMLS_CC);
}

int is_write_type(char* sql) {
    char *start = sql;
    int sql_len = strlen(sql);
    int i=0, len=0;
    char operator[10] = {0};

    while (i < sql_len) {
        i++;
        if (!isspace(*start) && (*start) != '\n' && (*start) != '\r' && (*start) != '\t') {
            break;
        }
        start++;
    }

    char* end = start;

    while (i < sql_len && len < 8) {
        if (isspace(*end) || (*end) == '\n' || (*end) == '\r' || (*end) == '\t') {
            break;
        }

        end++;
        i++;
        len++;
    }

    memcpy(operator, start, len + 1);
    rtrim(operator);
    php_strtoupper(operator, strlen(operator));

    if (strcmp(operator, "INSERT") == 0 || strcmp(operator, "UPDATE") == 0 || strcmp(operator, "DELETE") == 0 || strcmp(operator, "REPLACE") == 0
            || strcmp(operator, "SET") == 0 || strcmp(operator, "CREATE") == 0 || strcmp(operator, "DROP") == 0 || strcmp(operator, "TRUNCATE") == 0
            || strcmp(operator, "ALTER") == 0 || strcmp(operator, "LOCK") == 0 || strcmp(operator, "UNLOCK") == 0) {
        return 1;
    }

    return 0;
}