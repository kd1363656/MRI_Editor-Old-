#pragma once

namespace Calculation
{
	// クオータニオンからオイラー角(度)に戻す
	inline void QuaternionToEuler(const Math::Quaternion& Quaternion , Math::Vector3& ConvertedValue)
	{
		Math::Vector3 radian_ = Quaternion.ToEuler();

		ConvertedValue =
		{
			DirectX::XMConvertToDegrees(radian_.x) ,
			DirectX::XMConvertToDegrees(radian_.y) ,
			DirectX::XMConvertToDegrees(radian_.z)
		};
	}

	inline Math::Vector3 QuaternionToEuler(const Math::Quaternion& Quaternion)
	{
		Math::Vector3 radian_ = Quaternion.ToEuler();

		Math::Vector3 result_ = 
		{
			DirectX::XMConvertToDegrees(radian_.x) ,
			DirectX::XMConvertToDegrees(radian_.y) ,
			DirectX::XMConvertToDegrees(radian_.z)
		};

		return result_;
	}
	
	// オイラー角(度)からクオータニオンに変換
	inline void EulerToQuaternion(const Math::Vector3& Euler , Math::Quaternion& Quaternion)
	{
		Quaternion = Math::Quaternion::CreateFromYawPitchRoll
		(
			DirectX::XMConvertToRadians(Euler.y) ,
			DirectX::XMConvertToRadians(Euler.x) ,
			DirectX::XMConvertToRadians(Euler.z) 
		);
	}

	// オイラー角(度)からクオータニオンに変換
	inline Math::Quaternion EulerToQuaternion(const Math::Vector3& Euler)
	{
		Math::Quaternion result_ = Math::Quaternion::CreateFromYawPitchRoll
		(
			DirectX::XMConvertToRadians(Euler.y),
			DirectX::XMConvertToRadians(Euler.x),
			DirectX::XMConvertToRadians(Euler.z)
		);

		return result_;
	}

	// 行列から"Yaw"を取得
	inline float VectorToYaw(const Math::Vector3& Euler)
	{
		return std::atan2f(Euler.x, Euler.z);
	}

	inline void AddDirection(const Math::Vector3& AddDirection , Math::Vector3& Direction)
	{
		Direction += AddDirection;
		
		// ベクトル情報は"-1.0f ~ 1.0f"しかとってほしくない
		Direction.x = std::clamp(Direction.x , -Math::Vector3::UnitX.x , Math::Vector3::UnitX.x );
		Direction.y = std::clamp(Direction.y , -Math::Vector3::UnitY.y , Math::Vector3::UnitY.y );
		Direction.z = std::clamp(Direction.z , -Math::Vector3::UnitZ.z , Math::Vector3::UnitZ.z );
	}

	inline float Hermite(float T , float P0 , float P1 , float M0 , float M1)
	{
		const float t2_ = T   * T;
		const float t3_ = t2_ * T;

		float h00_ = (2.0f * t3_) - (3.0f * t2_) + 1.0f;
		float h10_ = t3_ - (2.0f * t2_) + T;
		float h01_ = -2.0f * t3_ + 3.0f * t2_;
		float h11_ = t3_ - t2_;

		return h00_ * P0 + h10_ * M0 + h01_ * P1 + h11_ * M1;
	}
}