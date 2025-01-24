#pragma once
#include "CertainDefines.h"
#include <math.h>
#include <iostream>
#include "Time.h"
struct Vector3Int
{
public:
	int32_t x, y, z;
	Vector3Int() : x(0), y(0), z(0) {}
	Vector3Int(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}
	//Vector3Int(Vector3 v) : x((int)v.x), y((int)v.y), z((int)v.z) {}
	int64_t GetValueForCompairing() const
	{
		int64_t tempX = (x & 0xFFFFF) | ((static_cast<unsigned>(x) >> 31) << 20);
		int64_t tempY = (y & 0xFFFFF) | ((static_cast<unsigned>(y) >> 31) << 20);
		int64_t tempZ = (z & 0xFFFFF) | ((static_cast<unsigned>(z) >> 31) << 20);
		return (tempX << 42) | (tempY << 21) | tempZ;
		//return (x * 2) + (y * 3) + (z * 5);
	}

	bool operator == (const Vector3Int& other)
	{
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator != (const Vector3Int& other)
	{
		return x != other.x || y != other.y || z != other.z;
	}
	bool operator<(const Vector3Int& other) const noexcept
	{
		// logic here
		return GetValueForCompairing() < other.GetValueForCompairing(); // for example
	}
	bool operator>(const Vector3Int& other) const noexcept
	{
		// logic here
		return GetValueForCompairing() > other.GetValueForCompairing(); // for example
	}
};
struct Vector3
{
public:
	float x;
	float y;
	float z;
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	inline Vector3Int ToVector3Int() const
	{
		return { (int)x, (int)y, (int)z };
	}

	inline float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	void Normalize()
	{
		float len = Length();
		if (len == 0.0f)
			return;
		x /= len;
		y /= len;
		z /= len;
	}
	Vector3 Normalized() const
	{
		float len = Length();
		if (len == 0.0f)
			return {0, 0, 0};
		return { x / len, y / len, z / len };
	}
	float Dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}


	Vector3 operator*(int scale)
	{
		return { x * scale, y * scale , z * scale };
	}
	Vector3 operator*(float scale)
	{
		return { x * scale, y * scale , z * scale };
	}
	Vector3 operator+(Vector3& other)
	{
		return { x + other.x, y + other.y , z + other.z };
	}
	Vector3 operator-(Vector3& other)
	{
		return { x - other.x, y - other.y , z - other.z };
	}
	/*static Vector3 operator+(Vector3& first, Vector3& other)
	{
		return { first.x + other.x, first.y + other.y , first.z + other.z };
	}*/
	Vector3 Cross(const Vector3& other) const
	{
		return
		{
			y * other.z - other.y * z,
			z * other.x - other.z * x,
			x * other.y - other.x * y
		};
	}
};


struct PositionAndChunk
{
	Vector3Int Position;
	Vector3Int Chunk;
	PositionAndChunk() : Position({0, 0, 0}), Chunk({ 0,0,0 }) {}
	PositionAndChunk(Vector3Int pos, Vector3Int chunk) : Position(pos), Chunk(chunk) {}
	PositionAndChunk(const Vector3& pos)
	{
		// find chunk
		Chunk = pos.ToVector3Int();
		if (Chunk.x < 0) Chunk.x -= ChunkWidth - 1;
		if (Chunk.y < 0) Chunk.y -= ChunkHeight - 1;
		if (Chunk.z < 0) Chunk.z -= ChunkWidth - 1;
		Chunk.x /= ChunkWidth;
		Chunk.y /= ChunkHeight;
		Chunk.z /= ChunkWidth;
		Position = pos.ToVector3Int();
		Position.x -= Chunk.x * ChunkWidth + (Chunk.x < 0 ? 1 : 0);
		Position.y -= Chunk.y * ChunkHeight + (Chunk.y < 0 ? 1 : 0);
		Position.z -= Chunk.z * ChunkWidth + (Chunk.z < 0 ? 1 : 0);
	}
};
struct Vector2
{
public:
	float x;
	float y;
};
struct Color
{
public:
	float r;
	float g;
	float b;
	float a;
};

struct Matrix4
{
public:
	float values[4 * 4];
	Matrix4() // construct an "identity matrix"
	{
		SetIdentity();
	}
	void SetIdentity()
	{
		for (int i = 0; i < 4 * 4; i++)
			values[i] = 0;
		for (int i = 0; i < 4; i++)
			values[i + i * 4] = 1;
	}

