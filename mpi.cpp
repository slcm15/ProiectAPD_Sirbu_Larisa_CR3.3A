#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

struct Point {
    double x, y, z;
};

const int MAX_POINTS = 8000;
Point points[MAX_POINTS];
double distanceMatrix[MAX_POINTS][MAX_POINTS]; 

int readPoints(const char* filename, Point points[]) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error opening input file" << endl;
        return 0;
    }
    int count = 0;
    while (file >> points[count].x >> points[count].y >> points[count].z) {
        count++;
    }
    return count;
}

double euclideanDistance(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y) +
        (p1.z - p2.z) * (p1.z - p2.z));
}

void getWorkload(int rank, int size, int total, int& start, int& num_rows) {
    int base = total / size;
    int extra = total % size;
    start = rank * base + min(rank, extra);
    num_rows = base + (rank < extra ? 1 : 0);
}

void saveMatrix(const char* filename, int num_points) {
    ofstream file(filename);
    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_points; j++) {
            file << distanceMatrix[i][j] << " ";
        }
        file << "\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto start_time = high_resolution_clock::now();

    const char* inputFile = "C:\\Users\\Larisa\\Desktop\\Facultate\\APD2025\\APD2025\\GenerareFisier\\input8000.txt";
    const char* outputFile = "distances.txt";

    int num_points = 0;

    
    MPI_Datatype MPI_Point;
    MPI_Type_contiguous(3, MPI_DOUBLE, &MPI_Point);
    MPI_Type_commit(&MPI_Point);

    if (rank == 0) {
        num_points = readPoints(inputFile, points);
        if (num_points == 0) {
            cerr << "Error" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&num_points, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(points, num_points, MPI_Point, 0, MPI_COMM_WORLD);

    
    int offset, num_rows;
    getWorkload(rank, size, num_points, offset, num_rows);

    
    double* localMatrix = new double[num_rows * num_points];

    
    for (int i = 0; i < num_rows; i++) {
        int global_i = offset + i;
        for (int j = 0; j < num_points; j++) {
            if (global_i == j)
                localMatrix[i * num_points + j] = 0.0;
            else
                localMatrix[i * num_points + j] = euclideanDistance(points[global_i], points[j]);
        }
    }

    if (rank == 0) {
        
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_points; j++) {
                distanceMatrix[offset + i][j] = localMatrix[i * num_points + j];
            }
        }

        
        for (int p = 1; p < size; ++p) {
            int p_offset, p_rows;
            getWorkload(p, size, num_points, p_offset, p_rows);

            double* recvBuffer = new double[p_rows * num_points];
            MPI_Recv(recvBuffer, p_rows * num_points, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < p_rows; i++) {
                for (int j = 0; j < num_points; j++) {
                    distanceMatrix[p_offset + i][j] = recvBuffer[i * num_points + j];
                }
            }
            delete[] recvBuffer;
        }

        saveMatrix(outputFile, num_points);

        auto end_time = high_resolution_clock::now();
        duration<double> elapsed = end_time - start_time;

        cout << "Distances saved in: " << outputFile << endl;
        cout << "Execution time: " << elapsed.count() << " seconds" << endl;
    }
    else {
        
        MPI_Send(localMatrix, num_rows * num_points, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    delete[] localMatrix;
    MPI_Type_free(&MPI_Point);
    MPI_Finalize();
    return 0;
}
