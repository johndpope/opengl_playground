#pragma once
#include <contour.h>

Contour::Contour(Grid& grid, ShaderBase* contourShader)
    : Surface(contourShader),
        m_grid(grid),
        m_isoValue(0.5f * (grid.pointScalars().getMax() - grid.pointScalars().getMin())),
        m_prevIsoValue(-1.0f)
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
}

void Contour::marchingSquares(float isoValue, std::vector<float>& data)
{
    data.clear();
    data.resize(m_grid.numCells() * 15 * 6);

    int numCells = m_grid.numCells();
    int offset = 0;
    for (int i = 0; i < numCells; i++)
    {
        int corners[4];
        m_grid.getCell(i, corners);
        glm::vec3 color = this->getColor(isoValue, corners);
        offset += this->updateCell(isoValue, corners, &data[offset], color);
    }

    data.resize(offset);
}

void Contour::updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    if (m_isoValue != m_prevIsoValue)
    {
        marchingSquares(m_isoValue, m_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);
        m_shader->setBufferPosition(6, 0);
        m_shader->setBufferColor(6, 3);

        m_prevIsoValue = m_isoValue;
    }

    glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
    m_shader->setModelViewProjection(modelViewProj);
    glDrawArrays(GL_LINES, 0, m_vertices.size() / 6);
}

int Contour::updateCell(float isoValue, int corners[4], float* buffer, glm::vec3& color)
{
    float zOffset = 0.005f * (m_grid.pointScalars().getMax() - m_grid.pointScalars().getMin());

    int code = 0;
    float weight[4];
    for (int j = 0; j < 4; j++)
    {
        float value1 = m_grid.pointScalars().getC0Scalar(corners[j]);
        float value2 = m_grid.pointScalars().getC0Scalar(corners[(j + 1) % 4]);

        code |= (int)(value1 > isoValue) << (3 - j);

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

    float v0[3];
    float v1[3];
    float v2[3];
    float v3[3];
    float v4[3];
    float v5[3];
    float v6[3];
    float v7[3];
    float v8[3];

    m_grid.getPoint(corners[0], v0);
    m_grid.getPoint(corners[1], v2);
    m_grid.getPoint(corners[2], v8);
    m_grid.getPoint(corners[3], v6);

    v0[2] = m_grid.function(v0[0], v0[1]);
    v2[2] = m_grid.function(v2[0], v2[1]);
    v6[2] = m_grid.function(v6[0], v6[1]);
    v8[2] = m_grid.function(v8[0], v8[1]);

    v1[0] = v0[0] + (v2[0] - v0[0]) * weight[0];
    v1[1] = v0[1] + (v2[1] - v0[1]) * weight[0];
    v1[2] = m_grid.function(v1[0], v1[1]);

    v5[0] = v2[0] + (v8[0] - v2[0]) * weight[1];
    v5[1] = v2[1] + (v8[1] - v2[1]) * weight[1];
    v5[2] = m_grid.function(v5[0], v5[1]);

    v7[0] = v6[0] + (v8[0] - v6[0]) * (1.0f - weight[2]);
    v7[1] = v6[1] + (v8[1] - v6[1]) * (1.0f - weight[2]);
    v7[2] = m_grid.function(v7[0], v7[1]);

    v3[0] = v0[0] + (v6[0] - v0[0]) * (1.0f - weight[3]);
    v3[1] = v0[1] + (v6[1] - v0[1]) * (1.0f - weight[3]);
    v3[2] = m_grid.function(v3[0], v3[1]);

    v4[0] = (v1[0] + v5[0] + v7[0] + v3[0]) / 4.0f;
    v4[1] = (v1[1] + v5[1] + v7[1] + v3[1]) / 4.0f;
    v4[2] = m_grid.function(v4[0], v4[1]);

    std::vector<float*> v = { v0, v1, v2, v3, v4, v5, v6, v7, v8 };

    std::vector<int> sequence = m_sequenceMap[code];
    int length = m_lengthMap[code];
    int bufferOffset = 0;
    for (int i = 0; i < length; i++)
    {
        float* vertex = v[sequence[i]];

        buffer[bufferOffset++] = vertex[0];
        buffer[bufferOffset++] = vertex[1];
        buffer[bufferOffset++] = vertex[2] + zOffset;
        buffer[bufferOffset++] = color.r;
        buffer[bufferOffset++] = color.g;
        buffer[bufferOffset++] = color.b;
    }

    return bufferOffset;
}

ColorContour::ColorContour(Grid& grid, const glm::vec3& color)
    : Contour(grid, new BasicColorShader()),
        m_color(color) { }

glm::vec3& ColorContour::getColor(float isoValue, int corners[4])
{
    return m_color;
}