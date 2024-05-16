#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h> 
#include <omp.h>

#define SIZE_X 100
#define SIZE_Y 100
#define TIME_STEPS 1000
#define SOURCE_X 50
#define SOURCE_Y 50
#define SOURCE_AMP 50.0
#define SOURCE_TIME_SPAN 100
#define c 0.5
#define dt 0.01
#define dx 1.0
#define dy 1.0

void update_field(float Ez[SIZE_X][SIZE_Y], float Hy[SIZE_X][SIZE_Y], float Hx[SIZE_X][SIZE_Y]) {
    int i, j;

    #pragma omp parallel for shared(Ez, Hy, Hx) private(i, j)
    for (i = 1; i < SIZE_X - 1; i++) {
        for (j = 1; j < SIZE_Y - 1; j++) {
            Ez[i][j] += (Hy[i][j] - Hy[i+1][j] - Hx[i][j] + Hx[i][j-1]) * dt / dx;
        }
    }

    #pragma omp parallel for shared(Ez, Hy, Hx) private(i, j)
    for (i = 1; i < SIZE_X - 1; i++) {
        for (j = 1; j < SIZE_Y - 1; j++) {
            Hy[i][j] += (Ez[i-1][j] - Ez[i][j] - Hx[i][j] + Hx[i][j+1]) * dt / dx;
        }
    }

    #pragma omp parallel for shared(Ez, Hy, Hx) private(i, j)
    for (i = 1; i < SIZE_X - 1; i++) {
        for (j = 1; j < SIZE_Y - 1; j++) {
            Hx[i][j] += (Ez[i][j] - Ez[i][j-1] - Hy[i][j] + Hy[i+1][j]) * dt / dx;
        }
    }
}


void initialize(float Ez[SIZE_X][SIZE_Y], float Hy[SIZE_X][SIZE_Y], float Hx[SIZE_X][SIZE_Y]) {
    int i, j;

    #pragma omp parallel for shared(Ez, Hy, Hx) private(i, j)
    for (i = 0; i < SIZE_X; i++) {
        for (j = 0; j < SIZE_Y; j++) {
            Ez[i][j] = 0.0;
            Hy[i][j] = 0.0;
            Hx[i][j] = 0.0;
        }
    }
}

void add_source(float Ez[SIZE_X][SIZE_Y], int time) {
    if (time < SOURCE_TIME_SPAN) {
        Ez[SOURCE_X][SOURCE_Y] = SOURCE_AMP * sin(2.0 * M_PI * time / 20.0);
    }
}

void save_data(float Ez[SIZE_X][SIZE_Y], int time_step) {
    char filename[50];
    FILE *file;

    sprintf(filename, "data_%d.dat", time_step);
    file = fopen(filename, "w");
    if (file != NULL) {
        for (int i = 0; i < SIZE_X; i++) {
            for (int j = 0; j < SIZE_Y; j++) {
                fprintf(file, "%f ", Ez[i][j]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
}

int main() {
    float Ez[SIZE_X][SIZE_Y];
    float Hy[SIZE_X][SIZE_Y];
    float Hx[SIZE_X][SIZE_Y];
    int t;

    initialize(Ez, Hy, Hx);

    #pragma omp parallel shared(Ez, Hy, Hx)
    {
        #pragma omp for
        for (t = 0; t < TIME_STEPS; t++) {
            add_source(Ez, t);
            update_field(Ez, Hy, Hx);
            save_data(Ez, t);
        }
    }

    // Plotting using Gnuplot
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "set pm3d map\n");
    fprintf(gnuplotPipe, "set xrange [0:%d]\n", SIZE_X);
    fprintf(gnuplotPipe, "set yrange [0:%d]\n", SIZE_Y);
    fprintf(gnuplotPipe, "set cbrange [-%f:%f]\n", SOURCE_AMP, SOURCE_AMP);
    fprintf(gnuplotPipe, "set title '2D FDTD Simulation'\n");
    fprintf(gnuplotPipe, "set xlabel 'X'\n");
    fprintf(gnuplotPipe, "set ylabel 'Y'\n");
    fprintf(gnuplotPipe, "set zlabel 'Ez'\n");
    fprintf(gnuplotPipe, "splot 'data_0.dat' matrix with lines\n");
    fflush(gnuplotPipe);

    for (int i = 1; i < TIME_STEPS; i++) {
        fprintf(gnuplotPipe, "splot 'data_%d.dat' matrix with image\n", i);
        fflush(gnuplotPipe);
        usleep(100000); 
    }

    pclose(gnuplotPipe);

    return 0;
}
