<?php
define("YCDB_BASE_PATH", realpath(dirname(__FILE__)));
define("YCDB_FAILURE", -1);

class YCDB_Driver {

    var $ycdb = null;
    var $unix_socket = '';
    var $host = '';
    var $username = '';
    var $password = '';
    var $dbname = '';
    var $port = 3306;
    var $pconnect = FALSE;
    var $db_debug = FALSE;
    var $char_set = 'utf8';
    var $dbcollat = 'utf8_general_ci';
    var $autoinit = FALSE;
    var $log_path = null;
    var $init_flag = FALSE;
    static $instance = array();

    function __construct($params) {
        $this->ob_level = ob_get_level();

        if (is_array($params)) {
            foreach ($params as $key => $val) {
                $this->$key = $val;
            }
        }
    }

    static function instance($params = '') {

        if (isset(self::$instance[$params])) {
            return self::$instance[$params];
        }

        $file_path = YCDB_BASE_PATH . '/database.php';

        if ( ! file_exists($file_path)) {
            die('The configuration file database.php does not exist.');
        }

        require_once($file_path);

        if ( ! isset($db) OR count($db) == 0) {
            die('No database connection settings were found in the database config file.');
        }

        if (empty($params)) {
            $params = 'default';
        }

        if (! isset($db[$params])) {
            die('You have specified an invalid database connection.');
        }

        $config = $db[$params];

        $DB = new YCDB_Driver($config);

        if ($DB->autoinit == TRUE) {
            $DB->initialize();
        }

        self::$instance[$params] = & $DB;
        return $DB;
    }

    function initialize() {
        if ($this->init_flag && is_object($this->ycdb)) {
            return TRUE;
        }

        if (!empty($this->unix_socket)) {
            $this->ycdb = new ycdb(["unix_socket" => $this->unix_socket]);
        } else {
            $options = array();
            $options['host'] = $this->host;
            $options['username'] = $this->username;
            $options['password'] = $this->password;
            $options['dbname'] = $this->dbname;
            $options['port'] = intval($this->port) == 0 ? 3306 : intval($this->port);
            $options['option'] = array(PDO::ATTR_CASE => PDO::CASE_NATURAL, PDO::ATTR_TIMEOUT => 2);
            if ($this->pconnect) $options['option'][PDO::ATTR_PERSISTENT] = true;

            $this->ycdb = new ycdb($options);
        }

        try {
            $this->ycdb->initialize();
        } catch (PDOException $e) {
            return $this->handle_error("ycdb initialize error : " . $e->getMessage());
        }

        $set_charset_sql = "SET NAMES '".$this->char_set."' COLLATE '".$this->dbcollat."'";
        $ret = $this->ycdb->exec($set_charset_sql);
        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        $this->init_flag = TRUE;
        return TRUE;
    }

