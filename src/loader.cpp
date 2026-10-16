#include "loader.hpp"
#include <vector>
#include <cctype>
#include <iostream>
#include <filesystem>
using namespace std;

//load dataset from directory
vector<Image> load_dataset(const string& hp_directory, const string& seq){
    vector<Image> images;

    //one sequence has 6 images
    for(int i=1;i<=6;i++){
        //build path to each image file
        filesystem::path p = filesystem::path (hp_directory) / seq / (to_string(i) + ".ppm");
        if(!filesystem::exists(p)){
            cout<<"file does not exist: "<<p<<endl;
            continue;
        }
        images.push_back({seq + "/" + to_string(i), p.string()});
    }
    return images;
}