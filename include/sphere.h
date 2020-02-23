#pragma once

#include <glm.hpp>
#include <shape.h>
#include <vector>

class Sphere : public Shape
{
public:
	Sphere(ShaderBase* shader, float radius, int levels = 5)
        : Shape(shader),
          m_levels(10),
		  m_radius(radius) { }

	Sphere(ShaderBase* shader)
        : Sphere(shader, 0.5f) { }

protected:
	virtual ~Sphere() = default;

	virtual void initSphere(const GLuint& vao, const GLuint& vbo) = 0;
	virtual void updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light) = 0;
    virtual void generateCustomSphere(std::vector<float>& vertices, float radius);

	void generateSphere(std::vector<float>& vertices, float radius, glm::vec4* color = nullptr);

protected:
    std::vector<float> m_vertices;

private:
    int triangle(float* vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c, int nDepth, float radius, glm::vec4* color);
    int divide_triangle(float* vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c, int n, int depth, float radius, glm::vec4* color);
	void initShape(const GLuint& vao, const GLuint& vbo);
	void updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

    int m_levels;
	float m_radius;
};

class ColorSphere : public Sphere
{
public:
	ColorSphere(const glm::vec4& color, float radius);
	ColorSphere(const Material& material, float radius)
    	: Sphere(new ColorShader(), radius),
	      m_material(material),
	      m_color(glm::vec4(1.0f)) { }

private:
	void generateCustomSphere(std::vector<float>& vertices, float radius);
    void initSphere(const GLuint& vao, const GLuint& vbo);
    void updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

	Material m_material;
	glm::vec4 m_color;
};

class BasicColorSphere : public Sphere
{
public:
    BasicColorSphere(const glm::vec4& color, float radius)
        : Sphere(new BasicColorShader(), radius),
          m_color(color) { }

    BasicColorSphere(float radius)
        : Sphere(new BasicColorShader(), radius),
          m_color(glm::vec4(1.0f)) { }

private:
	void generateCustomSphere(std::vector<float>& vertices, float radius);
    void initSphere(const GLuint& vao, const GLuint& vbo);
    void updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light);

    glm::vec4 m_color;
};