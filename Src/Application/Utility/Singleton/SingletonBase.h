#pragma once

// より安全なシングルトンを実装するための基底クラス
template<class Class>
class SingletonBase
{
public:

	static Class& GetInstance()
	{
		static Class instance_;
		return instance_;
	}

protected:
	SingletonBase         () = default;
	virtual ~SingletonBase() = default;
private:
	// コピー演算子削除
	SingletonBase(const SingletonBase&) = delete;
	// コピー代入演算子削除
	SingletonBase& operator=(const SingletonBase&) = delete;
	// 移動コンストラクタ削除
	SingletonBase(SingletonBase&&) = delete;
	// 移動代入演算子削除
	SingletonBase& operator=(SingletonBase&&) = delete;
};