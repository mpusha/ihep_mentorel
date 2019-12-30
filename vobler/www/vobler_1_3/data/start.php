<?php
// start authorisation and set lock file
header("X-SID: ".$_SERVER['PHP_AUTH_USER']); // write to log file on server side
header('Access-Control-Allow-Origin: *');  // cache disable (may be)
ini_set('max_execution_time', 120);
$stOk = array(
      array('status'=>1,'lock_ip'=>'')  // node is busy user working on node
	);
$stFault =array(
      array('status'=>0,'lock_ip'=>'')  // node is busy user working on node
	);
// set passwords and user names	
$valid_passwords = array ("serg" => "serg","sou" =>"sou");
$valid_users = array_keys($valid_passwords);

if(!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW'])|| !isset ($_SESSION['firstauthenticate'])) {
  $user = $_SERVER['PHP_AUTH_USER'];
  $pass = $_SERVER['PHP_AUTH_PW'];
  $validated = (in_array($user, $valid_users)) && ($pass == $valid_passwords[$user]);
}
else {
  $validated =false;
}

if (!isset ($_SESSION['firstauthenticate'])) {
    session_start();
}

function authenticate() {
  global $stFault;
  header('WWW-Authenticate: Basic realm="System autentification UnoAutoSur"');
  header('HTTP/1_0 401 Unauthorized');
  header("Status: 401 Access Denied"); 
  echo json_encode($stFault);  // authorization error
  exit;
}


if (! $validated || !isset($_SESSION['firstauthenticate'])) {
     $_SESSION['firstauthenticate']=true;
   authenticate();
}
else {
//I destroy the session var now
  session_unset();
  if(file_exists("./ctrl/lock")) {
    $handle = fopen("./ctrl/lock", "r");
    $buffer = fgets($handle, 4096);
    fclose($handle);
    $st = array(
      array('status'=>2,'lock_ip'=>$buffer)  // node is busy user working on node
	);
    echo json_encode($st); 
    exit;
  }
  $handle = fopen("./ctrl/lock", 'w');
  fwrite($handle, "Program locked by user: <".$user.">, from IP: <".$_SERVER['REMOTE_ADDR'].">.");
  fclose($handle);

  echo json_encode($stOk);   //all ok
}
?>