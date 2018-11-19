# ycdatabase
  

## Instruction
  ycdb is an mysql database orm written in c, built in php extension, as we known, database ORM is a very time-consuming operation, especially for interpretive languages such as PHP, and for a project, the proportion of ORM is very high,so here I will implement the MySQL ORM operation in C language, and use the performance of C language to improve the performance of ORM, and ycdabase can solve SQL injection through parameter binding <br><br>
  _ycdb是一个纯C语言写的mysql数据库ORM扩展，众所周知，数据库ORM是一个非常耗时的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将MySQL的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。同时，ycdabase能通过参数绑定的方式解决SQL注入的问题_

## Requirement
- PHP 7.0 + 
- need support **PDO** for mysql

## Compire ycdatabase in linux
```
$/path/to/phpize
$./configure --with-php-config=/path/to/php-config
$make && make install
```

## Start 
- new ycdb()
```
$db_conf = array("host" => "127.0.0.1", 
                 "username" => "root", 
                 "password" => "test123123", 
                 "dbname" => "userinfo", 
                 "port" => '3306', 
                 "option" => array(
                        PDO::ATTR_CASE => PDO::CASE_NATURAL,
                        PDO::ATTR_TIMEOUT => 2));

$ycdb = new ycdb($options);
```

  we can start by creating a ycdatabase object (ycdb) from the obove code, db_conf include host,username,password,dbname,port and option, option is a pdo attribution, you can get the detail from http://php.net/manual/en/pdo.setattribute.php, For example, PDO::ATTR_TIMEOUT in the above code is specifies the timeout duration in seconds, and PDO::ATTR_CASE is forcing column names to a specific case.<br><br>
  _我们通过上面代码创建ycdatabase对象(ycdb)，db_conf是数据库配置，包含host,username,password,dbname,port等信息，还包含option参数，这个参数是pdo的设置参数，具体您可以参考网站 http://php.net/manual/zh/pdo.setattribute.php  , 例如上面代码中的PDO::ATTR_TIMEOUT是连接超时时间(秒)，PDO::ATTR_CASE是强制列名为指定的大小写。_

## init pdo connection
- we need to init pdo connection before we use ycdatabase.
```
try{
	$ycdb->initialize();
} catch (PDOException $e) {
	echo "find PDOException when initialize\n";
	var_dump($e);
	exit;
}
```

## Native SQL query
