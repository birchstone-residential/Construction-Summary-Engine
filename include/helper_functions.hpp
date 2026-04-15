#ifndef __helper_functions__hpp__
#define __helper_functions__hpp__

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <initializer_list>
#include <algorithm>
#include <unordered_map>

static std::string sExportDate;
static std::string sExportDate_Forward_Slash_Format;
static std::string sRoutineType;

static unsigned sNumFilesGenerated = 0;
static unsigned sNumRowsProcessed = 0;

#define PRINT_FUNCTION_START() std::cout << "\033[32m" << "~ Starting" << "\e[0m" << " - " << __func__ << "()" << std::endl;
#define PRINT_FUNCTION_STOP() std::cout << "\033[31m" << "~ Stopping" << "\e[0m" << " - " << __func__ << "()" << std::endl;
#define PRINT_NUM_ITEMS_PROCESSED(x) std::cout << "~\t (" << x << ") items processed" << std::endl;
#define PRINT_WRITE_FILE_SUCCESS_MSG(x) std::cout << "~ ✅ Success writing file: \"" << x << "\"" << std::endl;
#define PRINT_WRITE_FILE_FAILURE_MSG(x) std::cout << "~ ❌ Failed writing file: \"" << x << "\"" << std::endl;

#define PRINT_IN_BOLD_RED(x) std::cout << "\033[1m" << "\033[31m" << x << "\e[0m" << std::endl;
#define PRINT_IN_BOLD_GREEN(x) std::cout << "\033[1m" << "\033[32m" << x << "\e[0m" << std::endl;
#define PRINT_IN_BOLD_YELLOW(x) std::cout << "\033[1m" << "\033[33m" << x << "\e[0m" << std::endl;

namespace fs = std::filesystem;

// Helper function to find the first .zip file in a directory
std::string findZipFile(const std::string& dir) {
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".zip") {
            return entry.path().string();  // Return the first .zip file found
        }
    }
    return "";  // Return empty string if no .zip file is found
}

// Helper function to unzip a file
bool unzipFile(const std::string& zipFilePath, const std::string& destDir) {
        // Ensure the zip file exists
    if (!fs::exists(zipFilePath)) {
        std::cerr << "ZIP file does not exist: " << zipFilePath << std::endl;
        return false;
    }

    // Create the destination directory if it doesn't exist
    if (!fs::exists(destDir)) {
        fs::create_directories(destDir);
    }

    // Use the system command to call an external unzip tool, handling spaces in filenames
    std::string command = "unzip -o \"" + zipFilePath + "\" -d \"" + destDir + "\"";
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to unzip the file. Command exited with code " << result << std::endl;
        return false;
    }

    return true;
}


void merge_csv_files(const std::string& dateStr,
    const std::string& fileAName,
    const std::string& fileBName,
    const std::string& fileCName) {
    fs::path projectRoot = fs::current_path();
    fs::path dataDir = projectRoot / "data" / dateStr / "clean";

    fs::path fileA = dataDir / fileAName;
    fs::path fileB = dataDir / fileBName;
    fs::path fileC = dataDir / fileCName;

    // Check if source files exist
    if (!fs::exists(fileA)) {
        std::cerr << "❌ File A not found: " << fileA << "\n";
        return;
    }

    if (!fs::exists(fileB)) {
        std::cerr << "❌ File B not found: " << fileB << "\n";
        return;
    }

    // Ensure destination directory exists
    if (!fs::exists(dataDir)) 
    {
        std::cout << "📁 Creating directory: " << dataDir << "\n";
        fs::create_directories(dataDir);
    }

    // Copy File A to File C
    try {
        fs::copy_file(fileA, fileC, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "❌ Failed to copy File A to File C: " << e.what() << "\n";
        return;
    }

    // Append File B content (excluding header) to File C
    std::ifstream inputB(fileB);
    std::ofstream outputC(fileC, std::ios::app);
    std::string line;
    bool isFirstLine = true;

    while (std::getline(inputB, line)) 
    {
        if (isFirstLine) 
        {
            isFirstLine = false;
            continue; // skip header
        }

        outputC << line << "\n";
    }

    std::cout << "✅ Successfully merged into: " << fileC << "\n";
}

// Helper function to get the current date in "YYYY/MM/DD" format
std::string getFormattedDate(const std::string& delimiter = "/")
{
    std::time_t t = std::time(nullptr);
    std::tm currentDate;
    #ifdef _WIN32
        localtime_s(&currentDate, &t); // For Windows
    #else
        currentDate = *std::localtime(&t); // For Unix-based systems
    #endif

    std::stringstream ss;
    ss << (1900 + currentDate.tm_year) << delimiter
       << std::setfill('0') << std::setw(2) << (1 + currentDate.tm_mon) << delimiter
       << std::setfill('0') << std::setw(2) << currentDate.tm_mday;

    return ss.str();
}

// Helper function to remove commas and parentheses, then convert to float
float parseFloatWithCommaAndParentheses(const std::string& str)
{
    std::string cleanStr = str;

    // Remove commas
    cleanStr.erase(std::remove(cleanStr.begin(), cleanStr.end(), ','), cleanStr.end());

    // If the string has parentheses (indicating negative value), handle it
    if (cleanStr.front() == '(' && cleanStr.back() == ')')
    {
        cleanStr = cleanStr.substr(1, cleanStr.size() - 2); // Remove the parentheses
        return -std::stof(cleanStr);
    }

    return std::stof(cleanStr);
}

// Helper function to remove commas and parentheses, then convert to double
double parseDoubleWithCommaAndParentheses(const std::string& str)
{
    std::string cleanStr = str;

    // Remove commas
    cleanStr.erase(std::remove(cleanStr.begin(), cleanStr.end(), ','), cleanStr.end());

    // If the string has parentheses (indicating negative value), handle it
    if (cleanStr.front() == '(' && cleanStr.back() == ')')
    {
        cleanStr = cleanStr.substr(1, cleanStr.size() - 2); // Remove the parentheses
        return -std::stod(cleanStr);
    }

    return std::stod(cleanStr);
}

// Utility to replace dots with slashes in a string
void replaceDotsWithSlashes(std::string& str) {
    for (char& ch : str) {
        if (ch == '.') {
            ch = '/';
        }
    }
}

// Utility to convert string to std::tm for dates
std::tm stotm(const std::string& dateStr, const char* format = "%Y/%m/%d") {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, format);  // Parses the date string into std::tm
    return tm;
}

