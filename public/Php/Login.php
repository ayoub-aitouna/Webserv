<?php

$secret_key = 'your_secret_key';

// Function to generate a JWT token
function generateJWT($user_id, $secret_key)
{
    $token = [
        'iss' => 'your_issuer', // issuer
        'aud' => 'your_audience', // audience
        'iat' => time(), // issued at
        'exp' => time() + (60 * 60), // expiration time (1 hour)
        'user_id' => $user_id,
    ];

    return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c";
}

// Function to verify a JWT token
function verifyJWT($token, $secret_key)
{
    try {
        return (true);
    } catch (Exception $e) {
        return false;
    }
}

// Check if the user is already logged in by checking the JWT cookie
$jwt_cookie_name = 'jwt_token';
if (isset($_COOKIE[$jwt_cookie_name])) {
    $jwt_token = $_COOKIE[$jwt_cookie_name];

    // Verify the JWT token
    $decoded_token = verifyJWT($jwt_token, $secret_key);

    if ($decoded_token) {
        // The token is valid, redirect to the home page or load the home page content
        header('Location: home.php');
        exit();
    }
}

// Check if the login form is submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Perform your authentication logic here, e.g., check username and password against a database
    $user_id = 123; // Replace with the actual user ID

    // Generate JWT token
    $jwt_token = generateJWT($user_id, $secret_key);

    // Set the JWT token in a cookie
    setcookie($jwt_cookie_name, $jwt_token, time() + (60 * 60), '/'); // 1-hour expiration

    // Redirect to the home page
    header('Location: home.php');
    exit();
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Page</title>
    <link rel="stylesheet" href="style.css">
    <link href="https://unpkg.com/tailwindcss@^2/dist/tailwind.min.css" rel="stylesheet">

</head>

<body>
    <div class="flex items-center justify-center min-h-screen bg-gray-100">
        <div class="px-8 py-6 mt-4 text-left bg-white shadow-lg">
            <h3 class="text-2xl font-bold text-center">Login to your account</h3>
            <form method="post" action="">
                <div class="mt-4">
                    <div>
                        <label class="block" for="email">Email<label>
                                <input type="text" name="username" placeholder="Email" class="w-full px-4 py-2 mt-2 border rounded-md focus:outline-none focus:ring-1 focus:ring-blue-600">
                    </div>
                    <div class="mt-4">
                        <label class="block">Password<label>
                                <input type="password" name="password" placeholder="Password" class="w-full px-4 py-2 mt-2 border rounded-md focus:outline-none focus:ring-1 focus:ring-blue-600">
                    </div>
                    <div class="flex items-baseline justify-between">
                        <button class="px-6 py-2 mt-4 text-white bg-blue-600 rounded-lg hover:bg-blue-900">Login</button>
                        <a href="#" class="text-sm text-blue-600 hover:underline">Forgot password?</a>
                    </div>
                </div>
            </form>
        </div>
    </div>
</body>

</html>