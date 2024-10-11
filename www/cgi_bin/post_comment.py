#!/usr/bin/env python3

import cgi
import cgitb
import os
from datetime import datetime
import html

# Enable debugging
cgitb.enable()

# Define the path to the comments file
COMMENTS_FILE = "/path/to/your/comments.txt"  # Update this path

def load_comments():
    """Load comments from the COMMENTS_FILE."""
    comments = []
    if os.path.exists(COMMENTS_FILE):
        with open(COMMENTS_FILE, "r", encoding="utf-8") as f:
            for line in f:
                try:
                    author, timestamp, content = line.strip().split("||")
                    comments.append({
                        "author": html.escape(author),
                        "timestamp": timestamp,
                        "content": html.escape(content)
                    })
                except ValueError:
                    continue  # Skip malformed lines
    return comments

def save_comment(author, content):
    """Save a new comment to the COMMENTS_FILE."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(COMMENTS_FILE, "a", encoding="utf-8") as f:
        f.write(f"{author}||{timestamp}||{content}\n")

def main():
    form = cgi.FieldStorage()
    author = form.getfirst("author", "").strip()
    comment = form.getfirst("comment", "").strip()

    if author and comment:
        # Sanitize inputs
        author = author.replace("||", " ")
        comment = comment.replace("||", " ")

        # Save the comment
        save_comment(author, comment)

    # Load all comments
    comments = load_comments()

    # Generate HTML response
    print("Content-Type: text/html\n")
    print("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Comment Section</title>
        <style>
            /* (Same CSS as your HTML file) */
            body {
                font-family: Arial, sans-serif;
                margin: 2em;
                background-color: #f9f9f9;
            }
            .comment-section {
                max-width: 600px;
                margin: auto;
            }
            .comment-form {
                display: flex;
                flex-direction: column;
                margin-bottom: 2em;
            }
            .comment-form textarea {
                resize: vertical;
                min-height: 100px;
                padding: 10px;
                font-size: 1em;
                margin-bottom: 1em;
            }
            .comment-form button {
                width: 100px;
                padding: 10px;
                font-size: 1em;
                cursor: pointer;
            }
            .comments {
                list-style-type: none;
                padding: 0;
            }
            .comment {
                background-color: #fff;
                padding: 1em;
                margin-bottom: 1em;
                border-radius: 5px;
                box-shadow: 0 0 5px rgba(0,0,0,0.1);
            }
            .comment .author {
                font-weight: bold;
                margin-bottom: 0.5em;
            }
            .comment .timestamp {
                font-size: 0.85em;
                color: #555;
                margin-bottom: 0.5em;
            }
            .comment .content {
                white-space: pre-wrap;
            }
        </style>
    </head>
    <body>
        <div class="comment-section">
            <h2>Leave a Comment</h2>
            <form class="comment-form" action="/cgi-bin/submit_comment.cgi" method="POST">
                <label for="author">Name:</label>
                <input type="text" id="author" name="author" required>

                <label for="comment">Comment:</label>
                <textarea id="comment" name="comment" required></textarea>

                <button type="submit">Submit</button>
            </form>

            <h3>Comments</h3>
            <ul class="comments" id="comments-list">
    """)

    for comment in comments:
        print(f"""
                <li class="comment">
                    <div class="author">{comment['author']}</div>
                    <div class="timestamp">{comment['timestamp']}</div>
                    <div class="content">{comment['content']}</div>
                </li>
        """)

    print("""
            </ul>
        </div>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()
