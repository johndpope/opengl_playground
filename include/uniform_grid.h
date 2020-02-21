#pragma once

#include <grid.h>
#include <scalar_attributes.h>
#include <shader.h>

class UniformGrid : public Grid
{
public:
    UniformGrid(Calculate2DFunction function, int numPointsX, int numPointsY, float minX, float minY, float maxX, float maxY);
	~UniformGrid() = default;

	int	numPoints() { return m_numPointsX * m_numPointsY; }
	int	numCells() { return (m_numPointsX - 1) * (m_numPointsY - 1); }
	int	getDimension1() { return m_numPointsX; }
	int getDimension2() { return m_numPointsY; }
	ScalarAttributes& pointScalars() { return m_scalars; }

	void getPoint(int i, float* p)
	{
		p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
		p[1] = m_minY + (i / m_numPointsX) * m_cellHeight;
	}

	int	findCell(float*);
	int	getCell(int i,int* c);

protected:
	ScalarAttributes m_scalars;
	int	m_numPointsX; // Number of points along the x-axis
	int m_numPointsY; // Number of points along the y−axis
	float m_minX; // Minimal x coordinate values in this grid
	float m_minY; // Minimal y coordinate values in this grid

private:
    void initGrid(const GLuint& vao, const GLuint& vbo);
    void updateGrid(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	float m_cellWidth; // Cell width in this grid
	float m_cellHeight; // Cell height in this grid
};