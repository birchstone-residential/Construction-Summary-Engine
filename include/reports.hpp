#ifndef __reports__hpp__
#define __reports__hpp__

#include <vector>

struct JobExpenseTotals
{
    float __reno_kits_         = 0.f;
    float __appliances_        = 0.f;
    float __paint_             = 0.f;
    float __flooring_          = 0.f;
    float __labor_             = 0.f;
    float __washer_and_dryer_  = 0.f;
    float __backsplash_        = 0.f;
    float __countertops_       = 0.f;
    float __cabinets_          = 0.f;
    float __other_             = 0.f;
    float __undefined_         = 0.f;
    
    float __sum_               = 0.f;
};

struct UnitExpenses
{
    std::string __property_;

    std::string __transaction_type_;
    float       __budget_;
    std::string __transaction_number_;
    std::string __vendor_;
    std::string __vendor_code_;
    std::string __bldg_unit_;
    std::string __post_month_;
    std::tm     __post_date_;
    std::string __post_date_str_ = "~";
    std::tm     __transaction_date_;
    std::string __transaction_date_str_ = "~";
    std::tm     __completion_date_;
    std::string __completion_date_str_ = "~";
    std::string __gl_account_;
    std::string __job_cost_code_;
    std::string __job_;
    std::string __description_;
    float       __amount_;
};

struct LeaseTradeOut
{
    std::string __property_;
    std::string __bldg_unit_;
    std::string __unit_;
    std::string __skip_mtm_;
    std::string __unit_type_;
    std::string __sqft_;
    int         __days_vacant_;
    std::string __days_vacant_str_ = "~";
    std::string __prior_resident_;
    std::tm     __move_in_;
    std::string __move_in_str_ = "~";
    std::string __current_resident_;
    std::string __current_lease_type_;
    std::tm     __prior_lease_start_;
    std::tm     __prior_lease_end_;
    std::tm     __current_lease_start_;
    std::tm     __current_lease_end_;
    float       __prior_base_rent_;
    std::string __prior_base_rent_str_ = "~";
    float       __prior_amenity_rent_;
    std::string __prior_amenity_rent_str_ = "~";
    float       __prior_lease_rent_;
    std::string __prior_lease_rent_str_ = "~";
    float       __current_base_rent_;
    std::string __current_base_rent_str_ = "~";
    float       __current_amenity_rent_;
    std::string __current_amenity_rent_str_ = "~";
    float       __current_lease_rent_;
    std::string __current_lease_rent_str_ = "~";
    float       __market_rent_;
    std::string __market_rent_str_ = "~";
    float       __budgeted_rent_;
    std::string __budgeted_rent_str_ = "~";
    float       __lease_rent_change_;
    std::string __lease_rent_change_str_ = "~";

    bool operator<(const LeaseTradeOut& other) const {
        std::tm lhs = __current_lease_start_;
        std::tm rhs = other.__current_lease_start_;
        return std::mktime(&lhs) < std::mktime(&rhs);
    }
};

struct RollingRenovation
{
    std::string __property_;
    std::string __bldg_unit_;
    std::string __unit_type_;
    std::string __sqft_;
    std::string __unit_status_;
    std::string __renovation_status_;
    std::string __phase_;
    std::tm     __move_out_date_;
    std::tm     __renovation_start_date_;
    std::string __renovation_start_date_str_ = "~";
    std::tm     __renovation_completion_date_;
    std::string __renovation_completion_date_str_ = "~";
    int         __actual_renovation_days_;
    std::string __actual_renovation_days_str_ = "~";
    std::tm     __move_in_date_;
    std::string __move_in_date_str_ = "~";
    float       __actual_renovation_cost_;
    float       __budgeted_renovation_cost_;
    float       __actual_vs_budget_variance_;
    float       __monthly_unit_premium_;
    float       __monthly_roi_;
    float       __prior_budgeted_rent_;
    float       __new_budgeted_rent_;
};

struct JobCostingHistorical
{
    std::string __property_;
    
    std::string __bldg_unit_;
    std::string __unit_type_;
    std::string __unit_status_;
    std::string __reno_status_;
    std::string __phase_;
    std::tm     __move_out_date_;
    std::tm     __reno_begin_;
    std::tm     __reno_end_;
    int         __days_in_reno_;
    std::tm     __move_in_date_;
    float       __actual_reno_cost_;
    float       __budgeted_reno_cost_;
    float       __actual_vs_budget_variance_;
    float       __prior_lease_rent_;
    float       __current_lease_rent_;
    float       __prior_premium_;
    float       __current_premium_;
    float       __premium_variance_;
    float       __annual_premium_;
    float       __roi_;
    float       __prior_market_rent_classic_;
    float       __current_market_rent_classic_;
    float       __current_market_rent_premium_;
    float       __prior_budgeted_rent_;
    float       __new_budgeted_rent_;
};

struct JobCosting
{
    std::string __property_;
    
    std::string __bldg_unit_;
    std::string __unit_type_;
    std::string __unit_status_;
    std::string __reno_status_;
    std::string __phase_;
    std::tm     __move_out_date_;
    std::tm     __reno_begin_;
    std::tm     __reno_end_;
    int         __days_in_reno_;
    std::tm     __move_in_date_;
    float       __actual_reno_cost_;
    float       __budgeted_reno_cost_;
    float       __actual_vs_budget_variance_;
    float       __prior_lease_rent_;
    float       __current_lease_rent_;
    float       __prior_premium_;
    float       __current_premium_;
    float       __premium_variance_;
    float       __annual_premium_;
    float       __roi_;
    float       __prior_budgeted_rent_;
    float       __new_budgeted_rent_;
};

struct Unit
{
    std::string       __property_;

