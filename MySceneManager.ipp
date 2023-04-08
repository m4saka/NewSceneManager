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

# pragma once

namespace My
{
	template <class Data>
	inline void IScene<Data>::drawFadeIn(const SceneManager<Data>& sceneManager, double t) const
	{
		draw(sceneManager);

		Transformer2D transform{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };

		Scene::Rect().draw(ColorF{ sceneManager.getFadeColor(), (1.0 - t) });
	}

	template <class Data>
	inline void IScene<Data>::drawFadeOut(const SceneManager<Data>& sceneManager, const double t) const
	{
		draw(sceneManager);

		Transformer2D transform{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };

		Scene::Rect().draw(ColorF{ sceneManager.getFadeColor(), t });
	}

	template <class Data>
	inline SceneManager<Data>::SceneManager()
	{
		if constexpr (not std::is_void_v<Data>)
		{
			m_data = std::make_shared<Data>();
		}
	}

	template <class Data>
	inline SceneManager<Data>::SceneManager(SceneManager&& other) noexcept
		: m_data{ std::move(other.m_data) }
		, m_current{ std::move(other.m_current) }
		, m_next{ std::move(other.m_next) }
		, m_transitionState{ std::move(other.m_transitionState) }
		, m_stopwatch{ std::move(other.m_stopwatch) }
		, m_transitionTimeMillisec{ std::move(other.m_transitionTimeMillisec) }
		, m_fadeColor{ std::move(other.m_fadeColor) }
		, m_crossFade{ std::move(other.m_crossFade) }
		, m_error{ std::move(other.m_error) }
	{
	}

	template <class Data>
	inline SceneManager<Data>::SceneManager(const std::shared_ptr<Data>& data)
		: m_data{ data } {}

