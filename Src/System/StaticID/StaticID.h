#pragma once

template <typename BaseType>
class StaticID
{

public:

	template <typename Type>
		requires std::derived_from <Type , BaseType>
	static inline uint32_t GetTypeID()
	{
		static uint32_t id_ = GenerateTypeID();

		return id_;
	}

	template <typename Type>
	static inline uint32_t GetUnrestrictedTypeID()
	{
		static uint32_t id_ = GenerateTypeID();

		return id_;
	}

private:

	static inline uint32_t GenerateTypeID()
	{
		// "0"が絶対に発行されないことを保証
		static uint32_t id_ = FIRST_STATIC_ID;
		return id_++;
	}

	static constexpr uint32_t FIRST_STATIC_ID = 1u;
	
	StaticID () = default;
	~StaticID() = default;

};