    std::string       __bldg_unit_;
    std::string       __floorplan_;
    std::string       __unit_type_;
    std::string       __unit_status_;
    std::tm           __move_out_;
    std::string       __move_out_str = "~";

    JobExpenseTotals  __totals_;

    double            __amenity_WD_cost_ = 0.f;
    double            __amenity_RENO_cost_ = 0.f;
};

struct MasterTrackerData
{
    std::string __property_;

    std::string __bldg_unit_;
    std::string __unit_only_;
    std::tm     __date_started_;
    std::string __date_started_str_ = "~";
    std::tm     __date_complete_;
    std::string __date_complete_str_ = "~";
    float       __budget_;
};

struct Amenities
{
    std::string __property_;

    std::string __amenity_;
    std::string __floorplan_;
    std::string __unit_type_;
    std::string __unit_space_;
    double      __amenity_rent_;
    std::string __charge_code_;
    std::tm     __effective_date_;
    std::string __effective_date_str_ = "~";
};

struct ConstructionSummaryEntry
{
    ConstructionSummaryEntry& operator= (const ConstructionSummaryEntry& other)
    {
        this->__property_                               = other.__property_;
        this->__bldg_unit_                              = other.__bldg_unit_;
        this->__floorplan_                              = other.__floorplan_;
        this->__unit_type_                              = other.__unit_type_;
        this->__status_                                 = other.__status_;    
        this->__move_out_date_                          = other.__move_out_date_;            
        this->__move_out_date_str_                      = other.__move_out_date_str_;                
        this->__job_                                    = other.__job_;    
        this->__reno_begin_                             = other.__reno_begin_;        
        this->__reno_begin_str_                         = other.__reno_begin_str_;            
        this->__reno_end_                               = other.__reno_end_;        
        this->__reno_end_str_                           = other.__reno_end_str_;            
        this->__days_in_reno_                           = other.__days_in_reno_;            
        this->__move_in_date_                           = other.__move_in_date_;            
        this->__move_in_date_str_                       = other.__move_in_date_str_;                
        this->__prior_lease_rent_                       = other.__prior_lease_rent_;                
        this->__current_lease_rent_                     = other.__current_lease_rent_;                
        this->__prior_premium_                          = other.__prior_premium_;            
        this->__current_premium_                        = other.__current_premium_;                
        this->__premium_variance_                       = other.__premium_variance_;                
        this->__annual_premium_                         = other.__annual_premium_;            
        this->__roi_                                    = other.__roi_;    
        this->__prior_budgeted_rent_                    = other.__prior_budgeted_rent_;                    
        this->__new_budgeted_rent                       = other.__new_budgeted_rent;                
        this->__rent_increase_                          = other.__rent_increase_;            
        this->__budgeted_reno_cost_                     = other.__budgeted_reno_cost_;                
        this->__amenity_cost_WD_                        = other.__amenity_cost_WD_;                
        this->__amenity_cost_RENO_                      = other.__amenity_cost_RENO_;                
        
        this->__job_expenses_.__reno_kits_              = other.__job_expenses_.__reno_kits_;        
        this->__job_expenses_.__appliances_             = other.__job_expenses_.__appliances_;        
        this->__job_expenses_.__paint_                  = other.__job_expenses_.__paint_;    
        this->__job_expenses_.__flooring_               = other.__job_expenses_.__flooring_;        
        this->__job_expenses_.__labor_                  = other.__job_expenses_.__labor_;    
        this->__job_expenses_.__washer_and_dryer_       = other.__job_expenses_.__washer_and_dryer_;                
        this->__job_expenses_.__backsplash_             = other.__job_expenses_.__backsplash_;        
        this->__job_expenses_.__countertops_            = other.__job_expenses_.__countertops_;            
        this->__job_expenses_.__cabinets_               = other.__job_expenses_.__cabinets_;        
        this->__job_expenses_.__other_                  = other.__job_expenses_.__other_;    
        this->__job_expenses_.__undefined_              = other.__job_expenses_.__undefined_;        
        this->__job_expenses_.__sum_                    = other.__job_expenses_.__sum_;    

        this->__savings_dollar_                         = other.__savings_dollar_;
        this->__savings_percent_                        = other.__savings_percent_;

        this->__is_renovated_                           = other.__is_renovated_;
        this->__entry_update_date_                      = other.__entry_update_date_;
        this->__entry_update_date_str_                  = other.__entry_update_date_str_;
    }

    std::string      __property_;
     
    std::string      __bldg_unit_;
    std::string      __floorplan_;
    std::string      __unit_type_;
    std::string      __status_;
    std::tm          __move_out_date_;
    std::string      __move_out_date_str_ = "~";
    std::string      __job_;
    std::tm          __reno_begin_;
    std::string      __reno_begin_str_ = "~";
    std::tm          __reno_end_;
    std::string      __reno_end_str_ = "~";
    int              __days_in_reno_;
    std::tm          __move_in_date_;
    std::string      __move_in_date_str_ = "~";
    double           __prior_lease_rent_;
    double           __current_lease_rent_;
    double           __prior_premium_;
    double           __current_premium_;
    double           __premium_variance_;
    double           __annual_premium_;
    double           __roi_;
    double           __prior_budgeted_rent_;
    double           __new_budgeted_rent;
    double           __rent_increase_;
    double           __budgeted_reno_cost_;
    double           __amenity_cost_WD_;
    double           __amenity_cost_RENO_;

    JobExpenseTotals __job_expenses_;

    double           __savings_dollar_;
    double           __savings_percent_;

    bool             __is_renovated_ = false;
    std::tm          __entry_update_date_;
    std::string      __entry_update_date_str_ = "~";

    std::string      __update_message_ = "";
};


#endif // __reports__hpp__