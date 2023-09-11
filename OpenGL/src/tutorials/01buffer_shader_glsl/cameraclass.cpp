////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_positionZ = 0.0f;

    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
    m_positionX = x;
    m_positionY = y;
    m_positionZ = z;
    return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
    return;
}


void CameraClass::GetPosition(float* position)
{
	position[0] = m_positionX;
	position[1] = m_positionY;
	position[2] = m_positionZ;
	return;
}


void CameraClass::GetRotation(float* rotation)
{
	rotation[0] = m_rotationX;
	rotation[1] = m_rotationY;
	rotation[2] = m_rotationZ;
	return;
}


void CameraClass::Render()
{
    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    float pitch = m_rotationX * 0.0174532925f;
    float yaw   = m_rotationY * 0.0174532925f;
    float roll  = m_rotationZ * 0.0174532925f;
    //float yaw{ -90.f }, pitch{ 0.f };
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraPos(m_positionX, m_positionY, m_positionZ);
    glm::vec3 cameraUp(0, 1.f, 0.f);
	// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    return;
}

glm::mat4 CameraClass::GetViewMatrix() const
{
    return m_viewMatrix;
}

void CameraClass::MatrixRotationYawPitchRoll(float* matrix, float yaw, float pitch, float roll)
{
    float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;


    // Get the cosine and sin of the yaw, pitch, and roll.
    cYaw = cosf(yaw);
    cPitch = cosf(pitch);
    cRoll = cosf(roll);

    sYaw = sinf(yaw);
    sPitch = sinf(pitch);
    sRoll = sinf(roll);

    // Calculate the yaw, pitch, roll rotation matrix.
    matrix[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
    matrix[1] = (sRoll * cPitch);
    matrix[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);

    matrix[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
    matrix[4] = (cRoll * cPitch);
    matrix[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);

    matrix[6] = (cPitch * sYaw);
    matrix[7] = -sPitch;
    matrix[8] = (cPitch * cYaw);

    return;
}


void CameraClass::TransformCoord(VectorType& vector, float* matrix)
{
    float x, y, z;


    // Transform the vector by the 3x3 matrix.
    x = (vector.x * matrix[0]) + (vector.y * matrix[3]) + (vector.z * matrix[6]);
    y = (vector.x * matrix[1]) + (vector.y * matrix[4]) + (vector.z * matrix[7]);
    z = (vector.x * matrix[2]) + (vector.y * matrix[5]) + (vector.z * matrix[8]);

    // Store the result in the reference.
    vector.x = x;
    vector.y = y;
    vector.z = z;

    return;
}

