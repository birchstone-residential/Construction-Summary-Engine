import csv
import os

def extract_log_lines(provided_date):
    log_file = "logs/latest.log"

    output_dir = os.path.join("data", provided_date, "bin_2")
    os.makedirs(output_dir, exist_ok=True)
    output_csv = os.path.join(output_dir, "Updates.csv")

    # Convert date only for CSV output
    formatted_date = provided_date.replace(".", "/")

    start_marker = "~ Creating directory:"
    end_marker = "~ ✅ Success writing file:"

    collect = False
    extracted = []

    with open(log_file, "r", encoding="utf-8") as f:
        for line in f:
            stripped = line.rstrip("\n")

            # Detect start marker
            if not collect and start_marker in stripped:
                collect = True
                continue

            # Detect end marker
            if collect and end_marker in stripped:
                collect = False
                continue

            # Capture lines between markers
            if collect:
                text = stripped

                # Extract only the part AFTER "INFO - "
                if "INFO - " in text:
                    text = text.split("INFO - ", 1)[1].strip()

                # Prepend formatted date
                extracted.append([formatted_date, text])

    # Write CSV
    with open(output_csv, "w", newline="", encoding="utf-8") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Date", "Update Message"])  # <-- header row
        writer.writerows(extracted)

    print(f"Extracted {len(extracted)} lines into {output_csv}")