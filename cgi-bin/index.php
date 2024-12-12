<?php
$config = parse_ini_file(".user.ini");
$uploaddir = $config['database'];


if ($_SERVER['REQUEST_METHOD'] == 'GET')
{
  if ($_GET && $_GET['filename'])
  {
    $filename =  $uploaddir . $_GET['filename'];;
    if (file_exists($filename) && is_file($filename))
    {
      // $size = filesize($filename);
      // if ($size > 4000)
      // {
      //   // header('Status: 200 OK');

      //   header('Status: 206 Partial Content');
      //   // http_response_code(200);

      //   require_once('./partial_content.php');
      //   serveFilePartial($filename, $filename);
      // }
      //   // header('Status: 206 Partial content');
      // else
      header('Status: 200 OK');
      $cont_disp = "attachment; filename=" . $_GET['filename'];
      header("Content-Disposition: $cont_disp");
      $size = filesize($filename);
      header("Content-length: $size");
      readfile($filename);
    }
    else if (!is_file($filename))
    {
      header('Status: 404 Not Found');
    }
    exit();
  }

  http_response_code(200);
  header('Status: 200 OK');
  include('./upload_listing/upload_listing.php');

  exit();

}
if ($_SERVER['REQUEST_METHOD'] == 'DELETE')
{
  // printf("DELETE!!!");
  // printf($_SERVER['QUERY_STRING']);
  $filename = $uploaddir . $_GET['filename'];
  if (file_exists($filename) && is_file($filename))
  {
    unlink($filename);
    header('Status: 200 OK');
    header('Content-length: 0');
  }
  else
  {
    header('Status: 204 No Content');
  }

  exit();
}

// function convertToBytes(string $from): ?int {
//   $units = ['B', 'KB', 'MB', 'GB', 'TB', 'PB'];
//   $number = substr($from, 0, -2);
//   $suffix = strtoupper(substr($from,-2));

//   //B or no suffix
//   if(is_numeric(substr($suffix, 0, 1))) {
//       return preg_replace('/[^\d]/', '', $from);
//   }

//   $exponent = array_flip($units)[$suffix] ?? null;
//   if($exponent === null) {
//       return null;
//   }

//   return $number * (1024 ** $exponent);
// }

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

  $uploadfile = $uploaddir . basename($_FILES['filename']['name']);

   if (move_uploaded_file($_FILES['filename']['tmp_name'], $uploadfile)) {
    http_response_code(200);
    header($_SERVER['SERVER_PROTOCOL'] . '200 OK');

    ob_start();
    echo "<head>";
    echo '<meta http-equiv="refresh" content="0; url=//localhost:5000/" /> ';
    echo "</head>";
    echo "<div>";
    echo "File is valid, and was successfully uploaded.\n";
    echo "</div>";
    ob_end_flush();
    exit();
  }
  else {
    print_r($_FILES['filename']['error']);
    http_response_code(404);
    header($_SERVER["SERVER_PROTOCOL"] . "404 Not Found");
    ob_start();
    echo "<div>";
      echo "Possible file upload attack!\n";
    echo "</div>";
    print('\0');
    ob_end_flush();
  }
}

exit();
?>
