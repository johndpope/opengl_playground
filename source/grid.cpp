#pragma once
#include <grid.h>

Grid2D::Grid2D(
    Calculate2DFunction function,
    int numPointsX,
	int numPointsY,
	float minX,
	float minY,
	float maxX,
	float maxY)
    : m_calcFunction(function),
	  m_numPointsX(numPointsX),
	  m_numPointsY(numPointsY),
	  m_minX(minX),
	  m_minY(minY),
	  m_cellWidth((maxX - minX) / (numPointsX - 1)),
	  m_cellHeight((maxY - minY) / (numPointsY - 1))
	{
		m_scalars = this->initScalars();
	}

ScalarAttributes* Grid2D::initScalars()
{
    ScalarAttributes* scalars = new ScalarAttributes(m_numPointsX * m_numPointsY);

    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[2];
        this->getPoint(i, point);

        try
        {
            float value = m_calcFunction(point[0], point[1]);
            scalars->setC0Scalar(i, value);
        }
        catch (...)
        {
            scalars->setC0Scalar(i, INFINITY);
        }
    }

    return scalars;
}

// Given lexicographic index, find cell vertices (quads assumed)
int	Grid2D::getCell(int i, int* v)
{
	int cell_row = i / (m_numPointsX - 1);

	v[0] = i + cell_row;
	v[1] = v[0] + 1;
	v[2] = v[1] + m_numPointsX;
	v[3] = v[0] + m_numPointsX;

	return 4;
}

// Given 2D position p = px[0], py[1] find cell which contains it
int Grid2D::findCell(float* p)
{
	int C[2];

	// Compute cell coordinates C[0], C[1]
	C[0] = floor((p[0] - m_minX) * m_numPointsX / m_cellWidth);
	C[1] = floor((p[1] - m_minY) * m_numPointsY / m_cellHeight);

	// Test if p is inside the dataset
	if (C[0] < 0 || C[0] >= m_numPointsX - 1 || C[1] < 0 || C[1] >= m_numPointsY - 1)
	{
		return -1;
	}

	// Go from cell coordinates to cell index
	return C[0] + C[1] * m_numPointsX;
}

void Grid2D::getPoint(int i, float* p)
{
    p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    p[1] = m_minY + (i / m_numPointsX) * m_cellHeight;
}