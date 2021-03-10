#include <bits/stdc++.h>

using namespace std;


int main(){
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");  // Open a pipe to gnuplot

    if (gnuplotPipe) {   // If gnuplot is found

        fprintf(gnuplotPipe, "reset\n"); //gnuplot commands
        // fprintf(gnuplotPipe, "set xrange [min:max]\n");
        // fprintf(gnuplotPipe, "set yrange [0:]\n");
        // fprintf(gnuplotPipe, "set xtics min,(max-min)/5,max\n");
        fprintf(gnuplotPipe, "set output 'Graph.png'\n");
        fprintf(gnuplotPipe, "set termoption lw 1.5\n");
        fprintf(gnuplotPipe, "set key right top\n");
        fprintf(gnuplotPipe, "set key box vertical width 1 height 0.5 maxcols 1 spacing 1\n");
        fprintf(gnuplotPipe, "set xlabel 'Time (in seconds)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Density'\n");
        fprintf(gnuplotPipe, "set title 'Vehicular density vs. Time'\n");
        fprintf(gnuplotPipe, "plot './plot.txt' using 1:2 with lines title 'Queue Density', './plot.txt' using 1:3 with lines title 'Moving Density'\n");

        fflush(gnuplotPipe); //flush pipe

        fprintf(gnuplotPipe,"exit \n");   // exit gnuplot
        pclose(gnuplotPipe);    //close pipe

    }
}