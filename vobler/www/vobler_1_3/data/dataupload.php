<?php
// operations with listing of uploaded files (get list file, remember selected file)
ini_set('max_execution_time', 120);

$i=0;
$sts="";
$idsel=0;
$fsel="";
// get from input 'filesel' feald
//$cmd = array();

//parse_str(file_get_contents("php://input"), $cmd);
//$fsel = $cmd["fselect"];

$cmd = array();
parse_str(file_get_contents("php://input"), $cmd);
$fsel = $cmd["fselect"];


if(strlen($fsel)) { // write name of selected file and exit
  file_put_contents("./data/wfile.dat",$fsel); 
  file_put_contents("./ctrl/update_data.req","[request]\n\rfname=".$fsel);
  exit(0) ; 
}
// find ID index of selected file if don't found return 0
if(file_exists("./data/wfile.dat")){ 
  $sts=file_get_contents("./data/wfile.dat");
  foreach (glob("./data/*.csv") as $filename) {
    $i++;
    $fname=substr(strrchr($filename,'/'),1);
    if($fname==$sts) $idsel=$i;
  }
}
// send list of uploading files, select file and its ID 
$i=0;
foreach (glob("./data/*.csv") as $filename) {
  $i++;
  $fname=substr(strrchr($filename,'/'),1);
  $fn[]=array('id'=>$i,'title'=>$fname,'selfile'=>$sts,'idsel'=>$idsel);
}

echo json_encode($fn); 
?>