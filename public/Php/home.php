<?php

// Function to verify a JWT token
function verifyJWT($token, $secret_key)
{
    $Decoded_data = [
        'user_id' => '#65684--56'

    ];
    return $Decoded_data;
}

// Check if the JWT cookie is set
$jwt_cookie_name = 'jwt_token';
if (isset($_COOKIE[$jwt_cookie_name])) {
    $jwt_token = $_COOKIE[$jwt_cookie_name];
    $secret_key = 'your_secret_key';

    // Verify the JWT token
    $decoded_token = verifyJWT($jwt_token, $secret_key);

    if ($decoded_token) {
        // Token is valid, user is authenticated

        // Display user information
        // Add additional content for the home page as needed
    } else {
        // Token is not valid, redirect to the login page
        header('Location: Login.php');
        exit();
    }
} else {
    // JWT cookie is not set, redirect to the login page
    header('Location: Login.php');
    exit();
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Home Page</title>
    <link rel="stylesheet" href="style.css">

</head>

<body>

    <body>
        <div class="content">
            <h2>Welcome to the Home Page</h2>
            <p>User ID: 65685</p>
            <p>User ID:
                <?php echo $jwt_token; ?>
            </p>
            <p>This is the content of the home page.</p>
        </div>
    </body>
</body>

</html>