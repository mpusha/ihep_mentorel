<?php
// finish sessiion with delete lock file
ini_set('max_execution_time', 120);
if(file_exists("./ctrl/lock")) {
 $handle = fopen("./ctrl/lock", "r");
 $count = fgets($handle);
 fclose($handle); 
 $count=($count-1);
 $handle = fopen("./ctrl/lock", "w"); 
 fwrite($handle,$count);
 fclose($handle);
}
 session_destroy();
?>