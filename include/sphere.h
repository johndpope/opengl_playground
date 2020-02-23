#pragma once

#include <ctime>
#include <glm.hpp>
#include <shape.h>

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

    virtual void generateCustomSphere(std::vector<float>& vertices, float radius)
    {
        this->generateSphere(vertices, radius);
    }

	void generateSphere(std::vector<float>& vertices, float radius, glm::vec3* color = nullptr)
    {
        // Convert radius to corners
        std::vector<glm::vec3> v = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.942809f, -0.333333f),
            glm::vec3(-0.816497f, -0.471405f, -0.333333f),
            glm::vec3(0.816497f, -0.471405f, -0.333333f)
        };

        // Collect default color
        glm::vec3 defaultColor = glm::vec3(1.0f);
        if (color == nullptr)
        {
            color = &defaultColor;
        }

        // Compute size of vertices
        int offset = 0;
        vertices.resize(9 * 3 * glm::pow(4.0f, (m_levels + 1)));

        // Generate vertices
        offset += divide_triangle(&vertices[offset], v[0], v[1], v[2], m_levels, m_levels, radius, color);
        offset += divide_triangle(&vertices[offset], v[3], v[2], v[1], m_levels, m_levels, radius, color);
        offset += divide_triangle(&vertices[offset], v[0], v[3], v[1], m_levels, m_levels, radius, color);
        offset += divide_triangle(&vertices[offset], v[0], v[2], v[3], m_levels, m_levels, radius, color);
    }

protected:
    std::vector<float> m_vertices;

private:
    int triangle(
        float* vertices,
        glm::vec3 a,
        glm::vec3 b,
        glm::vec3 c,
        int nDepth,
        float radius,
        glm::vec3* color
    )
    {
        // Compute normal
        glm::vec3 n = (nDepth % 2 == 0) ?
                        glm::normalize(glm::cross((a - b), (a - c))) :
                        glm::normalize(glm::cross((a - c), (a - b)));

        int offset = 0;
        std::vector<glm::vec3> points = { a, b, c };
        for (const glm::vec3& point : points)
        {
            // Vertex coordinate
            vertices[offset++] = radius * point.x;
            vertices[offset++] = radius * point.y;
            vertices[offset++] = radius * point.z;

            // Vertex normal
            vertices[offset++] = n.x;
            vertices[offset++] = n.y;
            vertices[offset++] = n.z;

            // Vertex color
            vertices[offset++] = color->x;
            vertices[offset++] = color->y;
            vertices[offset++] = color->z;
        }

        return offset;
    }

    int divide_triangle(
        float* vertices,
        glm::vec3 a,
        glm::vec3 b,
        glm::vec3 c,
        int n,
        int depth,
        float radius,
        glm::vec3* color
    )
    {
        int offset = 0;

        if (n > 0)
        {
            glm::vec3 v1 = glm::normalize(a + b);
            glm::vec3 v2 = glm::normalize(a + c);
            glm::vec3 v3 = glm::normalize(b + c);
            offset += divide_triangle(&vertices[offset], a, v2, v1, n - 1, depth, radius, color);
            offset += divide_triangle(&vertices[offset], c, v3, v2, n - 1, depth, radius, color);
            offset += divide_triangle(&vertices[offset], b, v1, v3, n - 1, depth, radius, color);
            offset += divide_triangle(&vertices[offset], v1, v2, v3, n - 1, depth, radius, color);
        }
        else
        {
            offset += triangle(vertices, a, b, c, depth, radius, color);
        }

        return offset;
    }

	void initShape(const GLuint& vao, const GLuint& vbo)
    {
        this->generateCustomSphere(m_vertices, m_radius);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

        this->initSphere(vao, vbo);
    }

	void updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
    {
        this->updateSphere(totalTime, frameTime, camera, light);
    }

    int m_levels;
	float m_radius;
};

class ColorSphere : public Sphere
{
public:
	ColorSphere(const glm::vec3 color, float radius)
        : Sphere(new ColorShader(), radius),
          m_color(color)
    {
        srand(time(NULL));

        m_material.shininess = rand() / (float)RAND_MAX;
        m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
        m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
        m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
    }

	ColorSphere(const Material material, float radius)
    	: Sphere(new ColorShader(), radius),
	      m_material(material),
	      m_color(glm::vec3(1.0f)) { }

private:
	void generateCustomSphere(std::vector<float>& vertices, float radius)
    {
        this->generateSphere(vertices, radius, &m_color);
    }

    void initSphere(const GLuint& vao, const GLuint& vbo)
    {
        m_shader->setBufferPosition(9, 0);
        m_shader->setBufferNormal(9, 3);
        m_shader->setBufferColor(9, 6);
    }

    void updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
    {
        glm::mat4 modelView = camera.pose() * this->pose();
        glm::mat4 modelViewProj = camera.projection() * modelView;

        m_shader->setView(camera.pose());
        m_shader->setModelView(modelView);
        m_shader->setModelViewProjection(modelViewProj);
        m_shader->setLightPosition(light.translation());

        m_shader->setMaterial(m_material);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 9);
    }

	Material m_material;
	glm::vec3 m_color;
};

class BasicColorSphere : public Sphere
{
public:
    BasicColorSphere(const glm::vec3 color, float radius)
        : Sphere(new BasicColorShader(), radius),
        m_color(color) { }

    BasicColorSphere(float radius)
        : Sphere(new BasicColorShader(), radius),
        m_color(glm::vec3(1.0f)) { }

private:
	void generateCustomSphere(std::vector<float>& vertices, float radius)
    {
        this->generateSphere(vertices, radius, &m_color);
    }

    void initSphere(const GLuint& vao, const GLuint& vbo)
    {
		m_shader->setBufferPosition(9, 0);
		m_shader->setBufferColor(9, 6);
    }

    void updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
    {
        glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
        m_shader->setModelViewProjection(modelViewProj);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 9);
    }

    glm::vec3 m_color;
};