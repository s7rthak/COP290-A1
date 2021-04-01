#include <bits/stdc++.h>
#include <fstream>

using namespace std;

vector<float> store_file(string s){
    ifstream base;
    base.open(s);
    string line;

    vector<float> baseline_values;
    while(getline(base,line))
    {
        stringstream linestream(line);
        string value;
        vector<float> temp;
        while(getline(linestream,value,','))
        {
            temp.push_back(stof(value));
        }
        baseline_values.push_back(temp[1]);
    }
    return baseline_values;
}