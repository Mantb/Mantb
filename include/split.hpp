#pragma once
#include <vector>
#include <string>

struct Split{
    std::vector<std::string> train,validation,test;
};

Split load_split(const std::string& path);
