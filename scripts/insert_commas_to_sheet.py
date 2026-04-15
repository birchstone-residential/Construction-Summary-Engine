import csv
import os

NUM_MISSING_COMMAS = 40

def fix_commas_in_rows(input_file, output_file):
    with open(input_file, "r", newline="", encoding="utf-8") as infile, \
         open(output_file, "w", newline="", encoding="utf-8") as outfile:
        
        reader = csv.reader(infile)
        writer = csv.writer(outfile)
        
        for row in reader:
            # current comma count = fields - 1
            comma_count = len(row) - 1
            
            if comma_count < NUM_MISSING_COMMAS:
                # number of missing commas = NUM_MISSING_COMMAS - current count
                missing = NUM_MISSING_COMMAS - comma_count
                row.extend([""] * missing)
            
            writer.writerow(row)

if __name__ == "__main__":
    # Get absolute paths relative to this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    attachments_dir = os.path.join(os.path.dirname(script_dir), "attachments")

    input_file = os.path.join(attachments_dir, "downloaded_sheet.csv")
    output_file = os.path.join(attachments_dir, "downloaded_sheet_modified.csv")

    fix_commas_in_rows(input_file, output_file)
    print(f"Modified file saved as {output_file}")
