<?php
$option = array("host" => "47.106.114.151", 
				"username" => "root", 
				"password" => "gaoqu123123", 
				"dbname" => "dabaojian", 
				"port" => '3306', 
				'option' => array(
					PDO::ATTR_CASE => PDO::CASE_NATURAL,
                	PDO::ATTR_TIMEOUT => 2));

$ycdb = new ycdb($option);


try{
	$ycdb->initialize();
} catch (PDOException $e) {
	echo "find PDOException when initialize\n";
	var_dump($e);
	exit;
}

$ret = $ycdb->exec("insert into user_info_test(username, sexuality, age, height) 
                    values('smallhow', 'male', 29, 180)");
if($ret == -1) {
    $code = $ycdb->errorCode();
    $info = $ycdb->errorInfo();
    echo "code:" . $code . "\n";
    echo "info:" . $info[2] . "\n";
}

$ret = $ycdb->exec("update user_info_test set remark='test' where height>=180");
if($ret == -1) {
	$code = $ycdb->errorCode();
	$info = $ycdb->errorInfo();
	echo "code:" . $code . "\n";
	echo "info:" . $info[2] . "\n";
} else {
	echo $ret;
}

$ret = $ycdb->query("select * from user_info_test where bool_flag=1");

$ret = $ycdb->select("user_info_test", "username", ["sexuality" => "male"]);
// WHERE sexuality = 'male'
if($ret == -1) {
	$code = $ycdb->errorCode();
	$info = $ycdb->errorInfo();
	echo "code:" . $code . "\n";
	echo "info:" . $info[2] . "\n";
}

$ret = $ycdb->select("user_info_test", "username", ["age" => 29]);
// WHERE age = 29

$ret = $ycdb->select("user_info_test", "username", ["age[>]" => 29]);
// WHERE age > 29

$ret = $ycdb->select("user_info_test", "username", ["age[>=]" => 29]);
// WHERE age >= 29

$ret = $ycdb->select("user_info_test", "username", ["age[!]" => 29]);
// WHERE age != 29

$ret = $ycdb->select("user_info_test", "username", ["age[<>]" => [28, 29]]);
// WHERE age  BETWEEN 28 AND 29

$ret = $ycdb->select("user_info_test", "username", ["age[><]" => [28, 29]]);
// WHERE age NOT BETWEEN 28 AND 29


//you can use array
$ret = $ycdb->select("user_info_test", "*", [
  "OR" =>[
    "uid" => [2, 3, 4, 7, 9],
    "username" => ["Tom", "Red", "carlo"]]
]);

$ret = $ycdb->select("user_info_test", "*", [
    "uid[!]" => 10,
    "username[!]" => "James",
    "height[!]" => [165, 168, 172],
    "bool_flag" => true,
    "remark[!]" => null
]);

$data = $ycdb->select("user_info_test", "*", [
  "AND" => [
    "uid[>]" => 3,
    "age[<>]" => [28, 29],
    "sexuality" => "female"
  ]
]);
// WHERE uid > 3 AND age BETWEEN 29 AND 29 AND sexuality = 'female'

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


$data = $ycdb->select("user_info_test", "*", [
  "username[~]" => "%ide%"
]);
// WHERE username LIKE '%ide%'

$data = $ycdb->select("user_info_test", "*", [
  "username[~]" => ["%ide%", "Jam%", "%ace"]
]);
// WHERE username LIKE '%ide%' OR username LIKE 'Jam%' OR username LIKE '%ace'

$data = $ycdb->select("user_info_test", "*", [
  "username[!~]" => "%ide%"
]);
// WHERE username NOT LIKE '%ide%'


$ycdb->select("user_info_test", "*", [
  "username[~]" => "Londo_"  // London, Londox, Londos...
]);

$ycdb->select("user_info_test", "id", [
	"username[~]" => "[BCR]at" // Bat, Cat, Rat
]);
 
$ycdb->select("user_info_test", "id", [
	"username[~]" => "[!BCR]at" // Eat, Fat, Hat...
]);


$data = array('username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$ret = $ycdb->insert("user_info_test", $data);
if($ret == -1) {
	$code = $ycdb->errorCode();
	$info = $ycdb->errorInfo();
	echo "code:" . $code . "\n";
	echo "info:" . $info[2] . "\n";
}

$ret = $ycdb->insert_id();

$data = array('height' => 182,'age' => 33);
$where = array('username' => 'smallhow');
$ret = $ycdb->update("user_info_test", $data, $where);

$where = array('uid' => 14);
$ret = $ycdb->delete("user_info_test", $where);
if($ret == -1) {
	$code = $ycdb->errorCode();
	$info = $ycdb->errorInfo();
	echo "code:" . $code . "\n";
	echo "info:" . $info[2] . "\n";
} else {
	echo $ret;
}


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

$columns = ["name(a)", "avatar(b)", "age"];

$table = "table_a(a)";

$join = [
	"[>]AAAA(a1)" => "id",
	"[<]BBBB" => ["E1", "E2", "E3"],
	"[>]CCCC(c1)" => [ "GG" => "HH", "II.KK" => "LL"]
];

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

