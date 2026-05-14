#pragma once
#include "../InterpolatorModifierBase.h"

class HermiteInterpolatorModifier : public InterpolatorModifierBase
{

public:

	struct KeyFrame
	{
		float time    = 0.0f;
		float value   = 0.0f;
		float tangent = 0.0f;
	};

	HermiteInterpolatorModifier ()          = default;
	~HermiteInterpolatorModifier() override = default;

	void Init() override;

	void Update() override;

	void ImGuiInspector() override;
	
	void DeserializeData(const nlohmann::json& Json) override;

	nlohmann::json SerializeData() override;

private:

	void ImGuiInterpolatorGraph();
	void ImGuiSortList         ();
	void ImGuiDrawGraph        ();

	static constexpr size_t MIN_SIZE_TO_NEXT_LIST_ACCESS = 2;

	static constexpr ImVec2 DEBUG_DEFAULT_GRAPH_SIZE = { 500.0f , 300.0f };
	static constexpr ImVec2 DEBUG_MIN_GRAPH_SIZE     = { 10.0f  , 10.0f  };
	static constexpr ImVec2 DEBUG_MOUSE_HIT_RANGE    = { 8.0f   , 8.0f   };

	static constexpr float DEBUG_AUTO_INCREMENT_KEYFRAME_TIME_STEP = 0.1f;
	static constexpr float DEBUG_KEY_FRAME_DRAW_STEP				   = 0.05f;

	static constexpr int DEBUG_MAX_GRID_NUM  = 10;
	static constexpr int DEBUG_INVALID_INDEX = -1;

	int m_debugDraggingSelectedIndex = DEBUG_INVALID_INDEX;
	int m_debugDeleteSelectedIndex   = DEBUG_INVALID_INDEX;

	std::vector<HermiteInterpolatorModifier::KeyFrame> m_keyFrameList;

	ImVec2 m_graphSize = DEBUG_DEFAULT_GRAPH_SIZE;

};