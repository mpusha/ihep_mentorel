<?php
 //header('Access-Control-Allow-Origin: *'); 
//$dataFromFile = json_decode(file_get_contents("gr.json"));
//$dataFromFile =file_get_contents("gr.json");
//echo json_encode($dataFromFile);
//echo $dataFromFile;
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Expires: " . date("r"));
echo "[";
for ($x=0; $x<10; $x++)echo "{x:$x*10,y:$x*20},";
echo "]";