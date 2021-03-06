#pragma once
#include <contour.h>
#include <glm\gtx\matrix_decompose.hpp>

Contour::Contour(Grid2D& grid, ShaderBase* shader)
    : m_grid(grid),
      m_prevIsoValue(-1.0f),
      m_shader(shader)
{
    m_sequenceMap[0] = { };
    m_sequenceMap[1] = { 7, 3 };
    m_sequenceMap[2] = { 7, 5 };
    m_sequenceMap[3] = { 5, 3 };
    m_sequenceMap[4] = { 1, 5 };
    m_sequenceMap[5] = { 1, 5, 7, 3 };
    m_sequenceMap[6] = { 7, 1 };
    m_sequenceMap[7] = { 3, 1 };
    m_sequenceMap[8] = { 3, 1 };
    m_sequenceMap[9] = { 7, 1 };
    m_sequenceMap[10] = { 3, 1, 7, 5 };
    m_sequenceMap[11] = { 1, 5 };
    m_sequenceMap[12] = { 5, 3 };
    m_sequenceMap[13] = { 7, 5 };
    m_sequenceMap[14] = { 7, 3 };
    m_sequenceMap[15] = { };

    m_lengthMap[0] = 0;
    m_lengthMap[1] = 2;
    m_lengthMap[2] = 2;
    m_lengthMap[3] = 2;
    m_lengthMap[4] = 2;
    m_lengthMap[5] = 4;
    m_lengthMap[6] = 2;
    m_lengthMap[7] = 2;
    m_lengthMap[8] = 2;
    m_lengthMap[9] = 2;
    m_lengthMap[10] = 4;
    m_lengthMap[11] = 2;
    m_lengthMap[12] = 2;
    m_lengthMap[13] = 2;
    m_lengthMap[14] = 2;
    m_lengthMap[15] = 0;

    this->setIsoValue(0.5f * (grid.pointScalars()->getMax() - grid.pointScalars()->getMin()));
}

void Contour::update(const Camera& camera)
{
    m_camera = &camera;
    UpdatableObject::update();
}

void Contour::incrementHeight(float value)
{
	m_grid.translate(value * glm::vec4(0, 0, 1.0f, 0));
	m_heightIncremented = true;
}

void Contour::marchingSquares(float isoValue, std::vector<ContourVertexAttribute>& data)
{
    int numCells = m_grid.numCells();
	int corners[CORNERS_PER_CELL];

    data.resize(numCells * VERTICES_PER_EDGE * MAX_EDGES_PER_CELL);

    int offset = 0;
    for (int i = 0; i < numCells; i++)
    {
        m_grid.getCell(i, corners);
        glm::vec4 color = this->getColor(isoValue, corners);
        offset += this->updateCell(isoValue, corners, &data[offset], color);
    }

    data.resize(offset);
}

void Contour::updateMovable(const float& totalTime, const float& frameTime)
{
    m_shader->use();
    m_grid.update();

    if ((m_isoValue != m_prevIsoValue) || m_heightIncremented)
    {
        std::vector<ContourVertexAttribute> vertices;
        marchingSquares(m_isoValue, vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ContourVertexAttribute) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        m_numVertices = vertices.size();

        m_shader->setBufferPosition(sizeof(ContourVertexAttribute), offsetof(ContourVertexAttribute, position));
        m_shader->setBufferColor(sizeof(ContourVertexAttribute), offsetof(ContourVertexAttribute, color));

        m_prevIsoValue = m_isoValue;
		m_heightIncremented = false;
    }

    glm::mat4 modelViewProj = m_camera->projection() * m_camera->pose() * this->pose() * m_grid.pose();
    m_shader->setModelViewProjection(modelViewProj);
    glDrawArrays(GL_LINES, 0, m_numVertices);
}

int Contour::updateCell(float isoValue, int corners[CORNERS_PER_CELL], ContourVertexAttribute* buffer, glm::vec4& color)
{
    float zOffset = 0.005f * (m_grid.pointScalars()->getMax() - m_grid.pointScalars()->getMin());

    int code = 0;
    float weight[CORNERS_PER_CELL];
    for (int j = 0; j < CORNERS_PER_CELL; j++)
    {
        float value1 = m_grid.pointScalars()->getC0Scalar(corners[j]);
        float value2 = m_grid.pointScalars()->getC0Scalar(corners[(j + 1) % CORNERS_PER_CELL]);

        code |= (int)(value1 > isoValue) << (CORNERS_PER_CELL - 1 - j);

        weight[j] = (value1 - isoValue) / (value1 - value2);
    }

    if ((code == 0) || (code == 15))
    {
        return 0;
    }

    //             <--------- x --------->
    //                     edge 0
    //           v0          v1          v2            ^
    //              o ------- * ------- *              |
    //              |         |         |              |
    //              |         | v4      |              |
    //  edge 3   v3 * ------- * ------- * v5   edge 1  y
    //              |         |         |              |
    //              |         |         |              |
    //              * ------- * ------- *              |
    //           v6          v7           v8          \ /
    //                     edge 2

    float v0[2];
    float v1[2];
    float v2[2];
    float v3[2];
    float v4[2];
    float v5[2];
    float v6[2];
    float v7[2];
    float v8[2];

    m_grid.getPoint(corners[0], v0);
    m_grid.getPoint(corners[1], v2);
    m_grid.getPoint(corners[2], v8);
    m_grid.getPoint(corners[3], v6);

    v1[0] = v0[0] + (v2[0] - v0[0]) * weight[0];
    v1[1] = v0[1] + (v2[1] - v0[1]) * weight[0];

	v3[0] = v0[0] + (v6[0] - v0[0]) * (1.0f - weight[3]);
	v3[1] = v0[1] + (v6[1] - v0[1]) * (1.0f - weight[3]);

    v5[0] = v2[0] + (v8[0] - v2[0]) * weight[1];
    v5[1] = v2[1] + (v8[1] - v2[1]) * weight[1];

    v7[0] = v6[0] + (v8[0] - v6[0]) * (1.0f - weight[2]);
    v7[1] = v6[1] + (v8[1] - v6[1]) * (1.0f - weight[2]);

    v4[0] = (v1[0] + v5[0] + v7[0] + v3[0]) / 4.0f;
    v4[1] = (v1[1] + v5[1] + v7[1] + v3[1]) / 4.0f;

    std::vector<float*> v = { v0, v1, v2, v3, v4, v5, v6, v7, v8 };

    std::vector<int> sequence = m_sequenceMap[code];
    int length = m_lengthMap[code];
    int vertexOffset = 0;
    for (int i = 0; i < length; i++)
    {
        float* position = v[sequence[i]];

        buffer[i].position.x = position[0];
        buffer[i].position.y = position[1];
        buffer[i].position.z = zOffset;
        buffer[i].color = color;
    }

    return length;
}

ColorContour::ColorContour(Grid2D& grid, const glm::vec4& color)
    : Contour(grid, new BasicColorShader()),
      m_color(color) { }

glm::vec4& ColorContour::getColor(float isoValue, int corners[CORNERS_PER_CELL])
{
    return m_color;
}