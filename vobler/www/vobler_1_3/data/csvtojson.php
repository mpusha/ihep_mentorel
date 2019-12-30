<?php
// read request csv file, translate to JSON and send to client 
ini_set('max_execution_time', 120);
// get from input 'filename' feald
$cmd = array();
parse_str(file_get_contents("php://input"), $cmd);
$fname = $cmd["filename"];
$delfile=$cmd["delfile"];
$column = array();
$mx=-1000;
$mi=1000;
if(file_exists($fname)){
  if (($handle = fopen($fname, "r")) !== FALSE) {
    $column_headers = fgetcsv($handle); // read the row.
    $l=0;
    while (($data = fgetcsv($handle)) !== FALSE) {
      $i = 0;
      if(abs($data[$i+1])>$mx) $mx=abs($data[$i+1]); if(abs($data[$i+2])>$mx) $mx=abs($data[$i+2]); // define max 
      if(!($l%5)) 
      $column[]=array($column_headers[0]=>(float)$data[$i++]*1000,$column_headers[1]=>(float)$data[$i++],$column_headers[2]=>(float)$data[$i++]);
      // remove every 9 data from file and translate sec into msec
      $l++;
    }
    fclose($handle);
  }
}
else
  $column[]=array("t"=>(float)0,"y1"=>(float)0,"y2"=>(float)0);

$mx=round($mx/50.0)*50;
$mi=-$mx;
$column[]=array("max"=>$mx,"min"=>$mi,"step"=>($mx-$mi)/10);

echo json_encode($column);
if($delfile==1) {
  if(file_exists($fname)){ unlink($fname);}
}
?>
