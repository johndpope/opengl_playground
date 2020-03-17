#pragma once
#include <grid.h>
#include <glm\gtx\matrix_decompose.hpp>

Grid2D::Grid2D()
    : m_numPointsX(0),
	  m_numPointsY(0),
	  m_minX(0),
	  m_minY(0),
	  m_cellWidth(0),
	  m_cellHeight(0) { }

int Grid2D::findCell(float* p)
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

// Given lexicographic index, find cell vertices (quads assumed)
int	Grid2D::getCell(int i, int* v)
{
	int cellRow = i / (m_numPointsX - 1);

	v[0] = i + cellRow;
	v[1] = v[0] + 1;
	v[2] = v[1] + m_numPointsX;
	v[3] = v[0] + m_numPointsX;

	return 4;
}

void Grid2D::getPoint(int i, float* p)
{
    p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    p[1] = m_minY + (i / m_numPointsX) * m_cellHeight;
}

CalculateGrid2D::CalculateGrid2D(
	Calculate2DFunction calcFunction,
	int numPointsX,
	int numPointsY,
	float minX,
	float minY,
	float maxX,
	float maxY)
    : m_calcFunction(calcFunction)
	{
		m_numPointsX = numPointsX;
		m_numPointsY = numPointsY;
		m_minX = minX;
		m_minY = minY;
		m_cellWidth  = (maxX - minX) / (numPointsX - 1);
		m_cellHeight = (maxY - minY) / (numPointsY - 1);
		m_scalars = this->initScalars();
	}

ScalarAttributes* CalculateGrid2D::initScalars()
{
    ScalarAttributes* scalars = new ScalarAttributes(m_numPointsX * m_numPointsY);

    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[2];
        this->getPoint(i, point);

        try
        {
            float value = m_calcFunction(point[0], point[1]);
            scalars->setC0Scalar(i, value);
        }
        catch (...)
        {
            scalars->setC0Scalar(i, INFINITY);
        }
    }

    return scalars;
}

SliceGrid2D::SliceGrid2D(
	Grid3D& grid,
	Plane& plane,
	int numPointsX,
	int numPointsY,
	float minX,
	float minY,
	float maxX,
	float maxY)
    : m_grid(grid)
	{
		m_numPointsX = numPointsX;
		m_numPointsY = numPointsY;
		m_minX = minX;
		m_minY = minY;
		m_cellWidth  = (maxX - minX) / (numPointsX - 1);
		m_cellHeight = (maxY - minY) / (numPointsY - 1);

		this->setPlane(plane);

		m_scalars = this->initScalars();
	}

void SliceGrid2D::setPlane(Plane& plane)
{
	// Compute the pose transform for the plane
	glm::vec3 oldNormal = glm::vec3(0, 0, 1.0f);
	glm::vec3 newNormal = glm::normalize(glm::vec3(plane.a, plane.b, plane.c));
	if (glm::dot(oldNormal, newNormal) != 1.0f)
	{
		glm::vec3 rotationAxis = glm::normalize(glm::cross(oldNormal, newNormal));
		float rotationAngle = acosf(glm::dot(oldNormal, newNormal) / (glm::length(oldNormal) * glm::length(newNormal)));
		glm::mat4 rotation = glm::rotate(glm::mat4(), rotationAngle, rotationAxis);
		glm::mat4 translation = glm::translate(glm::mat4(), newNormal * plane.d);
		this->transformTo(rotation * translation);
		this->update();
	}
}

ScalarAttributes* SliceGrid2D::initScalars()
{
    ScalarAttributes* scalars = new ScalarAttributes(m_numPointsX * m_numPointsY);

    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point2D[2];
        this->getPoint(i, point2D);

		float result = this->evaluate(point2D[0], point2D[1]);
		scalars->setC0Scalar(i, result);
    }

    return scalars;
}

float SliceGrid2D::evaluate(float x, float y, float z)
{
	glm::vec4 hPoint(x, y, 0, 1.0f);
	hPoint = this->pose() * hPoint;
	float point3D[3] = { hPoint.x / hPoint.w, hPoint.y / hPoint.w, hPoint.z / hPoint.w };
	return m_grid.evaluate(point3D[0], point3D[1], point3D[2]);
}

