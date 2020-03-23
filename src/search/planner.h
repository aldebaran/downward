#ifndef PLANNER_H
#define PLANNER_H

#include <string>

#define EXPORT __attribute__((visibility("default")))

EXPORT std::string plan_from_sas(const std::string& sas, const std::string& search_strategy);

#endif //PLANNER_H
