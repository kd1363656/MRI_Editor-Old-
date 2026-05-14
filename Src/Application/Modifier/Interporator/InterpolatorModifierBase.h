#pragma once

class InterpolatorModifierBase
{

public:

	InterpolatorModifierBase         () = default;
	virtual ~InterpolatorModifierBase() = default;

	virtual void Init();

	virtual void Update() = 0;

	virtual void ImGuiInspector();

	virtual void DeserializeData(const nlohmann::json& Json);

	virtual nlohmann::json SerializeData();

	virtual void ResetInterpolate();

	float GetCurrentValue() const { return m_currentValue; }

private:

	float m_progress = 0.0f;
	
	float m_maxMeasurementTime = CommonConstant::FLOAT_ONE;
	float m_elapsedTime        = 0.0f;
	
	float m_clampMaxValue = CommonConstant::FLOAT_ONE;
	float m_currentValue  = 0.0f;
	
protected:

	float GetElapsedTime       () const { return m_elapsedTime;		   }
	float GetMaxMeasurementTime() const { return m_maxMeasurementTime; }

	float GetClampMaxValue() const { return m_clampMaxValue; }

	void SetProgress(float Set) { m_progress = Set; }
	
	void SetElapsedTime(float Set) { m_elapsedTime = Set; }

	void SetClampMaxValue(float Set) { m_clampMaxValue = Set; }
	void SetCurrentValue (float Set) { m_currentValue  = Set; }
};