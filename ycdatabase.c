#include "php_ycdatabase.h"
#include "zend_variables.h"

ZEND_DECLARE_MODULE_GLOBALS(ycdatabase)

//普通函数
const zend_function_entry ycdatabase_functions[] =
{
    PHP_FE(ycdatabase_version, arginfo_ycdatabase_void)
    PHP_FE_END
};

//模块实体定义
zend_module_entry ycdatabase_module_entry =
{
#if ZEND_MODULE_API_NO >= 20050922
    STANDARD_MODULE_HEADER_EX,
    NULL,
    NULL,
#else
    STANDARD_MODULE_HEADER,
#endif
    "ycdatabase",
    ycdatabase_functions,
    PHP_MINIT(ycdatabase),
    PHP_MSHUTDOWN(ycdatabase),
    PHP_RINIT(ycdatabase),     //RINIT
    PHP_RSHUTDOWN(ycdatabase), //RSHUTDOWN
    PHP_MINFO(ycdatabase),
    PHP_YCDATABASE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YCDATABASE
ZEND_GET_MODULE(ycdatabase)
#endif

zend_class_entry yc_exception_ce;
zend_class_entry* yc_exception_class_entry_ptr;

// ini 配置
PHP_INI_BEGIN()
PHP_INI_END()

static void php_ycdatabase_init_globals(zend_ycdatabase_globals *ycdatabase_globals)
{
}

PHP_MINIT_FUNCTION(ycdatabase)
{
	ZEND_INIT_MODULE_GLOBALS(ycdatabase, php_ycdatabase_init_globals, NULL);
    REGISTER_INI_ENTRIES();
    
    //初始化 exception 类
    INIT_CLASS_ENTRY(yc_exception_ce, "YC_EXCEPTION", NULL);
    yc_exception_class_entry_ptr = yc_zend_register_internal_class_ex(&yc_exception_ce, zend_get_error_exception(TSRMLS_C), NULL TSRMLS_CC);
    
    //初始化 ycdb 类
    ycdb_init();
    
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ycdatabase)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(ycdatabase)
{
	DISPLAY_INI_ENTRIES();
}

PHP_RINIT_FUNCTION(ycdatabase)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(ycdatabase)
{
	return SUCCESS;
}

//Function ycdatabase_version
PHP_FUNCTION(ycdatabase_version)
{
    YC_RETURN_STRING(PHP_YCDATABASE_VERSION, 1);
}
