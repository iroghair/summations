#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <chrono>
#include <omp.h>
#include <quadmath.h>

typedef std::chrono::high_resolution_clock Clock;

using namespace std;

int main(int argc, char* argv[])
{
    int npart;
    
    double** pos;

    cout.precision(numeric_limits<double>::max_digits10);
    cout << "Trying to open file " << argv[1] << " ..." << endl;
    ifstream file(argv[1], ios::binary);

    if (file.is_open())
    {   
        // Read number of particles
        file.read((char*) &npart, sizeof(int));
        cout << "Trying to read " << npart << " positions from file..." << endl;
        
        // Allocate memory
        pos = new double*[npart];
        for (int i = 0; i < npart; i++)
           pos[i] = new double[3];
        
        // Read all particle positions into array
        for (int i = 0; i < npart; i++) {
            file.read((char*) pos[i], sizeof(double[3]));
        }
        file.close();


        double sum1 = 0.0, sum2 = 0.0;
        double sum3 = 0.0, sum4 = 0.0;
        __float128 sum5 = 0.0;

        // Parallel with OpenMP (dynamic scheduler may yield different 
        // results each time the program is run!)
        #pragma omp parallel for schedule(dynamic) reduction(+:sum1)
        for (int i=0;i<npart;i++) {
            sum1 +=pos[i][0]+pos[i][1]+pos[i][2];
        }
        
        // Serial forward
        for (int i=0;i<npart;i++) {
            sum2 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        // Serial backward
        for (int i=npart-1;i>=0;i--) {
            sum3 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        // Kahan summation algorithm
        double c = 0.0;
        double y, t;

        for (int i=0;i<npart;i++) {
            y = pos[i][0]+pos[i][1]+pos[i][2] - c;
            t = sum4+y;
            c = (t - sum4) - y;
            sum4 = t;
        }

        // Now for the quad-precision answer
        for (int i=0;i<npart;i++) {
            sum5 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        int width = 46;
        char buf[128];
        int n = quadmath_snprintf (buf, sizeof(buf), "%1.30Qe", width, sum5);
        printf ("The truth, and nothing but the truth (i.e. quad precision):\n\t\t\t %s\n", buf);

        printf("Four different summations with double:\n");
        printf(" OpenMP: \t\t%1.16e (delta: %+-#1.16e)\n Serial 0->nparts: \t%1.16e (delta: %+-#1.16e)\n Serial nparts-->0: \t%1.16e (delta: %+-#1.16e)\n Kahan: \t\t%1.16e (delta: %+-#1.16e)\n", sum1,(double) sum5-sum1,sum2,(double) sum5-sum2,sum3,(double) sum5-sum3,sum4,(double) sum5-sum4);
    }
    else cout << "Error opening file " << argv[1] << ".\n\n" << endl;

    cout << "Exiting normally..." << endl;
}