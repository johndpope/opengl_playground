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

typedef std::map<int, std::vector<int>> SequenceMap;
typedef std::map<int, int> LengthMap;

struct ContourVertexAttribute
{
    glm::vec3 position;
    glm::vec4 color;
};

class Contour : public MovableObject
{
public:
	Contour(Grid& grid, ShaderBase* shader);
	virtual ~Contour() = default;

	void update(const Camera& camera);

    void setIsoValue(float value) { m_isoValue = value; }
	float getIsoValue() { return m_isoValue; }

protected:
	virtual glm::vec4& getColor(float isoValue, int corners[CORNERS_PER_CELL]) = 0;

private:
	using UpdatableObject::update;

    void marchingSquares(float isoValue, std::vector<ContourVertexAttribute>& data);
	void initMovable(const GLuint& vao, const GLuint& vbo) { }
	void updateMovable(const float& totalTime, const float& frameTime);
    int updateCell(float isoValue, int corners[CORNERS_PER_CELL], ContourVertexAttribute* buffer, glm::vec4& color);

	const Camera* 	m_camera;
    Grid&           m_grid;
    float           m_isoValue;
    LengthMap       m_lengthMap;
    int             m_numVertices;
    float           m_prevIsoValue;
    SequenceMap     m_sequenceMap;
	ShaderBase*     m_shader;
};

class ColorContour : public Contour
{
public:
	ColorContour(Grid& grid, const glm::vec4& color = glm::vec4(0, 0, 1.0f, 1.0f));

private:
	glm::vec4& getColor(float isoValue, int corners[CORNERS_PER_CELL]);

	glm::vec4 m_color;
};