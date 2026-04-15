#ifndef __output_routines__hpp__
#define __output_routines__hpp__

#include "PropertyList.hpp"
#include "dictionaries.hpp"

// Utility to create output directory
void createOutputDirectory()
{
    std::string path = "data/" + sExportDate + "/bin/";
    std::cout << "~ Creating directory: \t\t (" << path << ")" << std::endl;
    fs::create_directories(path);
}

void assignAmenityCosts(PropertyList& propList)
{
    std::cout << "Assigning Unit Amenity Costs..." << std::endl;
    for (auto &prop: propList())
    {
        for (auto &unit: prop.__units_)
        {
            for (auto &amenity: prop.__amenities_)
            {
                // Matched unit number with W&D Amenity Cost
                if (amenity.__property_   == prop.__name_  &&  
                    amenity.__unit_space_ == unit.__bldg_unit_  &&  
                    amenity.__amenity_.find("Washer and Dryer") != SIZE_MAX)
                {
                    unit.__amenity_WD_cost_ = amenity.__amenity_rent_;
                }
                else if (amenity.__property_      == prop.__name_  &&  
                         amenity.__unit_space_ == unit.__bldg_unit_  &&
                         amenity.__amenity_.find("Washer and Dryer") == SIZE_MAX)
                {
                    unit.__amenity_RENO_cost_ = amenity.__amenity_rent_;
                }
            }
        }
    }
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create "Job Costing Report (Historical).csv" to bin directory
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void createJobCostingReport_Historical(PropertyList& propList) noexcept(false)
{
    static const std::string FILENAME = "data/" + sExportDate + "/clean/Job Costing Report (Historical).csv";
    std::ofstream outFS;

    outFS.open(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,"
          << "Date," 
          << "Bldg-Unit,"
          << "Unit Type,"
          << "Unit Status,"
          << "Reno Status,"
          << "Phase,"
          << "Move-Out Date,"
          << "Reno Begin,"
          << "Reno End,"
          << "Days In Reno,"
          << "Move-In Date,"
          << "Actual Reno Cost,"
          << "Budgeted Reno Cost,"
          << "Actual vs Budget Variance,"
          << "Prior Lease Rent,"
          << "Current Lease Rent,"
          << "Prior Premium,"
          << "Current Premium,"
          << "Premium Variance," 
          << "Annual Premium,"
          << "ROI,"
          << "Prior Market Rent (Classic),"
          << "Current Market Rent (Classic),"
          << "Current Market Rent (Premium),"
          << "Prior Budgeted Rent,"
          << "New Budgeted Rent,"
          << std::endl;
    
    for (auto &prop: propList())
    {
        for (auto &reno: prop.__rolling_renovation_)
        {
            std::string reno_bldg_unit = reno.__bldg_unit_;
            bool found_first = false;
            bool found_last = false;
            std::vector<LeaseTradeOut> matching_ltos;
            
            // Find matching Lease Trade Out entries
            for (auto &lto: prop.__lease_trade_out_)
            {
                // Only consider completed renovations
                if (reno.__renovation_status_ != "Completed ")
                    break;

                std::string lto_bldg_unit = lto.__bldg_unit_;
                bool matchFound = (lto_bldg_unit == reno_bldg_unit) ? true : false;
                
                if (matchFound)
                {
                    if (!found_first)
                    {
                        found_first = true;
                    }

                    matching_ltos.push_back(lto);
                }
                else if (!matchFound  &&  found_first)
                {
                    found_last = true;
                }

                if (found_last)
                {
                    break;
                }
            }

            std::cout << prop.__name_ << " - " << "Found " << matching_ltos.size() << " matches for bldg-unit: " << reno_bldg_unit << std::endl;
            std::cout << "Renovation Completed Date: " << tmtostr(reno.__renovation_completion_date_) << std::endl;

            // Sort the matching Lease Trade Out entries by move-in date
            std::sort(matching_ltos.begin(), matching_ltos.end());
            
            LeaseTradeOut* prior_reno_lease = nullptr;
            LeaseTradeOut* post_reno_lease = nullptr;

            int count = 0;
            for (auto &match: matching_ltos)
            {
                std::cout << "LTO " << count + 1 << "/" << matching_ltos.size() << std::endl;
                std::cout << "Lease Start: " << tmtostr(match.__current_lease_start_) << std::endl;

                if ( !prior_reno_lease  &&  isBefore(match.__current_lease_start_, reno.__renovation_completion_date_) )
                {
                    std::cout << "Found match for prior_reno_lease" << std::endl;
                    prior_reno_lease = &match;
                }
                else if ( !post_reno_lease  &&  isAfterOrEqual(match.__current_lease_start_, reno.__renovation_completion_date_) )
                {
                    std::cout << "Found match for post_reno_lease" << std::endl;
                    post_reno_lease = &match;
                }

                count++;
            }

            if (reno.__actual_renovation_cost_ == 0)
            {
                std::cout << "Aborting: Actual Renovation Cost is 0" << std::endl;
            }

            if (prior_reno_lease  &&  post_reno_lease)
            {
                std::cout << "Found both prior and post renovation leases" << std::endl;
                std::cout << "Renovation Completed Date: " << tmtostr(reno.__renovation_completion_date_) << std::endl;
                std::cout << "Prior: " << tmtostr(prior_reno_lease->__current_lease_start_) << std::endl;
                std::cout << "Post: " << tmtostr(post_reno_lease->__current_lease_start_) << std::endl;

                // Write Property Name
                outFS << prop.__name_ << ",";
                // Write Export Date
                outFS << sExportDate_Forward_Slash_Format << ",";
                // Write Bldg-Unit
                outFS << reno.__bldg_unit_ << ",";
                // Write Unit Type
                outFS << reno.__unit_type_ << ",";
                // Write Unit Status
                outFS << reno.__unit_status_ << ",";
                // Write Reno Status
                outFS << removeSpaces(reno.__renovation_status_) << ",";
                // Write Phase
                outFS << reno.__phase_ << ",";
                // Write Move-Out Date
                outFS << tmtostr(reno.__move_out_date_) << ",";
                // Write Reno Begin
                outFS << tmtostr(reno.__renovation_start_date_) << ",";
                // Write Reno End
                outFS << tmtostr(reno.__renovation_completion_date_) << ",";
                // Write Days In Reno
                outFS << reno.__actual_renovation_days_ << ",";
                // Write Move-In Date
                outFS << tmtostr(post_reno_lease->__current_lease_start_) << ",";
                // Write Actual Renovation Cost
                outFS << reno.__actual_renovation_cost_ << ",";
                // Write Budgeted Renovation Cost
                outFS << reno.__budgeted_renovation_cost_ << ",";
                // Write Actual vs Budget Variance
                outFS << reno.__actual_vs_budget_variance_ << ",";
                // Write Prior Lease - Current Lease Rent
                outFS << prior_reno_lease->__current_lease_rent_ << ",";
                // Write Current Lease - Current Lease Rent
                outFS << post_reno_lease->__current_lease_rent_ << ",";
                // Write Prior Premium
                outFS << prior_reno_lease->__current_amenity_rent_ << ",";
                // Write Current Premium
                outFS << post_reno_lease->__current_amenity_rent_ << ",";
                // Write Premium Variance
                outFS << post_reno_lease->__current_amenity_rent_ - prior_reno_lease->__current_amenity_rent_ << ",";
                // Write Annual Premium
                outFS << post_reno_lease->__current_amenity_rent_ * 12 << ",";
                // Write ROI
                outFS << (post_reno_lease->__current_amenity_rent_ * 12) / reno.__actual_renovation_cost_ << ",";
                // Write Prior Market Rent (Classic)
                outFS << prior_reno_lease->__market_rent_ << ",";
                // Write Current Market Rent (Classic)
                outFS << post_reno_lease->__market_rent_ << ",";
                // Write Current Market Rent (Premium)
                outFS << post_reno_lease->__current_amenity_rent_ << ",";
                // Write Prior Budgeted Rent
                outFS << reno.__prior_budgeted_rent_ << ",";
                // Write New Budgeted Rent
                outFS << reno.__new_budgeted_rent_ << ",";
                // Write end of line
                outFS << std::endl;
            }
            else if ( !prior_reno_lease  &&  post_reno_lease )
            {
                std::cout << "Incomplete: Found ONLY post renovation lease" << std::endl;
            }
            else if ( prior_reno_lease  &&  !post_reno_lease )
            {
                std::cout << "Incomplete: Found ONLY prior renovation lease" << std::endl;
            }
            else if ( !prior_reno_lease  &&  !post_reno_lease )
            {
                std::cout << "Incomplete: Found NEITHER prior nor post renovation lease" << std::endl;
            }
        }
    }
    
    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create "Job Costing Report (Historical).csv" to bin directory
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void createJobCostingReport_Historical_New(PropertyList& propList) noexcept(false)
{
    static const std::string FILENAME = "data/" + sExportDate + "/clean/Job Costing Report (Historical).csv";
    std::ofstream outFS;

    outFS.open(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,"
          << "Date," 
          << "Bldg-Unit,"
          << "Unit Type,"
          << "Unit Status,"
          << "Reno Status,"
          << "Phase,"
          << "Move-Out Date,"
          << "Reno Begin,"
          << "Reno End,"
          << "Days In Reno,"
          << "Move-In Date,"
          << "Actual Reno Cost,"
          << "Budgeted Reno Cost,"
          << "Actual vs Budget Variance,"
          << "Prior Lease Rent,"
          << "Current Lease Rent,"
          << "Prior Premium,"
          << "Current Premium,"
          << "Premium Variance," 
          << "Annual Premium,"
          << "ROI,"
          << "Prior Budgeted Rent,"
          << "New Budgeted Rent,"
          << std::endl;
    
    for (auto &prop: propList())
    {
        for (auto &reno: prop.__rolling_renovation_)
        {
            std::string reno_bldg_unit = reno.__bldg_unit_;
            bool found_first = false;
            bool found_last = false;
            std::vector<LeaseTradeOut> matching_ltos;
            
            // Find matching Lease Trade Out entries
            for (auto &lto: prop.__lease_trade_out_)
            {
                // Only consider completed renovations
                if (reno.__renovation_status_ != "Completed ")
                    break;

                std::string lto_bldg_unit = lto.__bldg_unit_;
                bool matchFound = (lto_bldg_unit == reno_bldg_unit) ? true : false;
                
                if (matchFound)
                {
                    if (!found_first)
                    {
                        found_first = true;
                    }

                    matching_ltos.push_back(lto);
                }
                else if (!matchFound  &&  found_first)
                {
                    found_last = true;
                }

                if (found_last)
                {
                    break;
                }
            }

            std::cout << prop.__name_ << " - " << "Found " << matching_ltos.size() << " matches for bldg-unit: " << reno_bldg_unit << std::endl;
            std::cout << "Renovation Completed Date: " << tmtostr(reno.__renovation_completion_date_) << std::endl;

            // Sort the matching Lease Trade Out entries by move-in date
            std::sort(matching_ltos.begin(), matching_ltos.end());
            
            LeaseTradeOut* prior_reno_lease = nullptr;
            LeaseTradeOut* post_reno_lease = nullptr;

            int count = 0;
            for (auto &match: matching_ltos)
            {
                std::cout << "LTO " << count + 1 << "/" << matching_ltos.size() << std::endl;
                std::cout << "Lease Start: " << tmtostr(match.__current_lease_start_) << std::endl;

                if ( !prior_reno_lease  &&  isBefore(match.__current_lease_start_, reno.__renovation_completion_date_) )
                {
                    std::cout << "Found match for prior_reno_lease" << std::endl;
                    prior_reno_lease = &match;
                }
                //else if ( !post_reno_lease  &&  isAfterOrEqual(match.__current_lease_start_, reno.__renovation_completion_date_) )
                //{
                //    std::cout << "Found match for post_reno_lease" << std::endl;
                //    post_reno_lease = &match;
                //}

                count++;
            }

            if ( matching_ltos.size() )
            {
                std::cout << "Assigned match for post_reno_lease at index " << matching_ltos.size() << std::endl;
                post_reno_lease = &matching_ltos.at(matching_ltos.size() - 1);
            }
            

            


            if (reno.__actual_renovation_cost_ == 0)
            {
                std::cout << "Aborting: Actual Renovation Cost is 0" << std::endl;
            }

            if (prior_reno_lease  &&  post_reno_lease)
            {
                std::cout << "Found both prior and post renovation leases" << std::endl;
                std::cout << "Renovation Completed Date: " << tmtostr(reno.__renovation_completion_date_) << std::endl;
                std::cout << "Prior: " << tmtostr(prior_reno_lease->__current_lease_start_) << std::endl;
                std::cout << "Post: " << tmtostr(post_reno_lease->__current_lease_start_) << std::endl;

                // Write Property Name
                outFS << prop.__name_ << ",";
                // Write Export Date
                outFS << sExportDate_Forward_Slash_Format << ",";
                // Write Bldg-Unit
                outFS << reno.__bldg_unit_ << ",";
                // Write Unit Type
                outFS << reno.__unit_type_ << ",";
                // Write Unit Status
                outFS << reno.__unit_status_ << ",";
                // Write Reno Status
                outFS << removeSpaces(reno.__renovation_status_) << ",";
                // Write Phase
                outFS << reno.__phase_ << ",";
                // Write Move-Out Date
                outFS << tmtostr(reno.__move_out_date_) << ",";
                // Write Reno Begin
                outFS << tmtostr(reno.__renovation_start_date_) << ",";
                // Write Reno End
                outFS << tmtostr(reno.__renovation_completion_date_) << ",";
                // Write Days In Reno
                outFS << reno.__actual_renovation_days_ << ",";
                // Write Move-In Date
                outFS << tmtostr(post_reno_lease->__current_lease_start_) << ",";
                // Write Actual Renovation Cost
                outFS << reno.__actual_renovation_cost_ << ",";
                // Write Budgeted Renovation Cost
                outFS << reno.__budgeted_renovation_cost_ << ",";
                // Write Actual vs Budget Variance
                outFS << reno.__actual_vs_budget_variance_ << ",";
                // Write Prior Lease - Current Lease Rent
                outFS << prior_reno_lease->__current_lease_rent_ << ",";
                // Write Current Lease - Current Lease Rent
                outFS << post_reno_lease->__current_lease_rent_ << ",";
                // Write Prior Premium
                outFS << prior_reno_lease->__current_amenity_rent_ << ",";
                // Write Current Premium
                outFS << post_reno_lease->__current_amenity_rent_ << ",";
                // Write Premium Variance
                outFS << post_reno_lease->__current_amenity_rent_ - prior_reno_lease->__current_amenity_rent_ << ",";
                // Write Annual Premium
                outFS << post_reno_lease->__current_amenity_rent_ * 12 << ",";
                // Write ROI
                outFS << (post_reno_lease->__current_amenity_rent_ * 12) / reno.__actual_renovation_cost_ << ",";
                // Write Prior Budgeted Rent
                outFS << reno.__prior_budgeted_rent_ << ",";
                // Write New Budgeted Rent
                outFS << reno.__new_budgeted_rent_ << ",";

                // Write end of line
                outFS << std::endl;
            }
            else if ( !prior_reno_lease  &&  post_reno_lease )
            {
                std::cout << "Incomplete: Found ONLY post renovation lease" << std::endl;
            }
            else if ( prior_reno_lease  &&  !post_reno_lease )
            {
                std::cout << "Incomplete: Found ONLY prior renovation lease" << std::endl;
            }
            else if ( !prior_reno_lease  &&  !post_reno_lease )
            {
                std::cout << "Incomplete: Found NEITHER prior nor post renovation lease" << std::endl;
            }
        }
        
    }
    
    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create "Job Costing Report.csv" to bin directory
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void createJobCostingReport(PropertyList& propList) noexcept(false)
{
    static const std::string FILENAME = "data/" + sExportDate + "/clean/Job Costing Report.csv";
    std::ofstream outFS;

    outFS.open(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,"
          << "Date," 
          << "Bldg-Unit,"
          << "Unit Type,"
          << "Unit Status,"
          << "Reno Status,"
          << "Phase,"
          << "Move-Out Date,"
          << "Reno Begin,"
          << "Reno End,"
          << "Days In Reno,"
          << "Move-In Date,"
          << "Actual Reno Cost,"
          << "Budgeted Reno Cost,"
          << "Actual vs Budget Variance,"
          << "Prior Lease Rent,"
          << "Current Lease Rent,"
          << "Prior Premium,"
          << "Current Premium,"
          << "Premium Variance," 
          << "Annual Premium,"
          << "ROI,"
          << "Prior Budgeted Rent,"
          << "New Budgeted Rent" // add comma if extended
          << std::endl;
    
    for (auto &prop: propList())
    {
        for (auto &reno: prop.__rolling_renovation_)
        {
            for (auto &lto: prop.__lease_trade_out_)
            {
                if (reno.__bldg_unit_ == lto.__bldg_unit_  &&  reno.__renovation_status_ == "Completed ")
                {
                    //std::cout << "Matched Reno unit " << reno.__bldg_unit_ << " to LTO unit " << lto.__bldg_unit_ << std::endl;

                    // Write Property Name
                    outFS << prop.__name_ << ",";
                    // Write Export Date
                    outFS << sExportDate_Forward_Slash_Format << ",";
                    // Write Bldg-Unit
                    outFS << reno.__bldg_unit_ << ",";
                    // Write Unit Type
                    outFS << reno.__unit_type_ << ",";
                    // Write Unit Status
                    outFS << reno.__unit_status_ << ",";
                    // Write Reno Status
                    outFS << removeSpaces(reno.__renovation_status_) << ",";
                    // Write Phase
                    outFS << reno.__phase_ << ",";
                    // Write Move-Out Date
                    outFS << tmtostr(reno.__move_out_date_) << ",";
                    // Write Reno Begin
                    outFS << tmtostr(reno.__renovation_start_date_) << ",";
                    // Write Reno End
                    outFS << tmtostr(reno.__renovation_completion_date_) << ",";
                    // Write Days In Reno
                    outFS << reno.__actual_renovation_days_ << ",";

                    // Write Move-In Date
                    outFS << tmtostr(lto.__current_lease_start_) << ",";
                    // Write Actual Renovation Cost
                    outFS << reno.__actual_renovation_cost_ << ",";
                    // Write Budgeted Renovation Cost
                    outFS << reno.__budgeted_renovation_cost_ << ",";
                    // Write Actual vs Budget Variance
                    outFS << reno.__actual_vs_budget_variance_ << ",";
                    // Write Prior Lease Rent
                    outFS << lto.__prior_lease_rent_ << ",";
                    // Write Current Lease Rent
                    outFS << lto.__current_lease_rent_ << ",";
                    // Write Prior Premium
                    outFS << lto.__prior_amenity_rent_ << ",";
                    // Write Current Premium
                    outFS << lto.__current_amenity_rent_ << ",";
                    // Write Premium Variance
                    outFS << lto.__current_amenity_rent_ - lto.__prior_amenity_rent_ << ",";

                    // Write Annual Premium
                    outFS << lto.__current_amenity_rent_ * 12 << ",";
                    // Write ROI
                    if (reno.__actual_renovation_cost_ != 0.f)
                        outFS << (lto.__current_amenity_rent_ * 12) / reno.__actual_renovation_cost_ << ",";
                    else
                        outFS << 0.f << ",";


                    // Write Prior Budgeted Rent
                    outFS << reno.__prior_budgeted_rent_ << ",";
                    // Write New Budgeted Rent
                    outFS << reno.__new_budgeted_rent_ << ",";
                    // Write end of line
                    outFS << std::endl;

                }
            }
        }
    }
    
    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeConstructionSummary_Historical(PropertyList& propList)
{
    static const std::string FILENAME = "data/" + sExportDate + "/bin/Construction Summary (Historical).csv";

    std::ofstream outFS(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,"
          << "Bldg-Unit,"
          << "Floorplan,"
          << "Unit Type,"
          << "Status,"
          << "Move-Out Date,"
          << "Phase,"
          << "Reno Begin,"
          << "Reno End,"
          << "Days In Reno,"
          << "Move-In Date,"
          << "Prior Lease Rent,"
          << "Current Lease Rent,"
          << "Prior Premium,"
          << "Current Premium,"
          << "Premium Variance,"
          << "Annual Premium,"
          << "ROI,"
          << "Prior Budgeted Rent,"
          << "New Budgeted Rent,"
          << "Rent Increase,"
          << "Budgeted Reno Cost,"
          << "Amenity Cost - W&D,"
          << "Amenity Cost - Reno,"
          << "Reno Kits,"
          << "Appliances,"
          << "Paint,"
          << "Flooring,"
          << "Labor,"
          << "Washer & Dryer,"
          << "Backsplash,"
          << "Countertops,"
          << "Cabinets,"
          << "Other,"
          << "Undefined,"
          << "Total Actual,"
          << "Savings ($),"
          << "Savings (%)," // add comma if extended

          << std::endl;

    for (auto &prop : propList())
    {
        for (auto &unit: prop.__units_)
        {
            std::string this_bldg_unit = unit.__bldg_unit_;
            float this_budgeted_reno_cost = 0.f;
            bool foundInJobCosting = false;

            outFS << prop.__name_ << ","
                  << unit.__bldg_unit_ << ","
                  << unit.__floorplan_ << ","
                  << unit.__unit_type_ << ","
                  << unit.__unit_status_ << ",";

            (unit.__move_out_str == "") 
                ? outFS << unit.__move_out_str << "," 
                : outFS << tmtostr(unit.__move_out_) << ",";

            for (auto &item: prop.__job_costing_historical_)
            {
                if (this_bldg_unit == item.__bldg_unit_)
                {
                    foundInJobCosting = true;
                    this_budgeted_reno_cost = item.__budgeted_reno_cost_;

                    outFS << item.__phase_ << ","                                                                           // 1
                          << tmtostr(item.__reno_begin_) << ","                                                             // 2
                          << tmtostr(item.__reno_end_) << ","                                                               // 3
                          << item.__days_in_reno_ << ","                                                                    // 4
                          << tmtostr(item.__move_in_date_) << ","                                                           // 5
                          << item.__prior_lease_rent_ << ","                                                                // 6
                          << item.__current_lease_rent_ << ","                                                              // 7
                          << item.__prior_premium_ << ","                                                                   // 8
                          << item.__current_premium_ << ","                                                                 // 9
                          << item.__premium_variance_ << ","                                                                // 10
                          << item.__annual_premium_ << ","                                                                  // 11
                          << item.__roi_ << ","                                                                             // 12
                          << item.__prior_budgeted_rent_ << ","                                                             // 13
                          << item.__new_budgeted_rent_ << ","                                                               // 14
                          << ((item.__current_lease_rent_ - item.__prior_lease_rent_) / item.__prior_lease_rent_) << ","    // 15
                          << item.__budgeted_reno_cost_ << ","                                                              // 16
                          << unit.__amenity_WD_cost_ << ","                                                                 // 17
                          << unit.__amenity_RENO_cost_ << ",";                                                              // 18

                    break;
                }
            }

            bool foundInMasterTrackerData = false;

            if (!foundInJobCosting)
            {
                std::cout << "Unit " << this_bldg_unit << " at " << prop.__name_ << " NOT FOUND in Job Costing" << std::endl;
                std::cout << "Searching Master Tracker Data..." << std::endl;

                for (auto &item: prop.__master_tracker_data_)
                {
                    if (this_bldg_unit == item.__bldg_unit_)
                    {
                        foundInMasterTrackerData = true;
                        std::cout << "Unit " << this_bldg_unit << " at " << prop.__name_ << " FOUND in Master Tracker Data" << std::endl;

                        outFS << ",";                                       // 1

                        if (item.__date_started_str_ != "") 
                            outFS << tmtostr(item.__date_started_) << ",";  // 2
                        else 
                            outFS << ",";

                        if (item.__date_complete_str_ != "") 
                            outFS << tmtostr(item.__date_complete_) << ","; // 3
                        else
                            outFS << ",";

                        outFS << ","                                        // 4
                              << ","                                        // 5
                              << ","                                        // 6
                              << ","                                        // 7
                              << ","                                        // 8
                              << ","                                        // 9
                              << ","                                        // 10
                              << ","                                        // 11
                              << ","                                        // 12
                              << ","                                        // 13
                              << ","                                        // 14
                              << ","                                        // 15
                              << item.__budget_ << ","                      // 16
                              << unit.__amenity_WD_cost_ << ","             // 17
                              << unit.__amenity_RENO_cost_ << ",";          // 18
                    }
                }
            }

            if (!foundInJobCosting  &&  !foundInMasterTrackerData)
            {
                outFS << ","                                 // 1
                      << ","                                 // 2
                      << ","                                 // 3
                      << ","                                 // 4
                      << ","                                 // 5
                      << ","                                 // 6
                      << ","                                 // 7
                      << ","                                 // 8
                      << ","                                 // 9
                      << ","                                 // 10
                      << ","                                 // 11
                      << ","                                 // 12
                      << ","                                 // 13
                      << ","                                 // 14
                      << ","                                 // 15
                      << ","                                 // 16
                      << unit.__amenity_WD_cost_ << ","      // 17
                      << unit.__amenity_RENO_cost_ << ",";   // 18
            }

            if ( unit.__totals_.__sum_ )
            {
                outFS << unit.__totals_.__reno_kits_ << ","        // 1
                      << unit.__totals_.__appliances_ << ","       // 2
                      << unit.__totals_.__paint_ << ","            // 3
                      << unit.__totals_.__flooring_ << ","         // 4
                      << unit.__totals_.__labor_ << ","            // 5
                      << unit.__totals_.__washer_and_dryer_ << "," // 6
                      << unit.__totals_.__backsplash_ << ","       // 7
                      << unit.__totals_.__countertops_ << ","      // 8
                      << unit.__totals_.__cabinets_ << ","         // 9
                      << unit.__totals_.__other_ << ","            // 10
                      << unit.__totals_.__undefined_ << ","        // 11
                      << unit.__totals_.__sum_ << ",";             // 12
            }
            else
            {
                outFS << ","   // 1
                      << ","   // 2
                      << ","   // 3
                      << ","   // 4
                      << ","   // 5
                      << ","   // 6
                      << ","   // 7
                      << ","   // 8
                      << ","   // 9
                      << ","   // 10
                      << ","   // 11
                      << ",";  // 12
            }

            float savingsInDollars = unit.__totals_.__sum_ - this_budgeted_reno_cost;
            if ( savingsInDollars ) 
                outFS << savingsInDollars << ",";
            else
                outFS << ",";
                  
            if ( unit.__totals_.__sum_ && this_budgeted_reno_cost )
            {
                outFS << unit.__totals_.__sum_ / this_budgeted_reno_cost;
            }
            else
            {
                //outFS << ",";
            }

            outFS << std::endl;
        }
    }

    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {   
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write "Construction Summary.csv" to bin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeConstructionSummary(PropertyList& propList)
{
    static const std::string FILENAME = "data/" + sExportDate + "/bin/Construction Summary.csv";

    std::ofstream outFS(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,"
          << "Bldg-Unit,"
          << "Floorplan,"
          << "Unit Type,"
          << "Status,"
          << "Job,"
          << "Move-Out Date,"
          << "Reno Begin,"
          << "Reno End,"
          << "Days In Reno,"
          << "Move-In Date,"
          << "Prior Lease Rent,"
          << "Current Lease Rent,"
          << "Prior Premium,"
          << "Current Premium,"
          << "Premium Variance,"
          << "Annual Premium,"
          << "ROI,"
          << "Prior Budgeted Rent,"
          << "New Budgeted Rent,"
          << "Rent Increase,"
          << "Budgeted Reno Cost,"
          << "Amenity Cost - W&D,"
          << "Amenity Cost - Reno,"
          << "Reno Kits,"
          << "Appliances,"
          << "Paint,"
          << "Flooring,"
          << "Labor,"
          << "Washer & Dryer,"
          << "Backsplash,"
          << "Countertops,"
          << "Cabinets,"
          << "Other,"
          << "Undefined,"
          << "Total Actual,"
          << "Savings ($),"
          << "Savings (%)," 
          << "Is Renovated?,"
          << "Entry Update Date," 
          << "Update Message" // add comma if extended

          << std::endl;    

    ///< MARK: Loop 1: Update Current Lease details to historically renovated units already assigned Prior Lease details
    for (auto &prop : propList())
    {
        for (auto &item: prop.__construction_summary_historical_)
        {
            ConstructionSummaryEntry updatedEntry = item;
            std::string this_bldg_unit = item.__bldg_unit_;

            for (auto &job: prop.__job_costing_)
            {
                // Found a unit in JC that is tagged as renovated that has already has prior lease rent
                if (this_bldg_unit == job.__bldg_unit_  &&  item.__is_renovated_  &&  item.__prior_lease_rent_ != 0.f )
                {        
                    std::cout << item.__property_ << "'s unit " << this_bldg_unit << " was historically renovated and has prior lease rent of " << item.__prior_lease_rent_ << " and current lease rent of " << item.__current_lease_rent_ 
                              << ". This unit has a new lease trade out with a new current lease rent of " << job.__current_lease_rent_ << ". (";

                    updatedEntry.__current_lease_rent_ = job.__current_lease_rent_;
                    updatedEntry.__entry_update_date_ = stotm(sExportDate_Forward_Slash_Format);
                    updatedEntry.__entry_update_date_str_ = "~";
                    updatedEntry.__update_message_ = "Updated Current Lease Rent";

                    std::cout << "Current Lease Rent from " << item.__current_lease_rent_ << " to " << updatedEntry.__current_lease_rent_ << ")" << std::endl;
                }
            }

            prop.__construction_summary_updated_.push_back(updatedEntry);
        }
    }

    ///< MARK: Loop 2: Update Prior Lease AND Current Lease details to NEWLY renovated units found in Rolling Renovation report
    for (auto &prop : propList())
    {
        for (auto &item: prop.__construction_summary_updated_)
        {
            std::string this_bldg_unit = item.__bldg_unit_;

            for (auto &job: prop.__job_costing_)
            {
                // Found a unit in JC that does NOT have prior lease rent
                if (this_bldg_unit == job.__bldg_unit_  &&  item.__prior_lease_rent_ == 0.f )
                {
                    std::cout << item.__property_ << "'s unit " << this_bldg_unit << " was historically renovated but does NOT have a prior lease rent" << std::endl;

                    item.__move_out_date_               = job.__move_out_date_;
                    item.__move_out_date_str_           = "~";
                    item.__job_                         = job.__phase_;
                    item.__reno_begin_                  = job.__reno_begin_;
                    item.__reno_begin_str_              = "~";
                    item.__reno_end_                    = job.__reno_end_;
                    item.__reno_end_str_                = "~";
                    item.__entry_update_date_           = stotm(sExportDate_Forward_Slash_Format);
                    item.__entry_update_date_str_       = "~";
                    item.__days_in_reno_                = job.__days_in_reno_;
                    item.__move_in_date_                = job.__move_in_date_;
                    item.__move_in_date_str_            = "~";
                    item.__prior_lease_rent_            = job.__prior_lease_rent_;
                    item.__current_lease_rent_          = job.__current_lease_rent_;
                    item.__prior_premium_               = job.__prior_premium_;
                    item.__current_premium_             = job.__current_premium_;
                    item.__premium_variance_            = job.__premium_variance_;
                    item.__annual_premium_              = job.__annual_premium_;
                    item.__roi_                         = job.__roi_;
                    item.__prior_budgeted_rent_         = job.__prior_budgeted_rent_;
                    item.__new_budgeted_rent            = job.__new_budgeted_rent_;
                    item.__rent_increase_               = ((job.__current_lease_rent_ - job.__prior_lease_rent_) / job.__prior_lease_rent_);
                    item.__budgeted_reno_cost_          = job.__budgeted_reno_cost_;

                    for (auto &unit: prop.__units_)
                    {
                        if (this_bldg_unit == unit.__bldg_unit_)
                        {
                            item.__amenity_cost_WD_                     = unit.__amenity_WD_cost_;
                            item.__amenity_cost_RENO_                   = unit.__amenity_RENO_cost_;

                            item.__job_expenses_.__reno_kits_           = unit.__totals_.__reno_kits_;
                            item.__job_expenses_.__appliances_          = unit.__totals_.__appliances_;
                            item.__job_expenses_.__paint_               = unit.__totals_.__paint_;
                            item.__job_expenses_.__flooring_            = unit.__totals_.__flooring_;
                            item.__job_expenses_.__labor_               = unit.__totals_.__labor_;
                            item.__job_expenses_.__washer_and_dryer_    = unit.__totals_.__washer_and_dryer_;
                            item.__job_expenses_.__backsplash_          = unit.__totals_.__backsplash_;
                            item.__job_expenses_.__countertops_         = unit.__totals_.__countertops_;
                            item.__job_expenses_.__cabinets_            = unit.__totals_.__cabinets_;
                            item.__job_expenses_.__other_               = unit.__totals_.__other_;
                            item.__job_expenses_.__undefined_           = unit.__totals_.__undefined_;
                            item.__job_expenses_.__sum_                 = unit.__totals_.__sum_;

                            float savingsInDollars = unit.__totals_.__sum_ - job.__budgeted_reno_cost_;
                            item.__savings_dollar_ = ( savingsInDollars ) ? savingsInDollars : 0.f;
                                
                            if ( unit.__totals_.__sum_ && job.__budgeted_reno_cost_ )
                                item.__savings_percent_ = unit.__totals_.__sum_ / job.__budgeted_reno_cost_;

                            item.__is_renovated_ = true;

                            item.__update_message_ = "Populated Prior & Current Lease Details";

                            break;
                        }
                    }
                }
            }
        }
    }

    ///< MARK: Loop 3: Write updated Construction Summary report
    for (auto &prop : propList())
    {
        for (auto &item: prop.__construction_summary_updated_)
        {
            outFS << item.__property_ << ","
                  << item.__bldg_unit_ << ","
                  << item.__floorplan_ << ","
                  << item.__unit_type_ << ","
                  << item.__status_ << ","; 

            // Job
            outFS << item.__job_ << ","; 

            // Move-Out Date
            (item.__move_out_date_str_ == "") 
                ? outFS << item.__move_out_date_str_ << "," 
                : outFS << tmtostr(item.__move_out_date_) << ",";

            // Reno Begin
            (item.__reno_begin_str_ == "") 
                ? outFS << item.__reno_begin_str_ << "," 
                : outFS << tmtostr(item.__reno_begin_) << ",";

            // Reno End
            (item.__reno_end_str_ == "") 
                ? outFS << item.__reno_end_str_ << "," 
                : outFS << tmtostr(item.__reno_end_) << ",";

            // Days In Reno
            if ( item.__days_in_reno_ != 0 )
                outFS << item.__days_in_reno_ << ",";
            else 
                outFS << ",";

            // Move-In Date
            (item.__move_in_date_str_ == "") 
                ? outFS << item.__move_in_date_str_ << "," 
                : outFS << tmtostr(item.__move_in_date_) << ",";

            // Prior Lease Rent
            if ( item.__prior_lease_rent_ != 0 )
                outFS << item.__prior_lease_rent_ << ",";
            else 
                outFS << ",";

            // Current Lease Rent
            if ( item.__current_lease_rent_ != 0 )
                outFS << item.__current_lease_rent_ << ",";
            else
                outFS << ",";

            // Prior Premium
            if ( item.__prior_premium_ != 0 )
                outFS << item.__prior_premium_ << ",";
            else
                outFS << ",";

            // Current Premium
            if ( item.__current_premium_ != 0 )
                outFS << item.__current_premium_ << ",";
            else
                outFS << ",";

            // Premium Variance
            if ( item.__premium_variance_ != 0 )
                outFS << item.__premium_variance_ << ",";
            else
                outFS << ",";

            // Annual Premium
            if ( item.__annual_premium_ != 0 )
                outFS << item.__annual_premium_ << ",";
            else
                outFS << ",";

            // ROI
            if ( item.__roi_ != 0 )
                outFS << item.__roi_ << ",";
            else
                outFS << ",";

            // Prior Budgeted Rent
            if ( item.__prior_budgeted_rent_ != 0 )
                outFS << item.__prior_budgeted_rent_ << ",";
            else
                outFS << ",";

            // New Budgeted Rent
            if ( item.__new_budgeted_rent != 0 )
                outFS << item.__new_budgeted_rent << ",";
            else
                outFS << ",";

            // Rent Increase
            if ( item.__rent_increase_ != 0 )
                outFS << item.__rent_increase_ << ",";
            else
                outFS << ",";

            // Budgeted Reno Cost
            if ( item.__budgeted_reno_cost_ != 0 )
                outFS << item.__budgeted_reno_cost_ << ",";
            else
                outFS << ",";

            // Amenity Cost - W&D
            if ( item.__amenity_cost_WD_ != 0 )
                outFS << item.__amenity_cost_WD_ << ",";
            else
                outFS << ",";

            // Amenity Cost - Reno
            if ( item.__amenity_cost_RENO_ != 0 )
                outFS << item.__amenity_cost_RENO_ << ",";
            else
                outFS << ",";

            // Reno Kits
            if ( item.__job_expenses_.__reno_kits_ != 0 )
                outFS << item.__job_expenses_.__reno_kits_ << ",";
            else
                outFS << ",";

            // Appliances
            if ( item.__job_expenses_.__appliances_ != 0 )
                outFS << item.__job_expenses_.__appliances_ << ",";
            else
                outFS << ",";

            // Painting
            if ( item.__job_expenses_.__paint_ != 0 )
                outFS << item.__job_expenses_.__paint_ << ",";
            else
                outFS << ",";

            // Flooring
            if ( item.__job_expenses_.__flooring_ != 0 )
                outFS << item.__job_expenses_.__flooring_ << ",";
            else
                outFS << ",";

            // Labor
            if ( item.__job_expenses_.__labor_ != 0 )
                outFS << item.__job_expenses_.__labor_ << ",";
            else
                outFS << ",";

            // Washer and Dryer
            if ( item.__job_expenses_.__washer_and_dryer_ != 0 )
                outFS << item.__job_expenses_.__washer_and_dryer_ << ",";
            else
                outFS << ",";

            // Backsplash
            if ( item.__job_expenses_.__backsplash_ != 0 )
                outFS << item.__job_expenses_.__backsplash_ << ",";
            else
                outFS << ",";

            // Countertops
            if ( item.__job_expenses_.__countertops_ != 0 )
                outFS << item.__job_expenses_.__countertops_ << ",";
            else
                outFS << ",";

            // Cabinets
            if ( item.__job_expenses_.__cabinets_ != 0 )
                outFS << item.__job_expenses_.__cabinets_ << ",";
            else
                outFS << ",";

            // Other
            if ( item.__job_expenses_.__other_ != 0 )
                outFS << item.__job_expenses_.__other_ << ",";
            else
                outFS << ",";

            // Undefined
            if ( item.__job_expenses_.__undefined_ != 0 )
                outFS << item.__job_expenses_.__undefined_ << ",";
            else
                outFS << ",";

            // Total Actual
            if ( item.__job_expenses_.__sum_ != 0 )
                outFS << item.__job_expenses_.__sum_ << ",";
            else
                outFS << ",";

            // Savings ($)
            if ( item.__savings_dollar_ != 0 )
                outFS << item.__savings_dollar_ << ",";
            else
                outFS << ",";

            // Savings (%)
            if ( item.__savings_percent_ != 0 )
                outFS << item.__savings_percent_ << ",";
            else
                outFS << ",";

            // Is Renovated?
            outFS << booltostr(item.__is_renovated_) << ",";

            // Entry Update Date
            (item.__entry_update_date_str_ == "") 
                ? outFS << item.__entry_update_date_str_ << "," 
                : outFS << tmtostr(item.__entry_update_date_) << ",";

            // Update Message
            outFS << item.__update_message_ << ",";

            outFS << std::endl;
        }
    }

    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {   
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}


/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write "Construction Summary.csv" to bin
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeConstructionSummary(PropertyList& propList)
{
    static const std::string FILENAME = "data/" + sExportDate + "/bin/Construction Summary.csv";

    std::ofstream outFS(FILENAME, std::ios::out);

    // Write header
    outFS << "Property,";
    outFS << "Bldg-Unit,";
    outFS << "Floorplan,";
    outFS << "Unit Type,";
    outFS << "Unit Status,";
    outFS << "Phase,";
    outFS << "Move-Out Date,";
    outFS << "Reno Begin,";
    outFS << "Reno End,";
    outFS << "Days In Reno,";
    outFS << "Move-In Date,";
    outFS << "Prior Lease Rent,";
    outFS << "Current Lease Rent,";
    outFS << "Prior Premium,";
    outFS << "Current Premium,";
    outFS << "Premium Variance,";
    outFS << "Annual Premium,";
    outFS << "ROI,";
    outFS << "Prior Budgeted Rent,";
    outFS << "New Budgeted Rent,";
    outFS << "Rent Increase,";
    outFS << "Budgeted Reno Cost,";
    outFS << "Reno Kits,";
    outFS << "Appliances,";
    outFS << "Paint,";
    outFS << "Flooring,";
    outFS << "Labor,";
    outFS << "Washer & Dryer,";
    outFS << "Backsplash,";
    outFS << "Countertops,";
    outFS << "Cabinets,";
    outFS << "Other,";
    outFS << "Undefined,";
    outFS << "Total Actual,";
    outFS << "Savings ($),";
    outFS << "Savings (%)"; // add comma if extended

    outFS << std::endl;

    for (auto &prop : propList())
    {
        for (auto &job: prop.__job_costing_)
        {
            std::string job_bldg_unit = job.__bldg_unit_;

            outFS << prop.__name_ << ","
                  << sExportDate_Forward_Slash_Format << ","
                  << job.__bldg_unit_ << ","
                  << job.__unit_type_ << ","
                  << job.__unit_status_ << ","
                  << job.__phase_ << ","
                  << tmtostr(job.__move_out_date_) << ","
                  << tmtostr(job.__reno_begin_) << ","
                  << tmtostr(job.__reno_end_) << ","
                  << job.__days_in_reno_ << ","
                  << tmtostr(job.__move_in_date_) << ","
                  << job.__prior_lease_rent_ << ","
                  << job.__current_lease_rent_ << ","
                  << job.__prior_premium_ << ","
                  << job.__current_premium_ << ","
                  << job.__premium_variance_ << ","
                  << job.__annual_premium_ << ","
                  << job.__roi_ << ","
                  << job.__prior_budgeted_rent_ << ","
                  << job.__new_budgeted_rent_ << ","
                  << ((job.__current_lease_rent_ - job.__prior_lease_rent_) / job.__prior_lease_rent_) << ","
                  << job.__budgeted_reno_cost_ << ",";

            for (auto &unit: prop.__units_)
            {
                std::string this_bldg_unit = unit.__bldg_unit_;

                if (job.__property_ == prop.__name_  &&  job_bldg_unit == this_bldg_unit)
                {
                    outFS << unit.__totals_.__reno_kits_ << ","       
                          << unit.__totals_.__applicances_ << ","     
                          << unit.__totals_.__paint_ << ","           
                          << unit.__totals_.__flooring_ << ","        
                          << unit.__totals_.__labor_ << ","           
                          << unit.__totals_.__washer_and_dryer_ << ","
                          << unit.__totals_.__backsplash_ << ","      
                          << unit.__totals_.__countertops_ << ","     
                          << unit.__totals_.__cabinets_ << ","        
                          << unit.__totals_.__other_ << ","            
                          << unit.__totals_.__undefined_ << ","        
                          << unit.__totals_.__sum_ << ",";    
                          
                    float savingsInDollars = unit.__totals_.__sum_ - job.__budgeted_reno_cost_;
                    // Write Savings ($)
                    if ( savingsInDollars ) 
                        outFS << savingsInDollars << "," ;
                    else
                        outFS << ",";
                    
                    // Write Savings (%)
                    if ( unit.__totals_.__sum_ && job.__budgeted_reno_cost_ )
                        outFS << unit.__totals_.__sum_ / job.__budgeted_reno_cost_;
                    else
                        outFS << ",";

                    break;
                }
            }

            outFS << std::endl;
        }
    }

    if ( fs::exists(FILENAME) )
    {
        sNumFilesGenerated += 1;
        PRINT_WRITE_FILE_SUCCESS_MSG(FILENAME);
    }
    else
    {   
        PRINT_WRITE_FILE_FAILURE_MSG(FILENAME);
        throw std::runtime_error("❌ Failed to generate file: " + FILENAME);
    }

    outFS.close();
}
*/

#endif // __output_routines__hpp__