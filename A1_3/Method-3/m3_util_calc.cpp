#include <bits/stdc++.h>

using namespace std;

float calc_utility(int c, int m){
    float utility = c*1.0/600.0 + m*1.0/7000000;
    return utility;
}

int main(){
    vector<int> threads {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 24};
    vector<float> time_taken {559.781, 370.711, 209.43, 198.865, 154.744, 138.999, 128.151, 148.364, 228.224, 164.144, 153.062, 147.524};
    vector<int> cpu_usage {630, 657, 656, 655, 656, 634, 656, 653, 658, 654, 658, 657};
    vector<int> memory_usage {7183960, 7183880, 7184524, 7184080, 7183656, 7183580, 7184336, 7183352, 7184572, 7184988, 7185180, 7184008};

    assert(threads.size() == time_taken.size());
    assert(threads.size() == cpu_usage.size());
    assert(threads.size() == memory_usage.size());

    cout<<"Threads,time_taken,utility\n";
    for(int i=0;i<threads.size();i++){
        cout<<threads[i]<<","<<time_taken[i]<<","<<calc_utility(cpu_usage[i], memory_usage[i])<<"\n";
    }
}