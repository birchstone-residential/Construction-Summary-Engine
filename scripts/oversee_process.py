import os
import subprocess
import sys
from datetime import datetime
from logging_config import setup_logging  # Import centralized logging configuration
import logging
from rename_latest_log import rename_log_file
from extract_log_lines import extract_log_lines

# Constants
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PARENT_DIR = os.path.dirname(SCRIPT_DIR)
SCRIPTS_DIR = os.path.join(PARENT_DIR, "scripts")
BIN_DIR = os.path.join(PARENT_DIR, "bin")
DOWNLOAD_FOLDER = os.path.join(PARENT_DIR, "attachments")
ARCHIVE_FOLDER = os.path.join(DOWNLOAD_FOLDER, "Archive")
ZIP_FILE_NAME = os.path.join(DOWNLOAD_FOLDER, "Archive.zip")

def get_script_path(script_name):
    return os.path.join(SCRIPTS_DIR, script_name)

def get_executable_path(executable_name):
    return os.path.join(BIN_DIR, executable_name)

# Rename the latest log file
rename_log_file()

# Set up centralized logging
setup_logging()
logger = logging.getLogger(__name__)

# Get the date from the command line argument or default to today's date
if len(sys.argv) == 2:
    try:
        provided_date = sys.argv[1]
        datetime.strptime(provided_date, "%Y.%m.%d")  # Validate date format
        logger.info(f"Using provided date: {provided_date}")
    except ValueError:
        logger.error("❌ Error: The provided date must be in the format YYYY.MM.DD")
        sys.exit(1)
else:
    provided_date = datetime.now().strftime("%Y.%m.%d")
    logger.info(f"No date provided. Defaulting to today's date: {provided_date}")

