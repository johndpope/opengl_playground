#pragma once
#include <glm.hpp>
#include <camera.h>
#include <light.h>
#include <grid.h>
#include <shader.h>
#include <surface.h>
#include <map>
#include <vector>

#define MAX_EDGES_PER_CELL 2
#define VERTICES_PER_EDGE 2
#define CORNERS_PER_CELL 4

struct ContourVertexAttribute
{
    glm::vec3 position;
    glm::vec4 color;
};

class Contour : public Surface
{
public:
	Contour(Grid& grid, ShaderBase* contourShader);
	virtual ~Contour() = default;

    void setIsoValue(float value) { m_isoValue = value; }
	float getIsoValue() { return m_isoValue; }

protected:
	virtual glm::vec4& getColor(float isoValue, int corners[VERTICES_PER_CELL]) = 0;

private:
    void marchingSquares(float isoValue, std::vector<ContourVertexAttribute>& data);
	void initSurface(const GLuint& vao, const GLuint& vbo) { }
	void updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);
    int updateCell(float isoValue, int corners[VERTICES_PER_CELL], ContourVertexAttribute* buffer, glm::vec4& color);

    float m_isoValue;
    float m_prevIsoValue;
    Grid& m_grid;
    int m_numVertices;
    std::map<int, std::vector<int>> m_sequenceMap;
    std::map<int, int> m_lengthMap;
};

class ColorContour : public Contour
{
public:
	ColorContour(Grid& grid, const glm::vec4& color = glm::vec4(0, 0, 1.0f, 1.0f));

private:
	glm::vec4& getColor(float isoValue, int corners[VERTICES_PER_CELL]);

	glm::vec4 m_color;
};