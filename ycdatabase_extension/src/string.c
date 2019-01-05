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
#include <ext/pcre/php_pcre.h>
#include <regex.h>


//字符串复制
#if defined(YC_USE_JEMALLOC) || defined(YC_USE_TCMALLOC)
yc_inline char* yc_strdup(const char *s)
{
    size_t l = strlen(s) + 1;
    char *p = yc_malloc(l);
    memcpy(p, s, l);
    return p;
}
yc_inline char* yc_strndup(const char *s, size_t n)
{
    char *p = yc_malloc(n + 1);
    strncpy(p, s, n);
    p[n] = '\0';
    return p;
}
#endif

//preg_match函数
zval* yc_preg_match(const char* regex_str, char* subject_str)
{
	//return NULL;
	zval retval;
	zval* matches;
	
	if(regex_str == NULL || subject_str == NULL) {
		return NULL;
	}
	
	pcre_cache_entry *pce;
	zend_string *regex = zend_string_init(regex_str, strlen(regex_str), 0);
	pce = pcre_get_compiled_regex_cache(regex);
	zend_string_free(regex);
	
	if (pce == NULL) {
		return NULL;
	}
	
	YC_ALLOC_INIT_ZVAL(matches);
	ZVAL_NULL(matches);
	
	//执行正则
	php_pcre_match_impl(pce, subject_str, strlen(subject_str), &retval, matches, 0, 0, Z_L(0), Z_L(0));

	if(Z_TYPE(retval) == IS_FALSE) {
		return NULL;
	} else {
		return matches;
	}
}

//找到 needle 在 haystack 中的位置， 找到则 >= 0，否则为 -1
int yc_strpos(const char* haystack,const char* needle)    
{
	int ignorecase = 0;
    register unsigned char c, needc;  
    unsigned char const *from, *end;  
    int len = strlen(haystack);  
    int needlen = strlen(needle);
    from = (unsigned char *)haystack;  
    end = (unsigned char *)haystack + len;  
    const char *findreset = needle;  
    
    int i = 0;
    
    while (from < end) {
        c = *from++;  
        needc = *needle;  
        if (ignorecase) {
            if (c >= 65 && c < 97)  
                c += 32;  
            if (needc >= 65 && needc < 97)  
                needc += 32;  
        }  
        if(c == needc) {
            ++needle;  
            if(*needle == '\0') {  
                if (len == needlen)   
                    return 0;  
                else  
                    return i - needlen+1;  
            }  
        }
        else {
            if(*needle == '\0' && needlen > 0)  
                return i - needlen +1;  
            needle = findreset;    
        }
    		i++;
    }    
    return  -1;    
}

//去除尾部空格
char *rtrim(char *str)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str  && (isspace(*p) || (*p) == '\n' || (*p) == '\r' || (*p) == '\t')) {
		*p = '\0';
		--p;
	}
	return str;
}
 
//去除首部空格
char *ltrim(char *str)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = 0;
	char *p = str;
	while (*p != '\0' && (isspace(*p) || (*p) == '\n' || (*p) == '\r' || (*p) == '\t')) {
		++p;
		++len;
	}
 
	memmove(str, p, strlen(str) - len + 1);
 
	return str;
}

//去除首尾空格
char *trim(char *str)
{
	str = rtrim(str);
	str = ltrim(str);
	return str;
}

//去除尾部字符串
char* rtrim_str(char *str, char *remove)
{
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	int r_len = strlen(remove);
	
	if(r_len > len) {
		return str;
	}
	
	char *end = str + len - 1;
	char *r_end = remove + r_len - 1;
	
	int remove_flag = 1;
	
	while (end >= str && r_end >= remove) {
		if((*r_end) == (*end)) {
			--r_end;
			--end;
		} else {
			remove_flag = 0;
			break;
		}
	}
	
	if (remove_flag){
		char *end = str + len - 1;
		char *r_end = remove + r_len - 1;
		while (end >= str && r_end >= remove) {
			if((*r_end) == (*end)) {
				*end = '\0';
				--r_end;
				--end;
			} else {
				break;
			}
		}
	}
	
	return str;
}

//去除头部字符串
char *ltrim_str(char *str, char *remove){
	if (str == NULL || *str == '\0') {
		return str;
	}
 
	int len = strlen(str);
	int r_len = strlen(remove);
	
	if(r_len > len) {
		return str;
	}
	
	char *end = str + len - 1;
	char *r_end = remove + r_len - 1;
	
	char *start = str;
	char *r_start = remove;
	
	int remove_flag = 1;
	while (start <= end && r_start <= r_end) {
		if((*start) == (*r_start)) {
			++r_start;
			++start;
		} else {
			remove_flag = 0;
			break;
		}
	}
	
	if(remove_flag) {
		memmove(str, start, len - r_len);
		str[len - r_len] = '\0';
	}
	
	return str;
}

char* yc_itoa(long num, char* str) {
	int radix = 10; //十进制
	memset(str, 0, MAP_ITOA_INT_SIZE);
    char index[]="0123456789ABCDEF";
    unsigned unum;
    int i=0,j,k;
    if (radix==10&&num<0) {
        unum=(unsigned)-num;
        str[i++]='-';
    } else unum=(unsigned)num;
    do {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    } while (unum);
    str[i]='\0';
    if (str[0]=='-')k=1;
    else k=0;
    char temp;
    for (j=k;j<=(i-1)/2;j++) {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

char* strreplace(char* original, char const * const pattern, char const * const replacement)
{
    size_t const replen = strlen(replacement);
    size_t const patlen = strlen(pattern);
    size_t const orilen = strlen(original);

    size_t patcnt = 0;
    const char * oriptr;
    const char * patloc;

    for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen) {
        patcnt++;
    }
    // allocate memory for the new string
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) emalloc( sizeof(char) * (retlen + 1) );

    if (returned != NULL) {
        // copy the original string,
        // replacing all the instances of the pattern
        char * retptr = returned;
        for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen) {
            size_t const skplen = patloc - oriptr;
            // copy the section until the occurence of the pattern
            strncpy(retptr, oriptr, skplen);
            retptr += skplen;
            // copy the replacement
            strncpy(retptr, replacement, replen);
            retptr += replen;
        }
        // copy the rest of the string.
        strcpy(retptr, oriptr);
    }

    size_t val_len = strlen(returned);
    strcpy(original, returned);
    efree(returned);
    return original;
}

/**
 * Serializes php variables without using the PHP userland
 */
void yc_serialize(zval *return_value, zval *var TSRMLS_DC) 
{
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	PHP_VAR_SERIALIZE_INIT(var_hash);
	yc_php_var_serialize(&buf, var, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (EG(exception)) {
		smart_str_free(&buf);
		RETURN_FALSE;
	}
    
	if (buf.s) {
		RETURN_STR(buf.s);
	} else {
		RETURN_NULL();
	}
}

/**
 * Unserializes php variables without using the PHP userland
 */
void yc_unserialize(zval *return_value, zval *var TSRMLS_DC) {

	const unsigned char *p;
	php_unserialize_data_t var_hash;

	if (Z_TYPE_P(var) != IS_STRING) {
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(var) == 0) {
		RETURN_FALSE;
	}
	
	p = (const unsigned char*) Z_STRVAL_P(var);
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (!yc_php_var_unserialize(&return_value, &p, p + Z_STRLEN_P(var), &var_hash TSRMLS_CC)) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		zval_dtor(return_value);
		ZVAL_NULL(return_value);
		if (!EG(exception)) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Error at offset %ld of %d bytes", (long)((char*)p - Z_STRVAL_P(var)), Z_STRLEN_P(var));
		}
		RETURN_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}