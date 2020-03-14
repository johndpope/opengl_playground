#pragma once
#include <glm.hpp>
#include <string>
#include <movable.h>
#include <camera.h>
#include <grid.h>

#define VERTICES_PER_CELL 6
#define COLOR_MAP_RESOLUTION 32

typedef std::function<glm::vec4(float)> ColorFunction;

struct SurfaceVertexAttribute
{
    glm::vec3 position;
	float colorNorm;
};

class Surface : public MovableObject
{
public:
	Surface(Grid2D& grid, ColorFunction colorFunction = nullptr);
	~Surface();

	void wireframe(bool enable);
	void update(const Camera& camera);

private:
	using UpdatableObject::update;

	void initMovable(const GLuint& vao, const GLuint& vbo);
	void updateMovable(const float& totalTime, const float& frameTime);
	void triangulate(std::vector<SurfaceVertexAttribute>& data);

    glm::vec4& defaultColor(float value) { return m_defaultColor; }
	inline float computeNorm(float value, float min, float max) { return ((value - min) / (max - min)); }

	const Camera* 	m_camera;
    ColorFunction 	m_colorFunction;
    GLuint 			m_colorTexture;
    glm::vec4 		m_defaultColor;
	Grid2D&			m_grid;
    int 			m_numVertices;
	ShaderBase* 	m_shader;
	GLenum 			m_wireframe;
};