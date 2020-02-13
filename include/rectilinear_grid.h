#pragma once

#include "uniform_grid.h"
#include <vector>

// INPUTS
// dimsX : a float vector of world-space x-coordinates of sample points
// dimsY : a float vector of world-space y-coordinates of sample points

class RectilinearGrid : public UniformGrid
{
public:
    RectilinearGrid(Calculate2DFunction function, std::vector<float>& dimsX, std::vector<float>& dimsY);
	void getPoint(int i,float* p);

private:
	std::vector<float> m_dX; // Sampling positions along the X axis
	std::vector<float> m_dY; // Sampling positions along the Y axis
};