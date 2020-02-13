#include <uniform_grid.h>
#include <math.h>

UniformGrid::UniformGrid(Calculate2DFunction function, int numPointsX, int numPointsY, float minX, float minY, float maxX, float maxY)
    : Grid(function, new ColorShader),
        m_scalars(numPointsX * numPointsY),
        m_numPointsX(numPointsX),
        m_numPointsY(numPointsY),
        m_minX(minX),
        m_minY(minY),
        m_cellWidth((maxX - m_minX) / (numPointsX - 1)),
        m_cellHeight((maxY - m_minY) / (numPointsY - 1)) { }

// Given lexicographic index, find cell vertices (quads assumed)
int	UniformGrid::getCell(int i, int* v)
{
	int cell_row = i / (m_numPointsX - 1);

    v[0] = i + cell_row;
	v[1] = v[0] + 1;
	v[2] = v[1] + m_numPointsX;
	v[3] = v[0] + m_numPointsX;

	return 4;
}

// Given 2D position p = px[0], py[1] find cell which contains it
int UniformGrid::findCell(float* p)
{
	int C[2];

	// Compute cell coordinates C[0], C[1]
	C[0] = floor((p[0] - m_minX) * m_numPointsX / m_cellWidth);
	C[1] = floor((p[1] - m_minY) * m_numPointsY / m_cellHeight);

	// Test if p is inside the dataset
	if (C[0]<0 || C[0] >= m_numPointsX-1 || C[1] < 0 || C[1] >= m_numPointsY-1)
	{
		return -1;
	}

	// Go from cell coordinates to cell index
	return C[0] + C[1] * m_numPointsX;
}

void UniformGrid::initGrid(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(11, 0);
	m_shader->setBufferNormal(11, 5);
	m_shader->setBufferColor(11, 8);
}

void UniformGrid::updateGrid(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelView = camera.pose() * this->pose();
	glm::mat4 modelViewProj = camera.projection() * modelView;

	m_shader->setView(camera.pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);
	m_shader->setLightPosition(light.translation());

	m_shader->setMaterial(m_material);
	glDrawArrays(GL_TRIANGLES, 0, this->numVertices());
}