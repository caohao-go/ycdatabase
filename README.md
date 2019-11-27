English | [中文](./README-CN.md) 

ycdatabase
===

## Catalogue
  - Instruction
  - Requirement
  - Create test table
  - Compire ycdatabase in linux
  - Start ycdatabase
  - Init ycdb connection
  - Native SQL query
  - Error Info
  - Where statement
  - Select statement
  - Insert statement
  - Replace statement
  - Update statement
  - Delete statement
  - Whole Example
  - Database Transaction
  - Data Caching
  - PHP Database Connection Pool
  - Redis Connection Pool
  
## Instruction
  1、Fast : ycdb is an mysql database ORM written in c, built in php extension, as we known, database ORM is a very time-consuming operation, especially for interpretive languages such as PHP, and for a project, the proportion of ORM is very high,so here I will implement the MySQL ORM operation in C language, and use the performance of C language to improve the performance of ORM.<br>
  2、Safe : ycdb can solve SQL injection through parameter binding, and pre-binding can avoid the database compiling SQL every time, the performance is better.<br>
  3、Powerful : concise and powerful usage , support any operation in database.<br>
  4、Easy : Extremely easy to learn and use, friendly construction.<br>
  5、Data-cache : ycdb supports data caching. You can use redis as a medium to cache database data, but remember that when the update, insert, and delete operations involve caching data, you need to delete your cache to ensure data consistency.<br>
  6、Connection-pool : ycdb uses a special way to establish a stable connection pool with MySQL. performance can be increased by at least 30%, According to PHP's operating mechanism, long connections can only reside on top of the worker process after establishment, that is, how many work processes are there. How many long connections, for example, we have 10 PHP servers, each launching 1000 PHP-FPM worker processes, they connect to the same MySQL instance, then there will be a maximum of 10,000 long connections on this MySQL instance, the number is completely Out of control! And PHP's connection pool heartbeat mechanism is not perfect<br><br>
  1、快速 - ycdb是一个为PHP扩展写的纯C语言写的mysql数据库ORM扩展，众所周知，数据库ORM是一个非常耗时的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将MySQL的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。<br>
  2、安全 - ycdb能通过参数绑定的方式解决SQL注入的问题。<br>
  3、强大 - 便捷的函数，支持所有数据库操作。<br>
  4、简单 - 使用和学习非常简单，界面友好。<br>
  5、数据缓存 - ycdb支持数据缓存，你可以采用redis作为介质来缓存数据库的数据，但是记得在update、insert、delete 操作涉及到与缓存数据相关的数据修改时，需要按key删除您的缓存，以保证数据一致性。<br>
  6、连接池 - ycdb通过一种特殊的方式来建立一个稳定的与MySQL之间的连接池，性能至少能提升30%，按照 PHP 的运行机制，长连接在建立之后只能寄居在工作进程之上，也就是说有多少个工作进程，就有多少个长连接，打个比方，我们有 10 台 PHP 服务器，每台启动 1000 个 PHP-FPM 工作进程，它们连接同一个 MySQL 实例，那么此 MySQL 实例上最多将存在 10000 个长连接，数量完全失控了！而且PHP的连接池心跳机制不完善。
  
#### 中文文档(Chinese Document)： https://blog.csdn.net/caohao0591/article/details/84390713
  
## Requirement
- PHP 7.0 + 
- need support **PDO** for mysql

## Create test table
```sql
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
////  path to is your PHP install dir ////
$cd ~/ycdatabase/ycdatabase_extension
$/path/to/phpize
$chmod +x ./configure
$./configure --with-php-config=/path/to/php-config
$make
$make install
```

