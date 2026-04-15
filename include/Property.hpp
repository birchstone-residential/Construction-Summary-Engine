#ifndef __Property__hpp__
#define __Property__hpp__

#include "reports.hpp"

struct Property {
    Property() = default;
    ~Property() = default;

    std::string __name_;
    std::string __entrata_name_;
    unsigned    __id_;

    std::vector<UnitExpenses>             __unit_expenses_;
    std::vector<LeaseTradeOut>            __lease_trade_out_;
    std::vector<RollingRenovation>        __rolling_renovation_;
    std::vector<JobCostingHistorical>     __job_costing_historical_;
    std::vector<JobCosting>               __job_costing_;
    std::vector<Unit>                     __units_;
    std::vector<MasterTrackerData>        __master_tracker_data_;
    std::vector<Amenities>                __amenities_;

    std::vector<ConstructionSummaryEntry> __construction_summary_historical_;
    std::vector<ConstructionSummaryEntry> __construction_summary_updated_;
};

#endif // __Property__hpp__