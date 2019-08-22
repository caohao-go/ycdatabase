[English](./README.md) | 中文

数据库 ORM 用户指南
===

## 目录
  - 介绍
  - 安装环境
  - 创建测试表
  - 在linux中编译ycdatabase
  - Start ycdatabase
  - 初始化ycdb连接
  - 原生SQL查询
  - 错误处理
  - Where 语句
  - Select 语句
  - Insert 语句
  - Replace 语句
  - Update 语句
  - Delete 语句
  - 完整的例子
  - 数据库事务
  - 数据缓存
  - PHP数据库连接池
  - Redis 连接池方案

## 介绍
  1、快速 - swoole_orm是一个为PHP扩展写的纯C语言数据库ORM扩展，可适用于MySQL/PostgreSQL等任何数据库引擎，众所周知，数据库ORM是一个非常吃性能的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将数据库的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。<br>
  2、安全 - swoole_orm返回的是sql语句和绑定参数值，能通过参数绑定的方式解决SQL注入的问题。<br>
  3、强大 - 便捷的函数，支持所有数据库操作。<br>
  4、简单 - 使用和学习非常简单，界面友好。<br>
  5、连接池 - 我们可以通过 swoole 的 channel 和 Corountine MySQL 来实现协程版 MySQL 连接池，也可用于 PostgreSQL 等其他数据库<br>
  
## 安装环境
- PHP 7.0 + 
- 需要安装PDO扩展

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

## linux环境编译ycdatabase
```
//// /path/to 是你的 PHP7 安装路径
$ cd ~/ycdatabase/ycdatabase_extension
$ /path/to/phpize
$ chmod +x ./configure
$ ./configure --with-php-config=/path/to/php-config
$ make 
$ make install
```

## 更多参考文档请到 CSDN
https://blog.csdn.net/caohao0591/article/details/84390713
