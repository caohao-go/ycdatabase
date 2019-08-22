[English](./README.md) | 中文

数据库 ORM 用户指南
===

## 目录
  - 介绍
  - 安装环境
  - linux环境编译 swoole_orm
  - 创建测试表
  - 启动协程 MySQL/PostgreSQL
  - 原生SQL查询
  - 错误信息
  - Where 语句
  - Select 语句
  - Insert 语句
  - Replace 语句
  - Update 语句
  - Delete 语句
  - 完整的例子
  - 协程版MySQL连接池

## 介绍
  1、快速 - swoole_orm是一个为PHP扩展写的纯C语言数据库ORM扩展，可适用于MySQL/PostgreSQL等任何数据库引擎，众所周知，数据库ORM是一个非常吃性能的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将数据库的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。<br>
  2、安全 - swoole_orm返回的是sql语句和绑定参数值，能通过参数绑定的方式解决SQL注入的问题。<br>
  3、强大 - 便捷的函数，支持所有数据库操作。<br>
  4、简单 - 使用和学习非常简单，界面友好。<br>
  5、连接池 - 我们可以通过 swoole 的 channel 和 Corountine MySQL 来实现协程版 MySQL 连接池，也可用于 PostgreSQL 等其他数据库<br>
  
## 安装环境
- PHP 7.0 + 

## linux环境编译 swoole_orm
```
## path/to 是你的PHP安装目录, 目标库: swoole_orm.so

$cd ext-orm
$/path/to/phpize
$chmod +x ./configure
$./configure --with-php-config=/path/to/php-config
$make
$make install 
```

## 创建测试表
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

## 启动协程 MySQL
- new Swoole\Coroutine\MySQL();

```php
$mysql = new Swoole\Coroutine\MySQL();
$options = array();
$options['host'] = '127.0.0.1';
$options['port'] = 3306;
$options['user'] = 'root';
$options['password'] = 'hao123123';
$options['database'] = 'user';
$ret = $mysql->connect($options);
```

也可用于 PostgreSQL 等其他数据库。

## 原生SQL查询
- insert data
```php
$ret = $mysql->query("insert into user_info_test(username, sexuality, age, height) 
			values('smallhow', 'male', 29, 180)");

if ($ret === false) {
  echo $mysql->errno . "\n";
  echo $mysql->error . "\n";
} else {
  echo $mysql->insert_id . "\n";
}
```

- update data

```php
$ret = $mysql->query("update user_info_test set remark='test' where height>=180");

if ($ret === false) {
  echo $mysql->errno . "\n";
  echo $mysql->error . "\n";
}
```

- select data
```php
$ret = $mysql->query("select * from user_info_test where bool_flag=1");
		
if ($ret === false) {
  echo $mysql->errno . "\n";
  echo $mysql->error . "\n";
} else {
  var_dump($ret);
}
```

## 错误信息

错误码和错误信息可以从 Swoole\Coroutine\MySQL 对象的成员变量 errno 和 error 中获取<br>

```php
$code = $mysql->errno;
$info = $mysql->error;
```

## Where 语句
- 基础用法
```php
//swoole_orm::select 的返回是一个数组，包含 sql 和参数绑定值。采用参数绑定方式执行SQL更安全。避免被注入
//sql_stat['sql'] : SELECT * FROM `user_info_test` WHERE `sexuality` = ?
//sql_stat['bind_value'] : array("male")

$sql_stat = swoole_orm::select("user_info_test", "*", ["sexuality" => "male"]);
if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  $ret = $stmt->execute($sql_stat['bind_value']);
}


swoole_orm::select("user_info_test", "*", ["age" => 29]);  // WHERE age = 29

swoole_orm::select("user_info_test", "*", ["age[>]" => 29]); // WHERE age > 29

swoole_orm::select("user_info_test", "*", ["age[>=]" => 29]); // WHERE age >= 29

swoole_orm::select("user_info_test", "*", ["age[!]" => 29]); // WHERE age != 29

swoole_orm::select("user_info_test", "*", ["age[<>]" => [28, 29]]); // WHERE age  BETWEEN 28 AND 29

swoole_orm::select("user_info_test", "*", ["age[><]" => [28, 29]]); // WHERE age NOT BETWEEN 28 AND 29

swoole_orm::select("user_info_test", "*", ["username" => ["Tom", "Red", "carlo"]]); // WHERE username in ('Tom', 'Red', 'carlo')

//多条件查询
swoole_orm::select("user_info_test", "*", [
    "uid[!]" => 10,
    "username[!]" => "James",
    "height[!]" => [165, 168, 172],
    "bool_flag" => true,
    "remark[!]" => null
]);
// WHERE uid != 10 AND username != "James" AND height NOT IN ( 165, 168, 172) AND bool_flag = 1 AND remark IS NOT NULL
```