	template <class Data>
	inline SceneManager<Data>& SceneManager<Data>::operator =(SceneManager&& other) noexcept
	{
		m_data = std::move(other.m_data);
		m_current = std::move(other.m_current);
		m_next = std::move(other.m_next);
		m_transitionState = std::move(other.m_transitionState);
		m_stopwatch = std::move(other.m_stopwatch);
		m_transitionTimeMillisec = std::move(other.m_transitionTimeMillisec);
		m_fadeColor = std::move(other.m_fadeColor);
		m_crossFade = std::move(other.m_crossFade);
		m_error = std::move(other.m_error);
		return *this;
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::init()
	{
		return init(std::make_unique<SceneType>());
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::init(std::unique_ptr<SceneType>&& firstScene)
	{
		if (m_current)
		{
			return false;
		}

		m_current = std::move(firstScene);

		if (hasError())
		{
			return false;
		}

		m_transitionState = TransitionState::FadeIn;

		m_stopwatch.restart();

		return true;
	}

	template <class Data>
	inline bool SceneManager<Data>::updateScene()
	{
		if (hasError())
		{
			return false;
		}

		if (not m_current)
		{
			return false;
		}

		if (m_crossFade)
		{
			return updateCross();
		}
		else
		{
			return updateSingle();
		}
	}

	template <class Data>
	inline void SceneManager<Data>::drawScene() const
	{
		if (not m_current)
		{
			return;
		}

		if ((m_transitionState == TransitionState::Active)
			|| (m_transitionTimeMillisec <= 0))
		{
			m_current->draw(*this);
		}

		const double elapsed = m_stopwatch.msF();
		const double t = (m_transitionTimeMillisec ? (elapsed / m_transitionTimeMillisec) : 1.0);

		if (m_transitionState == TransitionState::FadeIn)
		{
			m_current->drawFadeIn(*this, t);
		}
		else if (m_transitionState == TransitionState::FadeOut)
		{
			m_current->drawFadeOut(*this, t);
		}
		else if (m_transitionState == TransitionState::FadeInOut)
		{
			m_current->drawFadeOut(*this, t);

			if (m_next)
			{
				m_next->drawFadeIn(*this, t);
			}
		}
	}

	template <class Data>
	inline bool SceneManager<Data>::update()
	{
		if (not updateScene())
		{
			return false;
		}

		drawScene();

		return true;
	}

	template <class Data>
	inline Data& SceneManager<Data>::getData()
	{
		return *m_data;
	}

	template <class Data>
	inline const Data& SceneManager<Data>::getData() const
	{
		return *m_data;
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::changeScene(const Duration& transitionTime, const CrossFade crossFade)
	{
		return changeScene(std::make_unique<SceneType>(), transitionTime, crossFade);
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::changeScene(std::unique_ptr<SceneType>&& nextScene, const Duration& transitionTime, const CrossFade crossFade)
	{
		return changeScene(std::move(nextScene), static_cast<int32>(transitionTime.count() * 1000), crossFade);
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::changeScene(const int32 transitionTimeMillisec, CrossFade crossFade)
	{
		return changeScene(std::make_unique<SceneType>(), transitionTimeMillisec, crossFade);
	}

	template <class Data>
	template <class SceneType>
	inline bool SceneManager<Data>::changeScene(std::unique_ptr<SceneType>&& nextScene, const int32 transitionTimeMillisec, CrossFade crossFade)
	{
		m_crossFade = crossFade;

		if (crossFade)
		{
			m_transitionTimeMillisec = transitionTimeMillisec;

			m_transitionState = TransitionState::FadeInOut;
		}
		else
		{
			m_transitionTimeMillisec = (transitionTimeMillisec / 2);

			m_transitionState = TransitionState::FadeOut;
		}

		m_next = std::move(nextScene);

		if (hasError())
		{
			return false;
		}

		m_stopwatch.restart();

		return true;
	}

	template <class Data>
	inline SceneManager<Data>& SceneManager<Data>::setFadeColor(const ColorF& color) noexcept
	{
		m_fadeColor = color;

		return *this;
	}

	template <class Data>
	inline const ColorF& SceneManager<Data>::getFadeColor() const noexcept
	{
		return m_fadeColor;
	}

	template <class Data>
	inline void SceneManager<Data>::notifyError() noexcept
	{
		m_error = true;
	}

	template <class Data>
	inline bool SceneManager<Data>::updateSingle()
	{
		double elapsed = m_stopwatch.msF();

		if ((m_transitionState == TransitionState::FadeOut)
			&& (m_transitionTimeMillisec <= elapsed))
		{
			m_current = std::exchange(m_next, nullptr);

			if (hasError())
			{
				return false;
			}

			m_transitionState = TransitionState::FadeIn;

			m_stopwatch.restart();

			elapsed = 0.0;
		}

		if ((m_transitionState == TransitionState::FadeIn)
			&& (m_transitionTimeMillisec <= elapsed))
		{
			m_stopwatch.reset();

			m_transitionState = TransitionState::Active;
		}

		const double t = (m_transitionTimeMillisec ? (elapsed / m_transitionTimeMillisec) : 1.0);

		switch (m_transitionState)
		{
		case TransitionState::FadeIn:
			m_current->updateFadeIn(*this, t);
			break;
		case TransitionState::Active:
			m_current->update(*this);
			break;
		case TransitionState::FadeOut:
			m_current->updateFadeOut(*this, t);
			break;
		default:
			return false;
		}

		return (not hasError());
	}

	template <class Data>
	inline bool SceneManager<Data>::updateCross()
	{
		const double elapsed = m_stopwatch.msF();

		if ((m_transitionState == TransitionState::FadeInOut)
			&& (m_transitionTimeMillisec <= elapsed))
		{
			m_current = std::exchange(m_next, nullptr);

			m_stopwatch.reset();

			m_transitionState = TransitionState::Active;
		}

		if (m_transitionState == TransitionState::Active)
		{
			m_current->update(*this);
		}
		else
		{
			assert(m_transitionTimeMillisec);

			const double t = (m_transitionTimeMillisec ? (elapsed / m_transitionTimeMillisec) : 1.0);

			m_current->updateFadeOut(*this, t);

			if (hasError())
			{
				return false;
			}

			m_next->updateFadeIn(*this, t);
		}

		return (not hasError());
	}

	template <class Data>
	inline bool SceneManager<Data>::hasError() const noexcept
	{
		return m_error;
	}
}
