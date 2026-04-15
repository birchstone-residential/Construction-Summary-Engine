import os
from datetime import datetime

def rename_log_file():
    # Define the directory and log file name
    logs_dir = "logs"
    log_file = os.path.join(logs_dir, "latest.log")
    
    # Check if the logs directory exists
    if not os.path.exists(logs_dir):
        print(f"❌ The directory '{logs_dir}' does not exist.")
        return
    
    # Check if the log file exists in the logs directory
    if not os.path.exists(log_file):
        print(f"❌ The file '{log_file}' does not exist in '{logs_dir}'.")
        return
    
    # Get the creation time of the log file
    creation_time = os.path.getctime(log_file)
    timestamp = datetime.fromtimestamp(creation_time).strftime('%Y-%m-%d-%H-%M-%S')
    
    # Create the new file name
    new_log_file = os.path.join(logs_dir, f"log-{timestamp}.log")
    
    # Rename the file
    os.rename(log_file, new_log_file)
    print(f"✅ Renamed '{log_file}' to '{new_log_file}'.")

if __name__ == "__main__":
    rename_log_file()
