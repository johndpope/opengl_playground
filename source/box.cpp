#include <cstddef>
#include <ctime>
#include <box.h>

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

void Box::generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size)
{
    this->generateBox(vertices, size);
}

void Box::generateBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size, glm::vec4* color)
{
    glm::vec4 defaultColor = glm::vec4(1.0f);
    if (color == nullptr)
    {
        color = &defaultColor;
    }

    float minX = -size.x / 2;
    float maxX = size.x / 2;
    float minY = -size.y / 2;
    float maxY = size.y / 2;
    float minZ = -size.z / 2;
    float maxZ = size.z / 2;

    glm::vec3 v0 = glm::vec3(minX, minY, maxZ);
    glm::vec3 v1 = glm::vec3(maxX, minY, maxZ);
    glm::vec3 v2 = glm::vec3(maxX, maxY, maxZ);
    glm::vec3 v3 = glm::vec3(minX, maxY, maxZ);
    glm::vec3 v4 = glm::vec3(maxX, minY, minZ);
    glm::vec3 v5 = glm::vec3(minX, minY, minZ);
    glm::vec3 v6 = glm::vec3(minX, maxY, minZ);
    glm::vec3 v7 = glm::vec3(maxX, maxY, minZ);

    glm::vec3 posZ[4] = { v0, v1, v2, v3 };
    glm::vec3 posY[4] = { v3, v2, v7, v6 };
    glm::vec3 posX[4] = { v1, v4, v7, v2 };
    glm::vec3 negZ[4] = { v4, v5, v6, v7 };
    glm::vec3 negY[4] = { v0, v5, v4, v1 };
    glm::vec3 negX[4] = { v5, v0, v3, v6 };

    glm::vec3* faces[NUM_BOX_SIDES] = { posX, posY, posZ, negX, negY, negZ };
    int sequence[VERTICES_PER_SIDE] = { 0, 1, 2, 0, 2, 3 };
    int vertexOffset = 0;

    // Preallocate vertices
    vertices.resize(NUM_BOX_SIDES * VERTICES_PER_SIDE);

    // Iterate over faces
    for (int i = 0; i < NUM_BOX_SIDES; i++)
    {
        // Calculate face normal
        glm::vec3 a = faces[i][2] - faces[i][0];
        glm::vec3 b = faces[i][1] - faces[i][0];
        glm::vec3 n = glm::normalize(glm::cross(b, a));

        // Iterate over vertices
        for (int k = 0; k < VERTICES_PER_SIDE; k++)
        {
			int m = sequence[k];

            BoxVertexAttribute* attrib = &vertices[vertexOffset++];
            attrib->position = faces[i][m];
            attrib->normal = n;
            attrib->color = *color;
            attrib->texture.x = ((m == 0) || (m == 3)) ? 0.0f : 1.0f;
            attrib->texture.y = ((m == 0) || (m == 1)) ? 0.0f : 1.0f;
        }
    }
}

void Box::initShape(const GLuint& vao, const GLuint& vbo)
{
    std::vector<BoxVertexAttribute> vertices;
    this->generateCustomBox(vertices, m_size);
    m_numVertices = vertices.size();

    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxVertexAttribute) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    this->initBox(vao, vbo);
}

void Box::updateShape(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
    this->updateBox(totalTime, frameTime, camera, light);
}

BasicColorBox::BasicColorBox(const glm::vec4 color, const glm::vec3& size)
	: Box(new BasicColorShader(), size),
	  m_color(color) { }

BasicColorBox::BasicColorBox(const glm::vec3& size)
	: Box(new BasicColorShader(), size),
	  m_color(glm::vec4(1.0f)) { }

void BasicColorBox::generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size)
{
	this->generateBox(vertices, size, &m_color);
}

void BasicColorBox::initBox(const GLuint& vao, const GLuint& vbo)
{
	m_shader->setBufferPosition(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, position));
	m_shader->setBufferColor(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, color));
}

void BasicColorBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelViewProj = camera.projection() * camera.pose() * this->pose();
	m_shader->setModelViewProjection(modelViewProj);
	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

ColorBox::ColorBox(const glm::vec4 color, glm::vec3 size)
	: Box(new ColorShader(), size),
	  m_color(color)
{
	srand(time(NULL));

	m_material.shininess = rand() / (float)RAND_MAX;
	m_material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	m_material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	m_material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
}

ColorBox::ColorBox(const Material material, glm::vec3 size)
	: Box(new ColorShader(), size),
	  m_material(material),
	  m_color(glm::vec4(1.0f)) { }

void ColorBox::generateCustomBox(std::vector<BoxVertexAttribute>& vertices, glm::vec3& size)
{
	this->generateBox(vertices, size, &m_color);
}

void ColorBox::initBox(const GLuint& vao, const GLuint& vbo)
{
    m_shader->setBufferPosition(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, position));
    m_shader->setBufferNormal(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, normal));
	m_shader->setBufferColor(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, color));
}

void ColorBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
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

MultiTextureBox::MultiTextureBox(const std::string texturePaths[NUM_BOX_SIDES], glm::vec3 size)
	: Box(new TextureShader(), size)
{
	srand(time(NULL));

	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_textureMaterials[i].texturePath = texturePaths[i];
		m_textureMaterials[i].material.shininess = rand() / (float)RAND_MAX;
		m_textureMaterials[i].material.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
		m_textureMaterials[i].material.diffuseColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
		m_textureMaterials[i].material.specularColor = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
	}
}

MultiTextureBox::MultiTextureBox(const TextureMaterial textureMaterials[NUM_BOX_SIDES], glm::vec3 size)
	: Box(new TextureShader(), size)
{
	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_textureMaterials[i] = textureMaterials[i];
	}
}

void MultiTextureBox::initBox(const GLuint& vao, const GLuint& vbo)
{
    m_shader->setBufferPosition(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, position));
    m_shader->setBufferNormal(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, normal));
	m_shader->setBufferTextureCoord(sizeof(BoxVertexAttribute), offsetof(BoxVertexAttribute, texture));

	glGenTextures(NUM_BOX_SIDES, m_textures);
	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(m_textureMaterials[i].texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
	}
}

void MultiTextureBox::updateBox(const float& totalTime, const float& frameTime, const Camera& camera, const Light& light)
{
	glm::mat4 modelView = camera.pose() * this->pose();
	glm::mat4 modelViewProj = camera.projection() * modelView;

	m_shader->setView(camera.pose());
	m_shader->setModelView(modelView);
	m_shader->setModelViewProjection(modelViewProj);
	m_shader->setLightPosition(light.translation());

	for (int i = 0; i < NUM_BOX_SIDES; i++)
	{
		m_shader->setMaterial(m_textureMaterials[i].material);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glDrawArrays(GL_TRIANGLES, VERTICES_PER_SIDE * i, VERTICES_PER_SIDE);
	}
}