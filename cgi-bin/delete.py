#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
if not (form.getvalue('filename')):
     message = 'Deleting Failed'
else:
    fileitem = form.getvalue('filename')
    file_path = os.path.join(os.getcwd(), 'cgi-bin', 'tmp', fileitem)

    # Check if the file exists
    if os.path.isfile(file_path):
        # Delete the chosen file
        os.remove(file_path)
    
        # Create a response for the client
        # file_link = f'<a href="/cgi-bin/tmp/{os.path.basename(fileitem)}">{os.path.basename(fileitem)}</a>'
        message = f'The file {fileitem} was successfully deleted'
    else:
        message = 'Deleting Failed'

print("Content-Type: text/html;charset=utf-8")
print("Content-type:text/html\r\n")
print("<H1>" + message + "</H1>")