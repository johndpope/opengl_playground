#pragma once
#include <functional>
#include <glm.hpp>
#include <scalar_attributes.h>

typedef std::function<float(float, float)> Calculate2DFunction;

class Grid
{
public:
	virtual ~Grid() { if (m_scalars != nullptr) { delete m_scalars; } }

	virtual int	numPoints()	= 0;
	virtual int numCells() = 0;
	virtual void getPoint(int, float*) = 0;
	virtual int getCell(int, int*) = 0;
	virtual int findCell(float*) = 0;
    virtual float evaluate(float x, float y, float z = 0) = 0;

	virtual ScalarAttributes* pointScalars() { return m_scalars; }

protected:
    Grid() = default;

    virtual ScalarAttributes* initScalars() = 0;

    ScalarAttributes* m_scalars;
};

class Grid2D : public Grid
{
public:
	Grid2D(
        Calculate2DFunction calcFunction,
        int numPointsX,
        int numPointsY,
        float minX,
        float minY,
        float maxX,
        float maxY);

	int	findCell(float*);
	int	getCell(int i, int* c);
    void getPoint(int i, float* p);

	int	numPoints() { return m_numPointsX * m_numPointsY; }
	int	numCells() { return (m_numPointsX - 1) * (m_numPointsY - 1); }
    float evaluate(float x, float y, float z = 0) { return m_calcFunction(x, y); }

private:
    ScalarAttributes* initScalars();

    Calculate2DFunction m_calcFunction;
	float 				m_cellHeight; 	// Cell height in this grid
	float 				m_cellWidth; 	// Cell width in this grid
	float 				m_minX; 		// Minimal x coordinate values in this grid
	float 				m_minY; 		// Minimal y coordinate values in this grid
	int					m_numPointsX; 	// Number of points along the x-axis
	int 				m_numPointsY; 	// Number of points along the y−axis
};