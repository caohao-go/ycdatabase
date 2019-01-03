<?php
/*
| -------------------------------------------------------------------
| DATABASE CONNECTIVITY SETTINGS
| -------------------------------------------------------------------
| This file will contain the settings needed to access your database.
|
| For complete instructions please consult the 'Database Connection'
| page of the User Guide.
|
| -------------------------------------------------------------------
| EXPLANATION OF VARIABLES
| -------------------------------------------------------------------
|
|	['host'] The hostname of your database server.
|	['username'] The username used to connect to the database
|	['password'] The password used to connect to the database
|	['dbname'] The name of the database you want to connect to
|	['pconnect'] TRUE/FALSE - Whether to use a persistent connection
|	['db_debug'] TRUE/FALSE - Whether database errors should be displayed.
|	['char_set'] The character set used in communicating with the database
|	['dbcollat'] The character collation used in communicating with the database
|	['autoinit'] Whether or not to automatically initialize the database.
|							- good for ensuring strict SQL while developing
|
| The $active_group variable lets you choose which connection group to
| make active.  By default there is only one group (the 'default' group).
|
| The $active_record variables lets you determine whether or not to load
| the active record class
*/

$db['default']['host']     = '127.0.0.1';
$db['default']['username'] = 'root';
$db['default']['password'] = 'test';
$db['default']['dbname']   = 'user';
$db['default']['pconnect'] = TRUE;
$db['default']['db_debug'] = TRUE;
$db['default']['char_set'] = 'utf8';
$db['default']['dbcollat'] = 'utf8_general_ci';
$db['default']['autoinit'] = TRUE;
$db['default']['port'] = 3306;
$db['default']['log_path'] = '/data/app/logs';

$db['dabaojian']['unix_sock'] = '/tmp/mysql.sock';
$db['dabaojian']['db_debug'] = TRUE;
$db['dabaojian']['autoinit'] = TRUE;
$db['dabaojian']['log_path'] = '/data/app/logs';


/* End of file database.php */
/* Location: ./application/config/database.php */
