#pragma once
#include "../Utility/Singleton/SingletonBase.h"

class BaseScene;
class SceneChanger;

class SceneManager : public SingletonBase<SceneManager>
{

public :

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
	};

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

	void EarlyUpdate();
	void Update     ();
	void LateUpdate () const;

	void PreDraw   ();
	void Draw      ();
	void DrawSprite();
	void DrawDebug ();

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	std::weak_ptr<BaseScene>    GetCurrentScene() { return m_currentScene;    }

	std::weak_ptr<SceneChanger> GetNowSceneChanger () { return m_nowSceneChanger;  }
	std::weak_ptr<SceneChanger> GetNextSceneChanger() { return m_nextSceneChanger; }

	void SetNowSceneChanger (std::shared_ptr<SceneChanger>& Set) { m_nowSceneChanger  = Set; }
	void SetNextSceneChanger(std::shared_ptr<SceneChanger>& Set) { m_nextSceneChanger = Set; }

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	SceneType GetCurrentSceneType() { return m_currentSceneType; }

	SceneType& GetWorkNextSceneType() { return m_nextSceneType; }

private:

	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	std::shared_ptr<SceneChanger> m_nowSceneChanger  = nullptr;
	std::shared_ptr<SceneChanger> m_nextSceneChanger = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Game;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	// ==============================
	// "Singleton"
	// ==============================
	friend class SingletonBase;

	SceneManager ()          = default;
	~SceneManager() override = default;
};
