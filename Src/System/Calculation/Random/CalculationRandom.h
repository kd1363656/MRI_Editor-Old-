#pragma once
#include "../../../Application/Utility/Singleton/SingletonBase.h"

class CalculationRandom : public SingletonBase<CalculationRandom>
{

public:

	template <typename TypeInfo>
	// コンストラクタがコピー可能かを調べる
		requires std::is_copy_constructible_v<TypeInfo>
	struct RandomWeightData
	{
		TypeInfo type   = TypeInfo();
		float	 weight = 0.0f;
	};

	template <typename TypeInfo>
		requires std::is_arithmetic_v<TypeInfo>
	static TypeInfo GetRandom(TypeInfo Min , TypeInfo Max)
	{
		auto& mt_ = GetMT();

		if constexpr(std::is_integral_v<TypeInfo>)
		{
			std::uniform_int_distribution<TypeInfo> dist_(Min , Max);
			return dist_(mt_);
		}
		else if constexpr (std::is_floating_point_v<TypeInfo>)
		{
			std::uniform_real_distribution<TypeInfo> dist_(Min , Max);
			return dist_;
		}
	}

	template <typename TypeInfo , size_t Size>
		requires std::is_copy_constructible_v<TypeInfo>
	static TypeInfo GetRandomWeight(const std::array<CalculationRandom::RandomWeightData<TypeInfo> , Size>& WeightedValues)
	{
		// もしリストが空ならコンストラクタを返す
		if (WeightedValues.empty())
		{
			return TypeInfo();
		}

		// 重みのみを配列に格納
		std::vector<float> weights_;
		for (const auto& data_ : WeightedValues)
		{
			weights_.emplace_back(data_.weight);
		}

		// 重みを考慮したランダムな配列の要素数を取得
		std::discrete_distribution<size_t> dist_(weights_.begin() , weights_.end());
		auto& mt_ = GetMT();

		size_t index_ = dist_(mt_);

		// 引数から取得した"vector"型の配列の要素に
		// 算出したインデックスを用いてアクセス
		return WeightedValues[index_].type;
	}
	
private:

	std::mt19937_64 m_rand;

	// ========================
	// Singleton
	// ========================
	friend class SingletonBase<CalculationRandom>;

	CalculationRandom ()
	{
		std::random_device rnd_;
		// "mt19937"は"64"ビット環境で高速に動く
		m_rand = std::mt19937_64(rnd_());
	}
	~CalculationRandom() override = default;

	// シングルトンで管理されている自身のメルセンヌツイスターを取得
	static std::mt19937_64& GetMT() { return CalculationRandom::GetInstance().m_rand; }
};