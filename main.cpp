#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <chrono>
#include <omp.h>

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

        double c = 0.0;
        double sum1 = 0.0, sum2 = 0.0;
        double sum3 = 0.0, sum4 = 0.0;
        double y, t;

                
        #pragma omp parallel for schedule(dynamic) reduction(+:sum1)
        for (int i=0;i<npart;i++) {
            sum1 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        for (int i=0;i<npart;i++) {
            sum2 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        for (int i=npart-1;i>=0;i--) {
            sum3 +=pos[i][0]+pos[i][1]+pos[i][2];
        }

        for (int i=0;i<npart;i++) {
            y = pos[i][0]+pos[i][1]+pos[i][2] - c;
            t = sum4+y;
            c = (t - sum4) - y;
            sum4 = t;
        }
        printf("Four different summations:\n");
        printf(" OpenMP: \t\t%1.16e\n Serial 0->nparts: \t%1.16e\n Serial nparts-->0: \t%1.16e\n Kahan: \t\t%1.16e\n", sum1,sum2,sum3,sum4);
    }
    else cout << "Error opening file " << argv[1] << ".\n\n" << endl;

    cout << "Exiting normally..." << endl;
}