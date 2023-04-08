//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------
// s3d::SceneManager is originally designed by Takuto Takahashi (OpenSiv3D Project)
//-----------------------------------------------

# pragma once

namespace My
{
	template <class Data> class SceneManager;

	/// @brief シーンのインタフェース
	/// @tparam Data シーン間で共有するデータの型
	template <class Data>
	class IScene : Uncopyable
	{
	public:

		using Data_t = Data;

		SIV3D_NODISCARD_CXX20
		IScene() = default;

		virtual ~IScene() = default;

		/// @brief フェードイン時の更新処理です。
		/// @param sceneManager シーン管理
		/// @param t フェードインの進度 [0.0, 1.0]
		virtual void updateFadeIn([[maybe_unused]] SceneManager<Data>& sceneManager, [[maybe_unused]] double t) {}

		/// @brief 通常時の更新処理です。
		/// @param sceneManager シーン管理
		virtual void update([[maybe_unused]] SceneManager<Data>& sceneManager) {}

		/// @brief フェードアウト時の更新処理です。
		/// @param sceneManager シーン管理
		/// @param t フェードアウトの進度 [0.0, 1.0]
		virtual void updateFadeOut([[maybe_unused]] SceneManager<Data>& sceneManager, [[maybe_unused]] double t) {}

		/// @brief 通常時の描画処理です。
		/// @param sceneManager シーン管理
		virtual void draw([[maybe_unused]] const SceneManager<Data>& sceneManager) const {}

		/// @brief フェードイン時の描画処理です。
		/// @param sceneManager シーン管理
		/// @param t フェードインの進度 [0.0, 1.0]
		virtual void drawFadeIn(const SceneManager<Data>& sceneManager, double t) const;

		/// @brief フェードアウト時の描画処理です。
		/// @param sceneManager シーン管理
		/// @param t フェードアウトの進度 [0.0, 1.0]
		virtual void drawFadeOut(const SceneManager<Data>& sceneManager, double t) const;
	};

	/// @brief シーン遷移管理
	/// @tparam Data シーン間で共有するデータの型
	template <class Data = void>
	class SceneManager
	{
	public:

		/// @brief シーンのインタフェース型
		using Scene = IScene<Data>;

		/// @brief シーン管理を初期化します。
		SIV3D_NODISCARD_CXX20
		SceneManager();

		SIV3D_NODISCARD_CXX20
		SceneManager(const SceneManager&) = delete;

		SIV3D_NODISCARD_CXX20
		SceneManager(SceneManager&& other) noexcept;

		/// @brief シーン管理を初期化します。
		/// @param data 共有データ
		SIV3D_NODISCARD_CXX20
		explicit SceneManager(const std::shared_ptr<Data>& data);

		SceneManager& operator =(const SceneManager&) = delete;

		SceneManager& operator =(SceneManager&& other) noexcept;

		/// @brief 最初のシーンを初期化します。
		/// @tparam SceneType 最初のシーンの型
		/// @return 初期化に成功した場合 true, それ以外の場合は false
		template <class SceneType>
		bool init();

		/// @brief 最初のシーンを初期化します。
		/// @tparam SceneType 最初のシーンの型
		/// @param firstScene 最初のシーン
		/// @return 初期化に成功した場合 true, それ以外の場合は false
		template <class SceneType>
		bool init(std::unique_ptr<SceneType>&& firstScene);

		/// @brief 現在のシーンの更新処理のみを行います。
		/// @remark 通常はこの関数は使用しません。
		/// @return シーンの更新処理に成功した場合 true, それ以外の場合は false
		bool updateScene();

		/// @brief 現在のシーンの描画処理のみを行います。
		/// @remark 通常はこの関数は使用しません。
		void drawScene() const;

		/// @brief 現在のシーンの更新処理と描画処理を行います。
		/// @return シーンの更新処理に成功した場合 true, それ以外の場合は false
		bool update();

		/// @brief 共有データを取得します。
		/// @return 共有データへの参照
		[[nodiscard]]
		Data& getData();

		/// @brief 共有データを取得します。
		/// @return 共有データへの参照
		[[nodiscard]]
		const Data& getData() const;

		/// @brief シーンを変更します。
		/// @tparam SceneType 次のシーンの型
		/// @param transitionTime フェードイン・アウトの時間
		/// @param crossFade ロスフェードを有効にするか
		/// @return シーンの変更が開始される場合 true, それ以外の場合は false
		template<class SceneType>
		bool changeScene(const Duration& transitionTime = Duration{ 2.0 }, const CrossFade crossFade = CrossFade::No);

		/// @brief シーンを変更します。
		/// @tparam SceneType 次のシーンの型
		/// @param nextScene 次のシーン
		/// @param transitionTime フェードイン・アウトの時間
		/// @param crossFade ロスフェードを有効にするか
		/// @return シーンの変更が開始される場合 true, それ以外の場合は false
		template <class SceneType>
		bool changeScene(std::unique_ptr<SceneType>&& nextScene, const Duration& transitionTime = Duration{ 2.0 }, CrossFade crossFade = CrossFade::No);

		/// @brief シーンを変更します。
		/// @tparam SceneType 次のシーンの型
		/// @param transitionTimeMillisec フェードイン・アウトの時間（ミリ秒）
		/// @param crossFade クロスフェードを有効にするか
		/// @return シーンの変更が開始される場合 true, それ以外の場合は false
		template<class SceneType>
		bool changeScene(int32 transitionTimeMillisec, CrossFade crossFade);

		/// @brief シーンを変更します。
		/// @tparam SceneType 次のシーンの型
		/// @param nextScene 次のシーン
		/// @param transitionTimeMillisec フェードイン・アウトの時間（ミリ秒）
		/// @param crossFade クロスフェードを有効にするか
		/// @return シーンの変更が開始される場合 true, それ以外の場合は false
		template <class SceneType>
		bool changeScene(std::unique_ptr<SceneType>&& nextScene, int32 transitionTimeMillisec, CrossFade crossFade = CrossFade::No);

		/// @brief デフォルトのフェードイン・アウトに使う色を設定します。
		/// @param color デフォルトのフェードイン・アウトに使う色
		/// @return *this
		SceneManager& setFadeColor(const ColorF& color) noexcept;

		/// @brief デフォルトのフェードイン・アウトに使う色を返します。
		/// @return デフォルトのフェードイン・アウトに使う色
		[[nodiscard]]
		const ColorF& getFadeColor() const noexcept;

		/// @brief エラーの発生を通知します。
		/// @return この関数を呼ぶと、以降のこのクラスの `SceneManager::update()` が false を返します。
		void notifyError() noexcept;

	private:

		using Scene_t = std::shared_ptr<IScene<Data>>;

		using FactoryFunction_t = std::function<Scene_t()>;

		std::shared_ptr<Data> m_data;

		Scene_t m_current;

		Scene_t m_next;

		enum class TransitionState
		{
			None_,

			FadeIn,

			Active,

			FadeOut,

			FadeInOut,

		} m_transitionState = TransitionState::None_;

		Stopwatch m_stopwatch;

		int32 m_transitionTimeMillisec = 1000;

		ColorF m_fadeColor = Palette::Black;

		CrossFade m_crossFade = CrossFade::No;

		bool m_error = false;

		[[nodiscard]]
		bool updateSingle();

		[[nodiscard]]
		bool updateCross();

		[[nodiscard]]
		bool hasError() const noexcept;
	};
}

# include "MySceneManager.ipp"
