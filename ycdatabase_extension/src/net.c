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

php_stream* unix_socket_conn(char *servername)
{
    char host[1024] = {0};
    int host_len, err = 0;
    
#if (PHP_MAJOR_VERSION < 7)
    char *estr = NULL;
#else
    zend_string *estr = NULL;
#endif
    host_len = snprintf(host, sizeof(host), "unix://%s", servername);
    
    //超时时间
    struct timeval tv, read_tv;
    tv.tv_sec  = (time_t) 1;  //连接超时
    tv.tv_usec = (int) 0;
    
    read_tv.tv_sec  = (time_t) 5; //读取超时
    read_tv.tv_usec = (int) 0;
    
    php_stream *stream = php_stream_xport_create(host, host_len, STREAM_OPEN_PERSISTENT | STREAM_LOCATE_WRAPPERS_ONLY, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT_ASYNC, NULL, &tv, NULL, &estr, &err);
    
    if (!stream) {
    	return NULL;
    }
    
    php_stream_auto_cleanup(stream);
    php_stream_set_option(stream, PHP_STREAM_OPTION_READ_TIMEOUT, 0, &read_tv);
    php_stream_set_option(stream, PHP_STREAM_OPTION_WRITE_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
    php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER, PHP_STREAM_BUFFER_NONE, NULL);
    
    return stream;
}
