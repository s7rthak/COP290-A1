#include <bits/stdc++.h>

using namespace std;


int main(){
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");  // Open a pipe to gnuplot

    if (gnuplotPipe) {   // If gnuplot is found

        fprintf(gnuplotPipe, "reset\n"); //gnuplot commands
        // fprintf(gnuplotPipe, "set xrange [0:1]\n");
        // fprintf(gnuplotPipe, "set yrange [0.5:1.5]\n");
        // fprintf(gnuplotPipe, "set xtics min,(max-min)/5,max\n");
        fprintf(gnuplotPipe, "set term push\n");
        fprintf(gnuplotPipe, "set term png\n");
        fprintf(gnuplotPipe, "set term png size 700,500\n");
        fprintf(gnuplotPipe, "set output 'm4-scatter-plot.png'\n");
        // fprintf(gnuplotPipe, "set termoption lw 1.5\n");
        // fprintf(gnuplotPipe, "set key right top\n");
        // fprintf(gnuplotPipe, "set key box vertical width 1 height 0.5 maxcols 1 spacing 1\n");
        fprintf(gnuplotPipe, "set xlabel 'Runtime (in ms)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Utility'\n");
        // fprintf(gnuplotPipe, "set title 'Vehicular density vs. Time'\n");
        // fprintf(gnuplotPipe, "set arrow from 99,0 to 99,1 nohead lc rgb 'forest-green' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 182,0 to 182,1 nohead lc rgb 'red' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 279,0 to 279,1 nohead lc rgb 'forest-green' lw 2 dt 2\n");
        // fprintf(gnuplotPipe, "set arrow from 352,0 to 352,1 nohead lc rgb 'red' lw 2 dt 2\n");
        fprintf(gnuplotPipe, "set datafile separator ','\n");
        fprintf(gnuplotPipe, "set boxwidth 0.5\n");
        fprintf(gnuplotPipe, "set style fill solid\n");
        // fprintf(gnuplotPipe, "plot './plot.txt' using 1:3:xtic(2) with boxes\n");
        fprintf(gnuplotPipe, "plot './plot.txt' using 2:3 with p lw 1.5 pt 7 ps 1, '' using 2:3:1 with labels offset 0,char 1\n");
        // fprintf(gnuplotPipe, "plot './plot.txt' using 1:2 with lines title 'Queue Density' lt rgb 'dark-violet' lw 1.5, './plot.txt' using 1:3 with lines title 'Moving Density' lt rgb 'coral' lw 1.5\n");
        fprintf(gnuplotPipe, "set term pop\n");
        fprintf(gnuplotPipe, "set term qt size 700,500\n");
        fprintf(gnuplotPipe, "set output\n");
        fprintf(gnuplotPipe, "replot\n");

        fflush(gnuplotPipe); //flush pipe

        fprintf(gnuplotPipe,"exit \n");   // exit gnuplot
        pclose(gnuplotPipe);    //close pipe

    }
}