- 组合查询

你可以用 "AND" 或者 "OR" 来组合更复杂的语句。
```php
swoole_orm::select("user_info_test", "*", [
  "OR" => [
    "uid[>]" => 3,
    "age[<>]" => [28, 29],
    "sexuality" => "female"
  ]
]);
// WHERE uid > 3 OR age BETWEEN 29 AND 29 OR sexuality = 'female'

swoole_orm::select("user_info_test", "*", [
  "AND" => [
    "OR" => [
      "age" => 29,
      "sexuality" => "female"
    ],
    "height" => 177
  ]
]);
// WHERE (age = 29 OR sexuality='female') AND height = 177

//注意：由于mysql使用数组参数，所以在下面的情况下，第一个 OR 会被覆盖。
swoole_orm::select("user_info_test", "*", [
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

//我们可以使用 "#" 加注释的方式去区分两个 "OR"
swoole_orm::select("user_info_test", "*", [
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
- 模糊匹配 _Like_

LIKE USAGE [~].
```php
swoole_orm::select("user_info_test", "*", [ "username[~]" => "%ide%" ]);
// WHERE username LIKE '%ide%'

swoole_orm::select("user_info_test", "*", ["username[~]" => ["%ide%", "Jam%", "%ace"]]);
// WHERE username LIKE '%ide%' OR username LIKE 'Jam%' OR username LIKE '%ace'

swoole_orm::select("user_info_test", "*", [ "username[!~]" => "%ide%" ]);
// WHERE username NOT LIKE '%ide%'
```

- Use of wildcards 
```php
swoole_orm::select("user_info_test", "*", [ "username[~]" => "Londo_" ]); // London, Londox, Londos...

swoole_orm::select("user_info_test", "id", [ "username[~]" => "[BCR]at" ]); // Bat, Cat, Rat

