import os
import csv
from google.oauth2 import service_account
from googleapiclient.discovery import build

# --- CONFIGURATION ---
SCOPES = ['https://www.googleapis.com/auth/spreadsheets.readonly']
SERVICE_ACCOUNT_FILE = os.path.join(os.path.dirname(__file__), '../secrets/service-account-credentials.json')

SPREADSHEET_ID = '1CnElayHIXS7uySxvu5BoVes0xhDH_m_BmMnL-f13IXM'
SHEET_NAME = 'Construction Summary'
CSV_OUTPUT_PATH = os.path.join(os.path.dirname(__file__), '../attachments/downloaded_sheet.csv')

def download_sheet_as_csv():
    creds = service_account.Credentials.from_service_account_file(
        SERVICE_ACCOUNT_FILE, scopes=SCOPES
    )

    service = build('sheets', 'v4', credentials=creds)
    sheet = service.spreadsheets()

    # Read data from the "Main" sheet
    result = sheet.values().get(spreadsheetId=SPREADSHEET_ID, range=SHEET_NAME).execute()
    values = result.get('values', [])

    if not values:
        print(f'No data found in sheet "{SHEET_NAME}".')
        return

    # Write to CSV
    with open(CSV_OUTPUT_PATH, 'w', newline='', encoding='utf-8') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerows(values)

    print(f'Download complete: {CSV_OUTPUT_PATH}')

if __name__ == '__main__':
    download_sheet_as_csv()
