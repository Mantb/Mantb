#include "args.hpp"
using namespace std;

//extract(-flag <value>) pairs from command line
map<string,string> tokenize(int argc, char** argv){
    map<string,string> tokens;

    for(int i = 1;i < argc;i++){
        //check if first argument is a flag
        string flag = argv[i];
        if(flag.size() < 2 || flag[0] != '-'){
            cout<<"invalid flag: "<<flag<<endl;
            continue;
        }
        //check if second argument is a value
        string value;
        if(i + 1 < argc && argv[i + 1][0] != '-'){
            value = argv[i + 1];
            i++;
        }
        //check for duplicate flags
        if(!tokens.emplace(flag,value).second){
            cout<<"duplicate flag: "<<flag<<endl;
            continue;
        }
    }
    return tokens;
}

//Helper functions
//converts Method enum to string
const char* method_name(Method m){
    switch(m){
        case Method::Exact: return "Exact";
        case Method::LSH: return "LSH";
        case Method::Hypercube: return "Hypercube";
        case Method::IVFFlat: return "IVFFlat";
        case Method::IVFPQ: return "IVFPQ";
    }
    return "Unknown method";
}

//convert string to int(throw exception if invalid)
int to_int(const std::string& flag,const string& s){
    size_t pos = 0;
    int num = 0;
    try{
        //starts parsting from position 0
        num = stoi(s,&pos);
    }catch(const exception&){
        pos = 0;
    }
    //check if nothing was read or part of the string was not read
    if(pos == 0 || pos != s.size()){
        throw std::runtime_error("invalid value for " + flag + ": " + s);
    }
    return num;
}

//convert strig to double
double to_double(const std::string& flag,const string& s){
    size_t pos = 0;
    double num = 0;
    try{
        num = stod(s,&pos);
    }catch(const exception&){
        pos = 0;
    }

    if(pos == 0 || pos != s.size()){
        throw std::runtime_error("invalid value for " + flag + ": " + s);
    }
    return num;
}

Args parse_args(int argc, char** argv){
    Args args;
    map<string,string> tokens = tokenize(argc,argv);

    //check if all flags are present and have values
    for(const char* flag : {"-hp", "-mir", "-split", "-vocab", "-D", "-set", "-o"}){
        auto it = tokens.find(flag);
        if(it == tokens.end() || it->second.empty())
            throw runtime_error(string("missing required option ") + flag);
    }

    //parse flags
    for(auto& [flag,value]:tokens){
        if(flag == "-hp") args.hp = value;
        else if(flag == "-mir") args.mir = value;
        else if(flag == "-split") args.split = value;
        else if(flag == "-set") args.set = value;
        else if(flag == "-o") args.output = value;
        else if(flag == "-vocab") args.vocab = to_int(flag, value);
        else if(flag == "-S") args.S = to_int(flag, value);
        else if(flag == "-D") args.D = to_int(flag, value);
        else if(flag == "-seed") args.seed = to_int(flag, value);
        else if(flag == "-exact") args.method = Method::Exact;
        else if(flag == "-lsh") args.method = Method::LSH;
        else if(flag == "-hypercube") args.method = Method::Hypercube;
        else if(flag == "-ivfflat") args.method = Method::IVFFlat;
        else if(flag == "-ivfpq") args.method = Method::IVFPQ;
        else if(flag == "-L") args.L = to_int(flag, value);
        else if(flag == "-k") args.k = to_int(flag, value);
        else if(flag == "-w") args.w = to_double(flag, value);
        else if(flag == "-probes") args.probes = to_int(flag, value);
        else if(flag == "-kproj") args.kproj = to_int(flag, value);
        else if(flag == "-nprobe") args.nprobe = to_int(flag, value);
        else if(flag == "-nbits") args.nbits = to_int(flag, value);
        else if(flag == "-M") args.M = to_int(flag, value);
        else if(flag == "-kclusters") args.kclusters = to_int(flag, value);
        else throw std::runtime_error("unknown flag: " + flag);
    }

    //total size of database
    int n = 115 + args.D;

    if(args.kclusters == 0){
        args.kclusters = (int)ceil((double)n);
    }

    //M default depends on the method, so it can only be set after the loop
    if(args.M == 0) args.M = (args.method == Method::IVFPQ) ? 16 : 500;

    //IVFPQ(K must be divisible by M)
    if(args.method == Method::IVFPQ && args.vocab % args.M != 0){
        throw runtime_error("-vocab must be divisible by -M for IVFPQ");
    }

    //-set must have one og two values(validation or test)
    if(args.set != "validation" && args.set != "test"){
        throw runtime_error("-set must be validation or test, got '" + args.set + "'");
    }

    return args;
}