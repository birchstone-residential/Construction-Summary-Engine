#ifndef __PropertyList__hpp__
#define __PropertyList__hpp__

#include "Property.hpp"

struct PropertyList {
    PropertyList() = default;
    ~PropertyList() = default;
    std::vector<Property>& operator()() { return __properties_; }

    std::vector<Property> __properties_;
};

#endif // __PropertyList__hpp__