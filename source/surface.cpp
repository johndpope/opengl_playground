#pragma once
#include <surface.h>

Surface::Surface(Grid2D& grid, ColorFunction colorFunction)
    : m_grid(grid),
      m_shader(new BasicColorMapShader()),
      m_colorFunction(colorFunction),
      m_defaultColor(glm::vec4(1.0f, 0, 0, 1.0f))
{
    if (m_colorFunction == nullptr)
    {
        m_colorFunction = ColorFunction(std::bind(&Surface::defaultColor, this, std::placeholders::_1));
    }
}

Surface::~Surface()
{
    delete m_shader;
}

void Surface::update(const Camera& camera)
{
    m_camera = &camera;
    UpdatableObject::update();
}

void Surface::wireframe(bool enable)
{
    m_wireframe = enable ? GL_LINE : GL_FILL;
}

void Surface::initMovable(const GLuint& vao, const GLuint& vbo)
{
    m_wireframe = GL_FILL;

    m_shader->use();

    // Build surface vertices
    std::vector<SurfaceVertexAttribute> vertices;
    this->triangulate(vertices);
    m_numVertices = vertices.size();

    // Build color map
    std::vector<glm::vec4>colorMap(COLOR_MAP_RESOLUTION);
    float min = m_grid.pointScalars()->getMin();
    float max = m_grid.pointScalars()->getMax();
    float inc = (max - min) / (COLOR_MAP_RESOLUTION - 1);
    for (int i = 0; i < COLOR_MAP_RESOLUTION; i++)
    {
		float normValue = computeNorm(min + i * inc, min, max);
        colorMap[i] = m_colorFunction(normValue);
    }

    // Build texture
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_1D, m_colorTexture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, COLOR_MAP_RESOLUTION, 0, GL_RGBA, GL_FLOAT, colorMap.data());

    // Setup buffer data
    glBufferData(GL_ARRAY_BUFFER, sizeof(SurfaceVertexAttribute) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    m_shader->setBufferPosition(sizeof(SurfaceVertexAttribute), offsetof(SurfaceVertexAttribute, position));
	m_shader->setBufferColorNorm(sizeof(SurfaceVertexAttribute), offsetof(SurfaceVertexAttribute, colorNorm));
}

void Surface::updateMovable(const float& totalTime, const float& frameTime)
{
    m_shader->use();
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe);

	glm::mat4 modelView = m_camera->pose() * this->pose();
	glm::mat4 modelViewProj = m_camera->projection() * modelView;

	m_shader->setView(m_camera->pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);

    glBindTexture(GL_TEXTURE_1D, m_colorTexture);
	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

void Surface::triangulate(std::vector<SurfaceVertexAttribute>& data)
{
    const int numCells = m_grid.numCells();
    const int size = VERTICES_PER_CELL * numCells;
    int vertexOffset = 0;
    data.resize(size);

    float min = m_grid.pointScalars()->getMin();
    float max = m_grid.pointScalars()->getMax();

    // Iterate over cells
    int sequence[VERTICES_PER_CELL] = { 3, 2, 1, 3, 1, 0 };
    for (int i = 0; i < numCells; i++)
    {
        int corners[4];
        m_grid.getCell(i, corners);

        glm::vec3 vertices[4];
        for (int j = 0; j < 4; j++)
        {
            float point[2];
            m_grid.getPoint(corners[j], point);

            vertices[j] = glm::vec3(point[0],
                                    point[1],
                                    m_grid.pointScalars()->getC0Scalar(corners[j]));
        }

        glm::vec3 a = vertices[1] - vertices[0];
        glm::vec3 b = vertices[3] - vertices[0];
        glm::vec3 n = glm::normalize(glm::cross(b, a));

        // Iterate over vertices
        for (int k = 0; k < VERTICES_PER_CELL; k++)
        {
            int m = sequence[k];

            SurfaceVertexAttribute* attrib = &data[vertexOffset++];
            attrib->position = vertices[m];
            attrib->colorNorm = computeNorm(attrib->position.z, min, max);
        }
    }
}