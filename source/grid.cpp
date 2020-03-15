#pragma once
#include <grid.h>

Grid2D::Grid2D()
    : m_numPointsX(0),
	  m_numPointsY(0),
	  m_minX(0),
	  m_minY(0),
	  m_cellWidth(0),
	  m_cellHeight(0) { }

// Given lexicographic index, find cell vertices (quads assumed)
int	Grid2D::getCell(int i, int* v)
{
	int cellRow = i / (m_numPointsX - 1);

	v[0] = i + cellRow;
	v[1] = v[0] + 1;
	v[2] = v[1] + m_numPointsX;
	v[3] = v[0] + m_numPointsX;

	return 4;
}

void Grid2D::getPoint(int i, float* p)
{
    p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    p[1] = m_minY + (i / m_numPointsX) * m_cellHeight;
}

CalculateGrid2D::CalculateGrid2D(
	Calculate2DFunction calcFunction,
	int numPointsX,
	int numPointsY,
	float minX,
	float minY,
	float maxX,
	float maxY)
    : m_calcFunction(calcFunction)
	{
		m_numPointsX = numPointsX;
		m_numPointsY = numPointsY;
		m_minX = minX;
		m_minY = minY;
		m_cellWidth  = (maxX - minX) / (numPointsX - 1);
		m_cellHeight = (maxY - minY) / (numPointsY - 1);
		m_scalars = this->initScalars();
	}

ScalarAttributes* CalculateGrid2D::initScalars()
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

Grid3D::Grid3D()
    : m_numPointsX(0),
	  m_numPointsY(0),
	  m_numPointsZ(0),
	  m_minX(0),
	  m_minY(0),
	  m_minZ(0),
	  m_cellWidth(0),
	  m_cellHeight(0),
	  m_cellDepth(0) { }

// Given lexicographic index, find cell vertices (voxels assumed)
int	Grid3D::getCell(int i, int* v)
{
	int incX = 1;
	int incY = m_numPointsX;
	int incZ = m_numPointsX * m_numPointsY;

	int cellRow = i / (m_numPointsX - 1);
	int cellDepth = i / ((m_numPointsX - 1) * (m_numPointsY - 1));

	v[0] = i + cellRow + cellDepth * m_numPointsX;
	v[1] = v[0] + incX;
	v[2] = v[1] + incY;
	v[3] = v[0] + incY;
	v[4] = v[0] + incZ;
	v[5] = v[1] + incZ;
	v[6] = v[2] + incZ;
	v[7] = v[3] + incZ;

	return 8;
}

void Grid3D::getPoint(int i, float* p)
{
	p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    i /= m_numPointsX;
    p[1] = m_minY + (i % m_numPointsY) * m_cellHeight;
    i /= m_numPointsY;
    p[2] = m_minZ + i * m_cellDepth;
}

CalculateGrid3D::CalculateGrid3D(
	Calculate3DFunction calcFunction,
	int numPointsX,
	int numPointsY,
	int numPointsZ,
	float minX,
	float minY,
	float minZ,
	float maxX,
	float maxY,
	float maxZ)
    : m_calcFunction(calcFunction)
	{
		m_numPointsX = numPointsX;
		m_numPointsY = numPointsY;
		m_numPointsZ = numPointsZ;
		m_minX = minX;
		m_minY = minY;
		m_minZ = minZ;
		m_cellWidth  = (maxX - minX) / (numPointsX - 1);
		m_cellHeight = (maxY - minY) / (numPointsY - 1);
		m_cellDepth  = (maxZ - minZ) / (numPointsZ - 1);
		m_scalars = this->initScalars();
	}

ScalarAttributes* CalculateGrid3D::initScalars()
{
    ScalarAttributes* scalars = new ScalarAttributes(m_numPointsX * m_numPointsY * m_numPointsZ);

    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[3];
        this->getPoint(i, point);

        try
        {
            float value = m_calcFunction(point[0], point[1], point[2]);
            scalars->setC0Scalar(i, value);
        }
        catch (...)
        {
            scalars->setC0Scalar(i, INFINITY);
        }
    }

    return scalars;
}