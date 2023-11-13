#!/usr/bin/php-cgi
<?php
// Set the content type to HTML
header("Content-Type: text/html");

// Get the current date and time
$date = date("Y-m-d");
$time = date("H:i:s");
?>

<html>
<head>
    <title>CGI-PHP Test</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f2f2f2;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
        }

        h1 {
            color: #333;
            text-align: center;
        }

        p {
            color: #666;
            margin-bottom: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI-PHP Test</h1>
        <p>Current Date: <?php echo $date; ?></p>
        <p>Current Time: <?php echo $time; ?></p>
    </div>
</body>
</html>
