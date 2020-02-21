#pragma once
#include <functional>
#include <vector>
#include <glm.hpp>
#include <material.h>
#include <surface.h>
#include <shader.h>
#include <scalar_attributes.h>

class ScalarAttributes;

typedef std::function<float(float, float)> Calculate2DFunction;

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
	int numVertices() { return 6 * this->numCells(); }
    float function(float x, float y) { return m_function(x, y); }

    void triangulate(std::vector<float>& data, glm::vec3* color = nullptr);

protected:
	Grid(Calculate2DFunction function, ShaderBase* shader);

	virtual void initGrid(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateGrid(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;

    Calculate2DFunction m_function;
    Material m_material;

private:
    void initSurface(const GLuint& vao, const GLuint& vbo);
	void updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);
};