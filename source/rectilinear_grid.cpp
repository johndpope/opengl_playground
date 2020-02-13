#include "rectilinear_grid.h"
#include <math.h>

RectilinearGrid::RectilinearGrid(Calculate2DFunction function, std::vector<float>& dimsX, std::vector<float>& dimsY)
	: UniformGrid(function,
                  dimsX.size(),				//number of samples along X
				  dimsY.size(),				//number of samples along y
				  dimsX[0],					//minimal X value
				  dimsY[0],					//minimal Y value
				  dimsX[dimsX.size()-1],	//maximal X value
				  dimsY[dimsY.size()-1]),	//maximal Y value
		  m_dX(dimsX),
		  m_dY(dimsX) { }

void RectilinearGrid::getPoint(int i, float* p)
{
	// Compute vertex grid-coordinates row, col
	int row = i / m_numPointsX;
	int col = i % m_numPointsX;

	// Find actual vertex coordinates using the sampling-steps
	// stored in dX[],dY[] for the X and Y axes
	p[0] = m_dX[col];
	p[1] = m_dY[row];
}