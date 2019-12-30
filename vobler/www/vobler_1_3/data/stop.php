<?php
// finish sessiion with delete lock file
ini_set('max_execution_time', 120);
//if(file_exists('./ctrl/lock'){
  unlink('./ctrl/lock');
//}
?>