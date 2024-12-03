<?php
$arrFiles = array();
$dirPath = "./MyFolder";

$files = scandir($dirPath);
?>
<ul>
	<?php
		foreach ($files as $file) {
			$filePath = $dirPath . '/' . $file;
			if (is_file($filePath)) {
				?>
					<li><php echo $file ?></li>
				<?php
			}
		}
	?>
</ul>