	void Translate(const Vector3& amount)
	{
		values[4 - 1] += amount.x;
		values[8 - 1] += amount.y;
		values[12 - 1] += amount.z;
	}
	/*void Rotate(const Vector3& amount)
	{
		Matrix4 rotX, rotY, rotZ;
		rotX.rotateX(amount.x * DegToRad);
		rotY.rotateY(amount.y * DegToRad);
		rotZ.rotateZ(amount.z * DegToRad);
		Matrix4 rotXY = MultiplyMatrix(rotX, rotY);
		Matrix4 rotXYZ = MultiplyMatrix(rotXY, rotZ);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				values[j + j * 4] = rotXYZ.values[j + j * 4];
	}*/
	void Scale(Vector3 amount)
	{

	}

	inline void Print() const
	{
		std::cout << "Printing matrix:" << "\n";
		std::cout << values[0] << " " << values[1] << " " << values[2] << " " << values[3] << "\n";
		std::cout << values[4] << " " << values[5] << " " << values[6] << " " << values[7] << "\n";
		std::cout << values[8] << " " << values[9] << " " << values[10] << " " << values[11] << "\n";
		std::cout << values[12] << " " << values[13] << " " << values[14] << " " << values[15] << std::endl;
	}

	
	void MultiplyMatrix(const Matrix4& other)
	{
		Matrix4 mat = MultiplyMatrix(*this, other);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				values[j + j * 4] = mat.values[j + j * 4];
	}
	inline Matrix4 MultiplyMatrix(const Matrix4& m1, const Matrix4& m2)
	{
		Matrix4 mat;
		for (int row = 0; row < 4; row++)
			for (int col = 0; col < 4; col++)
				mat.values[row + col * 4] = m1.values[row + 0 * 4] * m2.values[0 + col * 4] +
											m1.values[row + 1 * 4] * m2.values[1 + col * 4] +
											m1.values[row + 2 * 4] * m2.values[2 + col * 4] +
											m1.values[row + 3 * 4] * m2.values[3 + col * 4];
		return mat;
	}
	static Vector3 MultiplyMatrixVector3(const Vector3& vec, const Matrix4& mat)
	{
		float x, y, z;
		x = vec.x * mat.values[0 + 0 * 4] + vec.y * mat.values[1 + 0 * 4] + vec.z * mat.values[2 + 0 * 4] + mat.values[3 + 0 * 4];
		y = vec.x * mat.values[0 + 1 * 4] + vec.y * mat.values[1 + 1 * 4] + vec.z * mat.values[2 + 1 * 4] + mat.values[3 + 1 * 4];
		z = vec.x * mat.values[0 + 2 * 4] + vec.y * mat.values[1 + 2 * 4] + vec.z * mat.values[2 + 2 * 4] + mat.values[3 + 2 * 4];
		float w = vec.x * mat.values[0 + 3 * 4] + vec.y * mat.values[1 + 3 * 4] + vec.z * mat.values[2 + 3 * 4] + mat.values[3 + 3 * 4];
		if (w != 0.0f)
		{
			x /= w; y /= w; z /= w;
		}
		return { x, y, z };
	}
	static Matrix4 PointAt(const Vector3& pos, const Vector3& target, const Vector3& up)
	{
		Vector3 newForward = { target.x - pos.x, target.y - pos.y, target.z - pos.z };
		//Vector3 newForward = target - pos;
		newForward.Normalize();

		Vector3 a = newForward * up.Dot(newForward);
		Vector3 newUp = { up.x - a.x, up.y - a.y, up.z - a.z };
		newUp.Normalize();

		Vector3 newRight = newUp.Cross(newForward);

		Matrix4 matrix;
		matrix.values[0 + 0 * 4] = newRight.x;		matrix.values[0 + 1 * 4] = newRight.y;		matrix.values[0 + 2 * 4] = newRight.z;		matrix.values[0 + 3 * 4] = 0.0f;
		matrix.values[1 + 0 * 4] = newUp.x;			matrix.values[1 + 1 * 4] = newUp.y;			matrix.values[1 + 2 * 4] = newUp.z;			matrix.values[1 + 3 * 4] = 0.0f;
		matrix.values[2 + 0 * 4] = newForward.x;	matrix.values[2 + 1 * 4] = newForward.y;	matrix.values[2 + 2 * 4] = newForward.z;	matrix.values[2 + 3 * 4] = 0.0f;
		matrix.values[3 + 0 * 4] = pos.x;			matrix.values[3 + 1 * 4] = pos.y;			matrix.values[3 + 2 * 4] = pos.z;			matrix.values[3 + 3 * 4] = 1.0f;
		return matrix;

	}
	static Matrix4 Matrix_QuickInverse(const Matrix4& m) // Only for Rotation/Translation Matrices
	{
		Matrix4 matrix;
		matrix.values[0 + 0 * 4] = m.values[0 + 0 * 4]; matrix.values[0 + 1 * 4] = m.values[1 + 0 * 4]; matrix.values[0 + 2 * 4] = m.values[2 + 0 * 4]; matrix.values[0 + 3 * 4] = 0.0f;
		matrix.values[1 + 0 * 4] = m.values[0 + 1 * 4]; matrix.values[1 + 1 * 4] = m.values[1 + 1 * 4]; matrix.values[1 + 2 * 4] = m.values[2 + 1 * 4]; matrix.values[1 + 3 * 4] = 0.0f;
		matrix.values[2 + 0 * 4] = m.values[0 + 2 * 4]; matrix.values[2 + 1 * 4] = m.values[1 + 2 * 4]; matrix.values[2 + 2 * 4] = m.values[2 + 2 * 4]; matrix.values[2 + 3 * 4] = 0.0f;
		matrix.values[3 + 0 * 4] = -(m.values[3 + 0 * 4] * matrix.values[0 + 0 * 4] + m.values[3 + 1 * 4] * matrix.values[1 + 0 * 4] + m.values[3 + 2 * 4] * matrix.values[2 + 0 * 4]);
		matrix.values[3 + 1 * 4] = -(m.values[3 + 0 * 4] * matrix.values[0 + 1 * 4] + m.values[3 + 1 * 4] * matrix.values[1 + 1 * 4] + m.values[3 + 2 * 4] * matrix.values[2 + 1 * 4]);
		matrix.values[3 + 2 * 4] = -(m.values[3 + 0 * 4] * matrix.values[0 + 2 * 4] + m.values[3 + 1 * 4] * matrix.values[1 + 2 * 4] + m.values[3 + 2 * 4] * matrix.values[2 + 2 * 4]);
		matrix.values[3 + 3 * 4] = 1.0f;
		return matrix;
	}
	static Matrix4 RotationX(float amount)
	{
		Matrix4 mat;
		mat.values[0 + 0 * 4] = 1.0f;
		mat.values[1 + 1 * 4] = cos(amount);
		mat.values[1 + 2 * 4] = sin(amount);
		mat.values[2 + 1 * 4] = -sin(amount);
		mat.values[2 + 2 * 4] = cos(amount);
		mat.values[3 + 3 * 4] = 1.0f;
		return mat;
	}
	static Matrix4 RotationY(float amount)
	{
		Matrix4 mat;
		mat.values[0 + 0 * 4] = cos(amount);
		mat.values[2 + 0 * 4] = -sin(amount);
		mat.values[1 + 1 * 4] = 1.0f;
		mat.values[0 + 2 * 4] = sin(amount);
		mat.values[2 + 2 * 4] = cos(amount);
		mat.values[3 + 3 * 4] = 1.0f;
		return mat;
	}
	static Matrix4 RotationZ(float amount)
	{
		Matrix4 mat;
		mat.values[0] = cos(amount);
		mat.values[1] = -sin(amount);
		mat.values[0 + 1 * 4] = sin(amount);
		mat.values[1 + 1 * 4] = cos(amount);
		mat.values[2 + 2 * 4] = 1.0f;
		mat.values[3 + 3 * 4] = 1.0f;
		return mat;
	}
private:
	void rotateX(float amount)
	{
		//values[5] += cos(amount);
		//values[6] += -sin(amount);

		//values[9] += sin(amount);
		//values[10] += cos(amount);

		values[0 + 0 * 4] = 1.0f;
		values[1 + 1 * 4] = cos(amount);
		values[1 + 2 * 4] = sin(amount);
		values[2 + 1 * 4] = -sin(amount);
		values[2 + 2 * 4] = cos(amount);
		values[3 + 3 * 4] = 1.0f;
	}
	void rotateY(float amount)
	{
		//values[0] += cos(amount);
		//values[2] += sin(amount);

		//values[8] += -sin(amount);
		//values[10] += cos(amount);

		values[0 + 0 * 4] = cos(amount);
		values[2 + 0 * 4] = -sin(amount);
		values[1 + 1 * 4] = 1.0f;

		values[0 + 2 * 4] = sin(amount);
		values[2 + 2 * 4] = cos(amount);
		values[3 + 3 * 4] = 1.0f;
	}
	void rotateZ(float amount)
	{
		//values[0] += cos(amount);
		//values[1] += -sin(amount);

		//values[4] += sin(amount);
		//values[5] += cos(amount);
		values[0] = cos(amount);
		values[1] = -sin(amount);

		values[0 + 1 * 4] = sin(amount);
		values[1 + 1 * 4] = cos(amount);
		values[2 + 2 * 4] = 1.0f;
		values[3 + 3 * 4] = 1.0f;
	}
};


