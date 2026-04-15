import os
import sys

def clean_csv(input_file, output_file):
    input_file = os.path.abspath(input_file)
    output_file = os.path.abspath(output_file)

    with open(input_file, 'r', encoding='utf-8') as infile:
        lines = infile.readlines()

    fixed_lines = []
    current_line = ""

    for line in lines:
        current_line += line.strip("\n")  # Remove newlines but keep content
        if current_line.count('"') % 2 == 0:  # Ensure complete quoted fields
            fixed_lines.append(current_line)
            current_line = ""
    
    with open(output_file, 'w', encoding='utf-8') as outfile:
        outfile.write("\n".join(fixed_lines))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 fix_csv_format.py <input_file> <output_file>")
        sys.exit(1)

    input_csv = sys.argv[1]
    output_csv = sys.argv[2]

    clean_csv(input_csv, output_csv)
    print(f"Cleaned CSV saved as: {output_csv}")
