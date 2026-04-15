#ifndef __main__hpp__
#define __main__hpp__

#include "helper_functions.hpp"
#include "input_routines.hpp"
#include "output_routines.hpp"
#include "dictionaries.hpp"
#include <functional>
#include <regex>

#pragma region InitializeInputFiles

void checkDictionaryDefinitions() noexcept(false)
{
    int numPropertiesDefined = static_cast<unsigned>(PropertyNameDictionary::Count);
    int numPropertiesInNameMap = propertyNameMap.size();
    int numberPropertiesInEntrataNameMap = entrataNameMap.size();

    std::cout << "~ Number of properties defined in dictionary: \t\t (" << numPropertiesDefined - 1 << ")" << std::endl;
    std::cout << "~ Number of properties in propertyNameMap: \t\t (" << numPropertiesInNameMap - 1 << ")" << std::endl;
    std::cout << "~ Number of properties in entrataNameMap: \t\t (" << numberPropertiesInEntrataNameMap - 1 << ")" << std::endl;
    
    if (allValuesSame({numPropertiesDefined,numPropertiesInNameMap,numberPropertiesInEntrataNameMap}))
    {
        std::cout << "~ 🟢 All properties appear defined, but this is a soft check" << std::endl;
    }
    else
    {
        std::cout << "~ 🟡 Some properties are not fully defined in dictionaries" << std::endl;
    }
}

bool ifDateProvidedInCommandLineArguements(int argc, const char **argv)
{
    if ( argc > 1 )
    {
        std::string input = argv[1];

        // Define a regular expression for the YYYY.MM.DD format
        std::regex datePattern(R"(^\d{4}\.\d{2}\.\d{2}$)");

        // Check if the input matches the pattern
        return (std::regex_match(input, datePattern));
    }

    return false;
}

bool ifRoutineTypeProvidedInCommandLineArguements(int argc, const char **argv)
{
    if ( argc > 2 )
    {
        std::string input = argv[2];

        if (input == "-init" || input == "-run" )
        {
            return true;   
        }
        else
        {
            std::cout << "~ Invalid routine type provided: \t (" << input << ")" << std::endl;
            std::cout << "~ Valid options are: \t\t\t ('-init', '-run')" << std::endl;
        }
    }
    else
    {
        std::cout << "~ No routine type provided" << std::endl;
        std::cout << "~ Valid options are: \t\t\t ('-init', '-run')" << std::endl;
    }

    return false;
}

void configureDate(int argc, const char **argv) noexcept(false)
{
    if ( ifDateProvidedInCommandLineArguements(argc, argv) )
    {
        std::string customDate = argv[1];
        std::cout << "~ Custom date provided: \t (" << customDate << ")" << std::endl;

        sExportDate = customDate;
        replaceDotsWithSlashes(customDate);
        sExportDate_Forward_Slash_Format = customDate;
    }
    else
    {
        sExportDate = getFormattedDate(".");
        std::string date = sExportDate;
        replaceDotsWithSlashes(date);
        sExportDate_Forward_Slash_Format = date;
    }
}

void configureRoutineType(int argc, const char **argv) noexcept(false)
{
    if ( ifRoutineTypeProvidedInCommandLineArguements(argc, argv) )
    {
        std::string input = argv[2];
        std::cout << "~ Routine type provided: \t (" << input << ")" << std::endl;

        sRoutineType = input;
    }
    else
    {
        throw std::runtime_error("Invalid routine type provided");
    }
}

void inflateAttachmentsDir() noexcept(false) 
{
    std::string attachmentsDir = "attachments";
    std::string destDir = "data/" + sExportDate + "/raw";

    // Find the ZIP file in the attachments directory
    std::string zipFilePath = findZipFile(attachmentsDir);
    if (zipFilePath.empty()) {
        throw std::runtime_error("No ZIP file found in the attachments directory.");
    }

    // Unzip the file
    if (unzipFile(zipFilePath, destDir)) {
        std::cout << "Unzipped successfully!" << std::endl;
    } else {
        throw std::runtime_error("Failed to unzip the file.");
    }
}

