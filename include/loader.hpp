#pragma once
#include <vector>
#include <string>

struct Image{
    std::string id;
    std::string path;
};

//loads the 6 images (1.ppm ... 6.ppm) of one HPatches sequence
std::vector<Image> load_dataset(const std::string& hp_directory, const std::string& seq);
