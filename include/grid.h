#pragma once
#include <functional>
#include <vector>
#include <glm.hpp>
#include <material.h>
#include <surface.h>
#include <shader.h>
#include <scalar_attributes.h>

#define VERTICES_PER_CELL 6

typedef std::function<float(float, float)> Calculate2DFunction;
typedef std::function<glm::vec4(float, float, float)> ColorFunction;

struct GridVertexAttribute
{
    glm::vec3 position;
    glm::vec2 texture;
    glm::vec3 normal;
    glm::vec4 color;
};

class Grid : public Surface
{
public:
	virtual ~Grid() = default;

	virtual int	numPoints()	= 0;
	virtual int numCells() = 0;
	virtual void getPoint(int, float*) = 0;
	virtual int getCell(int, int*) = 0;
	virtual int findCell(float*) = 0;
	virtual ScalarAttributes& pointScalars() = 0;
    float function(float x, float y) { return m_calcFunction(x, y); }

    void triangulate(std::vector<GridVertexAttribute>& data);

protected:
	Grid(Calculate2DFunction calcFunction, ShaderBase* shader, ColorFunction colorFunction = nullptr);

	virtual void initGrid(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateGrid(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;

    Calculate2DFunction m_calcFunction;
    ColorFunction m_colorFunction;
    Material m_material;
    glm::vec4 m_defaultColor;
    int m_numVertices;

private:
    glm::vec4& defaultColor(float value, float min, float max) { return m_defaultColor; }

    void initSurface(const GLuint& vao, const GLuint& vbo);
	void updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);
};

class UniformGrid : public Grid
{
public:
	UniformGrid(
        Calculate2DFunction function,
        int numPointsX,
        int numPointsY,
        float minX,
        float minY,
        float maxX,
        float maxY,
        ColorFunction colorFunction = nullptr
    );

	~UniformGrid() = default;

	int	numPoints() { return m_numPointsX * m_numPointsY; }
	int	numCells() { return (m_numPointsX - 1) * (m_numPointsY - 1); }
	int	getDimension1() { return m_numPointsX; }
	int getDimension2() { return m_numPointsY; }
	ScalarAttributes& pointScalars() { return m_scalars; }

	int	findCell(float*);
	int	getCell(int i, int* c);
    void getPoint(int i, float* p);

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

class RectilinearGrid : public UniformGrid
{
public:
	RectilinearGrid(
        Calculate2DFunction function,
        std::vector<float>& dimsX,
        std::vector<float>& dimsY,
        ColorFunction colorFunction = nullptr
    );

	void getPoint(int i, float* p);

private:
	std::vector<float> m_dX; // Sampling positions along the X axis
	std::vector<float> m_dY; // Sampling positions along the Y axis
};