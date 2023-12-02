#!/usr/bin/env python
import cgi
import json
import jwt
from datetime import datetime, timedelta

# Set your secret key for JWT
SECRET_KEY = 'your_secret_key'

# Dummy user credentials (replace with your authentication logic)
VALID_USERS = {'user1': 'password1', 'user2': 'password2'}

def generate_jwt_token(username):
    expiration_time = datetime.utcnow() + timedelta(hours=1)
    payload = {'username': username, 'exp': expiration_time}
    return jwt.encode(payload, SECRET_KEY, algorithm='HS256')

def authenticate_user(username, password):
    return username in VALID_USERS and VALID_USERS[username] == password

def main():
    form = cgi.FieldStorage()

    if 'username' in form and 'password' in form:
        username = form['username'].value
        password = form['password'].value

        if authenticate_user(username, password):
            # Generate JWT token on successful authentication
            jwt_token = generate_jwt_token(username)

            # Return JSON response with the token
            response = {'success': True, 'token': jwt_token}
            print('Content-type: application/json')
            print('Status: 302')
            print('Location: /Python/Home.html')
            print('\n')
            exit(0)
        else:
            response = {'success': False, 'message': 'Invalid credentials'}
    else:
        response = {'success': False, 'message': 'Missing username or password'}

    print('Content-type: application/json\n')
    print(json.dumps(response))

if __name__ == '__main__':
    main()
