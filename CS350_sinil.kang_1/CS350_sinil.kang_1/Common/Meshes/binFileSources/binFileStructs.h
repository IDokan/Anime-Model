#pragma once

#include <string>
#include <glm/glm.hpp>

struct Quaternion
{
public:
    union 
    {
        struct {
            float x, y, z;
            float s;
        };
        struct {
            glm::vec3 v;
            float s;
        };
        float elements[4];
    };

    Quaternion() noexcept;      // ctor
    Quaternion(float x, float y, float z, float s) noexcept;
    Quaternion(glm::vec3 v, float s) noexcept;
    Quaternion(float s, glm::vec3 v) noexcept;
    ~Quaternion();      // dtor
    Quaternion(const Quaternion& other);        // copy ctor
    Quaternion(Quaternion&& other) noexcept;        // Move copy ctor
    Quaternion& operator=(const Quaternion& other);     // copy assignment opeartor
    Quaternion& operator=(Quaternion&& other);      // Move assignment operator
};

// ============== Quaternion operators =========================
void operator+=(Quaternion& q1, const Quaternion& q2) noexcept;
void operator-=(Quaternion& q1, const Quaternion& q2) noexcept;
void operator*=(Quaternion& q1, float scale) noexcept;
void operator/=(Quaternion& q1, float divisor) noexcept;

Quaternion operator-(const Quaternion& q1) noexcept;

Quaternion operator+(const Quaternion& q1, const Quaternion& q2) noexcept;
Quaternion operator-(const Quaternion& q1, const Quaternion& q2) noexcept;
Quaternion operator*(const Quaternion& q, float scale) noexcept;
Quaternion operator*(float scale, const Quaternion& q) noexcept;
Quaternion operator/(const Quaternion& q, float divisor) noexcept;

Quaternion operator*(const Quaternion& q1, const Quaternion& q2) noexcept;
void operator*=(Quaternion& q1, const Quaternion& q2) noexcept;

[[nodiscard]] float dot(const Quaternion& q1, const Quaternion& q2) noexcept;
[[nodiscard]] float dot(Quaternion&& q1, Quaternion&& q2) noexcept;
[[nodiscard]] float magnitude_squared(const Quaternion& q) noexcept;
[[nodiscard]] float magnitude_squared(Quaternion&& q) noexcept;
[[nodiscard]] float magnitude(const Quaternion& q) noexcept;
[[nodiscard]] float magnitude(Quaternion&& q) noexcept;
[[nodiscard]] Quaternion Inverse(const Quaternion& q) noexcept;
[[nodiscard]] Quaternion Inverse(Quaternion&& q) noexcept;

// Need q be unit quaternion
[[nodiscard]] glm::mat4 ConvertToMatrix(const Quaternion& q) noexcept;

struct Vqs
{
public:
    Vqs();
    Vqs(const Vqs& other);
    Vqs(Vqs&& other);
    Vqs& operator=(const Vqs& other);
    Vqs& operator=(Vqs&& other);

    glm::vec3 v;
    Quaternion q;
    float s;
};

struct BoneIndex
{
    union
    {
        struct
        {
            unsigned char i1, i2, i3, i4;
        };
        unsigned char elements[4];
    };
};

class Bone
{
    friend class Mesh;
public:
    Bone();
    Bone(const Bone& other);
    Bone(Bone&& other);
    Bone& operator=(const Bone& other);
    Bone& operator=(Bone&& other);
private:
    std::string name;
    int parentID;
    Vqs toModelFromBone;
    Vqs toBoneFromModel;
};