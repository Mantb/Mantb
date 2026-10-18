#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Split{
    std::vector<std::string> train,validation,test;
};

Split load_split(const std::string& path);