## Start ycdatabase
- new ycdb()
```php
$db_conf = array("host" => "127.0.0.1", 
                 "username" => "root", 
                 "password" => "test123123", 
                 "dbname" => "userinfo", 
                 "port" => '3306', 
                 "option" => array(
                        PDO::ATTR_CASE => PDO::CASE_NATURAL,
                        PDO::ATTR_TIMEOUT => 2));

$ycdb = new ycdb($db_conf);
```

  we can start by creating a ycdatabase object (ycdb) from the obove code, db_conf include host,username,password,dbname,port and option, option is a pdo attribution, you can get the detail from http://php.net/manual/en/pdo.setattribute.php, For example, PDO::ATTR_TIMEOUT in the above code is specifies the timeout duration in seconds, and PDO::ATTR_CASE is forcing column names to a specific case.<br><br>

## Init ycdb connection
- we need to init pdo connection before we use ycdatabase.
```php
try{
    $ycdb->initialize();
} catch (PDOException $e) {
    echo "find PDOException when initialize\n";
    var_dump($e);
    exit;
}
```

## Native SQL query

We can directly execute the sql statement through the exec() function,the return value is the number of rows affected by the execution, or return insert_id if it is insert statement, when the table has not AUTO_INCREMENT field, the insert_id should be zero, and execute select statement through the query() function, If $ret = -1 indicates that the sql execution error occurs, we can pass $ycdb->errorCode(), $ycdb- >errorInfo() returns the error code and error description respectively.<br><br>


- insert data
```php
$insert_id = $ycdb->exec("insert into user_info_test(username, sexuality, age, height) 
                    values('smallhow', 'male', 29, 180)");
if($insert_id == -1) {
    $code = $ycdb->errorCode();
    $info = $ycdb->errorInfo();
    echo "code:" . $code . "\n";
    echo "info:" . $info[2] . "\n";
} else {
    echo $insert_id;
}
```

