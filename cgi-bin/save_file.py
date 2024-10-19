#!/usr/bin/env python3

import cgi
import os
import cgitb
import sys
import html
import logging

# Enable CGI error reporting (disable in production)
cgitb.enable()

# Configure logging
logging.basicConfig(filename='/home/someng/42_webserv/cgi-bin/cgi_debug.log', level=logging.DEBUG)

def save_uploaded_file(field_storage):
    # Define the directory to save uploaded files
    upload_dir = "/home/someng/42_webserv/www/upload"  # Update to actual path

    # Ensure the upload directory exists
    os.makedirs(upload_dir, exist_ok=True)

    # Extract filename and sanitize it
    filename = os.path.basename(field_storage.filename)
    file_path = os.path.join(upload_dir, filename)

    # Write the file to the designated directory
    with open(file_path, 'wb') as output_file:
        data = field_storage.file.read()
        output_file.write(data)

    return file_path

def main():
    # Correctly terminate headers with CRLF
    sys.stdout.write("<!DOCTYPE html>")

    form = cgi.FieldStorage()

    # Log all received fields
    logging.debug("Received form fields:")
    for key in form.keys():
        logging.debug(f"Field: {key}, Value: {form.getvalue(key)}")

    if "file" not in form:
        logging.debug("No 'file' field found in the form.")
        sys.stdout.write("<html><body><h1>No file was uploaded.</h1></body></html>")
        return

    file_item = form["file"]

    if file_item.filename:
        saved_path = save_uploaded_file(file_item)
        logging.debug(f"File '{file_item.filename}' saved to '{saved_path}'.")
        sys.stdout.write("<html><body>")
        sys.stdout.write("<h1>File '{}' uploaded successfully!</h1>".format(html.escape(file_item.filename)))
        sys.stdout.write("<p>Saved to: {}</p>".format(html.escape(saved_path)))
        sys.stdout.write("</body></html>")
    else:
        logging.debug("No file was selected for upload.")
        sys.stdout.write("<html><body><h1>No file was selected for upload.</h1></body></html>")

if __name__ == "__main__":
    main()
