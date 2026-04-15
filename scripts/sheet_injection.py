import gspread
from oauth2client.service_account import ServiceAccountCredentials
import csv
import glob
import os
import sys
import time
import argparse
from datetime import datetime
from gspread.exceptions import APIError
from logging_config import setup_logging  # Import centralized logging configuration
import logging

# Constants
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PARENT_DIR = os.path.dirname(SCRIPT_DIR)
SECRETS_DIR = os.path.join(PARENT_DIR, "secrets") 
CREDENTIALS_FILE = os.path.join(SECRETS_DIR, "service-account-credentials.json")

# Set up centralized logging
setup_logging()
logger = logging.getLogger(__name__)

# Argument parsing
parser = argparse.ArgumentParser()
parser.add_argument("date", help="The date in YYYY.MM.DD format.")
args = parser.parse_args()

logger.info(f"Starting sheet_injection.py for date: {args.date}")

# Extract the provided date or default to today's date
try:
    provided_date = datetime.strptime(args.date, "%Y.%m.%d").strftime("%Y.%m.%d")
except ValueError:
    logger.error("Error: The provided date must be in the format YYYY.MM.DD")
    sys.exit(1)

# Define the bin directory using the date
bin_directory = f"data/{provided_date}/bin/"
if not os.path.exists(bin_directory):
    logger.error(f"Error: The directory '{bin_directory}' does not exist.")
    sys.exit(1)

# Google Sheets API setup
scope = ["https://spreadsheets.google.com/feeds", "https://www.googleapis.com/auth/drive"]
creds = ServiceAccountCredentials.from_json_keyfile_name(CREDENTIALS_FILE, scope)
client = gspread.authorize(creds)
spreadsheet = client.open("Job Costing Report")

# Find all CSV files in the specified bin folder
csv_files = glob.glob(f'{bin_directory}/*.csv')

if not csv_files:
    logger.info(f"No CSV files found in the directory: {bin_directory}")
    sys.exit(1)

# Loop through each CSV file
for csv_file in csv_files:
    sheet_name = os.path.splitext(os.path.basename(csv_file))[0]
    max_columns = 45  # Default to 45 if sheet isn't explicitly listed

    logger.info(f"Processing CSV file '{csv_file}' into sheet '{sheet_name}'")

    # Get or create worksheet
    try:
        worksheet = spreadsheet.worksheet(sheet_name)
    except gspread.exceptions.WorksheetNotFound:
        worksheet = spreadsheet.add_worksheet(title=sheet_name, rows="1000", cols=str(max_columns))
        logger.info(f"Created new sheet: {sheet_name}")

    # Read the CSV file (INCLUDING header row now)
    with open(csv_file, 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        raw_data = list(reader)

    # Limit columns to max_columns
    data = [row[:max_columns] for row in raw_data]

    if not data:
        logger.info(f"No data found in CSV '{csv_file}'. Skipping.")
        continue

    # Determine required worksheet size
    max_rows = len(data)
    max_cols = max(len(row) for row in data)

    # Resize sheet to fit data
    worksheet.resize(rows=max_rows, cols=max_cols)
    logger.info(f"Resized '{sheet_name}' to {max_rows} rows and {max_cols} columns.")

    # Clear existing content
    worksheet.clear()
    logger.info(f"Cleared existing data from '{sheet_name}'.")

    # Write entire 2D array starting at A1 (similar to sheet_injection.py)
    try:
        worksheet.update('A1', data, value_input_option="USER_ENTERED")
        logger.info(f"Sheet '{sheet_name}' updated successfully with fresh data from '{csv_file}'.")
        print(f"✅ Successfully uploaded fresh data to '{sheet_name}' with USER_ENTERED format.")
    except Exception as e:
        logger.error(f"❌ Error during sheet update for '{sheet_name}': {e}")
        print(f"❌ Error during sheet update for '{sheet_name}': {e}")
        raise

logger.info("All CSV files have been processed.")
print("All CSV files have been processed.")  # Notify completion on terminal