<?php
// get status and error information and send to client. Delete status file after success reading
ini_set('max_execution_time', 120);

chdir('ctrl');
$update=0;
if(file_exists("status.dat")){

  $sts=json_decode(file_get_contents("status.dat"),true);
  //var_dump($sts);
  if(file_exists("update.req")){
    $update=1;
//    unlink("update.req");
  }
}
else{
  $sts = array(
         array("status"=>"unknown", "start"=>"unknown", "datetime"=>"unknown","update"=>0 )
	      );
}
$sts[0]["update"]=$update;
echo json_encode($sts);
?>
