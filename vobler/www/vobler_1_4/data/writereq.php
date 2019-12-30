<?php
// write power operation request
ini_set('max_execution_time', 120);
$i=0;
$sts="";
$idsel=0;
// get from input 'filesel' feald
$cmd = array();
parse_str(file_get_contents("php://input"), $cmd);
$fsel = $cmd["power"];

if(strlen($fsel)) { // write name of selected file and exit

  file_put_contents("./ctrl/update_power.req","[request]\n\r".$fsel);
  exit(0) ; 
}
?>