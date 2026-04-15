#include "main.hpp"

int main(int argc, const char **argv)
{
    // Configure date used during session
    configureDate(argc, argv);

    // Configure routine type used during session
    configureRoutineType(argc, argv);

    // Setup input files
    initializeInputFiles();

    mergeDataGroupings();

    // If "-init" routine type is provided, end the program here
    if (sRoutineType == "-init")
    {
        std::cout << "~ ✅ Initialization routine successful" << std::endl;
        return 0;
    }

    checkDictionaryDefinitions();

    // Declare vector of properties
    PropertyList propList;

    // Initialize property names and populate vector
    initializePropertyNames(propList);

    // Initialize dictionary IDs
    initializeDictionaryIDs(propList);

    std::string routineTimeline = "future";

    // Inject input data into PropertyList vector
    runInputRoutines(propList, routineTimeline);

    // Create output report files
    runOutputRoutines(propList, routineTimeline);

    

    return 0;
}