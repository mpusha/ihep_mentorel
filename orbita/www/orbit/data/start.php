<?php
// start authorisation and set lock file
//header('HTTP/1.1 401 Unauthorized');
//header("X-SID: ".$_SERVER['PHP_AUTH_USER']); // write to log file on server side
header('Access-Control-Allow-Origin: *');  // cache disable (may be)
ini_set('max_execution_time', 120);
$stOk = array(
      array('status'=>1,'lock_ip'=>"")  // node is busy user working on node
	);
$stFault =array(
      array('status'=>0,'lock_ip'=>'')  // node is busy user working on node
	);
$count=0;
  if(file_exists("./ctrl/lock")) {
    $handle = fopen("./ctrl/lock", "r");
    $count = fgets($handle);
    fclose($handle);
    if($count>2){
      $st = array(
      array('status'=>2,'lock_ip'=>"You have a lot of open sessions. ".$count." Please close any.")  // node is busy user working on node
      );
      echo json_encode($st); 
      exit;
    }
  }
  $count=($count+1);
  $handle = fopen("./ctrl/lock", 'w');
  fwrite($handle,$count);
  fclose($handle);

echo json_encode($stOk);   //all ok
?>