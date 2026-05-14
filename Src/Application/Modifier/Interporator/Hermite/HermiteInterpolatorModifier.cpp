#include "HermiteInterpolatorModifier.h"

#include "../../../main.h"

#include "../../../Utility/Json/JsonUtility.h"
#include "../../../../System/Calculation/Calculation.h"

void HermiteInterpolatorModifier::Init()
{
	InterpolatorModifierBase::Init();

	m_graphSize = DEBUG_DEFAULT_GRAPH_SIZE;

	m_debugDraggingSelectedIndex = DEBUG_INVALID_INDEX;
	m_debugDeleteSelectedIndex   = DEBUG_INVALID_INDEX;
}

void HermiteInterpolatorModifier::Update()
{
	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	// 次のキーフレームがなければ実行しない
	if (m_keyFrameList.size() < MIN_SIZE_TO_NEXT_LIST_ACCESS) { return; }

	const float elapsed_ = GetElapsedTime();

	// 最後のキーを超えたら固定
	if (elapsed_ >= m_keyFrameList.back().time)
	{
		// 進捗率を"Max(1.0f)"に固定
		SetProgress    (CommonConstant::FLOAT_ONE);
		SetElapsedTime (m_keyFrameList.back().time);
		SetCurrentValue(m_keyFrameList.back().value);
		return;
	}

	// 補完対象のキーフレームを探す
	// ("size - 1"で最大容量を超えないように調整)
	{
		for (std::size_t ki_ = 0; ki_ < m_keyFrameList.size() - 1; ++ki_)
		{
			const auto& k0_ = m_keyFrameList[ki_];
			const auto& k1_ = m_keyFrameList[ki_ + 1];

			if (elapsed_ >= k0_.time && elapsed_ <= k1_.time)
			{
				float localT_ = (elapsed_ - k0_.time) / (k1_.time - k0_.time);

				// 現在の経過時間から次のキーフレームの開始時間を
				// 割ることで進捗度を算出
				SetProgress(localT_);

				// エルミート補完
				const float currentValue_ = Calculation::Hermite(localT_ , k0_.value , k1_.value , k0_.tangent , k1_.tangent);
				SetCurrentValue(currentValue_);

				// リストの最初から最後まで用総数が大きければ大きいほど終盤に近付く
				// つまり今の範囲を取るならば後の範囲を絶対に今は取らないのでループを抜ける
				break;	
			}
		}
	}
	
	// 経過時間を計測
	{
		SetElapsedTime(elapsed_ + deltaTime_);
	}
}

void HermiteInterpolatorModifier::ImGuiInspector()
{
	InterpolatorModifierBase::ImGuiInspector();

	ImGui::DragFloat2("GraphSize" , &m_graphSize.x , 0.1f);

	// グラフサイズが最小値を下回らないように調整
	m_graphSize.x = std::max(m_graphSize.x , DEBUG_MIN_GRAPH_SIZE.x);
	m_graphSize.y = std::max(m_graphSize.y , DEBUG_MIN_GRAPH_SIZE.y);

	ImGui::Separator();
	ImGui::Text("Hermite KeyFrameList (Draggable Graph)");

	ImGui::SameLine();
	if (ImGui::Button("Add KeyFrame"))
	{
		// もしリストが空なら二つキーフレームを作成
		if (m_keyFrameList.empty())
		{
			HermiteInterpolatorModifier::KeyFrame newKeyOne_ = {};
			HermiteInterpolatorModifier::KeyFrame newKeyTwo_ = {};
		
			newKeyOne_.time = 0.0f;
			newKeyTwo_.time = DEBUG_AUTO_INCREMENT_KEYFRAME_TIME_STEP;

			m_keyFrameList.emplace_back(newKeyOne_);
			m_keyFrameList.emplace_back(newKeyTwo_);
		}
		else
		{
			HermiteInterpolatorModifier::KeyFrame newKey_ = {};
			newKey_.time = m_keyFrameList.empty() ? 0.0f : m_keyFrameList.back().time + DEBUG_AUTO_INCREMENT_KEYFRAME_TIME_STEP;
			m_keyFrameList.emplace_back(newKey_);
		}
	}

	// "Tangent"編集スライダー
	ImGui::Separator();
	ImGui::Text("TangentEditor");
	for (std::size_t i_ = 0; i_ < m_keyFrameList.size(); ++i_)
	{
		std::string label_ = "Tangent : " + std::to_string(i_);
		ImGui::SliderFloat(label_.c_str(), &m_keyFrameList[i_].tangent, -10.0f, 10.0f);
	}

	ImGuiInterpolatorGraph();
}

void HermiteInterpolatorModifier::DeserializeData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	InterpolatorModifierBase::DeserializeData(Json);

	if (JsonUtility::CheckIsContainAndArray(Json , "KeyFrameList"))
	{
		for (const auto& jsonArray_ : Json["KeyFrameList"])
		{
			HermiteInterpolatorModifier::KeyFrame keyFrame_ = {};

			keyFrame_.time    = jsonArray_.value("Time"    , 0.0f);
			keyFrame_.tangent = jsonArray_.value("Tangent" , 0.0f);
			keyFrame_.value   = jsonArray_.value("Value"   , 0.0f);

			m_keyFrameList.emplace_back(keyFrame_);
		}
	}
}

