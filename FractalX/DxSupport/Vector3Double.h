#pragma once
#include <SimpleMath.h>

namespace DXF
{
	struct Vector3Double
	{
		double x;
		double y;
		double z;

		Vector3Double();

		Vector3Double(double x, double y, double z);

		Vector3Double(const Vector3Double& v);

		Vector3Double(const DirectX::SimpleMath::Vector3 v);

		// make one with vector3

		virtual ~Vector3Double();

		bool operator==(const Vector3Double& v) const;
		bool operator!=(const Vector3Double& v) const;

		Vector3Double& operator=(const Vector3Double& v);

		Vector3Double& operator=(const DirectX::SimpleMath::Vector3& v);

		Vector3Double& operator+=(const Vector3Double& v);

		Vector3Double& operator*=(const Vector3Double& v);

		Vector3Double& operator+=(double d);

		Vector3Double& operator*=(double d);

		DirectX::SimpleMath::Vector3 ToVector3();

		double Length() const;

		double Length(double root) const;

		friend Vector3Double operator+(const Vector3Double& lf, const Vector3Double& rt);

		friend Vector3Double operator*(const Vector3Double& lf, const Vector3Double& rt);

		friend Vector3Double operator*(const Vector3Double& lf, double d);

		friend Vector3Double operator*(double d, const Vector3Double& rt);
	};

}