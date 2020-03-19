#pragma once
#include <functional>
#include <glm.hpp>
#include <scalar_attributes.h>
#include <movable.h>

typedef std::function<float(float, float)> Calculate2DFunction;
typedef std::function<float(float, float, float)> Calculate3DFunction;

struct Plane
{
	float a;
	float b;
	float c;
	float d;
};

class Grid : public MovableObject
{
public:
	virtual ~Grid() { if (m_scalars != nullptr) { delete m_scalars; } }

	virtual int	numPoints()	= 0;
	virtual int numCells() = 0;
	virtual void getPoint(int, float*) = 0;
	virtual int getCell(int, int*) = 0;
	virtual int findCell(float* p) = 0;
    virtual float evaluate(float x, float y, float z = 0) = 0;

	ScalarAttributes* pointScalars() { return m_scalars; }

protected:
    Grid() = default;

	void initMovable(const GLuint& vao, const GLuint& vbo) { };
	void updateMovable(const float& totalTime, const float& frameTime) { };

    ScalarAttributes* m_scalars;
};

class Grid2D : public Grid
{
public:

	int findCell(float* p) override;
	int	getCell(int i, int* c) override;
    void getPoint(int i, float* p) override;

	int	numPoints() override { return m_numPointsX * m_numPointsY; }
	int	numCells() override { return (m_numPointsX - 1) * (m_numPointsY - 1); }

protected:
	Grid2D();

	float 	m_cellHeight; 	// Cell height in this grid
	float 	m_cellWidth; 	// Cell width in this grid
	float 	m_minX; 		// Minimal x coordinate values in this grid
	float 	m_minY; 		// Minimal y coordinate values in this grid
	int		m_numPointsX; 	// Number of points along the x-axis
	int 	m_numPointsY; 	// Number of points along the y−axis
};

class Grid3D : public Grid
{
public:
	int findCell(float* p) override;
	int	getCell(int i, int* c) override;
    void getPoint(int i, float* p) override;

	int	numPoints() override { return m_numPointsX * m_numPointsY * m_numPointsZ; }
	int	numCells() override { return (m_numPointsX - 1) * (m_numPointsY - 1) * (m_numPointsZ - 1); }

protected:
	Grid3D();

	float 	m_cellHeight; 	// Cell height in this grid
	float 	m_cellWidth; 	// Cell width in this grid
	float 	m_cellDepth; 	// Cell depth in this grid
	float 	m_minX; 		// Minimal x coordinate values in this grid
	float 	m_minY; 		// Minimal y coordinate values in this grid
	float 	m_minZ; 		// Minimal z coordinate values in this grid
	int		m_numPointsX; 	// Number of points along the x-axis
	int 	m_numPointsY; 	// Number of points along the y−axis
	int 	m_numPointsZ; 	// Number of points along the z−axis
};

class CalculateGrid2D : public Grid2D
{
public:
	CalculateGrid2D(
		Calculate2DFunction calcFunction,
		int numPointsX,
		int numPointsY,
		float minX,
		float minY,
		float maxX,
		float maxY);

	float evaluate(float x, float y, float z = 0) override { return m_calcFunction(x, y); }

private:
	ScalarAttributes* initScalars();

	Calculate2DFunction m_calcFunction;
};

class CalculateGrid3D : public Grid3D
{
public:
	CalculateGrid3D(
		Calculate3DFunction calcFunction,
        int numPointsX,
        int numPointsY,
		int numPointsZ,
        float minX,
        float minY,
		float minZ,
        float maxX,
        float maxY,
		float maxZ);

    float evaluate(float x, float y, float z) override { return m_calcFunction(x, y, z); }

private:
    ScalarAttributes* initScalars();

	Calculate3DFunction m_calcFunction;
};

class SliceGrid2D : public Grid2D
{
public:
	SliceGrid2D(
		Grid3D& grid,
		Plane& plane,
		int numPointsX,
		int numPointsY,
		float minX,
		float minY,
		float maxX,
		float maxY);

	float evaluate(float x, float y, float z = 0) override;
	void setPlane(Plane& plane);

private:
	void updateMovable(const float& totalTime, const float& frameTime);

	ScalarAttributes* initScalars();

	Grid3D& m_grid;
};

class PvmGrid3D : public Grid3D
{
public:
	PvmGrid3D(std::string pvmFilePath);

    float evaluate(float x, float y, float z) override;

private:
    ScalarAttributes* initScalars(unsigned char* volume, unsigned int bytesPerValue);
};