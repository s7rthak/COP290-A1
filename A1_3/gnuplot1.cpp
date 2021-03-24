#include <bits/stdc++.h>

using namespace std;


int main(){
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");  // Open a pipe to gnuplot

    if (gnuplotPipe) {   // If gnuplot is found

        fprintf(gnuplotPipe, "reset\n"); //gnuplot commands
        // fprintf(gnuplotPipe, "set xrange [0:1]\n");
        // fprintf(gnuplotPipe, "set yrange [0:]\n");
        // fprintf(gnuplotPipe, "set xtics min,(max-min)/5,max\n");
        fprintf(gnuplotPipe, "set term push\n");
        fprintf(gnuplotPipe, "set term png\n");
        fprintf(gnuplotPipe, "set term png size 1400,1000\n");
        fprintf(gnuplotPipe, "set output 'line-graph1.png'\n");
        fprintf(gnuplotPipe, "set termoption lw 1.5\n");
        fprintf(gnuplotPipe, "set key right top\n");
        fprintf(gnuplotPipe, "set key box vertical width 1 height 0.5 maxcols 1 spacing 1\n");
        fprintf(gnuplotPipe, "set xlabel 'Time (in seconds)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Density'\n");
        fprintf(gnuplotPipe, "set title 'Vehicular density vs. Time'\n");
        // fprintf(gnuplotPipe, "set arrow from 99,0 to 99,1 nohead lc rgb 'forest-green' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 182,0 to 182,1 nohead lc rgb 'red' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 279,0 to 279,1 nohead lc rgb 'forest-green' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 352,0 to 352,1 nohead lc rgb 'red' lw 2 dt 2\n");
        fprintf(gnuplotPipe, "plot './plot1.txt' using 1:2 with lines title 'Queue Density' lt rgb 'dark-violet' lw 1.5, './plot1.txt' using 1:3 with lines title 'Moving Density' lt rgb 'coral' lw 1.5\n");
        fprintf(gnuplotPipe, "set term pop\n");
        fprintf(gnuplotPipe, "set term qt size 1400,1000\n");
        fprintf(gnuplotPipe, "set output\n");
        fprintf(gnuplotPipe, "replot\n");

        fflush(gnuplotPipe); //flush pipe

        fprintf(gnuplotPipe,"exit \n");   // exit gnuplot
        pclose(gnuplotPipe);    //close pipe

    }
}