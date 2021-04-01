#include "util.hpp"

using namespace std;

int main (){
    vector<string> all_data;
    all_data.push_back("m1-5.txt");
    all_data.push_back("m1-10.txt");
    all_data.push_back("m1-15.txt");
    all_data.push_back("m1-20.txt");

    string base = "orig.txt";

    vector<float> utility;
    vector<float> baseline = store_baseline(base);
    vector<float> benchmark_5 = store_m1_bench(all_data[0], 5, baseline.size());
    vector<float> benchmark_10 = store_m1_bench(all_data[1], 10, baseline.size());
    vector<float> benchmark_15 = store_m1_bench(all_data[2], 15, baseline.size());
    vector<float> benchmark_20 = store_m1_bench(all_data[3], 20, baseline.size());

    assert(baseline.size() == benchmark_5.size());
    assert(baseline.size() == benchmark_10.size());
    assert(baseline.size() == benchmark_15.size());
    assert(baseline.size() == benchmark_20.size());
    
    utility.push_back(calc_utility(baseline, benchmark_5));
    utility.push_back(calc_utility(baseline, benchmark_10));
    utility.push_back(calc_utility(baseline, benchmark_15));
    utility.push_back(calc_utility(baseline, benchmark_20));

    for(int i=0;i<utility.size();i++){
        cout<<utility[i]<<"\n";
    }
}