#pragma once
#include <kgfw/Object.h>	// Include kgfw::Object to be used as a base class
#include <glm/glm.hpp>      // Include glm

class GameObject : public kgfw::Object {
public:
    GameObject(const char* const functionName);
    ~GameObject();

    void setPosition(const glm::vec3& position);
    void setRotationZ(float angleInRadians);
    void setScaling(const glm::vec3& scale);

    const glm::vec3& getPosition() const;
    float getRotationZ() const;
    const glm::vec3& getScaling() const;

    glm::mat4 getModelMatrix() const;

	void setLookAt(const glm::vec3& to, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

private:
    // Model position, rotation and scale
    glm::vec3 m_position;           // Store position of plane here
    float m_angleZInRadians;        // Store angle of plane here
    glm::vec3 m_scale;              // Store scale of plane here
	glm::mat4 m_oritentation;
};
