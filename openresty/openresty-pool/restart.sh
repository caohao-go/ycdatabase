pid=`ps -ef | grep nginx | grep -v grep | awk '{print $2}'`
echo $pid
kill -9 $pid
rm -rf /tmp/redis_pool.sock
rm -rf /tmp/mysql_pool.sock
/usr/local/openresty.1.13/nginx/sbin/nginx -p ~/openresty-pool