bool copyCsvFiles(const std::string& sourceDir, const std::string& destDir) 
{
    // Ensure the source directory exists
    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        std::cerr << "Source directory does not exist or is not a directory: " << sourceDir << std::endl;
        return false;
    }

    // Create the destination directory if it doesn't exist
    if (!fs::exists(destDir)) {
        fs::create_directories(destDir);
    }

    // Iterate through the source directory
    for (const auto& entry : fs::directory_iterator(sourceDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            try {
                // Construct the destination file path
                fs::path destFilePath = fs::path(destDir) / entry.path().filename();
                std::string filename = entry.path().filename();
                std::string newFilePath = "";
                
                if ( filename.find("Unit Expenses - Group") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Unit Expenses - Group.csv";
                }
                else if ( filename.find("Unit Expenses - 20th") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Unit Expenses - 20th.csv";
                }
                else if ( filename.find("Excluded Units 2024") != SIZE_MAX  ||  filename.find("Availability") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Units.csv";
                }
                else if ( filename.find("Amenities") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Amenities.csv";
                }
                else if ( filename.find("Lease Trade-out") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Lease Trade-out.csv";
                }
                else if ( filename.find("Rolling Renovation") != SIZE_MAX ) { 
                    newFilePath = destDir + "/" + "Rolling Renovation.csv";
                }

                if ( newFilePath.size() ) {
                    // Copy the file to the destination directory
                    fs::copy_file(entry.path(), newFilePath, fs::copy_options::overwrite_existing);
                    std::cout << "Copied " << entry.path() << " to " << newFilePath << std::endl;
                }
                
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error copying file " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }

    return true;
}

void cleanFiles() 
{
    std::string srcDir = "data/" + sExportDate + "/raw";
    std::string destDir = "data/" + sExportDate + "/clean";

    copyCsvFiles(srcDir, destDir);
}

void initializeInputFiles() 
{
    std::string path = "data/" + sExportDate + "/clean";
    std::cout << "~ Generating for:\t\t (" << sExportDate << ")" << std::endl;

    if ( fs::exists(path) ) {
        std::cout << "~ Bypassing inflation and clean: (Files exist)" << std::endl;
        return;
    }

    try {
        inflateAttachmentsDir();
        cleanFiles();
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void mergeDataGroupings()
{
    std::string dateStr = sExportDate;
    std::cout << "~ Merging data groupings for date: (" << dateStr << ")" << std::endl;

    // Define the pairs of files to merge
    std::vector<std::tuple<std::string, std::string, std::string>> fileTuples = {
        {"Unit Expenses - 20th.csv", "Unit Expenses - Group.csv", "Unit Expenses (Merged).csv"}
    };

    for (const auto& tuple : fileTuples) {
        merge_csv_files(dateStr, std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
    }
}

void copyDownloadedConstructionSummary()
{
    std::string dir = "data/" + sExportDate + "/" + "_history/";
    fs::create_directories(dir);

    std::string srcPath = "attachments/downloaded_sheet_modified.csv";
    std::string destPath = dir + "Historical.csv";

    fs::copy_file(srcPath, destPath, fs::copy_options::overwrite_existing);
}

#pragma endregion // InitializeInputFiles

#pragma region InitializePropertyList

void initializePropertyNames(PropertyList& propList) noexcept(false) 
{
    std::cout << "~ Initializing Property Names:   (" << static_cast<unsigned>(PropertyNameDictionary::Count) - 1 << ") properties defined" << std::endl;
    for (int i = 1; i < static_cast<unsigned>(PropertyNameDictionary::Count); ++i) 
    {
        Property property;
        property.__name_ = propertyNameMap[static_cast<PropertyNameDictionary>(i)];
        property.__entrata_name_ = entrataNameMap[static_cast<PropertyNameDictionary>(i)];
        propList().push_back(property);
    }

    std::cout << "~ Initialization Complete:\t (" << propList().size() << ") properties created" << std::endl;
}

void initializeDictionaryIDs(PropertyList& propList) noexcept(false)
{
    std::cout << "~ Initializing Dictionary IDs" << std::endl;
    for (auto &prop: propList()) 
    {
        for (int i = 0; i < static_cast<unsigned>(PropertyNameDictionary::Count); i++)
        {
            if (prop.__name_ == propertyNameMap[static_cast<PropertyNameDictionary>(i)])
            {
                prop.__id_ = i;
                //std::cout << "~ Assigned ID: (" << prop.__id_ << ") to " << prop.__name_ << std::endl;
            }
        }
    }
}

#pragma endregion // InitializePropertyList

#pragma region RunInputRoutines

void runInputRoutines(PropertyList& propList, std::string routineTimeline) noexcept(false)
{
    copyDownloadedConstructionSummary();

    ///< TODO:
    inputAmenities(propList);
    inputUnits(propList);
    assignAmenityCosts(propList);
    inputUnitExpenses(propList);
    inputRollingRenovation(propList);
    inputLeaseTradeOut(propList);

    if (routineTimeline == "historical")
    {
        inputMasterTrackerData(propList);

        createJobCostingReport_Historical_New(propList);
        inputJobCostingReport_Historical(propList);
    }
    else if (routineTimeline == "future")
    {
        inputConstructionSummaryHistorical(propList);

        createJobCostingReport(propList);
        inputJobCostingReport(propList);   
    }

    calculateJobExpenseTotals(propList);


    // Print summary of files generated
    std::cout << formatWithCommas(sNumRowsProcessed) << " rows processed" << std::endl;
}

#pragma endregion // RunInputRoutines

#pragma region RunOutputRoutines

void runOutputRoutines(PropertyList& propList, std::string routineTimeline) noexcept(false)
{
    // Create output directory
    createOutputDirectory();

    // Write output files
    
    if (routineTimeline == "historical")
    {
        writeConstructionSummary_Historical(propList);
    }
    else if (routineTimeline == "future")
    {
        writeConstructionSummary(propList);
    }

    // Print summary of files generated
    std::cout << sNumFilesGenerated << " files generated" << std::endl;
}

#pragma endregion // RunOutputRoutines

#endif // __main__hpp__