#include <bits/stdc++.h>

using namespace std;


int main(){
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");  // Open a pipe to gnuplot

    if (gnuplotPipe) {   // If gnuplot is found

        fprintf(gnuplotPipe, "reset\n"); //gnuplot commands
        // fprintf(gnuplotPipe, "set terminal postscript enhanced color\n");
        // fprintf(gnuplotPipe, "n='500'\n");
        // fprintf(gnuplotPipe, "max='1500'\n");
        // fprintf(gnuplotPipe, "min='-1500\n");
        // fprintf(gnuplotPipe, "width=(max-min)/n\n");
        // fprintf(gnuplotPipe, "hist(x,width)=width*floor(x/width)+width/2.0\n");
        // fprintf(gnuplotPipe, "set term png #output terminal and file\n");
        // fprintf(gnuplotPipe, "set output 'Observable_Histogram.png'\n");
        // fprintf(gnuplotPipe, "set xrange [min:max]\n");
        // fprintf(gnuplotPipe, "set yrange [0:]\n");
        // fprintf(gnuplotPipe, "set offset graph 0.05,0.05,0.05,0.0\n");
        // fprintf(gnuplotPipe, "set xtics min,(max-min)/5,max\n");
        // fprintf(gnuplotPipe, "set boxwidth width*0.9\n");
        // fprintf(gnuplotPipe, "set style fill solid 0.5\n");
        // fprintf(gnuplotPipe, "set tics out nomirror\n");
        fprintf(gnuplotPipe, "set termoption lw 1.5\n");
        fprintf(gnuplotPipe, "set key right top\n");
        fprintf(gnuplotPipe, "set key box vertical width 1 height 0.5 maxcols 1 spacing 1\n");
        fprintf(gnuplotPipe, "set xlabel 'Time (in seconds)'\n");
        fprintf(gnuplotPipe, "set ylabel 'Density'\n");
        fprintf(gnuplotPipe, "set title 'Vehicular density vs. Time'\n");
        // fprintf(gnuplotPipe, "plot 'output.txt' u (hist($1,width)):(1.0) smooth freq w boxes lc rgb'green' notitle\n");
        fprintf(gnuplotPipe, "plot './plot.txt' using 1:2 with lines title 'Queue Density', './plot.txt' using 1:3 with lines title 'Moving Density'\n");

        fflush(gnuplotPipe); //flush pipe

        fprintf(gnuplotPipe,"exit \n");   // exit gnuplot
        pclose(gnuplotPipe);    //close pipe

    }
}