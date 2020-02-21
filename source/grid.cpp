#pragma once
#include <grid.h>

void Grid::triangulate(std::vector<float>& data, glm::vec3* color)
{
    glm::vec3 defaultColor = glm::vec3(1.0f);
    if (color == nullptr)
    {
        color = &defaultColor;
    }

    const int size = 11 * this->numVertices();
    data.resize(size);

    int sequence[6] = { 3, 2, 1, 3, 1, 0 };
    int numCells = this->numCells();
    for (int i = 0; i < numCells; i++)
    {
        int corners[4];
        this->getCell(i, corners);

        glm::vec3 vertices[4];
        for (int j = 0; j < 4; j++)
        {
            float point[2];
            this->getPoint(corners[j], point);

            vertices[j] = glm::vec3(point[0],
                                    point[1],
                                    this->pointScalars().getC0Scalar(corners[j]));
        }

        glm::vec3 a = vertices[1] - vertices[0];
        glm::vec3 b = vertices[3] - vertices[0];
        glm::vec3 n = glm::normalize(glm::cross(b, a));

        for (int k = 0; k < 6; k++)
        {
            int m = sequence[k];
            int o = (i * 6 + k) * 11;

            // Vertex coordinate
            data[o + 0] = vertices[m].x;
            data[o + 1] = vertices[m].y;
            data[o + 2] = vertices[m].z;

            // Vertex texture coordinate
            data[o + 3] = ((m == 0) || (m == 3)) ? 0.0f : 1.0f;
            data[o + 4] = ((m == 0) || (m == 1)) ? 0.0f : 1.0f;

            // Vertex normal
            data[o + 5] = n.x;
            data[o + 6] = n.y;
            data[o + 7] = n.z;

            // Vertex color
            data[o + 8] = color->x;
            data[o + 9] = color->y;
            data[o + 10] = color->z;
        }
    }
}

Grid::Grid(Calculate2DFunction function, ShaderBase* shader) :
    Surface(shader),
    m_function(function)
{
    m_material.shininess = rand() / (float)RAND_MAX;
    m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
    m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
}

void Grid::initSurface(const GLuint& vao, const GLuint& vbo)
{
    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[2];
        this->getPoint(i, point);

        try
        {
            float value = m_function(point[0], point[1]);
            this->pointScalars().setC0Scalar(i, value);
        }
        catch (...)
        {
            this->pointScalars().setC0Scalar(i, INFINITY);
        }
    }

    std::vector<float> vertices;
    this->triangulate(vertices, &glm::vec3(1.0f, 0, 0));

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    this->initGrid(vao, vbo);
}

void Grid::updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    this->updateGrid(totalTime, frameTime, camera, light);
}