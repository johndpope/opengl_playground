#pragma once
#include <surface.h>

Surface::Surface(ShaderBase* shader) :
    m_shader(shader)
{
    if (shader == nullptr)
    {
        fprintf(stdout, "NULL shader provided...\n");
    }
}

Surface::~Surface()
{
    delete m_shader;
}

void Surface::wireframe(bool enable)
{
    m_wireframe = enable ? GL_LINE : GL_FILL;
}

void Surface::update(const Camera& camera, const Light& light)
{
    m_camera = &camera;
    m_light = &light;
    UpdatableObject::update();
}

void Surface::initMovable(const GLuint& vao, const GLuint& vbo)
{
    m_wireframe = GL_FILL;

    m_shader->use();
    this->initSurface(vao, vbo);
}

void Surface::updateMovable(const float& totalTime, const float& frameTime)
{
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe);

    m_shader->use();
    this->updateSurface(totalTime, frameTime, *m_camera, *m_light);
}