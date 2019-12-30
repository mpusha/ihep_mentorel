<?php
// upload file on server
ini_set('max_execution_time', 120);
$destination = realpath('./data');

if (isset($_FILES['upload'])){
  $file = $_FILES['upload'];
	
  $filename = $destination."/".preg_replace("|[\\\/]|", "", $file["name"]);
  $sname = md5($file["name"]);

  //check that file name is valid
  if ($filename !== ""){
    move_uploaded_file($file["tmp_name"], $filename);
    $res = array("status" => "server", "sname" => $sname);
  } else {
      $res = array("status" => "error");
  }
  echo json_encode($res);
}
	

?>