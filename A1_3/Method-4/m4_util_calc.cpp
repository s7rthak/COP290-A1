#include <bits/stdc++.h>

using namespace std;

float calc_utility(int c, int m){
    float utility = c*1.0/600.0 + m*1.0/7000000;
    return utility;
}

int main(){
    vector<int> threads {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 24};
    vector<float> time_taken {15254.9, 9638.66, 5025.26, 4420.66, 4391.02, 4076.64, 3961.64, 4082.39, 4058.83, 4031.87, 4130.09, 3942.28};
    vector<int> cpu_usage {412, 300, 352, 374, 394, 407, 431, 417, 432, 425, 431, 428};
    vector<int> memory_usage {7185908, 7194672, 7213576, 7233656, 7251176, 7272076, 7289540, 7308816, 7326220, 7346320, 7364588, 7402384};

    assert(threads.size() == time_taken.size());
    assert(threads.size() == cpu_usage.size());
    assert(threads.size() == memory_usage.size());

    cout<<"Threads,time_taken,utility\n";
    for(int i=0;i<threads.size();i++){
        cout<<threads[i]<<","<<time_taken[i]<<","<<calc_utility(cpu_usage[i], memory_usage[i])<<"\n";
    }
}   