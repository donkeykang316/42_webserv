#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
    # Save the uploaded file
    file_path = os.path.join(os.getcwd(), 'cgi-bin', 'tmp', os.path.basename(fileitem.filename))
    open(file_path, 'wb').write(fileitem.file.read())
    os.chmod(file_path, 0o777)
    
    # Create a link to the uploaded file
    file_link = f'<a href="/cgi-bin/tmp/{os.path.basename(fileitem.filename)}">{os.path.basename(fileitem.filename)}</a>'
    
    message = f'The file {file_link} was uploaded to {file_path}'
else:
    message = 'Uploading Failed'

print("Content-Type: text/html;charset=utf-8")
print("Content-type:text/html\r\n")
print("<H1>" + message + "</H1>")