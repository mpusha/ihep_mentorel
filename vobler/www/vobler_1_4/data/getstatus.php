<?php
// get status and error information and send to client. Delete status file after success reading
ini_set('max_execution_time', 120);

chdir('ctrl');
if(file_exists("status.dat")){ 
  $sts=json_decode(file_get_contents("status.dat"));
  unlink("status.dat");
}
else{exit(0);
  $sts = array(
         array('power'=>'unknown', 'error'=>'unknown', 'statusReq'=>'unknown','update'=>0)
	          );
}
echo json_encode($sts);
?>
