#pragma once

#include <ctime>
#include <sphere.h>

void Sphere::generateCustomSphere(std::vector<SphereVertexAttribute>& vertices, float radius)
{
    this->generateSphere(vertices, radius);
}

void Sphere::generateSphere(std::vector<SphereVertexAttribute>& vertices, float radius, glm::vec4* color)
{
    // Convert radius to corners
    std::vector<glm::vec3> v = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.942809f, -0.333333f),
        glm::vec3(-0.816497f, -0.471405f, -0.333333f),
        glm::vec3(0.816497f, -0.471405f, -0.333333f)
    };

    // Collect default color
    glm::vec4 defaultColor = glm::vec4(1.0f);
    if (color == nullptr)
    {
        color = &defaultColor;
    }

    // Compute size of vertices
    int offset = 0;
    vertices.resize(3 * glm::pow(4.0f, (m_levels + 1)));

    // Generate vertices
    offset += divide_triangle(&vertices[offset], v[0], v[1], v[2], m_levels, m_levels, radius, color);
    offset += divide_triangle(&vertices[offset], v[3], v[2], v[1], m_levels, m_levels, radius, color);
    offset += divide_triangle(&vertices[offset], v[0], v[3], v[1], m_levels, m_levels, radius, color);
    offset += divide_triangle(&vertices[offset], v[0], v[2], v[3], m_levels, m_levels, radius, color);
}

int Sphere::triangle(
    SphereVertexAttribute* vertices,
    glm::vec3 a,
    glm::vec3 b,
    glm::vec3 c,
    int nDepth,
    float radius,
    glm::vec4* color
)
{
    // Compute normal
    glm::vec3 n = (nDepth % 2 == 0) ?
                    glm::normalize(glm::cross((a - b), (a - c))) :
                    glm::normalize(glm::cross((a - c), (a - b)));

    int vertexOffset = 0;
    std::vector<glm::vec3> points = { a, b, c };
    for (const glm::vec3& point : points)
    {
        SphereVertexAttribute* attrib = &vertices[vertexOffset++];
        attrib->position = radius * point;
        attrib->normal = n;
        attrib->color = *color;
    }

    return vertexOffset;
}

int Sphere::divide_triangle(
    SphereVertexAttribute* vertices,
    glm::vec3 a,
    glm::vec3 b,
    glm::vec3 c,
    int n,
    int depth,
    float radius,
    glm::vec4* color
)
{
    int vertexOffset = 0;

    if (n > 0)
    {
        glm::vec3 v1 = glm::normalize(a + b);
        glm::vec3 v2 = glm::normalize(a + c);
        glm::vec3 v3 = glm::normalize(b + c);
        vertexOffset += divide_triangle(&vertices[vertexOffset], a, v2, v1, n - 1, depth, radius, color);
        vertexOffset += divide_triangle(&vertices[vertexOffset], c, v3, v2, n - 1, depth, radius, color);
        vertexOffset += divide_triangle(&vertices[vertexOffset], b, v1, v3, n - 1, depth, radius, color);
        vertexOffset += divide_triangle(&vertices[vertexOffset], v1, v2, v3, n - 1, depth, radius, color);
    }
    else
    {
        vertexOffset += triangle(vertices, a, b, c, depth, radius, color);
    }

    return vertexOffset;
}

void Sphere::initShape(const GLuint& vao, const GLuint& vbo)
{
    std::vector<SphereVertexAttribute> vertices;
    this->generateCustomSphere(vertices, m_radius);
    m_numVertices = vertices.size();

    glBufferData(GL_ARRAY_BUFFER, sizeof(SphereVertexAttribute) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    this->initSphere(vao, vbo);
}

void Sphere::updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    this->updateSphere(totalTime, frameTime, camera, light);
}

ColorSphere::ColorSphere(const glm::vec4& color, float radius)
    : Sphere(new ColorShader(), radius),
        m_color(color)
{
    srand(time(NULL));

    m_material.shininess = rand() / (float)RAND_MAX;
    m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
    m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
}

void ColorSphere::generateCustomSphere(std::vector<SphereVertexAttribute>& vertices, float radius)
{
    this->generateSphere(vertices, radius, &m_color);
}

void ColorSphere::initSphere(const GLuint& vao, const GLuint& vbo)
{
    m_shader->setBufferPosition(sizeof(SphereVertexAttribute), offsetof(SphereVertexAttribute, position));
    m_shader->setBufferNormal(sizeof(SphereVertexAttribute), offsetof(SphereVertexAttribute, normal));
	m_shader->setBufferColor(sizeof(SphereVertexAttribute), offsetof(SphereVertexAttribute, color));
}

void ColorSphere::updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    glm::mat4 modelView = camera.pose() * this->pose();
    glm::mat4 modelViewProj = camera.projection() * modelView;

    m_shader->setView(camera.pose());
    m_shader->setModelView(modelView);
    m_shader->setModelViewProjection(modelViewProj);
    m_shader->setLightPosition(light.translation());

    m_shader->setMaterial(m_material);
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

void BasicColorSphere::generateCustomSphere(std::vector<SphereVertexAttribute>& vertices, float radius)
{
    this->generateSphere(vertices, radius, &m_color);
}

void BasicColorSphere::initSphere(const GLuint& vao, const GLuint& vbo)
{
    m_shader->setBufferPosition(sizeof(SphereVertexAttribute), offsetof(SphereVertexAttribute, position));
	m_shader->setBufferColor(sizeof(SphereVertexAttribute), offsetof(SphereVertexAttribute, color));
}

void BasicColorSphere::updateSphere(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
    m_shader->setModelViewProjection(modelViewProj);
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}