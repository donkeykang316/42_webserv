<?php
$config = parse_ini_file(".user.ini");
$uploaddir = $config['database'];


if ($_SERVER['REQUEST_METHOD'] == 'GET')
{
	printf("GET\n");
	printf($uploaddir);
	print_r($_GET);
  if ($_GET && $_GET['filename'])
  {
	printf($_GET['filename']);

    $filename =  $uploaddir . $_GET['filename'];;
    if (file_exists($filename) && is_file($filename))
    {
      $size = filesize($filename);
      if ($size > 4000)
      {
        require_once('./partial_content.php');
        serveFilePartial($filename, $filename);
      }
      else
        header('Status: 200 OK');
      $cont_disp = "attachment; filename=" . $_GET['filename'];
      header("Content-Disposition: $cont_disp");
      header("Content-length: $size");
      readfile($filename);
    }
    // else if (!is_file($filename))
    // {
    //   header('Status: 404 Not Found');
    // }
    exit();
  }

//   http_response_code(200);
//   header('Status: 200 OK');
//   include('./upload_listing/upload_listing.php');

//   exit();

}
// exit();
?>
