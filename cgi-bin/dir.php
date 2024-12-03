<?php
$config = parse_ini_file(".user.ini");
$uploaddir = $config['database'];
$arrFiles = array();
echo $uploaddir;
$dirPath = "../upload";
    
// // Method 2: Using glob()
// echo "--------------------------------<br>";
// echo "Method 2: Using glob() <br>";
// echo "--------------------------------<br>";
// $files = glob($dirPath . "/*");
// foreach ($files as $file) {
//     if (is_file($file)) {
//         echo basename($file) . "<br>";
//     }
// }

// echo "--------------------------------<br>";
// echo "Display files ending in .txt<br>";
// echo "--------------------------------<br>";
// $files = glob($dirPath . "/*.txt");
// foreach ($files as $file) {
//     if (is_file($file)) {
//         echo basename($file) . "<br>";
//     }
// }

// Using scandir()
echo "--------------------------------<br>";
echo "Method 1: Using scandir() <br>";
echo "--------------------------------<br>";
$files = scandir($dirPath);
foreach ($files as $file) {
    $filePath = $dirPath . '/' . $file;
    if (is_file($filePath)) {
        echo $file . "<br>";
    }
}

?>