swoole_orm::select("user_info_test", "id", [	"username[~]" => "[!BCR]at" ]); // Eat, Fat, Hat...
```

- ORDER BY And LIMIT
```php
swoole_orm::select("user_info_test", "*", [
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
swoole_orm::select("user_info_test", "sexuality,age,height", [
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

## Select 语句
- usage

```php
select($table, $columns, $where) 
```

#### table [string]
> 表名

#### columns [string/array]
> 需要查询的列

#### where (optional) [array]
> 查询条件

```php
select($table, $join, $columns, $where) 
```
#### table [string]
> 表名

#### join [array]
> 多表关联join查询，不使用可以忽略

#### columns [string/array]
> 需要查询的列

#### where (optional) [array]
> 查询条件

#### return: [array]
> 失败返回false，否则返回一个数组，包含查询语句 sql 和绑定参数 bind_value.
<br>

- example

你可以使用 * 来查询所有列，不过指定列可以提升性能。<br>
```php
$sql_stat = swoole_orm::select("user_info_test", ["uid", "username"], ["age[>]" => 31]);

var_dump($sql_stat);

if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  $ret = $stmt->execute($sql_stat['bind_value']);
}

// $ret = array(
//  [0] => array(
//      "uid" => 6,
//      "username" => "Aiden"
//  ),
//  [1] => array(
//      "uid" => 11,
//      "username" => "smallhow"
//  )
// )

// 查询所有列
swoole_orm::select("user_info_test", "*");
```
<br>

- Table join

多表查询更复杂，但是他可以被swoole_orm轻易实现。<br>

```php
// [>] == RIGH JOIN
// [<] == LEFT JOIN
// [<>] == FULL JOIN
// [><] == INNER JOIN

swoole_orm::select("user_info_test",
[ // 表join。
  "[>]account" => ["uid" => "userid"], // RIGHT JOIN `account` ON `user_info_test`.`uid`= `account`.`userid`
 
  // 如果两个参与join的表列名相同，则使用下面快捷表达方式。
  "[>]album" => "uid", //RIGHT JOIN `album` USING (`uid`) 
  
  // 如上，两个表中多个列名相同的情况。
  "[<]detail" => ["uid", "age"], // LEFT JOIN `detail` USING (`uid`,`age`)
 
  // 为表分配别名。
  "[<]address(addr_alias)" => ["uid" => "userid"], //LEFT JOIN `address` AS `addr_alias` ON `user_info_test`.`uid`=`addr_alias`.`userid`
 
  // 可以在列之前加上表名来引用之前的联结表。
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

使用别名来防止冲突。<br>

```php
$data = swoole_orm::select("user_info_test(uinfo)", [
  "[<]account(A)" => "userid",
], [
  "uinfo.uid(uid)",
  "A.userid"
]);

// SELECT uinfo.uid AS `uid`, A.userid 
// FROM `user_info_test` AS `uinfo` 
// LEFT JOIN `account` AS `A` USING (`userid`)
```

## Insert 语句

```php
insert($table, $data)
```
#### table [string]
> 表名

#### data [array]
> 要插入的数据

#### return [int]
> 失败返回false，否则返回一个数组，包含查询语句 sql和绑定参数bind_value。
 
```php
$data = array('username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$sql_stat = swoole_orm::insert("user_info_test", $data);
		
if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  if($stmt === false) {
    echo $mysql->errno . "\n";
    echo $mysql->error . "\n";
  } else {
    $ret = $stmt->execute($sql_stat['bind_value']);
    echo $mysql->insert_id . "\n";
  }
}

```

## Replace 语句

```php
replace($table, $data)
```
#### table [string]
> 表名

#### data [array]
> 需要替换的数据

#### return [int]
> 失败返回false，否则返回一个数组，包含查询语句 sql和绑定参数bind_value。
 
```php
$data = array('uid' => 35, 'username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$sql_stat = swoole_orm::replace("user_info_test", $data);

if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  if($stmt === false) {
    echo $mysql->errno . "\n";
    echo $mysql->error . "\n";
  } else {
    $ret = $stmt->execute($sql_stat['bind_value']);
  }
}

```

## Update 语句

```php
update($table, $data, $where)
```
#### table [string]
> 表名

#### data [array]
> 需要更新的数据

#### where (optional) [array]
> where条件 [可选]

#### return [int]
> 失败返回false，否则返回一个数组，包含查询语句 sql 和绑定参数 bind_value.<br>

```php
$data = array('height' => 185,'age' => 32);
$where = array('username' => 'smallhow');
$sql_stat = swoole_orm::update("user_info_test", $data, $where);

if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  if($stmt === false) {
    echo $mysql->errno . "\n";
    echo $mysql->error . "\n";
  } else {
    $ret = $stmt->execute($sql_stat['bind_value']);
  }
}
```

## Delete 语句

```php
delete($table, $where)
```
#### table [string]
> 表名

#### where (optional) [array]
> where 条件 [可选]

#### return [int]
> 失败返回false，否则返回一个数组，包含查询语句 sql 和绑定参数 bind_value.<br>

```php
$where = array('username' => 'smallhow');
$sql_stat = swoole_orm::delete("user_info_test", $where);

if ($sql_stat !== false) {
  $stmt = $mysql->prepare($sql_stat['sql']);
  if($stmt === false) {
    echo $mysql->errno . "\n";
    echo $mysql->error . "\n";
  } else {
    $ret = $stmt->execute($sql_stat['bind_value']);
  }
}
```

## 完整例子

```php
$table = "table_a(a)";

$join = [
  "[>]AAAA(a1)" => "id",
  "[<]BBBB" => ["E1", "E2", "E3"],
  "[>]CCCC(c1)" => [ "GG" => "HH", "II.KK" => "LL"]
];

$columns = ["name(a)", "avatar(b)", "age"];

$where =  [
  "user.email[!]" => ["foo@bar.com", "cat@dog.com", "admin@mysql.in"],
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

$sql_stat = swoole_orm::select($table, $join, $columns, $where);
var_dump($sql_stat);
```

## 协程版MySQL连接池
本 ORM 适用于所有关系型数据库，这里仅以MySQL为例。
```php
//usage.php
include("DBConfig.php");
include("MySQLPool.php");

$table = 'user_info_test';
$join = array("[>]account" => ["uid" => "userid"]);
$columns = "uid,username";
$where = ["uid[<]" => 10, "age" => 31];

$ret = MySQLPool::instance('collect')->query("select * from $table where bool_flag=1");

$ret = MySQLPool::instance('collect')->select("user_info_test", $where, $columns, $join);

$ret = MySQLPool::instance('collect')->select_row($table, $where, $columns);

$data = array('username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$insert_id = MySQLPool::instance('collect')->insert($table, $data);

$data['uid'] = 12;
$ret = MySQLPool::instance('collect')->replace($table, $data);

$update_data = array('height' => 186,'age' => 29);
$where = array('username' => 'smallhow');
$ret = MySQLPool::instance('collect')->update($table, $update_data, $where);

$ret = MySQLPool::instance('collect')->delete($table, $where);
```

```php
//DBConfig.php
$util_db_config['default']['host']     = '127.0.0.1';
$util_db_config['default']['username'] = 'root';
$util_db_config['default']['password'] = 'hao123123';
$util_db_config['default']['dbname']   = 'caihongqiu';
$util_db_config['default']['char_set'] = 'utf8';
$util_db_config['default']['dbcollat'] = 'utf8_general_ci';
$util_db_config['default']['pool_size'] = 10;

$util_db_config['collect']['host']     = '127.0.0.1';
$util_db_config['collect']['username'] = 'root';
$util_db_config['collect']['password'] = 'hao123123';
$util_db_config['collect']['dbname']   = 'shine_light';
$util_db_config['collect']['char_set'] = 'utf8';
$util_db_config['collect']['dbcollat'] = 'utf8_general_ci';
$util_db_config['collect']['pool_size'] = 15;
```

```php
class MySQLPool {
    const POOL_SIZE = 10;

    protected $pool;
    static private $instances;

    var $host = '';
    var $username = '';
    var $password = '';
    var $dbname = '';
    var $port = 3306;
    var $char_set = 'utf8';
    var $dbcollat = 'utf8_general_ci';

    static public function instance($params) {
        if (!isset(self::$instances[$params])) {

            $params = empty($params) ? 'default' : $params;

            global $util_db_config;

            if (! isset($util_db_config[$params])) {
                throw new RuntimeException("You have specified an invalid database connection group.");
            }

            $config = $util_db_config[$params];

            $pool_size = isset($config['pool_size']) ? intval($config['pool_size']) : MySQLPool::POOL_SIZE;
            $pool_size = $pool_size <= 0 ? MySQLPool::POOL_SIZE : $pool_size;

            self::$instances[$params] = new MySQLPool($config, $pool_size);
        }

        return self::$instances[$params];
    }

    /**
     * MySQLPool constructor.
     * @param int $size 连接池的尺寸
     */
    function __construct($params, $size) {
        foreach ($params as $key => $val) {
            $this->$key = $val;
        }

        $this->pool = new Swoole\Coroutine\Channel($size);

        for ($i = 0; $i < $size; $i++) {
            $mysql = new Swoole\Coroutine\MySQL();

            $ret = $this->connect($mysql);

            if ($ret) {
                $this->pool->push($mysql);
                $this->query("SET NAMES '".$this->char_set."' COLLATE '".$this->dbcollat."'");
            } else {
                throw new RuntimeException("MySQL connect error host={$this->host}, port={$this->port}, user={$this->username}, database={$this->dbname}, errno=[" . $mysql->errno . "], error=[" . $mysql->error . "]");
            }
        }
    }

    function insert($table = '', $data = NULL) {
        if (empty($table) || empty($data) || !is_array($data)) {
            throw new RuntimeException("insert_table_or_data_must_be_set");
        }

        $sql_stat = swoole_orm::insert($table, $data);
        if ($sql_stat === false) {
            throw new RuntimeException("insert_sql error [$table][".json_encode($data)."]");
        }

        $ret = $this->query($sql_stat['sql'], $sql_stat['bind_value'], $mysql);
        if (!empty($ret)) {
            return $mysql->insert_id;
        } else {
            return intval($ret);
        }
    }

    function replace($table = '', $data = NULL) {
        if (empty($table) || empty($data) || !is_array($data)) {
            throw new RuntimeException("replace_table_or_data_must_be_set");
        }

        $sql_stat = swoole_orm::replace($table, $data);
        if ($sql_stat === false) {
            throw new RuntimeException("replace_sql error [$table][".json_encode($data)."]");
        }

        $ret = $this->query($sql_stat['sql'], $sql_stat['bind_value']);
        return $ret;
    }

    function update($table = '', $data = NULL, $where = NULL) {
        if (empty($table) || empty($data) || !is_array($data)) {
            throw new RuntimeException("update_table_or_data_must_be_set");
        }

        $sql_stat = swoole_orm::update($table, $data, $where);
        if ($sql_stat === false) {
            throw new RuntimeException("update_sql error [$table][".json_encode($data)."][".json_encode($where)."]");
        }

        $ret = $this->query($sql_stat['sql'], $sql_stat['bind_value']);
        return $ret;
    }

    function delete($table = '', $where = NULL) {
        if (empty($table)) {
            throw new RuntimeException("delete_table_must_be_set");
        }

        $sql_stat = swoole_orm::delete($table, $where);
        if ($sql_stat === false) {
            throw new RuntimeException("replace_sql error [$table][".json_encode($where)."]");
        }

        $ret = $this->query($sql_stat['sql'], $sql_stat['bind_value']);
        return $ret;
    }

    function select($table = '', $where = array(), $columns = "*", $join = null) {
        if (empty($table)) {
            throw new RuntimeException("select_table_or_columns_must_be_set");
        }

        if (empty($join)) {
            $sql_stat = swoole_orm::select($table, $columns, $where);
        } else {
            $sql_stat = swoole_orm::select($table, $join, $columns, $where);
        }

        if ($sql_stat === false) {
            throw new RuntimeException("select_sql error [$table][".json_encode($where)."][".json_encode($columns)."]");
        }

        $ret = $this->query($sql_stat['sql'], $sql_stat['bind_value']);
        return $ret;
    }

    function select_row($table = '', $where = array(), $columns = "*") {
        $where['LIMIT'] = 1;
        $ret = $this->select($table, $where, $columns);
        if (empty($ret) || !is_array($ret)) {
            return array();
        }

        return $ret[0];
    }

    private function connect(& $mysql, $reconn = false) {
        if ($reconn) {
            $mysql->close();
        }

        $options = array();
        $options['host'] = $this->host;
        $options['port'] = intval($this->port) == 0 ? 3306 : intval($this->port);
        $options['user'] = $this->username;
        $options['password'] = $this->password;
        $options['database'] = $this->dbname;
        $ret = $mysql->connect($options);
        return $ret;
    }

    private function real_query(& $mysql, & $sql, & $map) {
        if (empty($map)) {
            return $mysql->query($sql);
        } else {
            $stmt = $mysql->prepare($sql);

            if ($stmt == false) {
                return false;
            } else {
                return $stmt->execute($map);
            }
        }
    }

    function query($sql, $map = null, & $mysql = null) {
        if (empty($sql)) {
            throw new RuntimeException("input_empty_query_sql");
        }

        try {
            $mysql = $this->pool->pop();
            $ret = $this->real_query($mysql, $sql, $map);

            if ($ret === false) {
                echo "MySQL QUERY FAIL [".$mysql->errno."][".$mysql->error."], sql=[{$sql}], map=[".json_encode($map)."]";

                if ($mysql->errno == 2006 || $mysql->errno == 2013) {
                    //重连MySQL
                    $ret = $this->connect($mysql, true);
                    if ($ret) {
                        $ret = $this->real_query($mysql, $sql, $map);
                    } else {
                        throw new RuntimeException("reconnect fail: [" . $mysql->errno . "][" . $mysql->error . "], host={$this->host}, port={$this->port}, user={$this->username}, database={$this->dbname}");
                    }
                }
            }

            if ($ret === false) {
                throw new RuntimeException($mysql->errno . "|" . $mysql->error);
            }

            $this->pool->push($mysql);
            return $ret;
        } catch (Exception $e) {
            $this->pool->push($mysql);
            throw new RuntimeException("MySQL catch exception [".$e->getMessage()."], sql=[{$sql}], map=".json_encode($map));
        }
    }
}
```