# Function to execute a script and log its output
def execute_script(script, *args):
    if not os.path.isabs(script):
        script_path = os.path.join(SCRIPT_DIR, script)
    else:
        script_path = script
    script_path = get_script_path(script)
    command = ["python3", script_path] + list(args)
    logger.info(f"Executing: {' '.join(command)}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    for line in iter(process.stdout.readline, ''):
        logger.info(line.strip())
    for line in iter(process.stderr.readline, ''):
        logger.error(line.strip())
    
    if process.returncode == 0 or process.returncode == None:  # Allowing return code 1 for non-critical errors
        logger.info(f"✅ {script} completed successfully.")
    else:
        logger.error(f"❌ {script} failed with return code {process.returncode}.")

def print_and_log_message(msg):
    logger.info(msg)

#
def step_1():
    logger.info("Cleaning attachments directory...")
    clean_command = "rm -R attachments/*"
    clean_process = subprocess.run(clean_command, shell=True, capture_output=True, text=True)
    if clean_process.returncode == 0:
        logger.info("✅ Attachments directory cleaned successfully.")
    else:
        logger.error(f"❌ Error cleaning attachments directory: {clean_process.stderr}")
    # Print step 1 completion message
    print_and_log_message("Step 1/10 completed successfully.")

#
def step_2():
    execute_script("parse_inbox.py", provided_date)
    # Print step 2 completion message
    print_and_log_message("Step 2/10 completed successfully.")

def step_3():
    execute_script("download_sheet_as_csv.py", provided_date)
    execute_script("insert_commas_to_sheet.py", provided_date)
    print_and_log_message("Step 3/10 completed successfully.")

#
def step_4():
    logger.info("Compiling C++ Executable: Construction-Summary-Engine")
    compile_command = "cmake -S . -B build; cmake --build build"
    compile_process = subprocess.run(compile_command, shell=True, capture_output=True, text=True)
    if compile_process.returncode == 0:
        logger.info("✅ Compilation completed successfully.")
    else:
        logger.error(f"❌ Compilation failed with return code {compile_process.returncode}.")
        logger.error(compile_process.stderr)
        sys.exit(1)
    # Print step 3 completion message
    print_and_log_message("Step 4/10 completed successfully.")

# 
def step_5():
    logger.info("Executing C++ Executable: Construction-Summary-Engine")
    logger.info("Routine Type: INIT")
    executable_path = get_executable_path("Construction-Summary-Engine")
    process = subprocess.Popen([executable_path, provided_date, "-init"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    for line in iter(process.stdout.readline, ''):
        print(line, end='')
        logger.info(line.strip())
    for line in iter(process.stderr.readline, ''):
        print(line, end='')
        logger.error(line.strip())
    
    process.wait()
    if process.returncode == 0:
        logger.info("✅ Construction-Summary-Engine initialization completed successfully.")
    else:
        logger.error(f"❌ Construction-Summary-Engine failed with return code {process.returncode}.")
    # Print step 4 completion message
    print_and_log_message("Step 5/10 completed successfully.")

# 
def step_6():
    logger.info("Fixing CSV format...")
    try:
        input_file = f"data/{provided_date}/clean/Units.csv"
        output_file = f"data/{provided_date}/clean/Units_clean.csv"
        command = ["python3", get_script_path("fix_csv_format.py"), input_file, output_file]
        logger.info(f"Executing: {' '.join(command)}")
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        
        for line in iter(process.stdout.readline, ''):
            print(line, end='')
            logger.info(line.strip())
        for line in iter(process.stderr.readline, ''):
            print(line, end='')
            logger.error(line.strip())
        
        process.wait()
        if process.returncode == 0:
            logger.info("✅ fix_csv_format.py completed successfully.")
        else:
            logger.error(f"❌ fix_csv_format.py failed with return code {process.returncode}.")
    except Exception as e:
        logger.error(f"❌ An error occurred during Step 5: {e}")
    # Print step 5 completion message
    print_and_log_message("Step 6/10 completed successfully.")

#
def step_7():
    logger.info("Executing C++ Executable: Construction-Summary-Engine")
    logger.info("Routine Type: RUN")
    executable_path = get_executable_path("Construction-Summary-Engine")
    process = subprocess.Popen([executable_path, provided_date, "-run"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    for line in iter(process.stdout.readline, ''):
        print(line, end='')
        logger.info(line.strip())
    for line in iter(process.stderr.readline, ''):
        print(line, end='')
        logger.error(line.strip())
    
    process.wait()
    if process.returncode == 0:
        logger.info("✅ Construction-Summary-Engine initialization completed successfully.")
    else:
        logger.error(f"❌ Construction-Summary-Engine failed with return code {process.returncode}.")
    # Print step 6 completion message
    print_and_log_message("Step 7/10 completed successfully.")

def step_8():
    extract_log_lines(provided_date)
    print_and_log_message("Step 8/10 completed successfully.")

#
def step_9():
    execute_script("sheet_injection.py", provided_date)
    # Print step 7 completion message
    print_and_log_message("Step 9/10 completed successfully.")

#
def step_10():
    execute_script("append_sheet_injection.py", provided_date)
    # Print step 7 completion message
    print_and_log_message("Step 10/10 completed successfully.")
    


# Main workflow
if __name__ == "__main__":
    try:
        # Step 1: Clean attachments directory
        step_1()

        # Step 2: Parse Gmail inbox
        step_2()

        # Step 3: Download historical sheet
        step_3()

        # Step 4: Compile C++ program (Construction-Summary-Engine)
        step_4()

        # Step 5: Run C++ program (Construction-Summary-Engine) in initialization mode
        step_5()

        # Step 6: Fix CSV format
        step_6()

        # Step 7: Run C++ program (Construction-Summary-Engine) in run mode
        step_7()

        # Step 8: Extract log changes
        step_8()

        # Step 9: Insert data to Job Costing Report's "Construction Summary" sheet
        step_9()

        # Step 10: Insert data to Job Costing Report's "Updates" sheet
        step_10()

    except Exception as e:
        logger.error(f"❌ An error occurred: {e}")