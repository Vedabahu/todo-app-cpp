#pragma once

#include <string>

struct Todo
{
    std::string id;
    std::string user_id;
    std::string title;
    bool completed;
};