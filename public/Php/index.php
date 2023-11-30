<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>POST Form Example</title>
</head>

<body>

    <h2>POST Form Example</h2>

    <form action="index.php" method="post">
        <label for="name">Name:</label>
        <input type="text" name="name" id="name" required>

        <label for="email">Email:</label>
        <input type="email" name="email" id="email" required>

        <input type="submit" value="Submit">
    </form>

    <?php
    // Check if the form is submitted
    if ($_SERVER["REQUEST_METHOD"] === "POST") {
        // Retrieve and display the submitted data
        $name = $_POST["name"];
        $email = $_POST["email"];

        echo "<h3>Submitted Data:</h3>";
        echo "<p>Name: $name</p>";
        echo "<p>Email: $email</p>";
    }
    ?>

</body>

</html>