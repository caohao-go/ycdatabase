#include "ycdatabase.h"
#include <stdarg.h>

char* yc_multi_memcpy(char* source, int n_value, ...) {
	va_list var_arg;
	int count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(source + strlen(source), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	return source;
}

static yc_inline size_t get_string_emalloc_size(char* source) {
	size_t source_size = 0;
	memcpy(&source_size, source - 4, sizeof(size_t));
	return source_size;
}

char* yc_multi_memcpy_auto_realloc(char** source, int n_value, ...) {
	int source_size = get_string_emalloc_size(*source);
	
	//����Ŀ���ַ�������
	va_list var_arg;
	int count = 0;
	int dest_len = strlen(*source) + 1;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		dest_len += strlen(tmp);
		count++;
	}
	va_end(var_arg);
	
	//need realloc
	char* dest = NULL;
	if(source_size < MM_REAL_SIZE(dest_len)) {
		yc_string_emalloc_32(&dest, dest_len);
		memcpy(dest, *source, strlen(*source));
		yc_string_efree_32(*source);
		*source = dest;
	} else {
		dest = *source;
	}
	
	//���¸�ֵ
	count=0;
	va_start(var_arg, n_value);
	while(count < n_value) {
		char *tmp = va_arg(var_arg, char*);
		memcpy(dest + strlen(dest), tmp, strlen(tmp));
		count++;
	}
	va_end(var_arg);
	
	//php_printf("====yc_multi_memcpy_auto_alloc : source_size=[%d], dest_size=[%d], dest_size=[%d], \ndest_point=[%x]  dest=[%s]\n", source_size, dest_len, MM_REAL_SIZE(dest_len), dest, dest);
	return dest;
}
