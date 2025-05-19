using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

public class Point
{
    public double X, Y, Z;
}

class Program
{
    const int MAX_POINTS = 8000;
    static Point[] points = new Point[MAX_POINTS];
    static double[,] distanceMatrix;

    static int ReadPoints(string filename)
    {
        var lines = File.ReadAllLines(filename);
        int count = 0;
        foreach (var line in lines)
        {
            var tokens = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
            if (tokens.Length >= 3 &&
                double.TryParse(tokens[0], NumberStyles.Any, CultureInfo.InvariantCulture, out double x) &&
                double.TryParse(tokens[1], NumberStyles.Any, CultureInfo.InvariantCulture, out double y) &&
                double.TryParse(tokens[2], NumberStyles.Any, CultureInfo.InvariantCulture, out double z))
            {
                points[count++] = new Point { X = x, Y = y, Z = z };
            }
        }
        return count;
    }

    static double EuclideanDistance(Point p1, Point p2)
    {
        double dx = p1.X - p2.X;
        double dy = p1.Y - p2.Y;
        double dz = p1.Z - p2.Z;
        return Math.Sqrt(dx * dx + dy * dy + dz * dz);
    }

    static void ComputeMatrixParallel(int numPoints)
    {
        distanceMatrix = new double[numPoints, numPoints];

        Parallel.For(0, numPoints, i =>
        {
            for (int j = i; j < numPoints; j++)
            {
                if (i == j)
                {
                    distanceMatrix[i, j] = 0;
                }
                else
                {
                    double d = EuclideanDistance(points[i], points[j]);
                    distanceMatrix[i, j] = d;
                    distanceMatrix[j, i] = d; 
                }
            }
        });
    }

    static void SaveMatrix(string filename, int numPoints)
    {
        using var writer = new StreamWriter(filename);
        for (int i = 0; i < numPoints; i++)
        {
            for (int j = 0; j < numPoints; j++)
            {
                writer.Write(distanceMatrix[i, j].ToString("F4", CultureInfo.InvariantCulture));
                writer.Write(" ");
            }
            writer.WriteLine();
        }
    }

    static void Main()
    {
        var sw = Stopwatch.StartNew();

        string inputFile = @"C:\Users\Larisa\Desktop\Facultate\APD2025\APD2025\GenerareFisier\input500.txt";
        string outputFile = @"C:\Users\Larisa\Desktop\Facultate\APD2025\APD2025\ProiectImplementareC#\ProiectImplementareC#\distances.txt";

        int numPoints = ReadPoints(inputFile);

        ComputeMatrixParallel(numPoints);

        SaveMatrix(outputFile, numPoints);

        sw.Stop();
        Console.WriteLine($"Distances saved in: {outputFile}");
        Console.WriteLine($"Execution time: {sw.Elapsed.TotalSeconds:F3} seconds");
    }
}
