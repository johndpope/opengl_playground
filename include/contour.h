#pragma once
#include <glm.hpp>
#include <camera.h>
#include <light.h>
#include <grid.h>
#include <shader.h>
#include <surface.h>
#include <map>
#include <vector>

class Contour : public Surface
{
public:
	Contour(Grid& grid, ShaderBase* contourShader);
	virtual ~Contour() = default;

    void setIsoValue(float value) { m_isoValue = value; }
	float getIsoValue() { return m_isoValue; }

protected:
	virtual glm::vec4& getColor(float isoValue, int corners[4]) = 0;

private:
    void marchingSquares(float isoValue, std::vector<float>& data);
	void initSurface(const GLuint& vao, const GLuint& vbo) { }
	void updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);
    int updateCell(float isoValue, int corners[4], float* buffer, glm::vec4& color);

    float m_isoValue;
    float m_prevIsoValue;
    Grid& m_grid;
    std::vector<float> m_vertices;
    std::map<int, std::vector<int>> m_sequenceMap;
    std::map<int, int> m_lengthMap;
};

class ColorContour : public Contour
{
public:
	ColorContour(Grid& grid, const glm::vec4& color = glm::vec4(0, 0, 1.0f, 1.0f));

private:
	glm::vec4& getColor(float isoValue, int corners[4]);

	glm::vec4 m_color;
};