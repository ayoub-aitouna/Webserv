#!/usr/bin/env python3
# Import modules for CGI handling
import cgi, cgitb

cgitb.enable()
# Create instance of FieldStorage
form = cgi.FieldStorage()
if(not ("name" in form)):
    print("<h1>{}</h1>".form.name)
# Get data from fields
nm=form.getvalue('name')
f=form.getvalue('faculty')
b=form.getvalue('batch')
text = form.getvalue('about')
print ("Content-type:text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Python Script</title>")
print("</head>")
print("<body>")
print ("<h 2>Data received from client form</h 2>")
print("Name: {}".format(nm))
print ("</br>")
print("Faculty: {}".format(f))
print ("</br>")
print("Batch: {}".format(b))
print ("</br>")
print("Text : {}".format(text))
print ("</br>")
print("</body>")
print ("</html>")