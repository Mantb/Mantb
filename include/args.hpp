#pragma once
#include <string>

enum class Method{
    Exact,
    LSH,
    Hypercube,
    IVFFlat,
    IVFPQ
};

struct Args{
    //-hp,-mir,-split,-set,-output
    std::string hp,mir,split,set,output;

    //default values
    int vocab = 0;
    int S = 500;
    int D = 0;
    int seed = 1;
    Method method = Method::Exact;

    //LSH parameters
    int L = 5, k = 4;
    double w = 0.05;

    //Hypercube parameters
    int probes = 10,kproj = 10;

    //IVFFlat and IVFPQ parameters
    int nprobe = 5;
    int nbits = 8;
    int M = 0; //0 = not set(default depends on method)
    int kclusters = 0;
};

//parses arguments from command line
Args parse_args(int argc, char** argv);