#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define SIZE 200
#define MAX_TIME 250
#define COURANT_NUMBER 0.5
#define DELTA_X 0.01
#define DELTA_T (COURANT_NUMBER * DELTA_X / c)

double ez[SIZE] = {0.}, hy[SIZE] = {0.}, imp0 = 377.0;
const double c = 3e8; // Speed of light in vacuum

int main() {
    int qTime, mm;

    // Initialize gnuplot
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe == NULL) {
        printf("Error opening pipe to gnuplot.\n");
        return 1;
    }
    fprintf(gnuplotPipe, "set title 'Electric Field Animation'\n");
    fprintf(gnuplotPipe, "set xlabel 'Time Step'\n");
    fprintf(gnuplotPipe, "set ylabel 'Ez[50]'\n");
    fprintf(gnuplotPipe, "set yrange [-1.5:1.5]\n");
    fprintf(gnuplotPipe, "set xrange [0:%d]\n", SIZE - 1);
    fprintf(gnuplotPipe, "set key off\n");
    fprintf(gnuplotPipe, "set grid\n");
    fflush(gnuplotPipe);

    // Time-stepping loop
    for (qTime = 0; qTime < MAX_TIME; qTime++) {
        // Update magnetic field
        for (mm = 0; mm < SIZE - 1; mm++) {
            hy[mm] = hy[mm] + (ez[mm + 1] - ez[mm]) / imp0;
        }

        // Update electric field
        for (mm = 1; mm < SIZE; mm++) {
            ez[mm] = ez[mm] + (hy[mm] - hy[mm - 1]) * imp0;
        }

        // Hardwire a source node (e.g., a Gaussian pulse)
        ez[0] = exp(-(qTime - 30.) * (qTime - 30.) / 100.);

        // Plot the current electric field
        fprintf(gnuplotPipe, "plot '-' with lines\n");
        for (mm = 0; mm < SIZE; mm++) {
            fprintf(gnuplotPipe, "%d %g\n", mm, ez[mm]);
        }
        fprintf(gnuplotPipe, "e\n");
        fflush(gnuplotPipe);

        usleep(100000); 
    }

    pclose(gnuplotPipe);

    return 0;
}
