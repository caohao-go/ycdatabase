# ycdatabase
  

## Instruction
  ycdb is an mysql database orm written in c, built in php extension, as we known, database ORM is a very time-consuming operation, especially for interpretive languages such as PHP, and for a project, the proportion of ORM is very high,so here I will implement the MySQL ORM operation in C language, and use the performance of C language to improve the performance of ORM, and ycdabase can solve SQL injection through parameter binding <br><br>
  _ycdb是一个为PHP扩展写的纯C语言写的mysql数据库ORM扩展，众所周知，数据库ORM是一个非常耗时的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将MySQL的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。同时，ycdabase能通过参数绑定的方式解决SQL注入的问题_

## Requirement
- PHP 7.0 + 
- need support **PDO** for mysql

## create a test table
```
CREATE TABLE `user_info_test` (
  `uid` int(11) NOT NULL COMMENT 'userid' AUTO_INCREMENT,
  `username` varchar(64) NOT NULL COMMENT 'username',
  `sexuality` varchar(8) DEFAULT 'male' COMMENT 'sexuality：male - 男性  female - 女性',
  `age` int(11) DEFAULT 0 COMMENT 'age',
  `height` double(11,2) DEFAULT 0 COMMENT 'height of a person, 身高',
  `bool_flag` int(11) DEFAULT 1 COMMENT 'flag',
  `remark` varchar(11) DEFAULT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='userinfo';
```

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

## Init pdo connection
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

We can directly execute the sql statement through the exec() function,the return value is the number of rows affected by the execution, and execute select statement through the query() function, If $ret = -1 indicates that the sql execution error occurs, we can pass $ycdb->errorCode(), $ycdb- >errorInfo() returns the error code and error description respectively.<br><br>
_我们可以通过exec函数直接执行sql语句，返回值为执行结果影响行数，以及query函数执行select语句，如果 $ret = -1 则说明 sql 执行出错，我们可以通过 $ycdb->errorCode,$ycdb->errorInfo() 分别返回错误代码、错误描述。_


- insert data
```
$ret = $ycdb->exec("insert into user_info_test(username, sexuality, age, height) 
                    values('smallhow', 'male', 29, 180)");
if($ret == -1) {
    $code = $ycdb->errorCode();
    $info = $ycdb->errorInfo();
    echo "code:" . $code . "\n";
    echo "info:" . $info[2] . "\n";
} else {
    echo $ret;
}
```

- update data

 ![Image](https://github.com/caohao0730/ycdatabase/blob/master/image-folder/table.jpg)

if we execute the following update statement, $ret returns 3 if the current data is the above image.
```
$ret = $ycdb->exec("update user_info_test set remark='test' where height>=180");
echo $ret; //ret is 3
```

- select data
```
$ret = $ycdb->query("select * from user_info_test where bool_flag=1");
echo json_encode($ret);
```
 ![Image](https://github.com/caohao0730/ycdatabase/blob/master/image-folder/query_select.jpg)
 

## Where statement
- basic usage
```
$ycdb->select("user_info_test", "username", ["sexuality" => "male"]);
// WHERE sexuality = 'male'

$ycdb->select("user_info_test", "username", ["age" => 29]);
// WHERE age = 29

$ycdb->select("user_info_test", "username", ["age[>]" => 29]);
// WHERE age > 29

$ycdb->select("user_info_test", "username", ["age[>=]" => 29]);
// WHERE age >= 29

$ycdb->select("user_info_test", "username", ["age[!]" => 29]);
// WHERE age != 29

$ycdb->select("user_info_test", "username", ["age[<>]" => [28, 29]]);
// WHERE age  BETWEEN 28 AND 29

$ycdb->select("user_info_test", "username", ["age[><]" => [28, 29]]);
// WHERE age NOT BETWEEN 28 AND 29

//you can use array
$data = $ycdb->select("user_info_test", "*", [
    "OR" =>[
        "uid" => [2, 3, 4, 7, 9],
        "username" => ["Tom", "Red", "carlo"]]
    ]);
// WHERE uid in (2, 3, 4, 7, 9) OR username in ('Tom', 'Red', 'carlo')

//Multiple conditional query
$data = $ycdb->select("user_info_test", "*", [
    "uid[!]" => 10,
    "username[!]" => "James",
    "height[!]" => [165, 168, 172],
    "bool_flag" => true,
    "remark[!]" => null
  ]);
// WHERE 
// uid != 10 AND 
// username != "James" AND 
// height NOT IN ( 165, 168, 172) AND 
// bool_flag = 1 AND 
// remark IS NOT NULL
```