void SliceGrid2D::updateMovable(const float& totalTime, const float& frameTime)
{
	if (m_pose_updated)
	{
		delete m_scalars;
		m_scalars = this->initScalars();
	}
}

Grid3D::Grid3D()
    : m_numPointsX(0),
	  m_numPointsY(0),
	  m_numPointsZ(0),
	  m_minX(0),
	  m_minY(0),
	  m_minZ(0),
	  m_cellWidth(0),
	  m_cellHeight(0),
	  m_cellDepth(0) { }

int Grid3D::findCell(float* p)
{
	int C[3];

	// Compute cell coordinates C[0], C[1]
	C[0] = floor((p[0] - m_minX) * m_numPointsX / m_cellWidth);
	C[1] = floor((p[1] - m_minY) * m_numPointsY / m_cellHeight);
	C[2] = floor((p[2] - m_minZ) * m_numPointsZ / m_cellDepth);

	// Test if p is inside the dataset
	if (C[0] < 0 || C[0] >= m_numPointsX - 1 || C[1] < 0 || C[1] >= m_numPointsY - 1 || C[2] < 0 || C[2] >= m_numPointsZ - 1)
	{
		return -1;
	}

	// Go from cell coordinates to cell index
	return C[0] + C[1] * m_numPointsX + C[2] * m_numPointsX * m_numPointsY;
}

// Given lexicographic index, find cell vertices (voxels assumed)
int	Grid3D::getCell(int i, int* v)
{
	int incX = 1;
	int incY = m_numPointsX;
	int incZ = m_numPointsX * m_numPointsY;

	int cellRow = i / (m_numPointsX - 1);
	int cellDepth = i / ((m_numPointsX - 1) * (m_numPointsY - 1));

	v[0] = i + cellRow + cellDepth * m_numPointsX;
	v[1] = v[0] + incX;
	v[2] = v[1] + incY;
	v[3] = v[0] + incY;
	v[4] = v[0] + incZ;
	v[5] = v[1] + incZ;
	v[6] = v[2] + incZ;
	v[7] = v[3] + incZ;

	return 8;
}

void Grid3D::getPoint(int i, float* p)
{
	p[0] = m_minX + (i % m_numPointsX) * m_cellWidth;
    i /= m_numPointsX;
    p[1] = m_minY + (i % m_numPointsY) * m_cellHeight;
    i /= m_numPointsY;
    p[2] = m_minZ + i * m_cellDepth;
}

CalculateGrid3D::CalculateGrid3D(
	Calculate3DFunction calcFunction,
	int numPointsX,
	int numPointsY,
	int numPointsZ,
	float minX,
	float minY,
	float minZ,
	float maxX,
	float maxY,
	float maxZ)
    : m_calcFunction(calcFunction)
	{
		m_numPointsX = numPointsX;
		m_numPointsY = numPointsY;
		m_numPointsZ = numPointsZ;
		m_minX = minX;
		m_minY = minY;
		m_minZ = minZ;
		m_cellWidth  = (maxX - minX) / (numPointsX - 1);
		m_cellHeight = (maxY - minY) / (numPointsY - 1);
		m_cellDepth  = (maxZ - minZ) / (numPointsZ - 1);
		m_scalars = this->initScalars();
	}

ScalarAttributes* CalculateGrid3D::initScalars()
{
    ScalarAttributes* scalars = new ScalarAttributes(m_numPointsX * m_numPointsY * m_numPointsZ);

    int numPoints = this->numPoints();
    for (int i = 0; i < numPoints; i++)
    {
        float point[3];
        this->getPoint(i, point);

        try
        {
            float value = m_calcFunction(point[0], point[1], point[2]);
            scalars->setC0Scalar(i, value);
        }
        catch (...)
        {
            scalars->setC0Scalar(i, INFINITY);
        }
    }

    return scalars;
}