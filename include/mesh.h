#pragma once
#include <glm.hpp>
#include <string>
#include <movable.h>
#include <camera.h>
#include <grid.h>

#define COLOR_MAP_RESOLUTION 32
#define MAX_EDGES_PER_CELL 12
#define VERTICES_PER_EDGE 2
#define CORNERS_PER_VOXEL 8

typedef std::function<glm::vec4(float)> ColorFunction;

struct MeshVertexAttribute
{
    glm::vec3 position;
	float colorNorm;
};

class Mesh : public MovableObject
{
public:
	Mesh(Grid3D& grid, ColorFunction colorFunction = nullptr);
	~Mesh();

	void wireframe(bool enable);
	void update(const Camera& camera);

    void setIsoValue(float value) { m_isoValue = value; }
	float getIsoValue() { return m_isoValue; }

private:
	using UpdatableObject::update;

    void marchingCubes(float isoValue, std::vector<MeshVertexAttribute>& data);
	void initMovable(const GLuint& vao, const GLuint& vbo);
	void updateMovable(const float& totalTime, const float& frameTime);
    int updateVoxel(float isoValue, int corners[CORNERS_PER_VOXEL], MeshVertexAttribute* buffer);
    glm::vec3 vertexInterpolation(float isoValue, glm::vec3& p1, glm::vec3& p2, double p1Value, double p2Value);

    glm::vec4& defaultColor(float value) { return m_defaultColor; }
	inline float computeNorm(float value, float min, float max) { return ((value - min) / (max - min)); }

	const Camera* 	m_camera;
    ColorFunction 	m_colorFunction;
    GLuint 			m_colorTexture;
    glm::vec4 		m_defaultColor;
	Grid3D&			m_grid;
    float           m_isoValue;
    int 			m_numVertices;
    float           m_prevIsoValue;
	ShaderBase* 	m_shader;
	GLenum 			m_wireframe;
};