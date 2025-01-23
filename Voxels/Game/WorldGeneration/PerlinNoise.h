#pragma once
#include <iostream>
#include <cmath>
#include <vector>

class PerlinNoise
{
private:
    static std::vector<int> p;
    // Fade function as defined in Ken Perlin's paper
    static inline double fade(double t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // Linear interpolation
    static inline double lerp(double a, double b, double t)
    {
        return a + t * (b - a);
    }

    // Gradient function to return random gradients
    static inline double grad(int hash, double x, double y)
    {
        int h = hash & 15;  // Using the lower 4 bits of the hash value
        double u = (h < 8) ? x : y;  // Selects based on the hash
        double v = (h < 4) ? y : x;  // Further selection based on the hash
        return ((h & 1 ? -1 : 1) * u) + ((h & 2 ? -1 : 1) * v);
    }
public:
    static void Init()
    {
        // Initialize permutation table (just an example)
        //std::vector<int> p(512);
        for (int i = 0; i < 256; i++) {
            p[i] = i;
        }

        // Permute the table using a simple shuffle
        for (int i = 0; i < 256; i++) {
            int j = rand() % 256;
            std::swap(p[i], p[j]);
            p[i + 256] = p[i];  // Duplicate the permutation table
        }
    }
    // Generate 2D Perlin Noise
    static double Noise(double x, double y)
    {
        x /= 255.0;
        y /= 255.0;
        // Determine grid cell coordinates
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        // Relative position within the grid
        x -= std::floor(x);
        y -= std::floor(y);

        // Fade curves for each axis
        double u = fade(x);
        double v = fade(y);

        // Hash coordinates of the grid corners
        int A = p[X] + Y, AA = p[A], AB = p[A + 1];
        int B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];

        // Interpolate between the gradients
        double res = lerp(
            lerp(grad(p[AA], x, y), grad(p[BA], x - 1, y), u),
            lerp(grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1), u),
            v);

        return (res + 1.0) / 2.0;  // Normalize result to [0,1]
    }
};