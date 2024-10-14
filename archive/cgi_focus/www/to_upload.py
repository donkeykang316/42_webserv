import requests
import sys
import os

def upload_file(file_path, upload_url):
    """
    Uploads a file to the specified server URL.

    :param file_path: Path to the file to upload.
    :param upload_url: The server URL to upload the file to.
    :return: Server response.
    """
    if not os.path.isfile(file_path):
        print(f"Error: The file '{file_path}' does not exist.")
        sys.exit(1)

    filename = os.path.basename(file_path)

    # Open the file in binary mode
    with open(file_path, 'rb') as file:
        # Create a dictionary of files to send
        files = {'file': (filename, file)}
        
        try:
            print(f"Uploading '{filename}' to '{upload_url}'...")
            response = requests.post(upload_url, files=files)
            
            # Raise an exception for HTTP error codes
            response.raise_for_status()
            
            # Assuming the server returns JSON
            try:
                result = response.json()
                print(f"Success: {result.get('message', 'File uploaded successfully.')}")
            except ValueError:
                print("Success: File uploaded successfully. No JSON response received.")
        
        except requests.exceptions.RequestException as e:
            print(f"Error uploading file: {e}")
            sys.exit(1)

def main():
    if len(sys.argv) != 3:
        print("Usage: python upload_file_cli.py <file_path> <upload_url>")
        print("Example: python upload_file_cli.py /path/to/file.txt https://yourserver.com/upload")
        sys.exit(1)
    
    file_path = sys.argv[1]
    upload_url = sys.argv[2]
    
    upload_file(file_path, upload_url)

if __name__ == "__main__":
    main()
