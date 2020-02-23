#pragma once
#include <grid.h>

void Grid::triangulate(std::vector<float>& data, glm::vec4* color)
{
    glm::vec4 defaultColor = glm::vec4(1.0f);
    if (color == nullptr)
    {
        color = &defaultColor;
    }

    const int size = 12 * this->numVertices();
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
            int o = (i * 6 + k) * 12;

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
			data[o + 11] = color->a;
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
    this->triangulate(vertices, &glm::vec4(1.0f, 0, 0, 1.0f));

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    this->initGrid(vao, vbo);
}

void Grid::updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    this->updateGrid(totalTime, frameTime, camera, light);
}

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
	if (C[0] < 0 || C[0] >= m_numPointsX - 1 || C[1] < 0 || C[1] >= m_numPointsY - 1)
	{
		return -1;
	}

	// Go from cell coordinates to cell index
	return C[0] + C[1] * m_numPointsX;
}

void UniformGrid::initGrid(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(12, 0);
	m_shader->setBufferNormal(12, 5);
	m_shader->setBufferColor(12, 8);
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

RectilinearGrid::RectilinearGrid(Calculate2DFunction function, std::vector<float>& dimsX, std::vector<float>& dimsY)
	: UniformGrid(function,
		dimsX.size(),				//number of samples along X
		dimsY.size(),				//number of samples along y
		dimsX[0],					//minimal X value
		dimsY[0],					//minimal Y value
		dimsX[dimsX.size() - 1],	//maximal X value
		dimsY[dimsY.size() - 1]),	//maximal Y value
	m_dX(dimsX),
	m_dY(dimsX) { }

void RectilinearGrid::getPoint(int i, float* p)
{
	// Compute vertex grid-coordinates row, col
	int row = i / m_numPointsX;
	int col = i % m_numPointsX;

	// Find actual vertex coordinates using the sampling-steps
	// stored in dX[],dY[] for the X and Y axes
	p[0] = m_dX[col];
	p[1] = m_dY[row];
}