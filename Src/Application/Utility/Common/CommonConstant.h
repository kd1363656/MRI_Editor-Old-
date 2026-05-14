#pragma once

namespace CommonConstant
{
	static const char* const STRING_UNKNOWN = "Unknown";

	static const char* const ASSET_FOLDER_PATH = "Asset/";

	static constexpr float EPSILON = 0.0001f;
	
	static constexpr float DOT_PRODUCT_MIN = -1.0f;
	static constexpr float DOT_PRODUCT_MAX =  1.0f;

	static constexpr float EASING_MAX_TIME = 1.0f;

	static constexpr float ALL_DEGREE     = 360.0f;
	static constexpr float HALF_DEGREE    = 180.0f;
	static constexpr float QUARTER_DEGREE = 90.0f;

	static constexpr float MAX_SCREEN_WIDTH   = 1280.0f;
	static constexpr float MAX_SCREEN_HEIGHT  = 720.0f;
	static constexpr float HALF_SCREEN_WIDTH  = MAX_SCREEN_WIDTH  / 2.0f;
	static constexpr float HALF_SCREEN_HEIGHT = MAX_SCREEN_HEIGHT / 2.0f;

	static constexpr float GRAVITATIONAL_ACCELERATION = 9.8f;

	static constexpr float CAMERA_DEFAULT_FIELD_OF_VIEW = 40.0f;

	static constexpr float HALF_MAGNIFICATION     = 0.5f;
	static constexpr float FLOAT_ONE              = 1.0f;
	static constexpr float DURATION_MAGNIFICATION = 5.0f;

	static constexpr uint64_t UNREGISTERD_GUID = 0llu;
	static constexpr uint64_t INCREMENT_GUID   = 1llu;

	static constexpr uint32_t INVALID_STATIC_ID = 0u;

	static constexpr size_t INCREMENT_VECTOR_INDEX = 1llu;

	static constexpr ImVec2 FIRST_NODE_POS = { 50.0f , 50.0f };

	static constexpr int VIRTUAL_KEY_MAX = 256;
	static constexpr int INT_ONE		 = 1;
}