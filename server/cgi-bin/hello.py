#!/usr/bin/env python
import cgi
import os

print("Content-Type: text/html")
print("")

form = cgi.FieldStorage()

# Retrieve the 'name' parameter from the form data
name = form.getvalue('name', 'World')

# Generate the HTML response
print(f"""
<html>
<head>
    <title>CGI Response</title>
</head>
<body>
    <h1>Hello, {name}!</h1>
    <p>This response is generated by a CGI script.</p>
</body>
</html>
""")