#include "util.hpp"

using namespace std;

float calc_utility(vector<float> &baseline, vector<float> &benchmark, int c, int m){
    float utility = 0, error = 1;
    for(int i=0;i<baseline.size();i++){
        error += (baseline[i] - benchmark[i]) * (baseline[i] - benchmark[i]);
    }
    error = sqrt(error/baseline.size());
    utility = -(log10(error)) + c/200 + m/200000;
    return utility;
}

int main (){
    string bench = "sparse.txt";
    string base = "dense.txt";

    vector<float> baseline = store_file(base);
    vector<float> benchmark = store_file(bench);

    assert(baseline.size() == benchmark.size());

    float base_util = calc_utility(baseline, baseline, 110, 254320);
    float bench_util = calc_utility(benchmark, baseline, 240, 235788);
    cout<<"Base,"<<base_util<<"\n"<<"Bench,"<<bench_util<<"\n";
}