#pragma once
#include "../../Scene/SceneManager.h"
#include "../../Scene/BaseScene/BaseScene.h"
#include "../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../Tag/TagRegistry.h"

#include "../../Factory/Strategy/StrategyFactory.h"
#include "../../Factory/BehaviorTree/Node/BTNodeFactory.h"
#include "../../Factory/BehaviorTree/BlackBoardData/BTBlackBoardDataFactory.h"
#include "../../Factory/Notify/Component/NotifyToComponentFactory.h"
#include "../../Factory/RenderShaderParam/RenderShaderParamFactory.h"

#include "../../GUID/GameObject/GUIDGameObjectManager.h"
#include "../../GUID/BehaviorTree/GUIDBehaviorTreeManager.h"

#include "../../Component/BehaviorTree/Node/BTNodeBase.h"
#include "../../Component/BehaviorTree/Node/Root/BTNodeRoot.h"

#include "../../Component/BehaviorTree/BlackBoard/Data/IBTBlackBoardData.h"

#include "../../main.h"

class InterpolatorModifierBase;
class NotifyToComponentBase;

namespace ImGuiUtility
{
	static constexpr int MAX_TEXT_BUFFER    = 256;
	static constexpr int SEGMENTS           = 32;

	static constexpr int NODE_OUTPUT_PIN_OFFSET = 1000000;
	static constexpr int NODE_INPUT_PIN_OFFSET  = 2000000;

	static constexpr float NODE_EDITOR_MINIMAP_SIZE = 0.3f;

	static constexpr ImVec2 FIRST_NODE_POSITION = { 50.0f , 50.0f };

	static constexpr ImColor ROOT_NODE_COLOR      = { 0.50f , 0.0f  , 0.50f , 1.0f };
	static constexpr ImColor COMPOSITE_NODE_COLOR = { 0.50f , 0.50f , 0.0f  , 1.0f };
	static constexpr ImColor BRANCH_NODE_COLOR    = { 0.36f , 0.10f , 0.96f , 1.0f };
	static constexpr ImColor TASK_NODE_COLOR      = { 0.0f  , 0.50f , 0.0f  , 1.0f };
	static constexpr ImColor DECORATOR_NODE_COLOR = { 0.98f , 0.53f , 0.52f , 1.0f };
	static constexpr ImColor CANDIDATE_NODE_COLOR = { 0.20f , 0.20f , 0.20f , 1.0f };
	