// Utility to convert std::tm to string
std::string tmtostr(std::tm& timeStruct) {
    char buffer[100]; // Buffer to hold the formatted date string
    // Format the date as "YYYY/M/D"
    std::strftime(buffer, sizeof(buffer), "%Y/%m/%d", &timeStruct);
    return std::string(buffer); // Convert buffer to std::string
}

// Utility to enclose a string in double quotes
std::string encloseInQuotes(const std::string& str) {
    return "\"" + str + "\"";
}

// Utility to replace double quotes with two single quotes
std::string replaceDoubleQuotes(const std::string& input) {
    std::string result;
    result.reserve(input.size()); // Reserve space to optimize performance

    for (char ch : input) {
        if (ch == '"') {
            result += "''"; // Replace double quote with two single quotes
        } else {
            result += ch;
        }
    }
    
    return result;
}

// Helper function to remove double quotes from a string
std::string removeDoubleQuotes(const std::string& str) 
{
    std::string::size_type start = str.find_first_not_of('"');
    std::string::size_type end = str.find_last_not_of('"');

    if (start == std::string::npos || end == std::string::npos) 
    {
        return ""; // Return empty string if there are only quotes
    }

    return str.substr(start, end - start + 1);
}

// Helper function to remove commas from a string
std::string removeCommas(const std::string &str)
{
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), ','), result.end());
    return result;
}

// Helper function to remove spaces from a string
std::string removeSpaces(const std::string &str)
{
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    return result;
}

// Utility to format an integer with commas
std::string formatWithCommas(int number) {
    std::string numStr = std::to_string(number);
    int len = numStr.length();
    int commasNeeded = (len - 1) / 3;

    if (commasNeeded == 0) {
        return numStr;
    }

    std::string result = "";
    int count = 0;
    for (int i = len - 1; i >= 0; --i) {
        result += numStr[i];
        count++;
        if (count % 3 == 0 && i != 0) {
            result += ',';
        }
    }

    std::reverse(result.begin(), result.end());
    return result;
}

// Utility to check if a date is the epoch date "1900/01/00"
std::string checkIfDateIsEpoch(const std::string& str) {
    return str == "1900/01/00" ? "" : str;
}

std::string transformDatePeriodStr(const std::string& input) {
    // Map of month abbreviations to numbers
    std::unordered_map<std::string, int> monthMap = {
        {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
        {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
        {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
    };

    std::istringstream iss(input);
    std::string monthStr;
    int year;

    // Parse the month and year from the input string
    iss >> monthStr >> year;

    // Check if the month string is valid
    if (monthMap.find(monthStr) == monthMap.end()) {
        return ""; // Per Savannah King, return empty string if month is invalid
    }

    // Format the output as "YYYY/MM/DD" with leading zeros
    std::ostringstream oss;
    oss << year << "/"
        << std::setw(2) << std::setfill('0') << monthMap[monthStr] << "/"
        << "01"; // Always pad day as "01"
    
    return oss.str();
}

// Utility to check if all values in an initializer_list are the same
bool allValuesSame(std::initializer_list<int> values) {
    if (values.size() < 2) return true; // If there's 0 or 1 element, they are "the same" by default.

    auto first = *values.begin(); // Get the first element

    for (int value : values) {
        if (value != first) {
            return false; // Found a different value
        }
    }

    return true; // All values matched
}

int daysBetween(const std::tm& date1, const std::tm& date2) {
    // Convert std::tm to time_t
    std::time_t time1 = std::mktime(const_cast<std::tm*>(&date1));
    std::time_t time2 = std::mktime(const_cast<std::tm*>(&date2));

    // Compute difference in seconds and convert to days
    return std::difftime(time2, time1) / (60 * 60 * 24);
}

bool isBeforeOrEqual(const tm& date1, const tm& date2) {
    time_t t1 = mktime(const_cast<tm*>(&date1));
    time_t t2 = mktime(const_cast<tm*>(&date2));
    return difftime(t1, t2) <= 0;
}

bool isAfterOrEqual(const tm& date1, const tm& date2) {
    time_t t1 = mktime(const_cast<tm*>(&date1));
    time_t t2 = mktime(const_cast<tm*>(&date2));
    return difftime(t1, t2) >= 0;
}

bool isBefore(const tm& date1, const tm& date2) {
    time_t t1 = mktime(const_cast<tm*>(&date1));
    time_t t2 = mktime(const_cast<tm*>(&date2));
    return difftime(t1, t2) < 0;
}

bool isAfter(const tm& date1, const tm& date2) {
    time_t t1 = mktime(const_cast<tm*>(&date1));
    time_t t2 = mktime(const_cast<tm*>(&date2));
    return difftime(t1, t2) > 0;
}

std::string booltostr(bool booleanValue)
{
    return (booleanValue) ? "True" : "False";
}

#endif // __helper_functions__hpp__