#include "imgutils/split.hpp"
using namespace std;

//split data into train,validation or test
Split load_split(const string& path){
    //open file
    ifstream in(path);
    if(!in){
        cout<<"cannot open file";
    }

    string line;
    Split split;
    int linenum=0;

    //parse lines
    while(getline(in,line)){
        ++linenum;

        //check lines arguments (example name = "v_bird",settype="validation" )
        string name,settype;
        istringstream fields(line);

        if(!(fields >> name)) continue;
        if(!(fields >> settype)){
            cout<<"broken line";
        }

        if(settype == "train") split.train.push_back(name);
        else if (settype == "validation") split.validation.push_back(name);
        else if (settype == "test") split.test.push_back(name);
        else cout<<"unknown settype";
    }

    return split;
}