struct Transform
{
private:
	Vector3 m_rotation; // pitch, yaw, roll

	Vector3 m_forward;
	Vector3 m_right;
	Vector3 m_up;
public:
	Transform()
	{
		CalculateForward();
	}
	Vector3 Position;
	inline const Vector3& Rotation() const { return m_rotation; }
	Vector3 Scale;
	inline const Vector3& Forward() const { return m_forward; }
	inline const Vector3& Right() const { return m_right; }
	inline const Vector3& Up() const { return m_up; }

	Matrix4 TransformationMatrix;

	void Rotate(const Vector3& amount)
	{
		m_rotation.x += amount.x * Time::DeltaTime;
		float xRotClampValue = 89;
		if (m_rotation.x > xRotClampValue) m_rotation.x = xRotClampValue;
		if (m_rotation.x < -xRotClampValue) m_rotation.x = -xRotClampValue;
		m_rotation.y += amount.y * Time::DeltaTime;
		m_rotation.z += amount.z * Time::DeltaTime;
		CalculateForward();
	}
	void MoveForward(const Vector3& amount)
	{
		Vector3 deltaPos = m_forward * amount.z;
		deltaPos = m_right * amount.x + deltaPos;
		deltaPos = m_up * amount.y + deltaPos;

		Position.x += deltaPos.x * Time::DeltaTime;
		Position.y += deltaPos.y * Time::DeltaTime;
		Position.z += deltaPos.z * Time::DeltaTime;
	}
	/// <summary>
	/// MoveForward, but "up" is (0, 1, 0), no matter what
	/// </summary>
	/// <param name="amount"></param>
	void MoveForwardAbsoluteUp(const Vector3& amount)
	{
		Vector3 fwd = { m_forward.x, 0, m_forward.z };
		Vector3 right = { m_right.x, 0, m_right.z };
		fwd.Normalize();
		Vector3 deltaPos = fwd * amount.z;
		deltaPos = right * amount.x + deltaPos;
		deltaPos = Vector3(0, 1, 0) * amount.y + deltaPos;

		Position.x += deltaPos.x * Time::DeltaTime;
		Position.y += deltaPos.y * Time::DeltaTime;
		Position.z += deltaPos.z * Time::DeltaTime;
	}

private:
	void CalculateForward() // and sides...
	{
		// forward
		Vector3 target = { 0, 0, 1 };
		// x
		Matrix4 matCameraRot = Matrix4::RotationX(m_rotation.x * DegToRad);
		target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
		// y
		matCameraRot = Matrix4::RotationY(m_rotation.y * DegToRad);
		target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
		// z
		matCameraRot = Matrix4::RotationZ(m_rotation.z * DegToRad);
		m_forward = Matrix4::MultiplyMatrixVector3(target, matCameraRot);

		// right
		target = { 1, 0, 0 };
		// x
		matCameraRot = Matrix4::RotationX(m_rotation.x * DegToRad);
		target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
		// y
		matCameraRot = Matrix4::RotationY(m_rotation.y * DegToRad);
		target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
		// z
		matCameraRot = Matrix4::RotationZ(m_rotation.z * DegToRad);
		m_right = Matrix4::MultiplyMatrixVector3(target, matCameraRot);

		// up
		m_up = m_forward.Cross(m_right);
	}
};