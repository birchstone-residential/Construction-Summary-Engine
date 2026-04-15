#ifndef __input_routines__hpp__
#define __input_routines__hpp__

#include "PropertyList.hpp"
#include "helper_functions.hpp"
#include "rapidcsv.h"
#include "dictionaries.hpp"

std::string getGLCategory(std::string glAccountStr)
{
    std::string glCodeStr = glAccountStr.substr(0, 4);
    for (int i = 0; i < static_cast<unsigned>(GLCodeDictionary::COUNT); i++)
    {
        if (glCodeStr == GLCodeMap[static_cast<GLCodeDictionary>(i)])
        {
            //std::cout << "GL Code " << glCodeStr << " maps to category "<< GLCategoryMap[static_cast<GLCodeDictionary>(i)] << std::endl;
            return GLCategoryMap[static_cast<GLCodeDictionary>(i)];
        }
    }

    return static_cast<std::string>(GLCategoryMap[GLCodeDictionary::NONE]);
}

void calculateJobExpenseTotals(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    for (auto &prop: propList())
    {
        for (auto &unit: prop.__units_)
        {
            for (auto &item: prop.__unit_expenses_)
            {
                if (item.__bldg_unit_ == unit.__bldg_unit_)
                {
                    std::string jobCostCodeStr = item.__job_cost_code_;
                    std::string jobCostCode = getGLCategory(jobCostCodeStr);

                    //std::cout << glCategory << std::endl;

                    for (int i = 0; i < static_cast<unsigned>(JobTypeDictionary::COUNT); i++)
                    {   
                        if (jobCostCode == JobTypeMap[static_cast<JobTypeDictionary>(i)])
                        {
                            switch(static_cast<JobTypeDictionary>(i))
                            {
                                case JobTypeDictionary::RENO_KITS:
                                    unit.__totals_.__reno_kits_ += item.__amount_;
                                    break;
                                
                                case JobTypeDictionary::APPLIANCES:
                                    unit.__totals_.__appliances_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::PAINT:
                                    unit.__totals_.__paint_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::FLOORING:
                                    unit.__totals_.__flooring_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::LABOR:
                                    unit.__totals_.__labor_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::WASHER_AND_DRYER:
                                    unit.__totals_.__washer_and_dryer_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::BACKSPLASH:
                                    unit.__totals_.__backsplash_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::COUNTERTOPS:
                                    unit.__totals_.__countertops_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::CABINETS:
                                    unit.__totals_.__cabinets_ += item.__amount_;
                                    break;

                                case JobTypeDictionary::OTHER:
                                    unit.__totals_.__other_ += item.__amount_;
                                    break;

                                default:
                                    unit.__totals_.__undefined_ += item.__amount_;
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Calculate sum of all categories
    for (auto &prop: propList())
    {
        for (auto &unit: prop.__units_)
        {
            unit.__totals_.__sum_ = unit.__totals_.__reno_kits_ +
                                    unit.__totals_.__appliances_ + 
                                    unit.__totals_.__paint_ + 
                                    unit.__totals_.__flooring_ + 
                                    unit.__totals_.__labor_ + 
                                    unit.__totals_.__washer_and_dryer_ + 
                                    unit.__totals_.__backsplash_ + 
                                    unit.__totals_.__countertops_ + 
                                    unit.__totals_.__cabinets_ +
                                    unit.__totals_.__other_;
        }
    }

    PRINT_FUNCTION_STOP();
}

void inputUnitExpenses(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Unit Expenses (Merged).csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of UnitExpenses structs to store the results
    std::vector<UnitExpenses> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        UnitExpenses data;

        data.__property_             = doc.GetCell<std::string>("Property", i);
        data.__transaction_type_     = doc.GetCell<std::string>("Transaction Type", i); 
        data.__budget_               = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Budget", i));
        data.__transaction_number_   = doc.GetCell<std::string>("Transaction #", i);
        data.__vendor_               = doc.GetCell<std::string>("Vendor", i);
        data.__vendor_code_          = doc.GetCell<std::string>("Vendor Code", i); 
        data.__bldg_unit_            = doc.GetCell<std::string>("BLDG-Unit", i); 
        data.__post_month_           = doc.GetCell<std::string>("Post Month", i);
        
        if ( doc.GetCell<std::string>("Post Date", i) != "" )
            data.__post_date_ = stotm(doc.GetCell<std::string>("Post Date", i), "%m/%d/%Y");
        else
            data.__post_date_str_ = "";

        if ( doc.GetCell<std::string>("Transaction Date", i) != "" )
            data.__transaction_date_ = stotm(doc.GetCell<std::string>("Transaction Date", i), "%m/%d/%Y");
        else
            data.__transaction_date_str_ = "";

        if ( doc.GetCell<std::string>("Completion Date", i) != "" )
            data.__completion_date_ = stotm(doc.GetCell<std::string>("Completion Date", i), "%m/%d/%Y");
        else
            data.__completion_date_str_ = "";

        data.__gl_account_           = doc.GetCell<std::string>("GL Account", i);
        data.__job_cost_code_        = doc.GetCell<std::string>("Job Cost Code", i);
        data.__job_                  = doc.GetCell<std::string>("Job", i);
        data.__description_          = doc.GetCell<std::string>("Description", i);
        data.__amount_               = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Amount", i));

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__unit_expenses_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

// Read "Lease Trade-out.csv" and inject into PropertyList
void inputLeaseTradeOut(PropertyList& propList) noexcept(false)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Lease Trade-out.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of Availability structs to store the results
    std::vector<LeaseTradeOut> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        LeaseTradeOut data;

        data.__property_ = doc.GetCell<std::string>("Property", i);
        data.__bldg_unit_ = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__unit_ = doc.GetCell<std::string>("Unit", i);
        data.__skip_mtm_ = doc.GetCell<std::string>("Skip MTM?", i);
        data.__unit_type_ = doc.GetCell<std::string>("Unit Type", i);
        data.__sqft_ = doc.GetCell<std::string>("SQFT", i);
        
        if (doc.GetCell<std::string>("Days Vacant", i) != "")
            data.__days_vacant_ = doc.GetCell<int>("Days Vacant", i);
        else
            data.__days_vacant_str_ = "";

        data.__prior_resident_ = doc.GetCell<std::string>("Prior Resident", i);
        
        if (doc.GetCell<std::string>("Move-In", i) != "")
            data.__move_in_ = stotm(doc.GetCell<std::string>("Move-In", i), "%m/%d/%Y");
        else
            data.__move_in_str_ = "";

        data.__current_resident_ = doc.GetCell<std::string>("Current Resident", i);
        data.__current_lease_type_ = doc.GetCell<std::string>("Current Lease Type", i);
        data.__prior_lease_start_ = stotm(doc.GetCell<std::string>("Prior Lease Start", i), "%m/%d/%Y");
        data.__prior_lease_end_ = stotm(doc.GetCell<std::string>("Prior Lease End", i), "%m/%d/%Y");
        data.__current_lease_start_ = stotm(doc.GetCell<std::string>("Current Lease Start", i), "%m/%d/%Y");
        data.__current_lease_end_ = stotm(doc.GetCell<std::string>("Current Lease End", i), "%m/%d/%Y");
        
        if (doc.GetCell<std::string>("Prior Base Rent", i) != "")
            data.__prior_base_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Base Rent", i));
        else
            data.__prior_base_rent_str_ = "";

        if (doc.GetCell<std::string>("Prior Amenity Rent", i) != "")
            data.__prior_amenity_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Amenity Rent", i));
        else
            data.__prior_amenity_rent_str_ = "";

        if (doc.GetCell<std::string>("Prior Lease Rent", i) != "")
            data.__prior_lease_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Lease Rent", i));
        else
            data.__prior_lease_rent_str_ = "";

        if (doc.GetCell<std::string>("Current Base Rent", i) != "")
            data.__current_base_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Base Rent", i));
        else
            data.__current_base_rent_str_ = "";

        if (doc.GetCell<std::string>("Current Amenity Rent", i) != "")
            data.__current_amenity_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Amenity Rent", i));
        else
            data.__current_amenity_rent_str_ = "";

        if (doc.GetCell<std::string>("Current Lease Rent", i) != "")
            data.__current_lease_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Lease Rent", i));
        else
            data.__current_lease_rent_str_ = "";

        if (doc.GetCell<std::string>("Market Rent", i) != "")
            data.__market_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Market Rent", i));
        else
            data.__market_rent_str_ = "";

        if (doc.GetCell<std::string>("Budgeted Rent", i) != "")
            data.__budgeted_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Budgeted Rent", i));
        else
            data.__budgeted_rent_str_ = "";

        if (doc.GetCell<std::string>("Lease Rent Change ($)", i) != "")
            data.__lease_rent_change_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Lease Rent Change ($)", i));
        else
            data.__lease_rent_change_str_ = "";
        
        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__lease_trade_out_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

// Read "Rolling Renovation.csv" and inject into PropertyList
void inputRollingRenovation(PropertyList& propList) noexcept(false)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Rolling Renovation.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of Availability structs to store the results
    std::vector<RollingRenovation> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        RollingRenovation data;

        data.__property_                   = doc.GetCell<std::string>("Property", i);
        data.__bldg_unit_                  = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__unit_type_                  = doc.GetCell<std::string>("Unit Type", i);
        data.__sqft_                       = doc.GetCell<std::string>("SQFT", i);
        data.__unit_status_                = doc.GetCell<std::string>("Unit Status", i);
        data.__renovation_status_          = doc.GetCell<std::string>("Renovation Status", i);
        data.__phase_                      = doc.GetCell<std::string>("Phase", i);
        data.__move_out_date_              = stotm(doc.GetCell<std::string>("Move-Out Date", i), "%m/%d/%Y");

        if (doc.GetCell<std::string>("Renovation Start Date", i) != "")
            data.__renovation_start_date_ = stotm(doc.GetCell<std::string>("Renovation Start Date", i), "%m/%d/%Y");
        else
            data.__renovation_start_date_str_ = "";
        
        if (doc.GetCell<std::string>("Renovation Completion Date", i) != "")
            data.__renovation_completion_date_ = stotm(doc.GetCell<std::string>("Renovation Completion Date", i), "%m/%d/%Y");
        else
            data.__renovation_completion_date_str_ = "";

        if (doc.GetCell<std::string>("Actual Renovation Days", i) != "")
            data.__actual_renovation_days_ = doc.GetCell<int>("Actual Renovation Days", i);
        else
            data.__actual_renovation_days_str_ = "";

        if (doc.GetCell<std::string>("Move-In Date", i) != "")
            data.__move_in_date_ = stotm(doc.GetCell<std::string>("Move-In Date", i), "%m/%d/%Y");
        else
            data.__move_in_date_str_ = "";

        data.__actual_renovation_cost_     = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual Renovation Cost", i));
        data.__budgeted_renovation_cost_   = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Budgeted Renovation Cost", i));
        data.__actual_vs_budget_variance_  = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual vs Budget Variance", i));
        data.__monthly_unit_premium_       = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Monthly Unit Premium", i));
        data.__monthly_roi_                = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Monthly ROI", i));

        data.__prior_budgeted_rent_ = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Budgeted Rent", i));
        data.__new_budgeted_rent_   = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("New Budgeted Rent", i));
        
        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__rolling_renovation_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputJobCostingReport_Historical(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Job Costing Report (Historical).csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of JobCostingHistorical structs to store the results
    std::vector<JobCostingHistorical> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        JobCostingHistorical data;

        data.__property_                     = doc.GetCell<std::string>("Property", i);
                
        data.__bldg_unit_                    = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__unit_type_                    = doc.GetCell<std::string>("Unit Type", i);
        data.__unit_status_                  = doc.GetCell<std::string>("Unit Status", i);
        data.__reno_status_                  = doc.GetCell<std::string>("Reno Status", i);
        data.__phase_                        = doc.GetCell<std::string>("Phase", i);
        data.__move_out_date_                = stotm(doc.GetCell<std::string>("Move-Out Date", i), "%Y/%m/%d");
        data.__reno_begin_                   = stotm(doc.GetCell<std::string>("Reno Begin", i), "%Y/%m/%d");
        data.__reno_end_                     = stotm(doc.GetCell<std::string>("Reno End", i), "%Y/%m/%d");
        data.__days_in_reno_                 = doc.GetCell<int>("Days In Reno", i);
        data.__move_in_date_                 = stotm(doc.GetCell<std::string>("Move-In Date", i), "%Y/%m/%d");
        data.__actual_reno_cost_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual Reno Cost", i));
        data.__budgeted_reno_cost_           = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Budgeted Reno Cost", i));
        data.__actual_vs_budget_variance_    = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual vs Budget Variance", i));
        data.__prior_lease_rent_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Lease Rent", i));
        data.__current_lease_rent_           = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Lease Rent", i));
        data.__prior_premium_                = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Premium", i));
        data.__current_premium_              = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Premium", i));
        data.__premium_variance_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Premium Variance", i));
        data.__annual_premium_               = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Annual Premium", i));
        data.__roi_                          = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("ROI", i));
        //data.__prior_market_rent_classic_    = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Market Rent (Classic)", i));
        //data.__current_market_rent_classic_  = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Market Rent (Classic)", i));
        //data.__current_market_rent_premium_  = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Market Rent (Premium)", i));
        data.__prior_budgeted_rent_          = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Budgeted Rent", i));
        data.__new_budgeted_rent_            = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("New Budgeted Rent", i));

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__job_costing_historical_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputJobCostingReport(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Job Costing Report.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of JobCostingHistorical structs to store the results
    std::vector<JobCosting> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        JobCosting data;

        data.__property_                     = doc.GetCell<std::string>("Property", i);
                
        data.__bldg_unit_                    = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__unit_type_                    = doc.GetCell<std::string>("Unit Type", i);
        data.__unit_status_                  = doc.GetCell<std::string>("Unit Status", i);
        data.__reno_status_                  = doc.GetCell<std::string>("Reno Status", i);
        data.__phase_                        = doc.GetCell<std::string>("Phase", i);
        data.__move_out_date_                = stotm(doc.GetCell<std::string>("Move-Out Date", i), "%Y/%m/%d");
        data.__reno_begin_                   = stotm(doc.GetCell<std::string>("Reno Begin", i), "%Y/%m/%d");
        data.__reno_end_                     = stotm(doc.GetCell<std::string>("Reno End", i), "%Y/%m/%d");
        data.__days_in_reno_                 = doc.GetCell<int>("Days In Reno", i);
        data.__move_in_date_                 = stotm(doc.GetCell<std::string>("Move-In Date", i), "%Y/%m/%d");
        data.__actual_reno_cost_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual Reno Cost", i));
        data.__budgeted_reno_cost_           = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Budgeted Reno Cost", i));
        data.__actual_vs_budget_variance_    = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Actual vs Budget Variance", i));
        data.__prior_lease_rent_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Lease Rent", i));
        data.__current_lease_rent_           = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Lease Rent", i));
        data.__prior_premium_                = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Premium", i));
        data.__current_premium_              = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Current Premium", i));
        data.__premium_variance_             = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Premium Variance", i));
        data.__annual_premium_               = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Annual Premium", i));
        data.__roi_                          = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("ROI", i));
        data.__prior_budgeted_rent_          = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("Prior Budgeted Rent", i));
        data.__new_budgeted_rent_            = parseFloatWithCommaAndParentheses(doc.GetCell<std::string>("New Budgeted Rent", i));

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__job_costing_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputUnits(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Units_clean.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of Unit structs to store the results
    std::vector<Unit> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        Unit data;

        data.__property_                     = doc.GetCell<std::string>("Property", i);
        data.__bldg_unit_                    = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__floorplan_                    = doc.GetCell<std::string>("Floor Plan", i);
        data.__unit_type_                    = doc.GetCell<std::string>("Unit Type", i);
        data.__unit_status_                  = doc.GetCell<std::string>("Unit Status", i);

        if ( doc.GetCell<std::string>("Move-Out", i) != "" )
            data.__move_out_ = stotm(doc.GetCell<std::string>("Move-Out", i), "%m/%d/%Y");
        else
            data.__move_out_str = "";

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__units_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputMasterTrackerData(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/FOUNDATION/Master Tracker Data.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of MasterTrackerData structs to store the results
    std::vector<MasterTrackerData> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        MasterTrackerData data;

        data.__property_                     = doc.GetCell<std::string>("Property", i);
        data.__bldg_unit_                    = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__unit_only_                    = doc.GetCell<std::string>("Unit Only", i);
        
        if ( doc.GetCell<std::string>("Date Started", i) != "" )
            data.__date_started_ = stotm(doc.GetCell<std::string>("Date Started", i), "%m/%d/%Y");
        else
            data.__date_started_str_ = "";

        if ( doc.GetCell<std::string>("Date Complete", i) != "" )
            data.__date_complete_ = stotm(doc.GetCell<std::string>("Date Complete", i), "%m/%d/%Y");
        else
            data.__date_complete_str_ = "";

        std::string str = doc.GetCell<std::string>("Budget", i);
        str.erase(std::remove(str.begin(), str.end(), '$'), str.end());
        //std::cout << str << std::endl;
        if ( str.size() )
        {
            data.__budget_ = parseFloatWithCommaAndParentheses(str);
        }
        else
        {
            data.__budget_ = 0.f;
        }

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__master_tracker_data_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputAmenities(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/clean/Amenities.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of structs to store the results
    std::vector<Amenities> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        Amenities data;

        data.__property_        = doc.GetCell<std::string>("Property", i);
        data.__amenity_         = doc.GetCell<std::string>("Amenity", i);
        data.__floorplan_       = doc.GetCell<std::string>("Floor Plan", i);
        data.__unit_type_       = doc.GetCell<std::string>("Unit Type", i);
        data.__unit_space_      = doc.GetCell<std::string>("Unit Space", i);
        data.__amenity_rent_    = parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Amenity Rent", i));
        data.__charge_code_     = doc.GetCell<std::string>("Charge Code", i);

        if ( doc.GetCell<std::string>("Effective Date", i) != "" )
            data.__effective_date_ = stotm(doc.GetCell<std::string>("Effective Date", i), "%m/%d/%Y");
        else
            data.__effective_date_str_ = "";

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__amenities_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

void inputConstructionSummaryHistorical(PropertyList& propList)
{
    PRINT_FUNCTION_START();

    static const std::string FILENAME = "data/" + sExportDate + "/_history/Historical.csv";

    // Load the CSV file into a rapidcsv::Document
    rapidcsv::Document doc(FILENAME);

    // Get the number of rows (excluding the header)
    int numRows = doc.GetRowCount();

    // Create a vector of structs to store the results
    std::vector<ConstructionSummaryEntry> vect;

    // Loop through each row and extract the data
    for (int i = 0; i < numRows; i++)
    {
        ConstructionSummaryEntry data;

        data.__property_        = doc.GetCell<std::string>("Property", i);
        
        data.__bldg_unit_       = doc.GetCell<std::string>("Bldg-Unit", i);
        data.__floorplan_       = doc.GetCell<std::string>("Floorplan", i);
        data.__unit_type_       = doc.GetCell<std::string>("Unit Type", i);
        data.__status_          = doc.GetCell<std::string>("Status", i);

        // Job
        data.__job_ = doc.GetCell<std::string>("Job", i);

        // Move-Out Date
        if ( doc.GetCell<std::string>("Move-Out Date", i) != "" )
            data.__move_out_date_ = stotm(doc.GetCell<std::string>("Move-Out Date", i), "%Y/%m/%d");
        else
            data.__move_out_date_str_ = "";

        // Reno Begin
        if ( doc.GetCell<std::string>("Reno Begin", i) != "" )
            data.__reno_begin_ = stotm(doc.GetCell<std::string>("Reno Begin", i), "%Y/%m/%d");
        else
            data.__reno_begin_str_ = "";

        // Reno End
        if ( doc.GetCell<std::string>("Reno End", i) != "" )
            data.__reno_end_ = stotm(doc.GetCell<std::string>("Reno End", i), "%Y/%m/%d");
        else
            data.__reno_end_str_ = "";
        
        // Days In Reno
        if ( doc.GetCell<std::string>("Days In Reno", i) != "" )
            data.__days_in_reno_ = doc.GetCell<int>("Days In Reno", i);
        else
            data.__days_in_reno_ = 0;
        
        // Move-In Date
        if ( doc.GetCell<std::string>("Move-In Date", i) != "" )
            data.__move_in_date_ = stotm(doc.GetCell<std::string>("Move-In Date", i), "%Y/%m/%d");
        else
            data.__move_in_date_str_ = "";

        data.__prior_lease_rent_                    = ( doc.GetCell<std::string>("Prior Lease Rent", i) != "" )     ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Prior Lease Rent", i))       : 0.f;
        data.__current_lease_rent_                  = ( doc.GetCell<std::string>("Current Lease Rent", i) != "" )   ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Current Lease Rent", i))     : 0.f;
        data.__prior_premium_                       = ( doc.GetCell<std::string>("Prior Premium", i) != "" )        ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Prior Premium", i))          : 0.f;
        data.__current_premium_                     = ( doc.GetCell<std::string>("Current Premium", i) != "" )      ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Current Premium", i))        : 0.f;
        data.__premium_variance_                    = ( doc.GetCell<std::string>("Premium Variance", i) != "" )     ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Premium Variance", i))       : 0.f;
        data.__annual_premium_                      = ( doc.GetCell<std::string>("Annual Premium", i) != "" )       ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Annual Premium", i))         : 0.f;
        data.__roi_                                 = ( doc.GetCell<std::string>("ROI", i) != "" )                  ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("ROI", i))                    : 0.f;
        data.__prior_budgeted_rent_                 = ( doc.GetCell<std::string>("Prior Budgeted Rent", i) != "" )  ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Prior Budgeted Rent", i))    : 0.f;
        data.__new_budgeted_rent                    = ( doc.GetCell<std::string>("New Budgeted Rent", i) != "" )    ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("New Budgeted Rent", i))      : 0.f;
        data.__rent_increase_                       = ( doc.GetCell<std::string>("Rent Increase", i) != "" )        ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Rent Increase", i))          : 0.f;
        data.__budgeted_reno_cost_                  = ( doc.GetCell<std::string>("Budgeted Reno Cost", i) != "" )   ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Budgeted Reno Cost", i))     : 0.f;
        data.__amenity_cost_WD_                     = ( doc.GetCell<std::string>("Amenity Cost - W&D", i) != "" )   ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Amenity Cost - W&D", i))     : 0.f;
        data.__amenity_cost_RENO_                   = ( doc.GetCell<std::string>("Amenity Cost - Reno", i) != "" )  ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Amenity Cost - Reno", i))    : 0.f;
        data.__job_expenses_.__reno_kits_           = ( doc.GetCell<std::string>("Reno Kits", i) != "" )            ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Reno Kits", i))              : 0.f;
        data.__job_expenses_.__appliances_          = ( doc.GetCell<std::string>("Appliances", i) != "" )           ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Appliances", i))             : 0.f;
        data.__job_expenses_.__paint_               = ( doc.GetCell<std::string>("Paint", i) != "" )                ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Paint", i))                  : 0.f;
        data.__job_expenses_.__flooring_            = ( doc.GetCell<std::string>("Flooring", i) != "" )             ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Flooring", i))               : 0.f;
        data.__job_expenses_.__labor_               = ( doc.GetCell<std::string>("Labor", i) != "" )                ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Labor", i))                  : 0.f;
        data.__job_expenses_.__washer_and_dryer_    = ( doc.GetCell<std::string>("Washer & Dryer", i) != "" )       ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Washer & Dryer", i))         : 0.f;
        data.__job_expenses_.__backsplash_          = ( doc.GetCell<std::string>("Backsplash", i) != "" )           ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Backsplash", i))             : 0.f;
        data.__job_expenses_.__countertops_         = ( doc.GetCell<std::string>("Countertops", i) != "" )          ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Countertops", i))            : 0.f;
        data.__job_expenses_.__cabinets_            = ( doc.GetCell<std::string>("Cabinets", i) != "" )             ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Cabinets", i))               : 0.f;
        data.__job_expenses_.__other_               = ( doc.GetCell<std::string>("Other", i) != "" )                ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Other", i))                  : 0.f;
        data.__job_expenses_.__undefined_           = ( doc.GetCell<std::string>("Undefined", i) != "" )            ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Undefined", i))              : 0.f;
        data.__job_expenses_.__sum_                 = ( doc.GetCell<std::string>("Total Actual", i) != "" )         ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Total Actual", i))           : 0.f;
        data.__savings_dollar_                      = ( doc.GetCell<std::string>("Savings ($)", i) != "" )          ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Savings ($)", i))            : 0.f;
        data.__savings_percent_                     = ( doc.GetCell<std::string>("Savings (%)", i) != "" )          ? parseDoubleWithCommaAndParentheses(doc.GetCell<std::string>("Savings (%)", i))            : 0.f;
        
        // Is Renovated?
        if (data.__reno_begin_str_ != "" && data.__reno_end_str_ != "")
        {
            data.__is_renovated_ = true;
        }

        // Entry Update Date
        if ( doc.GetCell<std::string>("Entry Update Date", i) != "" )
            data.__entry_update_date_ = stotm(doc.GetCell<std::string>("Entry Update Date", i), "%Y/%m/%d");
        else
            data.__entry_update_date_str_ = "";

        // Input Update Message
        data.__update_message_ = doc.GetCell<std::string>("Update Message", i);

        vect.push_back(data);
    }

    // Inject the vector into the PropertyList by matching the property name
    for (auto &item: vect)
        for (auto &prop: propList())
            if (item.__property_ == prop.__entrata_name_)
                prop.__construction_summary_historical_.push_back(item);

    // Print the number of "items" (rows of data) processed
    PRINT_NUM_ITEMS_PROCESSED(vect.size());

    sNumRowsProcessed += vect.size();

    PRINT_FUNCTION_STOP();
}

#endif // __input_routines__hpp__