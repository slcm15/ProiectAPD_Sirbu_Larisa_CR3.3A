#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>

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

void computeMatrix(Point points[], int num_points) {
    for (int i = 0; i < num_points; i++) {
        for (int j = i; j < num_points; j++) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            }
            else {
                double d = euclideanDistance(points[i], points[j]);
                distanceMatrix[i][j] = d;
                distanceMatrix[j][i] = d;
            }
        }
    }
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

int main() {
    auto start = high_resolution_clock::now(); 
    const char* inputFile = "C:\\Users\\Larisa\\Desktop\\Facultate\\APD2025\\APD2025\\GenerareFisier\\inputTest.txt";
    const char* outputFile = "C:\\Users\\Larisa\\Desktop\\Facultate\\APD2025\\APD2025\\ProiectImplementareSecventiala\\ProiectImplementareSecventiala\\distances.txt";

  
    

    int num_points = readPoints(inputFile, points);

    computeMatrix(points, num_points);
    saveMatrix(outputFile, num_points);

    auto end = high_resolution_clock::now(); 

    duration<double> elapsed = end - start; 


    cout << "Distances saved in: " << outputFile << endl;
    cout << "Execution time: " << elapsed.count() << " seconds" << endl;
    return 0;
}
