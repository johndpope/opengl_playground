#pragma once
#include <glm.hpp>
#include <string>

struct Material
{
	float shininess;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};

struct TextureMaterial
{
	std::string texturePath;
	Material material;
};