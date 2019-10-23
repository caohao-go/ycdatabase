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
  1、快速 - ycdb是一个为PHP扩展写的纯C语言写的mysql数据库ORM扩展，众所周知，数据库ORM是一个非常耗时的操作，尤其对于解释性语言如PHP，而且对于一个项目来说，ORM大多数情况能占到项目很大的一个比例，所以这里我将MySQL的ORM操作用C语言实现，利用C语言的性能，提升ORM的性能。<br>
  2、安全 - ycdb 能通过参数绑定的方式解决SQL注入的问题，而且预绑定能够避免数据库每次都编译SQL，性能更好。<br>
  3、强大 - 便捷的调用方式，强大的功能支持<br>
  4、简单 - 使用和学习非常简单，交互友好。<br>
  5、数据缓存 - ycdb支持数据缓存，你可以采用redis作为介质来缓存数据库的数据，但是记得在update、insert、delete 操作涉及到与缓存数据相关的数据修改时，需要删除您的缓存，以保证数据一致性。<br>
  6、连接池 - ycdb通过一种特殊的方式来建立一个稳定的与MySQL之间的连接池，性能至少能提升30%，按照 PHP 的运行机制，长连接在建立之后只能寄居在工作进程之上，也就是说有多少个工作进程，就有多少个长连接，打个比方，我们有 10 台 PHP 服务器，每台启动 1000 个 PHP-FPM 工作进程，它们连接同一个 MySQL 实例，那么此 MySQL 实例上最多将存在 10000 个长连接，数量完全失控了！而且PHP的连接池心跳机制不完善<br>
  
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
