<?php
  ob_start();
?>
<!doctype html>
<html>

<head>
  <meta charset="UTF-8">
  <title>Directory Contents</title>
  <link rel="stylesheet" href="./styles/upload_listing.css">
  <link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">
</head>

<body>

  <div id="container">

    <div class="head_wrapper">
      <h1>Directory Contents</h1>
      <div class="upload_btn"><i class="material-icons upload_btn_icon">upload</i>
        <div class="popup_upload popup_upload_hidden">
            <div class="p-3">
              <form enctype="multipart/form-data" action="/cgi-bin/index.php?document_root=cgi-bin&fastcgi_script_name=index.php" method="post">

                <div>
                  <p>File: <input type ="file" name="filename" /></p>
                </div>
                <div>
                  <input type="submit" class="btn btn-primary px-4" name="submit"  value="Post"/>
                </div>
              </form>
            </div>
        </div>
      </div>
    </div>
    <table class="sortable">
      <thead>
        <tr>
          <th>Filename</th>
          <th>Type</th>
          <th>Size <small>(bytes)</small></th>
          <th>Date Modified</th>
          <th><i class="material-icons">delete</i></th>
          <th><i class="material-icons">cloud_download</i></th>
        </tr>
      </thead>
      <tbody>
      <?php
        // Opens directory
        if (!$config)
        {
          $config = parse_ini_file("../.user.ini");
          $uploaddir = $config['database'];
        }
        $myDirectory=opendir($uploaddir);

        // Gets each entry
        while($entryName=readdir($myDirectory)) {
          $dirArray[]=$entryName;
        }

        // Finds extensions of files
        function findexts ($filename) {
          $filename=strtolower($filename);
          $exts=explode("[/\\.]", $filename);
          $n=count($exts)-1;
          $exts=$exts[$n];
          return $exts;
        }

        // Closes directory
        closedir($myDirectory);

        // Counts elements in array
        $indexCount=count($dirArray);

        // Sorts files
        sort($dirArray);

        // Loops through the array of files

        for($index=0; $index < $indexCount; $index++) {

          // Allows ./?hidden to show hidden files
          if($_SERVER['QUERY_STRING']=="hidden")
          {$hide="";
          $ahref="./";
          $atext="Hide";}
          else
          {$hide=".";
          $ahref="./?hidden";
          $atext="Show";}
          if(substr("$dirArray[$index]", 0, 1) != $hide) {

          // Gets File Names
          $name=$dirArray[$index];
          $namehref=$uploaddir . $dirArray[$index];

          // Gets Extensions
          $extn=findexts($dirArray[$index]);

          // Gets file size
          // $dh = dir($uploaddir);
          $abs_file_path = $uploaddir . "/" . $dirArray[$index];
          $size=number_format(filesize($abs_file_path));

          // Gets Date Modified Data
          $modtime=date("M j Y g:i A", filemtime($abs_file_path));
          $timekey=date("YmdHis", filemtime($abs_file_path));

          // Prettifies File Types, add more to suit your needs.
          switch ($extn){
            case "png": $extn="PNG Image"; break;
            case "jpg": $extn="JPEG Image"; break;
            case "svg": $extn="SVG Image"; break;
            case "gif": $extn="GIF Image"; break;
            case "ico": $extn="Windows Icon"; break;

            case "txt": $extn="Text File"; break;
            case "log": $extn="Log File"; break;
            case "htm": $extn="HTML File"; break;
            case "php": $extn="PHP Script"; break;
            case "js": $extn="Javascript"; break;
            case "css": $extn="Stylesheet"; break;
            case "pdf": $extn="PDF Document"; break;

            case "zip": $extn="ZIP Archive"; break;
            case "bak": $extn="Backup File"; break;

            default: $extn=strtoupper($extn)." File"; break;
          }

          // Separates directories
          if(is_dir($dirArray[$index])) {
            $extn="&lt;Directory&gt;";
            $size="&lt;Directory&gt;";
            $class="dir";
          } else {
            $class="file";
          }

          // Cleans up . and .. directories
          if($name=="."){$name=". (Current Directory)"; $extn="&lt;System Dir&gt;";}
          if($name==".."){$name=".. (Parent Directory)"; $extn="&lt;System Dir&gt;";}

          // Print 'em
          print("
          <tr class='$class'>
            <td><a href='./$namehref'>$name</a></td>
            <td><a href='./$namehref'>$extn</a></td>
            <td><a href='./$namehref'>$size</a></td>
            <td sorttable_customkey='$timekey'><a href='./$namehref'>$modtime</a></td>

            <td>
              <button
                  class='file-delete'
                  data-file='$name'
              >
                  Delete
              </button>
            </td>
            <td>
              <button
                  class='file-download'
                  data-file='$name'
              >
                  Download
              </button>
            </td>
          </tr>");
          }
        }
      ?>
      </tbody>
    </table>

    <h2><?php print("<a href='$ahref'>$atext hidden files</a>"); ?></h2>

  </div>
  <div id="snackbar">Some text some message..</div>

  <script>
    console.log('init')
    document.addEventListener('click', async (event) => {
      if(event.target.classList.contains('file-delete')) {
        const name = event.target.dataset.file
        const result = await fetch('./upload' +"?filename=" + name,  {
          method: 'DELETE'
        })
        console.log(result)
        const x = document.getElementById("snackbar");
        x.innerHTML = `File ${name} was successfully deleted with a status ${result.status}`;
        x.className = "show";
        setTimeout(function(){
          x.className = x.className.replace("show", "");
          if(result.ok) {
            window.location.reload();
          }
        }, 3000);
      }
      else if(event.target.classList.contains('file-download')) {
        const name = event.target.dataset.file

        const url = './upload' +"?filename=" + name;
        window.location.href = url;
      }
      else if(event.target.classList.contains('upload_btn_icon')) {
        const popup = document.getElementsByClassName('popup_upload')[0];
        console.log(popup);
        popup.classList.toggle("popup_upload_hidden");

        // const url = './upload' +"?filename=" + name;
        // window.location.href = url;
      }
    })
  </script>
</body>

</html>

<?php
  $len = ob_get_length();
  header("Content-length: $len");
  ob_end_flush();
  exit();
?>

