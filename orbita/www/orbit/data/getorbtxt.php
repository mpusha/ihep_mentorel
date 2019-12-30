<?php
// read request csv file, translate to JSON and send to client 
ini_set('max_execution_time', 120);
// get from input 'filename' feald
$cmd = array();
parse_str(file_get_contents("php://input"), $cmd);
$fname = $cmd["filename"];
$delfile=$cmd["delfile"];
$column = array();
if(file_exists($fname)){
  if (($handle = fopen($fname, "r")) !== FALSE) {
    $column_headers = fgetcsv($handle); // read the row.
    $j=0;
    while (($data = fgetcsv($handle)) !== FALSE) {
      $i = 0;
      $column[]=array('id'=>(string)$j,
                     $column_headers[0]=>(int)$data[$i++],
                     $column_headers[1]=>(float)$data[$i++],
                     $column_headers[2]=>(float)$data[$i++],
                     $column_headers[3]=>(float)$data[$i++],
                     $column_headers[4]=>(float)$data[$i++],
                     $column_headers[5]=>(float)$data[$i++],
                     $column_headers[6]=>(float)$data[$i++],
                     $column_headers[7]=>(float)$data[$i++],
                     $column_headers[8]=>(float)$data[$i++],
                     $column_headers[9]=>(float)$data[$i++],
                     $column_headers[10]=>(float)$data[$i++],
                     $column_headers[11]=>(float)$data[$i++],
                     $column_headers[12]=>(float)$data[$i++],
                     $column_headers[13]=>(float)$data[$i++],
                     $column_headers[14]=>(float)$data[$i++],
                     $column_headers[15]=>(float)$data[$i++],
                     $column_headers[16]=>(float)$data[$i++]
                     );
      $j++;
    }
    fclose($handle);
  }
}
else
  $column[]=array("axe"=>(float)0,"t1"=>(float)0,"t2"=>(float)0);



echo json_encode($column);
unset($colum);
//if($delfile==1) {
//  if(file_exists($fname)){ unlink($fname);}
//}
?>
