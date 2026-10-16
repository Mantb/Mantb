#include <iostream>
#include "split.hpp"
#include "loader.hpp"
#include "args.hpp"
#include <fstream>
using namespace std;

int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);
    Split dataset = load_split("/Mantb/split.txt");
    //vector<Image> images = load_dataset("hp_directory",);

    return 0;
}