- update data

 ![Image](https://github.com/caohao0730/ycdatabase/blob/master/ycdatabase_extension/image-folder/table.jpg)

if we execute the following update statement, $ret returns 3 if the current data is the above image.
```php
$ret = $ycdb->exec("update user_info_test set remark='test' where height>=180");
echo $ret; //ret is 3
```

- select data
```php
$ret = $ycdb->query("select * from user_info_test where bool_flag=1");
echo json_encode($ret);
```
 ![Image](https://github.com/caohao0730/ycdatabase/blob/master/ycdatabase_extension/image-folder/query_select.jpg)

## Error Info
Error codes and error messages can be obtained through the errorCode and errorInfo function<br>

```php
$code = $ycdb->errorCode();
$info = $ycdb->errorInfo();
```

## Where statement
- Basic usage
```php
$ycdb->select("user_info_test", "*", ["sexuality" => "male"]);
// WHERE sexuality = 'male'

$ycdb->select("user_info_test", "*", ["age" => 29]);  // WHERE age = 29

$ycdb->select("user_info_test", "*", ["age[>]" => 29]); // WHERE age > 29

$ycdb->select("user_info_test", "*", ["age[>=]" => 29]); // WHERE age >= 29

$ycdb->select("user_info_test", "*", ["age[!]" => 29]); // WHERE age != 29

$ycdb->select("user_info_test", "*", ["age[<>]" => [28, 29]]); // WHERE age  BETWEEN 28 AND 29

$ycdb->select("user_info_test", "*", ["age[><]" => [28, 29]]); // WHERE age NOT BETWEEN 28 AND 29

$ycdb->select("user_info_test", "*", ["username" => ["Tom", "Red", "carlo"]]); // WHERE username in ('Tom', 'Red', 'carlo')

//Multiple conditional query
$data = $ycdb->select("user_info_test", "*", [
    "uid[!]" => 10,
    "username[!]" => "James",
    "height[!]" => [165, 168, 172],
    "bool_flag" => true,
    "remark[!]" => null
]);
// WHERE uid != 10 AND username != "James" AND height NOT IN ( 165, 168, 172) AND bool_flag = 1 AND remark IS NOT NULL
```

- Conditional Query

You can use "AND" or "OR" to make up very complex SQL statements.
```php
$data = $ycdb->select("user_info_test", "*", [
  "OR" => [
    "uid[>]" => 3,
    "age[<>]" => [28, 29],
    "sexuality" => "female"
  ]
]);
// WHERE uid > 3 OR age BETWEEN 29 AND 29 OR sexuality = 'female'

$data = $ycdb->select("user_info_test", "*", [
  "AND" => [
    "OR" => [
      "age" => 29,
      "sexuality" => "female"
    ],
    "height" => 177
  ]
]);
// WHERE (age = 29 OR sexuality='female') AND height = 177

//Attention： Because ycdb uses array arguments, the first OR is overwritten, the following usage is wrong, 
$data = $ycdb->select("user_info_test", "*", [
  "AND" => [
    "OR" => [
      "age" => 29,
      "sexuality" => "female"
    ],
    "OR" => [
      "uid[!]" => 3,
      "height[>=]" => 170
    ],
  ]
]);
// [X] SELECT * FROM user_info_test WHERE (uid != 3 OR height >= 170)

//We can use # and comments to distinguish between two diffrents OR
$data = $ycdb->select("user_info_test", "*", [
  "AND" => [
    "OR #1" => [
      "age" => 29,
      "sexuality" => "female"
    ],
    "OR #2" => [
      "uid[!]" => 3,
      "height[>=]" => 170
    ],
  ]
]);
// [√] SELECT * FROM user_info_test WHERE (age = 29 OR sexuality = 'female') AND (uid != 3 OR height >= 170)
```
- Fuzzy Matching _Like_

LIKE USAGE [~].
```php
$data = $ycdb->select("user_info_test", "*", [ "username[~]" => "%ide%" ]);
// WHERE username LIKE '%ide%'

$data = $ycdb->select("user_info_test", "*", ["username[~]" => ["%ide%", "Jam%", "%ace"]]);
// WHERE username LIKE '%ide%' OR username LIKE 'Jam%' OR username LIKE '%ace'

$data = $ycdb->select("user_info_test", "*", [ "username[!~]" => "%ide%" ]);
// WHERE username NOT LIKE '%ide%'
```

- Use of wildcards 
```php
$ycdb->select("user_info_test", "*", [ "username[~]" => "Londo_" ]); // London, Londox, Londos...

$ycdb->select("user_info_test", "id", [ "username[~]" => "[BCR]at" ]); // Bat, Cat, Rat

$ycdb->select("user_info_test", "id", [	"username[~]" => "[!BCR]at" ]); // Eat, Fat, Hat...
```

- ORDER BY And LIMIT
```php
$data = $ycdb->select("user_info_test", "*", [
  'sexuality' => 'male',
  'ORDER' => [
    "age",
    "height" => "DESC",
    "uid" => "ASC"
  ],
  'LIMIT' => '100',  //Get the first 100 of rows (overwritten by next LIMIT)
  'LIMIT' => [20, 100]  //Started from the top 20 rows, and get the next 100
]);
//SELECT * FROM `user_info_test` WHERE `sexuality` = 'male' ORDER BY `age`, `height` DESC, `uid` ASC LIMIT 100 OFFSET 20
```

- GROUP And HAVING
```php
$ycdb->select("user_info_test", "sexuality,age,height", [
  'GROUP' => 'sexuality',
 
  // GROUP by array of values
  'GROUP' => [
    'sexuality',
    'age',
    'height'
  ],
 
  // Must have to use it with GROUP together
  'HAVING' => [
    'age[>]' => 30
  ]
]);
//SELECT uid FROM `user_info_test` GROUP BY sexuality,age,height HAVING `age` > 30
```

## Select statement
- usage

```php
select($table, $columns, $where)
select_sql($table, $columns, $where)  //just get select sql and data map from ORM, not implement query
```

#### table [string]
> table name

#### columns [string/array]
> Columns to be queried.

#### where (optional) [array]
> The conditions of the query.

```php
select($table, $join, $columns, $where)
select_sql($table, $join, $columns, $where) //just get select sql and data map from ORM, not implement query
```
#### table [string]
> table name

#### join [array]
> Multi-table query, can be ignored if not used.

#### columns [string/array]
> Columns to be queried.

#### where (optional) [array]
> The conditions of the query.

#### return: [array]
>Fail if -1 is returned, otherwise result array is returned<br>
<br>

- example

You can use * to match all fields, but if you specify columns you can improve performance.<br>
```php
$datas = $ycdb->select("user_info_test", [
  "uid",
  "username"
], [
  "age[>]" => 31
]);

// $datas = array(
//  [0] => array(
//      "uid" => 6,
//      "username" => "Aiden"
//  ),
//  [1] => array(
//      "uid" => 11,
//      "username" => "smallhow"
//  )
// )

// Select all columns
$datas = $ycdb->select("user_info_test", "*");

// Select a column
$datas = $ycdb->select("user_info_test", "username");
 
// $datas = array(
//  [0] => "lucky",
//  [1] => "Tom",
//  [2] => "Red"
// )
```
<br>

- Table join

Multi-table query SQL is more complicated, and it can be easily solved with ycdb.<br>

```php
// [>] == RIGH JOIN
// [<] == LEFT JOIN
// [<>] == FULL JOIN
// [><] == INNER JOIN

$ycdb->select("user_info_test",
[ // Table Join Info
  "[>]account" => ["uid" => "userid"], // RIGHT JOIN `account` ON `user_info_test`.`uid`= `account`.`userid`
 
  // This is a shortcut to declare the relativity if the row name are the same in both table.
  "[>]album" => "uid", //RIGHT JOIN `album` USING (`uid`) 
  
  // Like above, there are two row or more are the same in both table.
  "[<]detail" => ["uid", "age"], // LEFT JOIN `detail` USING (`uid`,`age`)
 
  // You have to assign the table with alias.
  "[<]address(addr_alias)" => ["uid" => "userid"], //LEFT JOIN `address` AS `addr_alias` ON `user_info_test`.`uid`=`addr_alias`.`userid`
 
  // You can refer the previous joined table by adding the table name before the column.
  "[<>]album" => ["account.userid" => "userid"], //FULL JOIN `album` ON  `account`.`userid` = `album`.`userid`
 
  // Multiple condition
  "[><]account" => [
    "uid" => "userid",
    "album.userid" => "userid"
  ]
], [ // columns
  "user_info_test.uid",
  "user_info_test.age",
  "addr_alias.country",
  "addr_alias.city"
], [ // where condition
  "user_info_test.uid[>]" => 3,
  "ORDER" => ["user_info_test.uid" => "DESC"],
  "LIMIT" => '50'
]);


// SELECT 
//   user_info_test.uid,
//   user_info_test.age,
//   addr_alias.country,
//   addr_alias.city 
// FROM `user_info_test` 
// RIGHT JOIN `account` ON `user_info_test`.`uid`= `account`.`userid`  
// RIGHT JOIN `album` USING (`uid`) 
// LEFT JOIN `detail` USING (`uid`,`age`) 
// LEFT JOIN `address` AS `addr_alias` ON `user_info_test`.`uid`=`addr_alias`.`userid` 
// FULL JOIN `album` ON  `account`.`userid` = `album`.`userid` 
// INNER JOIN `account` ON `user_info_test`.`uid`= `account`.`userid` 
//   AND `album`.`userid` = `account`.`userid`  
// WHERE `user_info_test`.`uid` > 3 
// ORDER BY  `user_info_test`.`uid` DESC 
// LIMIT 50
```

- alias

You can use aliases to prevent field conflicts<br>

```php
$data = $ycdb->select("user_info_test(uinfo)", [
  "[<]account(A)" => "userid",
], [
  "uinfo.uid(uid)",
  "A.userid"
]);

// SELECT uinfo.uid AS `uid`, A.userid 
// FROM `user_info_test` AS `uinfo` 
// LEFT JOIN `account` AS `A` USING (`userid`)
```

## Insert statement

```php
insert($table, $data, $cache_info)
insert_sql($table, $data)  //just get insert sql from ORM, not implement query
```
#### table [string]
> table name

#### data [array]
> insert data

#### cache_info (optional) [array]
> cache info

#### return [int]
>Fail if -1 is returned, otherwise insert_id is returned, if the table 
