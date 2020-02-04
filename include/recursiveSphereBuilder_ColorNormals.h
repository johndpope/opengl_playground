#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>

////#include GLM GL Math header library
#include <glm.hpp>
#include ".\gtc\matrix_transform.hpp"
#include ".\gtc\matrix_access.hpp"
#include ".\gtc\matrix_inverse.hpp"

typedef glm::vec4 point4;
typedef glm::vec3 point3;
typedef glm::vec2 point2;

struct vertatt {
	point3 vertex;
	point3 normal;
	point4 color;
};

point3 v[4] = { glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec3(0.0f, 0.942809f, -0.333333f),
				glm::vec3(-0.816497f, -0.471405f, -0.333333f),
				glm::vec3(0.816497f, -0.471405f, -0.333333f) };

static int k = 0;
static int nLevels = 5;
int NUM_VERTS = 0; // 3 times #triangles
vertatt* data = 0;
uint16_t g_useVertexNormal = 0;

point4 SurfaceColor = point4(0.2941f, 0.4902f, 0.8196f, 1.0f);
//glm::vec3 light_dir = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));
//glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);


void triangle(point3 a, point3 b, point3 c, int nDepth)
{
	point3 facetNormal;
	if (nDepth % 2 == 0) // even
		facetNormal = glm::normalize(glm::cross((a - b), (a - c)));
	else
		facetNormal = glm::normalize(glm::cross((a - c), (a - b)));

	data[k].vertex = a;
	if (0 == g_useVertexNormal)
		data[k].normal = facetNormal;
	else
		data[k].normal = glm::normalize(a - glm::vec3(0.0f));
	data[k].color = SurfaceColor;
	k++;

	data[k].vertex = b;
	if (0 == g_useVertexNormal)
		data[k].normal = facetNormal;
	else
		data[k].normal = glm::normalize(b - glm::vec3(0.0f));
	data[k].color = SurfaceColor;
	k++;

	data[k].vertex = c;
	if (0 == g_useVertexNormal)
		data[k].normal = facetNormal;
	else
		data[k].normal = glm::normalize(c - glm::vec3(0.0f));
	data[k].color = SurfaceColor;
	k++;
}

void divide_triangle(point3 a, point3 b, point3 c, int n, int nDepth)
{
	// save recursion depth level to correct vertex orientation reversals odd/even
	point3 v1, v2, v3;
	if (n > 0)
	{
		v1 = glm::normalize(a + b);
		v2 = glm::normalize(a + c);
		v3 = glm::normalize(b + c);
		divide_triangle(a, v2, v1, n - 1, nDepth);
		divide_triangle(c, v3, v2, n - 1, nDepth);
		divide_triangle(b, v1, v3, n - 1, nDepth);
		divide_triangle(v1, v2, v3, n - 1, nDepth);
	}
	else triangle(a, b, c, nDepth);
}

void tetrahedron(int nRecurses)
{
	// allocate correct memory region size based on size of vertatt STRUCT * number-of-vertices
	data = (vertatt*)malloc(sizeof(vertatt)*NUM_VERTS);

	divide_triangle(v[0], v[1], v[2], nRecurses, nRecurses);
	divide_triangle(v[3], v[2], v[1], nRecurses, nRecurses);
	divide_triangle(v[0], v[3], v[1], nRecurses, nRecurses);
	divide_triangle(v[0], v[2], v[3], nRecurses, nRecurses);
}

void initMesh(int nLev)
{
	nLevels = nLev;
	NUM_VERTS = 3 * glm::pow(4.0f, (nLevels + 1)); // 3 times #triangles
}

void genSphere(int nLev, uint16_t bUseVertNorms)
{
	g_useVertexNormal = bUseVertNorms;
	initMesh(nLev);
	tetrahedron(nLev);
}


