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
#ifndef YCDB_H_
#define YCDB_H_

/////////////////////// 类声明 ///////////////////////
#define PDO_PARAM_NULL 0
#define PDO_PARAM_INT 1
#define PDO_PARAM_STR 2
#define PDO_PARAM_LOB 3
#define PDO_PARAM_STMT 4
#define PDO_PARAM_BOOL 5

#define PDO_FETCH_ASSOC 2
#define PDO_FETCH_COLUMN 7 

#define MAP_KEY_SIZE 16
#define MAX_TABLE_SIZE 48
#define MAX_OPERATOR_SIZE 4

zend_class_entry ycdb_ce;
zend_class_entry* ycdb_ce_ptr;

void ycdb_init(); //初始化函数

PHP_METHOD(ycdb, __construct);
PHP_METHOD(ycdb, __destruct);
PHP_METHOD(ycdb, select);
PHP_METHOD(ycdb, initialize);
PHP_METHOD(ycdb, begin);
PHP_METHOD(ycdb, commit);
PHP_METHOD(ycdb, rollback);
PHP_METHOD(ycdb, exec);
PHP_METHOD(ycdb, query);
PHP_METHOD(ycdb, insert);
PHP_METHOD(ycdb, replace);
PHP_METHOD(ycdb, insert_id);
PHP_METHOD(ycdb, update);
PHP_METHOD(ycdb, delete);
PHP_METHOD(ycdb, errorCode);
PHP_METHOD(ycdb, errorInfo);

//私有函数
char* select_context(char* table, zval* map, zval* join, zval* columns, zval* where, char** sql, zval** cache_info);
char* handle_join(zval* join, char* table, char** table_query);
char* column_quote(char* string, char* table_column);
char* get_join_type(char* type);
char* column_push(zval* columns, zval* map, char** column_query);
char* where_clause(zval* where, zval* map, char** where_query, zval** cache_info);
char* where_implode(char* key, zval* data, zval* map, char** where_clause, char* connector);
char* group_by_implode(zval* group, char** group_by_condition);
char* having_implode(zval* having, zval* map, char** having_condition);
char* order_implode(zval* order, char** order_condition);
char* limit_implode(zval* limit, char** limit_condition);
char* handle_where_not_in(zval* not_in_array, char** where_query, zval* map);
char* handle_like_array(zval* like_array, char** where_query, char* column, char* operator, zval* map, char* connector);
char* get_mapkey(char* mapkey);
char* get_mapkey_like(char* mapkey);
zval* add_map(zval* map, char* mapkey, zval* value);
void update_error_info(zval* obj, char* code, char* errmsg);
void update_pdo_error(zval* obj, zval* errorcode, zval* errorinfo);
int is_write_type(char* sql);
int is_insert(char* sql);
int set_cache(zval* cache_obj, zval* cache_key, zval* cache_expire, zval* cache_value);
zval* get_cache(zval* cache_obj, zval* cache_key);
void del_cache(zval* cache_obj, zval* cache_info);

//preg函数
int preg_join_match(char* key, char* join, char* table, char* alias); //匹配join表
int preg_operator_match(char* key, char* column, char* operator); //匹配列名和操作符
int preg_and_or_match(char* key, char* relation); //匹配是否 AND 或者 OR
int preg_table_match(char* key, char* table, char* alias);  //匹配表名和别名

//object函数
zend_class_entry* get_pdo_ce();

//错误处理函数
#define RETURN_MY_ERROR(errmsg) update_error_info(thisObject, "E0001", errmsg);yc_php_fatal_error(E_WARNING, errmsg);RETURN_LONG(-1);
#endif
