import logging
import logging.config
import os

def setup_logging(log_file='latest.log', level=logging.INFO):
   # Ensure the logs directory exists
   logs_dir = "logs"
   os.makedirs(logs_dir, exist_ok=True)
   
   # Set the full path for the log file
   log_file_path = os.path.join(logs_dir, log_file)

   logging_config = {
       'version': 1,
       'disable_existing_loggers': False,
       'formatters': {
           'detailed': {
               'format': '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
           },
           'simple': {
               'format': '%(levelname)s - %(message)s'
           },
       },
       'handlers': {
           'console': {
               'class': 'logging.StreamHandler',
               'formatter': 'simple',
               'level': level,
           },
           'file': {
               'class': 'logging.FileHandler',
               'filename': log_file_path,
               'formatter': 'detailed',
               'level': level,
           },
       },
       'root': {
           'handlers': ['console', 'file'],
           'level': level,
       },
   }
   logging.config.dictConfig(logging_config)

# Automatically set up logging when this file is imported
setup_logging()