	// "ImGui"用のリストのため"static"変数を定義
	static const std::vector<CommonStruct::BitShiftList> SHADER_TYPE_LIST =
	{
		{ "Lit"                       , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::Lit)					      },
		{ "UnLit"                     , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::UnLit)                     },
		{ "Bright"                    , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::Bright)                    },
		{ "UI"                        , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::UI)					 	  },
		{ "Sprite"                    , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::Sprite)					  },
		{ "GenerateDepthFromMapLight" , static_cast<uint64_t>(CommonEnum::StandardShaderTypeFlags::GenerateDepthMapFromLight) },
	};

	static const std::vector<CommonStruct::BitShiftList> GAME_OBJECT_CATEGORY_TYPE_LIST =
	{
		{"Camera"     , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Camera)               } ,
																							            
		{"Player"     , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Player)               } ,
		{"Enemy"      , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Enemy)                } ,
		{"Boss"       , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Boss)                 } ,
																							            
		{"Terrain"    , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Terrain)              } ,
		{"BackGround" , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::BackGround)           } ,
		{"Decoration" , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Decoration)           } ,
																							            
		{"UI"         , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::UI)                   } ,
		{"Projectile" , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Projectile)           } ,

		{"EffekseerEffect" , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::EffekseerEffect) } ,

		{"Weapon" , static_cast<uint64_t>(CommonEnum::GameObjectCategoryType::Weapon) }
	};

	static const std::vector<CommonStruct::BitShiftList> COLLIDER_CATEGORY_TYPE_LIST =
	{
		{ "TypeGround"     , static_cast<uint64_t>(KdCollider::TypeGround)     } , 
		{ "TypeBump"       , static_cast<uint64_t>(KdCollider::TypeBump)       } ,
		{ "TypeDamage"     , static_cast<uint64_t>(KdCollider::TypeDamage)     } ,
		{ "TypeDamageLine" , static_cast<uint64_t>(KdCollider::TypeDamageLine) } ,
		{ "TypeSight"      , static_cast<uint64_t>(KdCollider::TypeSight)      } ,
		{ "TypeEvent"      , static_cast<uint64_t>(KdCollider::TypeEvent)      } ,
		{ "TypeDebug"      , static_cast<uint64_t>(KdCollider::TypeDebug)      } ,
	};

	static const std::vector<CommonStruct::BitShiftList> EFFEKSEER_EFFECT_DELETE_TYPE_LIST =
	{
		{ "DeleteSelf"  , static_cast<uint32_t>(CommonEnum::EffekseerEffectDeleteType::DeleteSelf)   } ,
		{ "DeleteParen" , static_cast<uint32_t>(CommonEnum::EffekseerEffectDeleteType::DeleteParent) }
	};

	static const std::vector<CommonStruct::BitShiftList> OFFSET_BY_FACING_FLAGS =
	{
		{"None"  , static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::None ) } ,
		{"AxisX" , static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::X)     } ,
		{"AxisY" , static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::Y)     } ,
		{"AxisZ" , static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::Z)     } ,
	};

	static const std::vector<CommonStruct::DirectionList> DIRECTION_LIST =
	{
		{ "Up"       , Math::Vector3::Up}       ,
		{ "Down"     , Math::Vector3::Down}     , 
		{ "Right"    , Math::Vector3::Right}    ,
		{ "Left"     , Math::Vector3::Left}     ,
		{ "Forward"  , Math::Vector3::Forward}  ,
		{ "Backward" , Math::Vector3::Backward} ,
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::BoneType>> BONE_NODE_TYPE_LIST =
	{
		{ "None"      , CommonEnum::BoneType::None      } ,
		{ "RightHand" , CommonEnum::BoneType::RightHand } ,
		{ "LeftHand"  , CommonEnum::BoneType::LeftHand  }
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::ParentType>> PARENT_TYPE_LIST =
	{
		{"None"       , CommonEnum::ParentType::None       } ,
		{"GameObject" , CommonEnum::ParentType::GameObject } ,
		{"Model"	  , CommonEnum::ParentType::Model      }
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::EventLane>> EVENT_LANE_TYPE_LIST =
	{
		{"Keep"   , CommonEnum::EventLane::Keep   } ,
		{"Moment" , CommonEnum::EventLane::Moment }
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::BoolFlag>> EVENT_NOTIFY_FLAG_LIST =
	{
		{"None"  , CommonEnum::BoolFlag::None  } ,
		{"True"  , CommonEnum::BoolFlag::True  } ,
		{"False" , CommonEnum::BoolFlag::False }
	};

	static const std::vector<CommonStruct::EnumList<SceneManager::SceneType>> SCENE_TYPE_LIST =
	{
		{ "Title" , SceneManager::SceneType::Title } ,
		{ "Game"  , SceneManager::SceneType::Game  }
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::RayCastMode>> RAY_CAST_MODE_LIST =
	{
		{ "Normal"   , CommonEnum::RayCastMode::Normal    } ,
		{ "ToParent" , CommonEnum::RayCastMode::ToParent  }
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::FadeState>> FADE_STATE_LIST =
	{
		{ "None"    , CommonEnum::FadeState::None    } ,
		{ "FadeIn"  , CommonEnum::FadeState::FadeIn  } ,
		{ "Fading"  , CommonEnum::FadeState::Fading  } ,
		{ "FadeOut" , CommonEnum::FadeState::FadeOut } ,
		{ "FadeEnd" , CommonEnum::FadeState::FadeEnd } ,
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::Priority>> PRIORITY_LIST =
	{
		{ "None"  , CommonEnum::Priority::None    } ,
		{ "One"   , CommonEnum::Priority::One     } ,
		{ "Two"   , CommonEnum::Priority::Two     } ,
		{ "Three" , CommonEnum::Priority::Three   } ,
		{ "Four"  , CommonEnum::Priority::Four    } ,
		{ "Five"  , CommonEnum::Priority::Five    } ,
		{ "Six"   , CommonEnum::Priority::Six     } ,
		{ "Seven" , CommonEnum::Priority::Seven   } ,
		{ "Eight" , CommonEnum::Priority::Eight   } ,
		{ "Nine"  , CommonEnum::Priority::Nine    } ,
		{ "Ten"   , CommonEnum::Priority::Ten     } ,
	};

	static const std::vector<CommonStruct::EnumList<BTNodeBase::NodeState>> NODE_STATE_LIST =
	{
		{ "Idle"    , BTNodeBase::NodeState::Idle    } ,
		{ "Running" , BTNodeBase::NodeState::Running } ,
		{ "Success" , BTNodeBase::NodeState::Success } ,
		{ "Fail"    , BTNodeBase::NodeState::Fail    } ,
	};

	static const std::vector<CommonStruct::EnumList<CommonEnum::BranchNodeType>> BRANCH_NODE_TYPE_LIST =
	{
		{ "FalseNode" , CommonEnum::BranchNodeType::False } ,
		{ "TrueNode"  , CommonEnum::BranchNodeType::True  } 
	};

	bool SelectFilePath(const char* Label , std::string& FolderPath);
	
	static void BindToGameObjectByGUID(const char* Label, uint64_t& GUID)
	{
		auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene_) { return; }

		auto GUIDManager_ = scene_->GetGUIDGameObjectManager().lock();
		if (!GUIDManager_) { return; }

		bool isThrough_ = false;

		ImGui::PushID(&GUID);

		const std::string text_ = std::to_string(GUID);

		if (ImGui::BeginCombo(Label, text_.c_str()))
		{
			for (const auto& list_ : GUIDManager_->GetUsedGUIDList())
			{
				bool isSelected_ = (GUID == list_);

				const std::string item_ = std::to_string(list_);

				if (ImGui::RadioButton(item_.c_str(), isSelected_))
				{
					GUID = list_;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	// 選択した"GUID"を取得
	template<class ComponentType>
		requires std::derived_from<ComponentType , ComponentBase>
	static void BindToGameObjectByGUID(const char* Label , uint64_t& GUID , std::weak_ptr<ComponentType>& BoundComponent)
	{
		auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene_) { return; }

		auto GUIDManager_ = scene_->GetGUIDGameObjectManager().lock();
		if (!GUIDManager_) { return; }

		bool isThrough_ = false;

		ImGui::PushID(&GUID);

		const std::string text_ = std::to_string(GUID);

		if (ImGui::BeginCombo(Label , text_.c_str()))
		{
			for (const auto& list_ : GUIDManager_->GetUsedGUIDList())
			{
				bool isSelected_ = (GUID == list_);

				const std::string item_ = std::to_string(list_);

				if(ImGui::RadioButton(item_.c_str(), isSelected_))
				{
					GUID = list_;
				}

				// 選択されていなければ指定のコンポーネントを格納しない
				if (!isSelected_) { continue; }

				for (const auto& gameObject_ : scene_->GetGameObjectList())
				{
					BoundComponent = gameObject_->GetComponent<ComponentType>();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	template <typename FlagType>
	static void BitShiftComboSelector(const char* Label , FlagType& Flag , const std::vector<CommonStruct::BitShiftList>& List)
	{
		static_assert(std::is_unsigned_v<FlagType>, "ImGuiUtility : フラグ変数は符号なし整数型である必要があります");

		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& item_ : List)
			{
				bool isSelected_ = (Flag & item_.type);

				if (ImGui::Checkbox(item_.label, &isSelected_))
				{
					if (isSelected_)
					{
						Flag |= static_cast<FlagType>(item_.type);
					}
					else
					{
						Flag &= static_cast<FlagType>(~item_.type);
					}
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	template <typename FlagType>
	static void BitShiftRadioButtonSelector(const char* Label , FlagType& Flag , const std::vector<CommonStruct::BitShiftList>& List)
	{
		static_assert(std::is_unsigned_v<FlagType>, "ImGuiUtility : フラグ変数は符号なし整数型である必要があります");

		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& item_ : List)
			{
				bool isSelected_ = Flag == static_cast<FlagType>(item_.type);

				if (ImGui::RadioButton(item_.label, isSelected_))
				{
					Flag = static_cast<FlagType>(item_.type);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	template <typename EnumType>
	static void EnumRadioButtonSelector(const char* Label , EnumType& WantChangeValue , const std::vector<CommonStruct::EnumList<EnumType>>& List)
	{
		static_assert(std::is_enum_v<EnumType>, "ImGuiUtility : EnumリストがEnum型ではありません");

		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& item_ : List)
			{
				bool isSelected_ = WantChangeValue == item_.type;

				if (ImGui::RadioButton(item_.label , isSelected_))
				{
					WantChangeValue = item_.type;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	static void TagIDRadioButtonSelector(const char* Label, uint32_t& WantChangeValue)
	{
		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_ , value_] : TagRegistry<ComponentEventTagBase>::GetInstance().GetTagIDList())
			{
				bool isSelected_ = WantChangeValue == value_;

				if (ImGui::RadioButton(key_.c_str() , isSelected_))
				{
					WantChangeValue = value_;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	static void TagIDRadioButtonSelector(const char* Label, std::string& WantChangeValue)
	{
		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_, value_] : TagRegistry<ComponentEventTagBase>::GetInstance().GetTagIDList())
			{
				bool isSelected_ = WantChangeValue == key_;

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					WantChangeValue = key_;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	void DirectionComboSelector(const char* Label , Math::Vector3& Direction);

	static void BTNodeSelector(const char* Label, std::shared_ptr<BTNodeBase>& WantAttach , std::weak_ptr<BTBlackBoard> BlackBoardCache , std::weak_ptr<BTNodeRoot> NodeRootCache)
	{
		auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene_) { return; }

		auto guidManager_ = scene_->GetGUIDBehaviorTreeManager().lock();
		if (!guidManager_) { return; }

		ImGui::PushID(&WantAttach);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_, value_] : BTNodeFactory::GetInstance().GetFactoryMethodList())
			{
				bool isSelected_ = false;

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					auto created_ = value_();
					if (!created_) { continue; }
					
					created_->Init              ();
					created_->SetNodeName       (key_);
					created_->SetBlackBoardCache(BlackBoardCache);
					created_->SetNodeRootCache  (NodeRootCache);

					// もしインスタンス化されていなれれば
					// "GUID"を発行して"AllNodeList"に格納
					if (!WantAttach)
					{
						const uint64_t id_ = guidManager_->GenerateGUID();
						created_->SetGUID(id_);

						KdDebugGUI::Instance().AddLog("In ImGuiUtility::BTNodeSelector : GeneratedID : %llu\n", id_);
					}

					WantAttach = created_;
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	template <typename MidBaseNodeType>
		requires std::derived_from<MidBaseNodeType , BTNodeBase>
	static void BTNodeSelectorConditional(const char* Label, std::shared_ptr<BTNodeBase>& WantAttach , std::weak_ptr<BTBlackBoard> BlackBoardCache , std::weak_ptr<BTNodeRoot> NodeRootCache)
	{
		auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene_) { return; }

		auto guidManager_ = scene_->GetGUIDBehaviorTreeManager().lock();
		if (!guidManager_) { return; }

		const auto& btFactory_ = BTNodeFactory::GetInstance();

		// 基底クラスの型に対する"ID"を取得
		const uint32_t           baseTypeID_ = StaticID<BTNodeBase>::GetTypeID<MidBaseNodeType>();
		std::vector<std::string> vector_;

		// 派生クラスの文字列を配列に格納
		btFactory_.GetTypeToNameVector(baseTypeID_ , vector_);

		// 空だったら即時"return"
		if (vector_.empty()) { return; }

		ImGui::PushID(&WantAttach);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& typeToName_ : vector_)
			{
				bool isSelected_ = false;

				// インスタンス化されているなら文字列比較を行って選ばれてるかを確認
				if (WantAttach)
				{
					isSelected_ = WantAttach->GetNodeName().data() == typeToName_;
				}

				// ラジオボタンがクリックされたらインスタンスを生成
				if (ImGui::RadioButton(typeToName_.c_str(), isSelected_))
				{
					auto created_ = btFactory_.Create(typeToName_);
					if (!created_) { continue; }

					created_->Init();
					created_->SetNodeName(typeToName_);
					created_->SetBlackBoardCache(BlackBoardCache);
					created_->SetNodeRootCache(NodeRootCache);

					// もしインスタンス化されていなれれば
					// "GUID"を発行
					if (!WantAttach)
					{
						const uint64_t id_ = guidManager_->GenerateGUID();
						created_->SetGUID(id_);

						KdDebugGUI::Instance().AddLog("In ImGuiUtility::BTNodeSelector : GeneratedID : %llu\n", id_);
					}
					// ただノードの種類を変更するなら"GUID"は使いまわす
					else
					{
						created_->SetGUID(WantAttach->GetGUID());
					}

					WantAttach = created_;
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			
			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	void DrawSeparate();

	template <typename FlagType>
	static const char* ConvertEnumListToString(const FlagType& Flag, const std::vector<CommonStruct::BitShiftList>& List)
	{
		static_assert(std::is_unsigned_v<FlagType>, "ImGuiUtility : フラグ変数は符号なし整数型である必要があります");

		for (const auto& list_ : List)
		{
			if (Flag == static_cast<FlagType>(list_.type))
			{
				return list_.label;
			}
		}

		return CommonConstant::STRING_UNKNOWN;
	}

	template <class StrategyType>
	static void StrategySelector(const char* Label , std::string& StrategyName , std::shared_ptr<IStrategy<StrategyType>>& Strategy)
	{
		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label , StrategyName.c_str()))
		{
			for (const auto& [key_, value_] : StrategyFactory<StrategyType>::GetInstance().GetFactoryMethodList())
			{
				bool isSelected_ = (key_ == StrategyName);

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					StrategyName = key_;
					Strategy = StrategyFactory<StrategyType>::GetInstance().Create(StrategyName);
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	template <class StrategyType>
	static void StrategySelector(const char* Label, std::string& StrategyName)
	{
		ImGui::PushID(Label);

		if (ImGui::BeginCombo(Label, StrategyName.c_str()))
		{
			for (const auto& [key_, value_] : StrategyFactory<StrategyType>::GetInstance().GetStrategyFactoryList())
			{
				bool isSelected_ = (key_ == StrategyName);

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					StrategyName = key_;
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	static void BTBlackBoardDataSelector(const char* Label , std::string& ClassName , std::shared_ptr<IBTBlackBoardData>& WantAttach)
	{
		ImGui::PushID(&WantAttach);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_, value_] : BTBlackBoardDataFactory::GetInstance().GetFactoryMethodList())
			{
				bool isSelected_ = false;

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					auto created_ = value_();

					// インスタンスがしっかり生成されたら名前とインスタンスを引数に格納
					if (created_)
					{
						WantAttach = created_;
						ClassName  = key_;
					}
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	static bool NotifyToComponentTypeSelector(const char* Label, std::string& ClassName)
	{
		ImGui::PushID(&ClassName);

		bool isThrow_ = false;

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_, value_] : NotifyToComponentFactory::GetInstance().GetFactoryMethodList())
			{
				bool isSelected_ = (key_ == ClassName);

				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					isThrow_  = true;
					ClassName = key_;
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();

		return isThrow_;
	}

	static void RenderShaderSelector(const char* Label, CommonStruct::GenericClassData<RenderShaderParamBase>& WantAttach)
	{
		ImGui::PushID(&WantAttach);

		if (ImGui::BeginCombo(Label, Label))
		{
			for (const auto& [key_, value_] : RenderShaderParamFactory::GetInstance().GetFactoryMethodList())
			{
				bool isSelected_ = false;

				isSelected_ = WantAttach.name == key_;
				
				if (ImGui::RadioButton(key_.c_str(), isSelected_))
				{
					WantAttach.name      = key_;
					WantAttach.classData = value_();

					// しっかりインスタンス化出来れば初期化
					if (WantAttach.classData)
					{
						WantAttach.classData->Init();
					}
				}

				if (isSelected_)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
	}

	void InterpolatorModifierSelector(const char* Label, CommonStruct::GenericClassData<InterpolatorModifierBase>& Interpolator);
	
	const char* BoolToString(bool Flag);
	const char* BoolToString(CommonEnum::BoolFlag Flag);

	void LoadPrefabData(const std::string& PrefabName, std::shared_ptr<GameObject> GameObject);

	// ノード関係
	static void DrawNodeTitleBar(std::string_view NodeName)
	{
		ImNodes::BeginNodeTitleBar();
		ImGui::Text(NodeName.data());
		ImNodes::EndNodeTitleBar();
	}

	static void DrawNodeInputAttribute(uint64_t GUID)
	{
		const int inPutPinID_ = static_cast<int>(GUID) + ImGuiUtility::NODE_INPUT_PIN_OFFSET;

		// 出力ピンの描画
		ImNodes::BeginInputAttribute(inPutPinID_);
		ImGui::Text("In");
		ImNodes::EndInputAttribute();
	}

	static void DrawNodeOutputAttribute(uint64_t GUID)
	{
		const int outPutPinID_ = static_cast<int>(GUID) + ImGuiUtility::NODE_OUTPUT_PIN_OFFSET;

		// 出力ピンの描画
		ImNodes::BeginOutputAttribute(outPutPinID_);
		ImGui::Text("Out");
		ImNodes::EndOutputAttribute();
	}

	static void DrawLink(const uint64_t ParentGUID , const uint64_t ChildGUID)
	{
		const uint64_t childGUID_       = ChildGUID;
		const int	   childInputPin_   = static_cast<int>(childGUID_) + ImGuiUtility::NODE_INPUT_PIN_OFFSET;
		const int	   parentOutputPin_ = static_cast<int>(ParentGUID) + ImGuiUtility::NODE_OUTPUT_PIN_OFFSET;

		ImNodes::Link(static_cast<int>(childGUID_) , parentOutputPin_ , childInputPin_);
	}

	static bool IsLinkMatched(uint64_t ParentGUID , uint64_t ChildGUID , int LinkStartAttribute ,int LinkEndAttribute)
	{
		const int parentOutputPinID_ = static_cast<int>(ParentGUID)	+ ImGuiUtility::NODE_OUTPUT_PIN_OFFSET;
		const int childInputPinID_   = static_cast<int>(ChildGUID)  + ImGuiUtility::NODE_INPUT_PIN_OFFSET;
		
		return (parentOutputPinID_ == LinkStartAttribute && childInputPinID_ == LinkEndAttribute);
	}

	static std::shared_ptr<BTNodeBase> ImGuiReceiveLinkedNode(uint64_t ParentGUID , std::weak_ptr<BTNodeRoot> Node)
	{
		auto rootNode_ = Node.lock();
		if (!rootNode_) 
		{
			return nullptr;
		}

		int startAttributeID_ = 0;
		int endAttributeID_   = 0;
		if (ImNodes::IsLinkCreated(&startAttributeID_, &endAttributeID_))
		{
			// ルートノードが生成したノードを子ノードとして取り込む
			for (auto& candidate_ : rootNode_->GetWorkCandidateNodeList())
			{
				if (!candidate_.node && candidate_.isLinked) { continue; }

				bool matched_ = ImGuiUtility::IsLinkMatched(ParentGUID , candidate_.node->GetGUID() , startAttributeID_ , endAttributeID_);
								
				if (matched_)
				{
					candidate_.isLinked = true;
					return candidate_.node;
				}
			}
		}

		return nullptr;
	}
}