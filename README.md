# ycdatabase

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
  - Update statement
  - Delete statement
  - Whole Example
  - Database Transaction
  - Data Caching
  - MySQL Database Pool
  
## Instruction
  1、ycdb is an mysql database orm written in c, built in php extension, as we known, database ORM is a very time-consuming operation, especially for interpretive languages such as PHP, and for a project, the proportion of ORM is very high,so here I will implement the MySQL ORM operation in C language, and use the performance of C language to improve the performance of ORM.<br>
  2、ycdb can solve SQL injection through parameter binding. <br>
  3、ycdb supports data caching. You can use redis as a medium to cache database data, but remember that when the update, insert, and delete operations involve caching data, you need to delete your cache to ensure data consistency.<br>
  4、ycdb uses a special way to establish a stable connection pool with MySQL. performance can be increased by at least 30%, According to PHP's operating mechanism, long connections can only reside on top of the worker process after establishment, that is, how many work processes are there. How many long connections, for example, we have 10 PHP servers, each launching 1000 PHP-FPM worker processes, they connect to the same MySQL instance, then there will be a maximum of 10,000 long connections on this MySQL instance, the number is completely Out of control! And PHP's connection pool heartbeat mechanism is not perfect<br><br>
  1、ycdb是一个为PHP扩展写的纯C语言写的mysql数据库ORM扩展，众所周知，数据库ORM是一个非常耗时的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将MySQL的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。<br>
  2、ycdb能通过参数绑定的方式解决SQL注入的问题。<br>
  3、ycdb支持数据缓存，你可以采用redis作为介质来缓存数据库的数据，但是记得在update、insert、delete 操作涉及到与缓存数据相关的数据修改时，需要按key删除您的缓存，以保证数据一致性。<br>
  4、ycdb通过一种特殊的方式来建立一个稳定的与MySQL之间的连接池，性能至少能提升30%，按照 PHP 的运行机制，长连接在建立之后只能寄居在工作进程之上，也就是说有多少个工作进程，就有多少个长连接，打个比方，我们有 10 台 PHP 服务器，每台启动 1000 个 PHP-FPM 工作进程，它们连接同一个 MySQL 实例，那么此 MySQL 实例上最多将存在 10000 个长连接，数量完全失控了！而且PHP的连接池心跳机制不完善，
  
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
$cd ~/ycdatabase/ycdatabase_extension
$/path/to/phpize
$./configure --with-php-config=/path/to/php-config
$make && make install
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
  _我们通过上面代码创建ycdatabase对象(ycdb)，db_conf是数据库配置，包含host,username,password,dbname,port等信息，还包含option参数，这个参数是pdo的设置参数，具体您可以参考网站 http://php.net/manual/zh/pdo.setattribute.php  , 例如上面代码中的PDO::ATTR_TIMEOUT是连接超时时间(秒)，PDO::ATTR_CASE是强制列名为指定的大小写。_

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

We can directly execute the sql statement through the exec() function,the return value is the number of rows affected by the execution, and execute select statement through the query() function, If $ret = -1 indicates that the sql execution error occurs, we can pass $ycdb->errorCode(), $ycdb- >errorInfo() returns the error code and error description respectively.<br><br>
_我们可以通过exec函数直接执行sql语句，返回值为执行结果影响行数，以及query函数执行select语句，如果 $ret = -1 则说明 sql 执行出错，我们可以通过 $ycdb->errorCode,$ycdb->errorInfo() 分别返回错误代码、错误描述。_


- insert data
```php
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

```php
 $ret = $ycdb->query("select username from user_info_test where bool_flag=1");
echo json_encode($ret);
```
![Image](https://raw.githubusercontent.com/caohao0730/ycdatabase/master/ycdatabase_extension/image-folder/query_single.png)

## Error Info

Error codes and error messages can be obtained through the errorCode and errorInfo function<br>
_可以通过 errorCode() 和errorInfo() 函数获取错误码和错误信息_

```php
$code = $ycdb->errorCode();
$info = $ycdb->errorInfo();
```

## Where statement
- Basic usage _基本用法_
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

- Conditional Query _条件搜索_ 

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
//注意： 第一个 OR 被覆盖了，所以这个写法是错误的
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
//我们可以用 # + 注释 来区分两个不同的 OR
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
- Fuzzy Matching _模糊匹配 Like_

LIKE USAGE [~].
```php
$data = $ycdb->select("user_info_test", "*", [ "username[~]" => "%ide%" ]);
// WHERE username LIKE '%ide%'

