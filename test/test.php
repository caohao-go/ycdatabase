<?php

ini_set('display_errors', 'On');
error_reporting(E_ALL);

include_once("ycdb/YCDB_Driver.php");

$db = YCDB_Driver::instance('dabaojian');


$ret = $db->query("update user_info_test set remark='test2' where height>=180");
echo $ret . "\n";

$ret = $db->query("select * from user_info_test where bool_flag=1");
echo json_encode($ret) . "\n";


$data = array('username' => 'smallhow','sexuality' => 'male','age' => 35, 'height' => '168');
$insert_id = $db->insert("user_info_test", $data);
echo $insert_id . "\n";


$ret = $db->get('user_info_test', ['sexuality' => 'male']);
echo json_encode($ret) . "\n";

$ret = $db->get_one('user_info_test', ['uid' => 123]);
echo json_encode($ret) . "\n";

$ret = $db->replace('user_info_test', $data);
echo $ret . "\n";

$data = array('height' => 182,'age' => 33);
$where = array('username' => 'smallhow');
$ret = $db->update("user_info_test", $data, $where);

$where = array('username' => 'smallhow');
$ret = $db->delete("user_info_test", $where);