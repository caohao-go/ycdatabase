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
#include "ycdatabase.h"
#include "uthash.h"

//根据数组下标从数组中获取字符串
char* yc_get_string_from_array_index(zval *array, ulong index) 
{
	zval *pData = NULL;
	
	yc_zend_hash_index_find(Z_ARRVAL_P(array), index, (void**) &pData);

	if(pData == NULL){
		return NULL;
	}
	
	if(Z_TYPE_P(pData) != IS_STRING) {
		return NULL;
	}
	
	char * str = Z_STRVAL_P(pData);
	return str;
}

//获取数组的 array_keys ，注意不用了销毁返回的HashTable指针指向的内存地址
HashTable* yc_get_array_keys(zval *p) {
	if(YC_IS_NOT_ARRAY(p)) {
		return NULL;
	}
	
	uint32_t array_size = zend_hash_num_elements(Z_ARRVAL_P(p));
	if(array_size == 0) {
		return NULL;
	}
	
	char * key;
	zval *value;
	uint32_t key_len;
	int key_type;
	ulong_t num = 0;
	
	HashTable *new_hash;
	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, array_size, NULL, ZVAL_PTR_DTOR, 0);
	
	YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(p), key, key_len, key_type, value)
			if (HASH_KEY_IS_STRING != key_type) { //非字符串
				continue;
			}
			
			zval *zval_key;
			YC_MAKE_STD_ZVAL(zval_key);
			YC_ZVAL_STRING(zval_key, key, 1);
			yc_zend_hash_index_update(new_hash, num, (void*) zval_key, sizeof(zval *), NULL);
			yc_zval_ptr_dtor(&zval_key);
			num++;
  YC_HASHTABLE_FOREACH_END();
  
  if(zend_hash_num_elements(new_hash) == 0) {
  	yc_free_hash(new_hash);
  	new_hash = NULL;
  }
  
  return new_hash;
}

//获取数组的第N个key
char* yc_get_array_key_index(zval *p, uint32_t index) {
	if(YC_IS_NOT_ARRAY(p)) {
		return NULL;
	}
	
	uint32_t array_size = zend_hash_num_elements(Z_ARRVAL_P(p));
	if(array_size < index) {
		return NULL;
	}
	
	char * key;
	zval *value;
	uint32_t key_len;
	int key_type;
	ulong_t num = 0;
	
	YC_HASHTABLE_FOREACH_START2(Z_ARRVAL_P(p), key, key_len, key_type, value)
			if (HASH_KEY_IS_STRING != key_type) { //非字符串
				continue;
			}
			
			if(num == index) {
				return key;
			}
			
			num++;
  YC_HASHTABLE_FOREACH_END();
  return NULL;
}

//根据 index 获取 hashtable 的元素
zval* yc_get_element_by_hashtable_index(HashTable *ht, int index) {
	if(ht == NULL) {
		return NULL;
	}
	zval *value;
	yc_zend_hash_index_find(ht, index, (void**) &value);
	return value;
}

//根据 key 获取 hashtable 的元素
zval* php_yc_array_get_value(HashTable *ht, char *key) {
	zval *v;
	if( yc_zend_hash_find(ht, key, strlen(key), (void **) &v) == SUCCESS ) {
		if(ZVAL_IS_NULL(v)) {
			return NULL;
		} else {
			return v;
		}
	} else {
		return NULL;
	}
}

//销毁 HashTable， 删除HashTable的数据， 并销毁释放 HashTable 内存
void yc_destroy_hash(HashTable *ht) {
	uint32_t array_size = zend_hash_num_elements(ht);
	if(array_size == 0) {
		zend_hash_destroy(ht);
		FREE_HASHTABLE(ht);
		return;
	}
	
	char *key;
	zval *value;
	uint32_t key_len;
	int key_type;
	
	YC_HASHTABLE_FOREACH_START2(ht, key, key_len, key_type, value)
		if(YC_IS_ARRAY(value)) {
			yc_destroy_hash(Z_ARRVAL_P(value));
		}
		
		yc_zval_ptr_dtor(&value);
  	YC_HASHTABLE_FOREACH_END();
  	
	zend_hash_destroy(ht);
	FREE_HASHTABLE(ht);
}

//销毁数组， 删除其中HashTable的数据， 并销毁释放 HashTable 内存，并销毁数组内存，将数组指针置为NULL
void yc_destroy_array(zval **array) {
	yc_destroy_hash(Z_ARRVAL_P(*array));
	yc_zval_ptr_dtor(array);
	*array = NULL;
}

//清理 HashTable 内的数据元素。
void yc_clean_hash(HashTable *ht) {
	uint32_t array_size = zend_hash_num_elements(ht);
	if(array_size == 0) {
		return;
	}
	
	char *key;
	zval *value;
	uint32_t key_len;
	int key_type;
	
	YC_HASHTABLE_FOREACH_START2(ht, key, key_len, key_type, value)
		if(YC_IS_ARRAY(value)) {
			yc_clean_hash(Z_ARRVAL_P(value));
		}
		yc_zval_ptr_dtor(&value);
  	YC_HASHTABLE_FOREACH_END();
}

int is_set_array_index(HashTable *ht, int index) {
	zval* p = yc_get_element_by_hashtable_index(ht, index);
	if(YC_IS_NOT_EMPTY(p)) {
		return 1;
	} else {
		return 0;
	}
}


char* yc_get_string_from_hashtable_index(HashTable *ht, int index) {
	zval* val = yc_get_element_by_hashtable_index(ht, index);
	if(YC_IS_EMPTY(val)) {
		return NULL;
	}
	
	if(Z_TYPE_P(val) == IS_STRING) {
		return Z_STRVAL_P(val);
	} else {
		return NULL;
	}
}

char* yc_get_string_from_hashtable(HashTable *ht, char* key) {
	zval* val = php_yc_array_get_value(ht, key);
	if(YC_IS_EMPTY(val)) {
		return NULL;
	}
	
	if(Z_TYPE_P(val) == IS_STRING) {
		return Z_STRVAL_P(val);
	} else {
		return NULL;
	}
}

//php implode 功能
char* yc_implode(zval *arr, const char *delim_str, char** result)
{
	zval *return_value = NULL;
	YC_MAKE_STD_ZVAL(return_value);
	zend_string *delim = zend_string_init(delim_str, strlen(delim_str), 0);
	
	php_implode(delim, arr, return_value);
	
	yc_multi_memcpy_auto_realloc(result, 1, Z_STRVAL_P(return_value));
	
	efree(delim);
	yc_zval_ptr_dtor(&return_value);
	
	return *result;
}