$data = $ycdb->select("user_info_test", "*", ["username[~]" => ["%ide%", "Jam%", "%ace"]]);
// WHERE username LIKE '%ide%' OR username LIKE 'Jam%' OR username LIKE '%ace'

$data = $ycdb->select("user_info_test", "*", [ "username[!~]" => "%ide%" ]);
// WHERE username NOT LIKE '%ide%'
```

- Use of wildcards _通配符的使用_
```php
$ycdb->select("user_info_test", "*", [ "username[~]" => "Londo_" ]); // London, Londox, Londos...

$ycdb->select("user_info_test", "id", [ "username[~]" => "[BCR]at" ]); // Bat, Cat, Rat

$ycdb->select("user_info_test", "id", [	"username[~]" => "[!BCR]at" ]); // Eat, Fat, Hat...
```

- ORDER BY And LIMIT _排序和limit_
```php
$data = $ycdb->select("user_info_test", "*", [
  'sexuality' => 'male',
  'ORDER' => [
    "age",
    "height" => "DESC",
    "uid" => "ASC"
  ],
  'LIMIT' => 100,  //Get the first 100 of rows (overwritten by next LIMIT)
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
```

#### table [string]
> table name

#### columns [string/array]
> Columns to be queried.

#### where (optional) [array]
> The conditions of the query.

```php
select($table, $join, $columns, $where)
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
<br>

- example

You can use * to match all fields, but if you specify columns you can improve performance.<br>
_你可以使用*来匹配所有字段, 但如果你指名字段名可以很好的提高性能._
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
_多表查询SQL较为复杂，使用ycdb可以轻松的解决它_

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
  "LIMIT" => 50
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
_你可以使用别名，以防止字段冲突_

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
```
#### table [string]
> table name

#### data [array]
> insert data

#### cache_info (optional) [array]
> cache info

#### return [int]
>Fail if -1 is returned, otherwise the number of inserted records is returned<br>
>_如果返回 -1 则失败，否则返回插入记录数_
 
```php
$data = array('username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$ret = $ycdb->insert("user_info_test", $data);
if($ret == -1) {
  $code = $ycdb->errorCode();
  $info = $ycdb->errorInfo();
  echo "code:" . $code . "\n";
  echo "info:" . $info[2] . "\n";
}

//If you want to get the ID of the last inserted row, you need to call the insert_id() method separately.
$uid = $ycdb->insert_id();
```

## Update statement

```php
update($table, $data, $where)
```
#### table [string]
> table name

#### data [array]
> update data

#### where (optional) [array]
> where condition [可选]

#### return [int]
>Fail if -1 is returned, otherwise the number of update records is returned<br>
>_如果返回 -1 则失败，否则返回更新记录数_

```php
$data = array('height' => 182,'age' => 33);
$where = array('username' => 'smallhow');
$ret = $ycdb->update("user_info_test", $data, $where);
```

## Delete statement

```php
delete($table, $where)
```
#### table [string]
> table name

#### where (optional) [array]
> where condition [可选]

#### return [int]
>Fail if -1 is returned, otherwise the number of delete records is returned<br>
>_如果返回 -1 则失败，否则返回删除记录数_

```php
$where = array('username' => 'smallhow');
$ret = $ycdb->delete("user_info_test", $where);
```

## Whole Example

```php
/************** the complete example **********************
SELECT  `name` AS `a`, `avatar` AS `b`, `age` 
FROM `table_a` AS `a` 
RIGHT JOIN `AAAA` AS `a1` USING (`id`)  
LEFT JOIN `BBBB` USING (`E1`,`E2`,`E3`)  
RIGHT JOIN `CCCC` AS `c1` ON `a`.`GG`=`c1`.`HH` AND `II`.`KK` =`c1`.`LL`  
WHERE `user`.`email` NOT IN ('foo@bar.com', 'cat@dog.com', 'admin@ycdb.in') AND 
`user`.`uid` < 11111 AND `uid` >= 222 AND 
`uid` IS NOT NULL AND 
`count` NOT IN (36, 57, 89) AND 
`id` != 1 AND `int_num` != 3 AND `double_num` != 3.76 AND 
`AA` LIKE '%saa%' AND `BB` NOT LIKE '%sbb' AND 
(`CC` LIKE '11%' OR `CC` LIKE '22_' OR `CC` LIKE '33%') AND 
(`DD` NOT LIKE '%44%' OR `DD` NOT LIKE '55%' OR `DD` NOT LIKE '66%') AND 
(`EE` LIKE '%E11' AND `EE` LIKE 'E22') AND 
(`FF` LIKE '%F33' OR `FF` LIKE 'F44') AND 
(`GG` NOT LIKE '%G55' AND `GG` NOT LIKE 'G66') AND 
(`HH` NOT LIKE 'H77' OR `HH` NOT LIKE 'H88') AND 
(`II` BETWEEN 1 AND 12) AND NOT 
(`LL` BETWEEN 1 AND 12)  AND (
  (`user_name` IS NULL OR `email` = 'foo@bar.com') AND 
  (`user_name` = 'bar' OR `email` = 'bar@foo.com')
) AND (`user_name` != 'foo' OR `promoted` != 1) 
GROUP BY type,age,gender 
HAVING `uid`.`num` > 111 AND `type` > 'smart' AND 
	`id` != 0 AND `god3` != 9.86 AND `uid` IS NOT NULL 
	AND `AA` LIKE 'SSA%' AND (`CC` LIKE '11%' OR `CC` LIKE '22%' OR `CC` LIKE '33%')
ORDER BY  `user`.`score` , `user`.`uid` ASC, `time` DESC 
LIMIT 33
*/

$table = "table_a(a)";

$join = [
  "[>]AAAA(a1)" => "id",
  "[<]BBBB" => ["E1", "E2", "E3"],
  "[>]CCCC(c1)" => [ "GG" => "HH", "II.KK" => "LL"]
];

$columns = ["name(a)", "avatar(b)", "age"];

$where =  [
  "user.email[!]" => ["foo@bar.com", "cat@dog.com", "admin@ycdb.in"],
  "user.uid[<]" => 11111,
  "uid[>=]" => 222,
  "uid[!]" => null,
  "count[!]" => [36, 57, 89],
  "id[!]" => true,
  "int_num[!]" => 3,
  "double_num[!]" => 3.76,
  "AA[~]" => "%saa%",
  "BB[!~]" => "%sbb",
  "CC[~]" => ["11%", "22_", "33%"],
  "DD[!~]" => ["%44%", "55%", "66%"],
  "EE[~]" => ["AND" => ["%E11", "E22"]],
  "FF[~]" => ["OR" => ["%F33", "F44"]],
  "GG[!~]" => ["AND" => ["%G55", "G66"]],
  "HH[!~]" => ["OR" => ["H77", "H88"]],
  "II[<>]" => ["1", "12"],
  "LL[><]" => ["1", "12"],
    "AND #1" => [
        "OR #1" => [
          "user_name" => null,
          "email" => "foo@bar.com",
        ],
        "OR #2" => [
          "user_name" => "bar",
          "email" => "bar@foo.com"
        ]
    ],
    "OR" => [
        "user_name[!]" => "foo",
        "promoted[!]" => true
    ],
    'GROUP' => 'userid',
    'GROUP' => ['type', 'age', 'gender'],
    'HAVING' => [
        "uid.num[>]" => 111,
        "type[>]" => "smart",
        "id[!]" => false,
        "god3[!]" => 9.86,
        "uid[!]" => null,
        "AA[~]" => "SSA%",
        "CC[~]" => ["11%", "22%", "%33"],
    ],
    'ORDER' => [
        "user.score",
        "user.uid" => "ASC",
        "time" => "DESC",
    ],
    "LIMIT" => 33,
];

$ycdb->select($table, $join, $columns, $where);
```

## Database transaction

```php
$ycdb->begin();

$ret1 = $ycdb->exec("insert into user_info_test(username, sexuality, age, height) values('smallhow', 'male', 29, 180)");
$ret2 = $ycdb->exec("insert into user_info_test(username, sexuality, age, height) values('jesson', 'female', 28, 175)");

if($ret1 == -1 || $ret2 == -1 ) {
  $ycdb->rollback();
} else {
  $ycdb->commit()
}
```

## Data Caching

We can use redis, or any other cache system that supports set/get/del/expire function as the medium to store the data returned by the database. If you do not specify the expiration time, the default storage expiration time is 5 minutes. if The cache is specified. When we call data update function such as update/delete/insert, we should pass in the same cache key so that ycdb can clear the cache to ensure data consistency.<br><br>

_我们可以以 redis,或者其他任何支持 set/get/del/expire 这4种方法的缓存系统作为介质，存储 database 返回的数据，如果不指定到期时间，默认存储到期时间为5分钟，当我们指定了缓存，如果对数据有update/delete/insert等更新操作，我们最好是传入相同的缓存key，以便ycdb能够清理缓存来保持数据的一致性_
```php
//we want cache data by redis
$redis = new Redis();
$redis->connect('/home/redis/pid/redis.sock');

$option = array("host" => "127.0.0.1", 
                "username" => "test", 
                "password" => "test", 
                "dbname" => "test", 
                "port" => '3306',
                "cache" => $redis,  //cache instance
                'option' => array(
                    PDO::ATTR_CASE => PDO::CASE_NATURAL,
                    PDO::ATTR_TIMEOUT => 2));


$ycdb = new ycdb($option);

try{
  $ycdb->initialize();
} catch (PDOException $e) {
  echo "find PDOException when initialize\n";
  exit;
}

// I want to keep the 29-year-old user data queried by the database in the cache, and keep it for 10 minutes.
$age = 29;
$cache_key = 'pre_cache_key_' . $age;

$data = $ycdb->select("user_info_test", "*", [
	'age' => $age,
	'CACHE' => ['key' => $cache_key, 'expire' => 600]   //cache key an expire time (seconds)
]);

echo $redis->get($cache_key) . "\n";

// If I update these 29-year-old user data, or even add a new 29-year-old user information, 
// it's best to enter the cache key to clean up the cache to keep the data consistent.
$ycdb->update("user_info_test", ['remark' => '29-year-old'], [
  'age' => $age,
  'CACHE' => ['key' => $cache_key]  //cache key
]);

echo $redis->get($cache_key) . "\n";

//If you are going to delete the relevant data, it is best to also clean up the cache by cache_key.
$ycdb->delete("user_info_test", [
  'age' => $age,
  'CACHE' => ['key' => $cache_key]  //cache key
]);

echo $redis->get($cache_key) . "\n";

//Clean up the cache by cache_key when the data you insert is related to the cached data.
$insert_data = array();
$insert_data['username'] = 'test';
$insert_data['sexuality'] = 'male';
$insert_data['age'] = 29;
$insert_data['height'] = 176;

$data = $ycdb->insert("user_info_test", $insert_data, ['key' => $cache_key]);

echo $redis->get($cache_key) . "\n";
```

## MySQL Database Pool

Short connection performance is generally not available. CPU resources are consumed by the system. Once the network is jittered, there will be a large number of TIME_WAIT generated. The service has to be restarted periodically or the machine is restarted periodically. The server is unstable, QPS is high and low, and the connection is stable and efficient. The pool can effectively solve the above problems, it is the basis of high concurrency. ycdb uses a special way to establish a stable connection pool with MySQL. performance can be increased by at least 30%, According to PHP's operating mechanism, long connections can only reside on top of the worker process after establishment, that is, how many work processes are there. How many long connections, for example, we have 10 PHP servers, each launching 1000 PHP-FPM worker processes, they connect to the same MySQL instance, then there will be a maximum of 10,000 long connections on this MySQL instance, the number is completely Out of control! And PHP's connection pool heartbeat mechanism is not perfect<br><br>
_短连接性能普遍上不去，CPU 大量资源被系统消耗，网络一旦抖动，会有大量 TIME_WAIT 产生，不得不定期重启服务或定期重启机器，服务器工作不稳定，QPS 忽高忽低，稳定高效的连接池可以有效的解决上述问题，它是高并发的基础，ycdb通过一种特殊的方式来建立一个稳定的与MySQL之间的连接池，性能至少提升30%，按照 PHP 的运行机制，长连接在建立之后只能寄居在工作进程之上，也就是说有多少个工作进程，就有多少个长连接，打个比方，我们有 10 台 PHP 服务器，每台启动 1000 个 PHP-FPM 工作进程，它们连接同一个 MySQL 实例，那么此 MySQL 实例上最多将存在 10000 个长连接，数量完全失控了！而且PHP的连接池心跳机制不完善_

### How ?
Let's focus on Nginx, its stream module implements load balancing of TCP/UDP services, and with the stream-lua module, we can implement programmable stream services, that is, custom TCP/N with Nginx. UDP service! Of course, you can write TCP/UDP services from scratch, but standing on Nginx's shoulder is a more time-saving and labor-saving choice. We can choose the OpenResty library to complete the MySQL connection pool function. OpenResty is a very powerful and well-functioning Nginx Lua framework. It encapsulates Socket, MySQL, Redis, Memcache, etc. But what is the relationship between Nginx and PHP connection pool? And listen to me slowly: Usually most PHP is used with Nginx, and PHP and Nginx are mostly on the same server. With this objective condition, we can use Nginx to implement a connection pool, connect to services such as MySQL on Nginx, and then connect to Nginx through a local Unix Domain Socket, thus avoiding all kinds of short links. Disadvantages, but also enjoy the benefits of the connection pool.

_我们不妨绕着走。让我们把目光聚焦到 Nginx 的身上，其 stream 模块实现了 TCP/UDP 服务的负载均衡，同时借助 stream-lua 模块，我们就可以实现可编程的 stream 服务，也就是用 Nginx 实现自定义的 TCP/UDP 服务！当然你可以自己从头写 TCP/UDP 服务，不过站在 Nginx 肩膀上无疑是更省时省力的选择。我们可以选择 OpenResty 库来完成MySQL的连接池功能，OpenResty是一个非常强大，而且功能完善的Nginx Lua框架，他封装了Socket、MySQL, Redis, Memcache 等操作，可是 Nginx 和 PHP 连接池有什么关系？且听我慢慢道来：通常大部分 PHP 是搭配 Nginx 来使用的，而且 PHP 和 Nginx 多半是在同一台服务器上。有了这个客观条件，我们就可以利用 Nginx 来实现一个连接池，在 Nginx 上完成连接 MySQL 等服务的工作，然后 PHP 通过本地的 Unix Domain Socket 来连接 Nginx，如此一来既规避了短链接的种种弊端，也享受到了连接池带来的种种好处。_


### OpenResty Install
OpenResty Document: https://moonbingbing.gitbooks.io/openresty-best-practices/content/openresty/install_on_centos.html

CentOS 6.8 Install :
```
###### Install the necessary libraries ######
$yum install readline-devel pcre-devel openssl-devel perl

###### Install OpenResty ######
$cd ~/ycdatabase/openresty
$tar -xzvf openresty-1.13.6.1.tar.gz
$cd openresty-1.13.6.1
$./configure --prefix=/usr/local/openresty.1.13 --with-luajit --without-http_redis2_module --with-http_iconv_module
$gmake 
$gmake install

###### open mysql pool ######
$cp -rf ~/ycdatabase/openresty/openresty-pool ~/
$/usr/local/openresty.1.13/nginx/sbin/nginx -p ~/openresty-pool
```

### MySQL Database Pool Config
~/openresty-pool/conf/nginx.conf  :

```lua
worker_processes  1;        #nginx worker 数量

error_log logs/error.log;   #指定错误日志文件路径

events {
    worker_connections 1024;
}

stream {
  lua_code_cache on;

  lua_check_client_abort on;

# server {
# listen unix:/tmp/redis_pool.sock;

# content_by_lua_block {
#     local redis_pool = require "redis_pool"
			
#     pool = redis_pool:new({ip = "127.0.0.1", port = 6379})

#     pool:run()
#   }
# }
	
  server {
    listen unix:/tmp/mysql_pool.sock;
		
    content_by_lua_block {
      local mysql_pool = require "mysql_pool"
			
      local config = {host = "127.0.0.1", 
                      user = "root", 
                      password = "test", 
                      database = "collect", 
                      timeout = 2000, 
                      max_idle_timeout = 10000, 
                      pool_size = 200}
						   
      pool = mysql_pool:new(config)
			
      pool:run()
    }
  }
}
```

If you have more than a MySQL Server, you can start another server and add a new listener to unix domain socket.<br>
_你可以另起一个 server ， 并在listen unix 之后新增一个unix domain socket监听。_

### PHP Code
Except the option is array("unix_socket" => "/tmp/mysql_pool.sock") , Php mysql connection pool usage is exactly the same as before,But, MySQL does not support transactions in unix domain socket mode.<br>
_除了option 配置为 array("unix_socket" => "/tmp/mysql_pool.sock") 之外，php的mysql连接池使用方法和之前一模一样，另外， unix_socket 方式的 MySQL 不支持事务。_

```php
$option = array("unix_socket" => "/tmp/mysql_pool.sock");
$ycdb = new ycdb($option);
$ret = $ycdb->select("user_info_test", "*", ["sexuality" => "male"]);

if($ret == -1) {
  $code = $ycdb->errorCode();
  $info = $ycdb->errorInfo();
  echo "code:" . $code . "\n";
  echo "info:" . $info[2] . "\n";
} else {
  print_r($ret);
}
```

### MySQL Pool Lua
```lua
local mysql = require "resty.mysql"
local cjson = require "cjson"
 
local assert = assert
local setmetatable = setmetatable
local tonumber = tonumber
 
-- 解析请求
local function parse_request(sock)
    --获取 sql 语句
    local sql_size, err = sock:receive()
    
    if not sql_size then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
    end
    
    local size = tonumber(sql_size)
    if size <= 0 then
		return nil, "SQL size is zero"
	end
	
    local sql_str, err = sock:receive(size)
    if not sql_str then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
	end
	
	--获取 map
	local map_size, err = sock:receive()
		
    if not map_size then
        if err == "timeout" then
            sock:close()
        end
        return nil, err
    end
    
    size = tonumber(map_size);
    if size <= 0 then
    	-- 没有 map
		return sql_str
	end
	
    local map_res, err = sock:receive(map_size)
    if not map_res then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
	end
	
	-- 解析 map ，创建 SQL 语句，防止SQL注入
	local maps = cjson.decode(map_res)
	
	for k, v in pairs(maps) do
		if v == true then
			v = 1
		elseif v == false then
			v = 0
		end
		
		sql_str = ngx.re.gsub(sql_str, k, ngx.quote_sql_str(v))
	end
	
    return sql_str
end
 
-- 返回请求
local function response_success(sock, result)
	local ret = {
		errno = 0,
		data = result
	}
	
	local send_str = cjson.encode(ret)
	
	local ret, err = sock:send(string.len(send_str) ..  "\n" .. send_str)
	
	if not ret then
		ngx.log(ngx.ERR, "response success failed : [", err, "], send_str=[", send_str, "]")
		return nil, err
	end
end
 
-- 返回请求
local function response_error(sock, errno, errmsg, sqlstate)
	local ret = {
		errno = errno,
		errorCode = sqlstate,
		errorInfo = {sqlstate, errno, errmsg}
	}
	
	local send_str = cjson.encode(ret)
	local ret, err = sock:send(string.len(send_str) ..  "\n" .. send_str)
	
	if not ret then
		ngx.log(ngx.ERR, "response error failed : [", err, "], send_str=[", send_str, "]")
		return nil, err
	end
end
 
-- 关闭数据库
local function close_db(db)
	if not db then
		return
	end
	db:close()
end
 
-- 异常退出
local function exit(err)
    ngx.log(ngx.ERR, "ERROR EXIT: [", err, "]")
    return ngx.exit(ngx.ERROR)
end
 
----------------------------------------
local _M = {}
_M._VERSION = "1.0"
 
function _M.new(self, config)
    local t = {
        _host = config.host,
        _port = config.port or 3306,
        _user = config.user,
        _password = config.password,
        _database = config.database,
        _timeout = config.timeout or 2000,  -- default 2 sec
        _pool_size = config.pool_size or 100,
        _max_idle_timeout = config.max_idle_timeout or 10000
    }
    
    return setmetatable(t, { __index = _M })
end
 
function _M.run(self)
	local downstream_sock = assert(ngx.req.socket(true))
	
	local query_sql, err = parse_request(downstream_sock)
	
	if not query_sql then
		return exit("parse_request failed : " .. err)
	end
        
	--数据库连接
	local db, err = mysql:new()
	    
	db:set_timeout(self._timeout)
	
	local ok, err, errno, sqlstate = db:connect{
		host = self._host,
		port = self._port,
		database = self._database,
		user = self._user,
		password = self._password,
		max_packet_size = 1024 * 1024}
	
	if not ok then
		response_error(downstream_sock, -1, err, "E0001")
		return exit("connect mysql error : " .. err)
	end
	
	local result, err, errno, sqlstate = db:query(query_sql)
	
	-- 返回结果
	if result then
		response_success(downstream_sock, result)
	else
		ngx.log(ngx.ERR, "query failed: [", errno, "][", sqlstate, "][",err , "]")
		response_error(downstream_sock, errno, err, sqlstate)
	end
	
	-- 设置 mysql 连接池
	local ok, err = db:set_keepalive(self._max_idle_timeout, self._pool_size)
	if not ok then
		ngx.log(ngx.ERR, "set_keepalive failed: [", err, "]")
	end
end
 
return _M

```
