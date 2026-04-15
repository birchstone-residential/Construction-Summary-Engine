import os
import sys
import json
import base64
import zipfile
import pickle
import shutil
import pandas as pd
from datetime import datetime
from googleapiclient.discovery import build
from google.auth.transport.requests import Request
from google_auth_oauthlib.flow import InstalledAppFlow

# Constants
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PARENT_DIR = os.path.dirname(SCRIPT_DIR)
SECRETS_DIR = os.path.join(PARENT_DIR, "secrets")
SCOPES = ['https://www.googleapis.com/auth/gmail.modify']  # Updated to allow modifications
CREDENTIALS_FILE = os.path.join(SECRETS_DIR, "credentials.json")  # OAuth Client ID JSON
TOKEN_FILE = os.path.join(SECRETS_DIR, "token.pickle")  # Stores OAuth Refresh Token
DOWNLOAD_FOLDER = os.path.join(PARENT_DIR, "attachments")
ARCHIVE_FOLDER = os.path.join(PARENT_DIR, "attachments", "Archive")
ZIP_FILE_NAME = os.path.join(DOWNLOAD_FOLDER, "Archive.zip")  # Saved inside `attachments`

def authenticate_gmail():
    """Authenticates the user using OAuth 2.0 and reuses stored credentials."""
    creds = None
    
    # Load credentials if token file exists
    if os.path.exists(TOKEN_FILE):
        print("🔄 Loading stored credentials from token.pickle...")
        with open(TOKEN_FILE, 'rb') as token:
            creds = pickle.load(token)

    # Refresh token if expired
    if creds and creds.expired and creds.refresh_token:
        print("🔄 Refreshing access token...")
        creds.refresh(Request())

    # If no valid credentials, request user authentication
    if not creds or not creds.valid:
        print("🛠 No valid credentials found. Prompting for authentication...")
        flow = InstalledAppFlow.from_client_secrets_file(CREDENTIALS_FILE, SCOPES)
        creds = flow.run_local_server(port=8080)
        
        with open(TOKEN_FILE, 'wb') as token:
            pickle.dump(creds, token)
    
    return build("gmail", "v1", credentials=creds)

def search_email(service, user_id, subject_query):
    """Searches for an email with a specific subject and returns its message ID."""
    query = f"subject:{subject_query}"
    page_token = None

    while True:
        try:
            results = service.users().messages().list(
                userId=user_id, q=query, maxResults=5, pageToken=page_token
            ).execute()

            messages = results.get("messages", [])
            if messages:
                return messages[0]["id"]  # Return first matching email
            
            page_token = results.get("nextPageToken")
            if not page_token:
                break
        except Exception as e:
            print(f"❌ Error searching emails: {e}")
            break

    print("❌ No matching email found.")
    return None

def download_attachments(service, user_id, msg_id, download_folder):
    """Downloads attachments from a given email message."""
    os.makedirs(download_folder, exist_ok=True)
    
    message = service.users().messages().get(userId=user_id, id=msg_id).execute()
    parts = message["payload"].get("parts", [])

    attachment_count = 0
    for part in parts:
        if part.get("filename"):  # If it's an attachment
            att_id = part["body"].get("attachmentId")
            if not att_id:
                continue

            try:
                att = service.users().messages().attachments().get(
                    userId=user_id, messageId=msg_id, id=att_id
                ).execute()
                file_data = base64.urlsafe_b64decode(att["data"])
                
                file_path = os.path.join(download_folder, part["filename"])
                with open(file_path, "wb") as f:
                    f.write(file_data)

                attachment_count += 1
                print(f"📥 Downloaded: {file_path}")
            except Exception as e:
                print(f"❌ Error downloading attachment '{part['filename']}': {e}")

    return attachment_count

def move_csv_files(source_folder, archive_folder):
    """Moves all CSV files from the source folder to the archive folder."""
    os.makedirs(archive_folder, exist_ok=True)
    
    for filename in os.listdir(source_folder):
        if filename.endswith(".csv"):
            src_path = os.path.join(source_folder, filename)
            dest_path = os.path.join(archive_folder, filename)
            shutil.move(src_path, dest_path)
            print(f"📂 Moved {filename} to {archive_folder}")

def zip_attachments(source_folder, output_zip):
    """Zips all files inside the source folder into a zip file."""
    with zipfile.ZipFile(output_zip, "w", zipfile.ZIP_DEFLATED) as zipf:
        for root, _, files in os.walk(source_folder):
            for file in files:
                file_path = os.path.join(root, file)
                zipf.write(file_path, os.path.relpath(file_path, source_folder))
    print(f"📦 Zipped {source_folder} into {output_zip}")

def main():
    if len(sys.argv) != 2:
        print("❌ Usage: python parse_inbox.py YYYY.MM.DD")
        sys.exit(1)

    try:
        input_date = datetime.strptime(sys.argv[1], "%Y.%m.%d")
        formatted_date = input_date.strftime("%m/%d/%Y")
    except ValueError:
        print("❌ Invalid date format. Use YYYY.MM.DD")
        sys.exit(1)
    
    email_subject = f"Entrata Reports - Job Cost Engine - {formatted_date}"
    
    service = authenticate_gmail()
    user_id = "me"
    msg_id = search_email(service, user_id, email_subject)
    if not msg_id:
        sys.exit(1)
    
    attachment_count = download_attachments(service, user_id, msg_id, DOWNLOAD_FOLDER)
    if attachment_count == 0:
        sys.exit(1)
    
    move_csv_files(DOWNLOAD_FOLDER, ARCHIVE_FOLDER)
    zip_attachments(ARCHIVE_FOLDER, ZIP_FILE_NAME)

if __name__ == "__main__":
    main()
