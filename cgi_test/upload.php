<?php
// while(1);
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  // Check if file was uploaded
  if (isset($_FILES['upload'])) {
    // Check for file upload error
    if ($_FILES['upload']['error'] != UPLOAD_ERR_OK) {
      die('Upload failed with error code ' . $_FILES['upload']['error']);
    }

    // Move uploaded file to a permanent location
    $filename = $_FILES['upload']['name'];
    $tmpname = $_FILES['upload']['tmp_name'];
    $destination = '../uploads/' . $filename;
    if (!file_exists('../uploads/')) {
      mkdir('../uploads/', 0777, true);
    }
    if (move_uploaded_file ($tmpname ,$destination )) {
      echo 'File uploaded successfully! ';
      echo '<a href="login.php">
                <button>Back Home</button>
            </a>';
      exit();
    } else {
      echo 'Error while uploading file ';
      echo '<a href="login.php">
                <button>Back Home</button>
            </a>';
      exit();
    }

  } else {
    echo 'No file uploaded. ';
    echo '<a href="login.php">
                <button>Back Home</button>
            </a>';
    exit();
  }
}
?>

<!-- HTML form to upload files -->
<form action="upload.php" method="post" enctype="multipart/form-data">
  <input type="file" name="upload">
  <input type="submit" value="Upload">
</form>
<html>
<body>

<form method="post">
  Name: <input type="text" name="fname">
  <input type="submit">
</form>

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $name = $_POST['fname'];

  if (empty($name)) {
    echo "Name is empty";
  } else {
    echo $name;
  }
}
?>

</body>
</html>