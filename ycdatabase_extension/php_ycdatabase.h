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
#ifndef PHP_YCDATABASE_H
#define PHP_YCDATABASE_H 1

#include "php.h"
#include "php_ini.h"  //获取 php.ini 操作的方法

#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_variables.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ycdatabase.h"
BEGIN_EXTERN_C()

#define PHP_YCDATABASE_VERSION  "1.7.0"

#ifndef ZEND_MOD_END
#define ZEND_MOD_END {NULL,NULL,NULL}
#endif

extern zend_module_entry ycdatabase_module_entry;  //扩展模块实体的定义，(扩展必须)
#define phpext_ycdatabase_ptr &ycdatabase_module_entry  //定义php扩展ycdatabase指针，(扩展必须)

#ifdef PHP_WIN32
#	define PHP_YCDATABASE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_YCDATABASE_API __attribute__ ((visibility("default")))
#else
#	define PHP_YCDATABASE_API
#endif

//异常类实体
extern zend_class_entry yc_exception_ce;
extern zend_class_entry *yc_exception_class_entry_ptr;

//函数参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_ycdatabase_void, 0, 0, 0)
ZEND_END_ARG_INFO()

//初始化函数
PHP_MINIT_FUNCTION(ycdatabase);
PHP_MSHUTDOWN_FUNCTION(ycdatabase);
PHP_RINIT_FUNCTION(ycdatabase);
PHP_RSHUTDOWN_FUNCTION(ycdatabase);
PHP_MINFO_FUNCTION(ycdatabase);

//普通函数
PHP_FUNCTION(ycdatabase_version);

//全局变量
ZEND_BEGIN_MODULE_GLOBALS(ycdatabase)
ZEND_END_MODULE_GLOBALS(ycdatabase)

extern ZEND_DECLARE_MODULE_GLOBALS(ycdatabase);

#ifdef ZTS
#define YCDATABASE_G(v) TSRMG(ycdatabase_globals_id, zend_ycdatabase_globals *, v)
#else
#define YCDATABASE_G(v) (ycdatabase_globals.v)
#endif

////////////////// 类声明 ///////////////////
extern void ycdb_init();

END_EXTERN_C()

#endif
