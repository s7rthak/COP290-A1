#include "util.hpp"

using namespace std;

int main(){
    vector<string> all_data;
    all_data.push_back("m2-1600x1200.txt");
    all_data.push_back("m2-1440x1080.txt");
    all_data.push_back("m2-1280x960.txt");
    all_data.push_back("m2-1024x768.txt");

    string base = "orig.txt";

    vector<float> utility;
    vector<float> baseline = store_baseline(base);
    vector<float> benchmark_1 = store_m2_bench(all_data[0]);
    vector<float> benchmark_2 = store_m2_bench(all_data[1]);
    vector<float> benchmark_3 = store_m2_bench(all_data[2]);
    vector<float> benchmark_4 = store_m2_bench(all_data[3]);
    
    utility.push_back(calc_utility(baseline, benchmark_1));
    utility.push_back(calc_utility(baseline, benchmark_2));
    utility.push_back(calc_utility(baseline, benchmark_3));
    utility.push_back(calc_utility(baseline, benchmark_4));

    for(int i=0;i<utility.size();i++){
        cout<<utility[i]<<"\n";
    }
}