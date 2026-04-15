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

logger.info(f"Starting append_sheet_injection.py for date: {args.date}")

# Extract the provided date or default to today's date
try:
    provided_date = datetime.strptime(args.date, "%Y.%m.%d").strftime("%Y.%m.%d")
except ValueError:
    logger.error("Error: The provided date must be in the format YYYY.MM.DD")
    sys.exit(1)

# Define the bin directory using the date
bin_2_directory = f"data/{provided_date}/bin_2/"
if not os.path.exists(bin_2_directory):
    logger.error(f"Error: The directory '{bin_2_directory}' does not exist.")
    sys.exit(1)

# Google Sheets API setup
scope = ["https://spreadsheets.google.com/feeds", "https://www.googleapis.com/auth/drive"]
creds = ServiceAccountCredentials.from_json_keyfile_name(CREDENTIALS_FILE, scope)
client = gspread.authorize(creds)
spreadsheet = client.open("Job Costing Report")

# Find all CSV files in the specified bin folder
csv_files = glob.glob(f'{bin_2_directory}/*.csv')

if not csv_files:
    logger.info(f"No CSV files found in the directory: {bin_2_directory}")
    sys.exit(1)

# Loop through each CSV file
for csv_file in csv_files:
    sheet_name = os.path.splitext(os.path.basename(csv_file))[0]
    max_columns = 26
    try:
        worksheet = spreadsheet.worksheet(sheet_name)
    except gspread.exceptions.WorksheetNotFound:
        worksheet = spreadsheet.add_worksheet(title=sheet_name, rows="1000", cols=str(max_columns))
        logger.info(f"Created new sheet: {sheet_name}")

    with open(csv_file, 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        data = [row[:max_columns] for row in list(reader)[1:]]  # Limit columns to max_columns and skip header

    existing_data = worksheet.get_all_values()
    next_empty_row = len(existing_data) + 1

    required_rows = next_empty_row + len(data) - 1
    current_rows = worksheet.row_count
    if required_rows > current_rows:
        worksheet.add_rows(required_rows - current_rows)
        logger.info(f"Added {required_rows - current_rows} rows to '{sheet_name}'.")

    # Convert data into batch update format
    requests = []
    for row_idx, row in enumerate(data, start=next_empty_row):
        for col_idx, cell in enumerate(row, start=1):
            cell_range = gspread.utils.rowcol_to_a1(row_idx, col_idx)
            requests.append({
                "range": cell_range,
                "values": [[cell]]  # Ensures formulas are recognized properly
            })
    
    # Batch update the sheet to ensure formulas work
    if requests:
        try:
            worksheet.batch_update(requests, value_input_option="USER_ENTERED")
            logger.info(f"Batch update successful for '{sheet_name}'.")
            print(f"✅ Successfully uploaded data to '{sheet_name}' with USER_ENTERED format.")
        except Exception as e:
            logger.error(f"❌ Error during batch update for '{sheet_name}': {e}")
            print(f"❌ Error during batch update for '{sheet_name}': {e}")
            raise

logger.info("All CSV files have been processed.")
print("All CSV files have been processed.")  # Notify completion on terminal