nlohmann::json HermiteInterpolatorModifier::SerializeData()
{
	auto rootJson_ = nlohmann::json();

	// 基底クラスのシリアライズ
	auto baseJson_ = InterpolatorModifierBase::SerializeData();
	if (!baseJson_.is_null())
	{
		rootJson_.update(baseJson_);
	}

	auto arrayJson_ = nlohmann::json::array();
	for (const auto& kl_ : m_keyFrameList)
	{
		auto json_ = nlohmann::json();

		json_["Time"]    = kl_.time;
		json_["Tangent"] = kl_.tangent;
		json_["Value"]   = kl_.value;

		arrayJson_.emplace_back(json_);
	}

	rootJson_["KeyFrameList"] = arrayJson_;

	return rootJson_;
}

void HermiteInterpolatorModifier::ImGuiInterpolatorGraph()
{
	ImGuiSortList ();
	ImGuiDrawGraph();
}
void HermiteInterpolatorModifier::ImGuiDrawGraph()
{
	// ※大前提としてエルミート補完は"2"つのキーフレームがなければ成り立たない
	// キーフレームが二つ以下なら描画しない
	if (m_keyFrameList.size() < MIN_SIZE_TO_NEXT_LIST_ACCESS) { return; }

	// グラフのサイズとグラフの領域を定義
	ImVec2      graphSize_ = m_graphSize;					// グラフのサイズとグラフ領域を定義
	ImVec2      origin_    = ImGui::GetCursorScreenPos();	// グラフの左上位置
	ImDrawList* drawList_  = ImGui::GetWindowDrawList ();

	// グラフ領域にマウス入力を対応させる
	ImGui::InvisibleButton("GraphArea" , graphSize_);
	const bool hovered_ = ImGui::IsItemHovered();

	// 四角形を描画
	drawList_->AddRect(origin_ , origin_ + graphSize_ , IM_COL32(100.0f , 100.0f , 100.0f , 255.0f));
	// 斜めに線を描画
	drawList_->AddLine(origin_ , { origin_.x + graphSize_.x , origin_.y + graphSize_.y }, IM_COL32(80.0f  , 80.0f  , 80.0f  , 255.0f));

	// スケーリング調整
	const float xScale_ = graphSize_.x / GetMaxMeasurementTime   ();
	const float yScale_ = graphSize_.y / (2.0f * GetClampMaxValue());

	// グリッドの描画
	// 横線("X"軸方向)
	for (int i_ = 1; i_ < DEBUG_MAX_GRID_NUM; ++i_)
	{
		float x_ = origin_.x + (graphSize_.x / static_cast<float>(DEBUG_MAX_GRID_NUM)) * i_;
		drawList_->AddLine(ImVec2(x_ , origin_.y) , ImVec2(x_ , origin_.y + graphSize_.y) , IM_COL32(50.0f , 50.0f , 50.0f , 100.0f));
	}

	// 横線("Y"軸方向)
	for (int j_ = 1; j_ < DEBUG_MAX_GRID_NUM; ++j_)
	{
		float y_ = origin_.y + (graphSize_.y / static_cast<float>(DEBUG_MAX_GRID_NUM)) * j_;
		drawList_->AddLine(ImVec2(origin_.x, y_), ImVec2(origin_.x + graphSize_.x, y_), IM_COL32(50.0f, 50.0f, 50.0f, 100.0f));
	}

	// 右、左クリックの入力がなければドラッグ対象要素のインデックスをクリア
	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right) && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		m_debugDraggingSelectedIndex = DEBUG_INVALID_INDEX;
	}

	// 点の描画とドラッグ操作処理
	for (std::size_t ki_ = 0; ki_ < m_keyFrameList.size(); ++ki_)
	{
		auto& k_ = m_keyFrameList[ki_];

		// グラフ上の座標に変換("Y"は上下反転)
		ImVec2 pos_ = { origin_.x + k_.time * xScale_ , origin_.y + graphSize_.y - (k_.value * yScale_ + graphSize_.y / 2.0f) };

		// ホバー時に色変更
		bool  isHovered_ = hovered_ && ImGui::IsMouseHoveringRect(pos_ - ImVec2(6.0f , 6.0f) , pos_ + ImVec2(6.0f , 6.0f));
		ImU32 color_     = isHovered_ ? IM_COL32(255.0f, 100.0f, 0.0f, 255.f) : IM_COL32(255.0f, 200.0f, 0.0f, 255.0f);

		// 点の描画
		drawList_->AddCircleFilled(pos_ , 5.0f , color_);

		// マウス当たり判定
		if (hovered_ && ImGui::IsMouseHoveringRect(pos_ - DEBUG_MOUSE_HIT_RANGE , pos_ + DEBUG_MOUSE_HIT_RANGE))
		{
			ImGui::SetTooltip("Key %d : Time = %.2f , Value = %.2f , Tangent = %.2f" , static_cast<int>(ki_) , k_.time , k_.value , k_.tangent);

			const bool clickLeft_  = ImGui::IsMouseClicked(ImGuiMouseButton_Left );
			const bool clickRight_ = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

			if (clickLeft_ || clickRight_)
			{
				m_debugDraggingSelectedIndex = static_cast<int>(ki_);
			}
		}

		// ドラッグ中の点の更新
		if (m_debugDraggingSelectedIndex == static_cast<int>(ki_) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 delta_ = ImGui::GetIO().MouseDelta;
			k_.time  +=  delta_.x / xScale_;
			k_.value += -delta_.y / yScale_;
			k_.time   = std::clamp(k_.time  , 0.0f , GetMaxMeasurementTime());

			// "Y"方向は(値)は[-maxValue , +maxValue]に制限
			const float clampMaxValue_ = GetClampMaxValue();	// "yScale_ = height_  / +-10"が丁度中央基準 
			k_.value = std::clamp(k_.value , -clampMaxValue_ , clampMaxValue_);
		}

		// 右クリックされたらポップアップを開く
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			// ポップアップを開く
			m_debugDeleteSelectedIndex = static_cast<int>(ki_);
			ImGui::OpenPopup("DeleteConfirm");
		}
	}

	// 範囲を超えないように配列にアクセス
	// ポップアップメニューでキーフレーム削除管理
	if (ImGui::BeginPopup("DeleteConfirm"))
	{
		ImGui::Text("Delete this KeyFrame?");
		if (ImGui::Button("Yes"))
		{
			// 配列の要素数を超えていないかを確認
			if (m_keyFrameList.size() > m_debugDeleteSelectedIndex && m_debugDeleteSelectedIndex != DEBUG_INVALID_INDEX)
			{
				m_keyFrameList.erase(m_keyFrameList.begin() + m_debugDeleteSelectedIndex);

				// もしキーフレームが一つだけならリストをクリア
				if (m_keyFrameList.size() < m_debugDeleteSelectedIndex)
				{	
					m_keyFrameList.clear();
				}

				m_debugDeleteSelectedIndex = DEBUG_INVALID_INDEX;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	
	// 現在値(再生中位置の可視化)
	float currentTime_ = GetElapsedTime();
	if (currentTime_ <= GetMaxMeasurementTime())
	{
		float xPos_ = origin_.x + currentTime_ * xScale_;
		drawList_->AddLine(ImVec2(xPos_ , origin_.y) , ImVec2(xPos_ , origin_.y + graphSize_.y) , IM_COL32(0.0f , 255.0f , 0.0f , 150.0f) , 2.0f);
	}

	const float drawStep_ = DEBUG_KEY_FRAME_DRAW_STEP;	// 時間の刻み(高解像度)

	// "Hermite"曲線描画(各キーフレーム間)
	for (std::size_t ki_ = 0; ki_ < m_keyFrameList.size() - 1; ++ki_)
	{
		const auto& k0_ = m_keyFrameList[ki_];
		const auto& k1_ = m_keyFrameList[ki_ + CommonConstant::INCREMENT_VECTOR_INDEX];

		for (float l_ = k0_.time; l_ < k1_.time; l_ += drawStep_)
		{
			float nextX_ = std::min(l_ + drawStep_, k1_.time);
			float t1_ = (l_ - k0_.time) / (k1_.time - k0_.time);
			float t2_ = (nextX_ - k0_.time) / (k1_.time - k0_.time);

			float v1_ = Calculation::Hermite(t1_, k0_.value, k1_.value, k0_.tangent, k1_.tangent);
			float v2_ = Calculation::Hermite(t2_, k0_.value, k1_.value, k0_.tangent, k1_.tangent);

			// 画面座標に変換 ("Y"は反転)
			ImVec2 p1_ = { origin_.x + l_     * xScale_ , origin_.y + graphSize_.y - (v1_ * yScale_ + graphSize_.y / 2.0f) };
			ImVec2 p2_ = { origin_.x + nextX_ * xScale_ , origin_.y + graphSize_.y - (v2_ * yScale_ + graphSize_.y / 2.0f) };

			// 線分を描画(青~シアン)
			drawList_->AddLine(p1_, p2_, IM_COL32(255.0f, 255.0f, 0.0f, 255.0f), 2.0f);
		}
	}

	// 最大計測時間よりも大きい値を取らないよう調整
	const float clampMaxValue_ = GetClampMaxValue();

	for(auto& keyFrame_ : m_keyFrameList)
	{
		keyFrame_.time  = std::clamp(keyFrame_.time  , 0.0f            , GetMaxMeasurementTime());
		keyFrame_.value = std::clamp(keyFrame_.value , -clampMaxValue_ , clampMaxValue_);
	}
}
void HermiteInterpolatorModifier::ImGuiSortList()
{
	// 最低でもキーフレームが"2"点ないとソートをしない
	// 時間軸でキーフレームをソート
	if (m_keyFrameList.size() < MIN_SIZE_TO_NEXT_LIST_ACCESS) { return; }

	std::sort(m_keyFrameList.begin(), m_keyFrameList.end(), [](const auto& Comp_1, const auto& Comp_2)
	{
		return Comp_1.time < Comp_2.time;
	});
}