    function query($sql) {
        $this->initialize();

        if (empty($sql)) {
            return $this->handle_error("input_empty_query_sql");
        }

        try {
            if ($this->is_write_type($sql)) {
                $ret = $this->ycdb->exec($sql);
            } else {
                $ret = $this->ycdb->query($sql);
            }
        } catch (PDOException $e) {
            return $this->handle_error("ycdb query error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function insert($table = '', $data = NULL, $ignore = false) {
        $this->initialize();

        if (empty($table) || empty($data) || !is_array($data)) {
            return $this->display_error('insert_table_or_data_must_be_set');
        }

        try {
            $ret = $this->ycdb->insert($table, $data);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb insert error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function replace($table = '', $data = NULL) {
        $this->initialize();

        if (empty($table) || empty($data) || !is_array($data)) {
            return $this->display_error('replace_table_or_data_must_be_set');
        }

        try {
            $ret = $this->ycdb->replace($table, $data);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb replace error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function update($table = '', $data = NULL, $where = NULL) {
        $this->initialize();

        if (empty($table) || empty($where) || empty($data) || !is_array($data)) {
            return $this->display_error('update_table_or_data_must_be_set');
        }

        try {
            $ret = $this->ycdb->update($table, $data, $where);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb update error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function delete($table = '', $where = NULL) {
        $this->initialize();

        if (empty($table) || empty($where)) {
            return $this->display_error('delete_table_or_where_must_be_set');
        }

        try {
            $ret = $this->ycdb->delete($table, $where);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb delete error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function get($table = '', $where = array(), $columns = "*") {
        $this->initialize();

        if (empty($table) || empty($columns)) {
            return $this->display_error('select_table_or_columns_must_be_set');
        }

        try {
            $ret = $this->ycdb->select($table, $columns, $where);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb delete error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        return $ret;
    }

    function get_one($table = '', $where = array(), $columns = "*") {
        $this->initialize();

        if (empty($table) || empty($columns)) {
            return $this->display_error('select_table_or_columns_must_be_set');
        }

        $where['LIMIT'] = 1;

        try {
            $ret = $this->ycdb->select($table, $columns, $where);
        } catch (PDOException $e) {
            return $this->handle_error("ycdb delete error : " . $e->getMessage());
        }

        if ($ret == YCDB_FAILURE) {
            return $this->handle_error();
        }

        if (empty($ret[0])) {
            return array();
        }
        return $ret[0];
    }

    function handle_error($input_error = null) {
        $log_path = empty($this->log_path) ? '/var/log' : $this->log_path;

        if (empty($input_error)) {
            $info = $this->ycdb->errorInfo();
            $error_code = $info[0];
            $error_no = $info[1];
            $error_msg = $info[2];

            @file_put_contents($log_path . "/ycdb_error-" . date('Y-m-d') . ".log.wf", "Query error: [$error_code][$error_no][$error_msg]\n" , FILE_APPEND);

            if ($this->db_debug) {
                $this->display_error(array("Error Number: $error_no", "[$error_code] $error_msg", $sql));
            }
        } else {
            @file_put_contents($log_path . "/var/log/ycdb_error-" . date('Y-m-d') . ".log.wf", "$input_error\n" , FILE_APPEND);

            if ($this->db_debug) {
                $this->display_error($input_error);
            }
        }

        return YCDB_FAILURE;
    }

    /**
     * Display an error message
     *
     * @access    public
     * @param    string    the error message
     * @param    string    any "swap" values
     * @param    boolean    whether to localize the message
     * @return    string    sends the application/error_db.php template
     */
    function display_error($error = '', $swap = '', $native = FALSE) {
        $heading = "A DB Error Occured";

        if (!is_array($error)) {
            $message[] = $error;
        } else {
            $message = $error;
        }

        $trace = debug_backtrace();

        foreach ($trace as $call) {
            if (isset($call['file']) && strpos($call['file'], '/ycdb') === FALSE) {
                $message[] = 'Filename: ' . $call['file'];
                $message[] = 'Line Number: ' . $call['line'];

                break;
            }
        }

        $code = 500;
        $text = 'Internal Server Error';

        $server_protocol = (isset($_SERVER['SERVER_PROTOCOL'])) ? $_SERVER['SERVER_PROTOCOL'] : false;

        if (substr(php_sapi_name(), 0, 3) == 'cgi') {
            header("Status: {$code} {$text}", true);
        }
        elseif ($server_protocol == 'HTTP/1.1' OR $server_protocol == 'HTTP/1.0') {
            header($server_protocol." {$code} {$text}", true, $code);
        }
        else {
            header("HTTP/1.1 {$code} {$text}", true, $code);
        }

        $message = '<p>'.implode('</p><p>', ( ! is_array($message)) ? array($message) : $message).'</p>';

        ob_start();
        include(YCDB_BASE_PATH . '/error_db.php');
        $buffer = ob_get_contents();
        ob_end_clean();
        echo $buffer;
        exit;
    }

    function is_write_type($sql) {
        if ( ! preg_match('/^\s*"?(SET|INSERT|UPDATE|DELETE|REPLACE|CREATE|DROP|TRUNCATE|LOAD DATA|COPY|ALTER|GRANT|REVOKE|LOCK|UNLOCK)\s+/i', $sql)) {
            return FALSE;
        }
        return TRUE;
    }

}
