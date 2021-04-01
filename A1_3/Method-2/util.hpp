#include <bits/stdc++.h>
#include <fstream>

using namespace std;

vector<float> store_baseline(string s){
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

vector<float> store_m1_bench(string s, int p, int total_sz){
    ifstream bench;
    bench.open(s);
    string line;

    vector<float> bench_values;
    while(getline(bench,line))
    {
        stringstream linestream(line);
        string value;
        vector<float> temp;
        while(getline(linestream,value,','))
        {
            temp.push_back(stof(value));
        }
        for(int i=0;i<p;i++){
            bench_values.push_back(temp[1]);
        }
    }
    while(bench_values.size() != total_sz){
        bench_values.push_back(bench_values[bench_values.size()-1]);
    }
    return bench_values;
}

vector<float> store_m2_bench(string s){
    ifstream bench;
    bench.open(s);
    string line;

    vector<float> bench_values;
    while(getline(bench,line))
    {
        stringstream linestream(line);
        string value;
        vector<float> temp;
        while(getline(linestream,value,','))
        {
            temp.push_back(stof(value));
        }
        bench_values.push_back(temp[1]);
    }
    return bench_values;
}

float calc_utility(vector<float> &base, vector<float> &bench){
    float error = 0.0;
    for(int i=0;i<base.size();i++){
        error += (base[i] - bench[i]) * (base[i] - bench[i]);
    }
    error = sqrt(error/base.size());
    float utility = -(log10(error));
    return utility;
}