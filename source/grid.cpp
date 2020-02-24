#pragma once
#include <grid.h>

Grid::Grid(Calculate2DFunction function, ShaderBase* shader, ColorFunction colorFunction)
    : Surface(shader),
      m_calcFunction(function),
      m_colorFunction(colorFunction),
      m_defaultColor(glm::vec4(1.0f, 0, 0, 1.0f))
{
    if (m_colorFunction == nullptr)
    {
        m_colorFunction = ColorFunction(std::bind(&Grid::defaultColor, this, std::placeholders::_1));
    }

    m_material.shininess = rand() / (float)RAND_MAX;
    m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
    m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
}

void Grid::triangulate(std::vector<GridVertexAttribute>& data)
{
    const int numCells = this->numCells();
    const int size = VERTICES_PER_CELL * numCells;
    int vertexOffset = 0;
    data.resize(size);

    float min = this->pointScalars().getMin();
    float max = this->pointScalars().getMax();

    // Iterate over cells
    int sequence[VERTICES_PER_CELL] = { 3, 2, 1, 3, 1, 0 };
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

        // Iterate over vertices
        for (int k = 0; k < VERTICES_PER_CELL; k++)
        {
            int m = sequence[k];

            GridVertexAttribute* attrib = &data[vertexOffset++];
            attrib->position = vertices[m];
            attrib->colorNorm = computeNorm(attrib->position.z, min, max);
        }
    }
}

void Grid::initSurface(const GLuint& vao, const GLuint& vbo)
{
    // Populate scalar attribute
    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[2];
        this->getPoint(i, point);

        try
        {
            float value = m_calcFunction(point[0], point[1]);
            this->pointScalars().setC0Scalar(i, value);
        }
        catch (...)
        {
            this->pointScalars().setC0Scalar(i, INFINITY);
        }
    }

    // Build surface vertices
    std::vector<GridVertexAttribute> vertices;
    this->triangulate(vertices);
    m_numVertices = vertices.size();

    // Build color map
    std::vector<glm::vec4>colorMap(COLOR_MAP_RESOLUTION);
    float min = this->pointScalars().getMin();
    float max = this->pointScalars().getMax();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(GridVertexAttribute) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    m_shader->setBufferPosition(sizeof(GridVertexAttribute), offsetof(GridVertexAttribute, position));
	m_shader->setBufferColorNorm(sizeof(GridVertexAttribute), offsetof(GridVertexAttribute, colorNorm));
}

void Grid::updateSurface(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelView = camera.pose() * this->pose();
	glm::mat4 modelViewProj = camera.projection() * modelView;

	m_shader->setView(camera.pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);
	m_shader->setLightPosition(light.translation());

	m_shader->setMaterial(m_material);
    glBindTexture(GL_TEXTURE_1D, m_colorTexture);
	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

UniformGrid::UniformGrid(
	Calculate2DFunction function,
	int numPointsX,
	int numPointsY,
	float minX,
	float minY,
	float maxX,
	float maxY,
	ColorFunction colorFunction)
	: Grid(function, new ColorMapShader, colorFunction),
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

void UniformGrid::getPoint(int i, float* p)
{
    p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    p[1] = m_minY + (i / m_numPointsX) * m_cellHeight;
}

RectilinearGrid::RectilinearGrid(Calculate2DFunction function, std::vector<float>& dimsX, std::vector<float>& dimsY, ColorFunction colorFunction)
	: UniformGrid(function,
		dimsX.size(),				//number of samples along X
		dimsY.size(),				//number of samples along y
		dimsX[0],					//minimal X value
		dimsY[0],					//minimal Y value
		dimsX[dimsX.size() - 1],	//maximal X value
		dimsY[dimsY.size() - 1],
		colorFunction),	//maximal Y value
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