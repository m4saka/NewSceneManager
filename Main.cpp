//-----------------------------------------------
// 
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D.hpp>
# include "MySceneManager.hpp"

struct GameData
{
	Font font = Font{ 50 };
	int32 score = 0;
};

using MyApp = My::SceneManager<GameData>;

class Game;
class Result;

class Title : public MyApp::Scene
{
public:
	Title()
	{
		Print << U"Title";
	}

	void update(MyApp& sceneManager) override
	{
		if (MouseL.down())
		{
			sceneManager.changeScene(std::make_unique<Game>(sceneManager), 2.0s);
		}
	}

	void draw(const MyApp& sceneManager) const override
	{
		sceneManager.getData().font(U"Title")
			.drawAt(Scene::Center());
	}
};

class Game : public MyApp::Scene
{
public:
	explicit Game(MyApp& sceneManager)
	{
		Print << U"Game";
		sceneManager.getData().score = 0;
	}

	void update(MyApp& sceneManager) override
	{
		if (MouseL.down())
		{
			sceneManager.changeScene<Result>(2.0s);
		}
		++sceneManager.getData().score;

		Stopwatch{StartImmediately::Yes}.start();
	}

	void draw(const MyApp& sceneManager) const override
	{
		const Font& font = sceneManager.getData().font;
		font(U"Game")
			.drawAt(Scene::Center());
		font(sceneManager.getData().score)
			.drawAt(Scene::Center().movedBy(0, 60));
	}
};

class Result : public MyApp::Scene
{
public:
	Result()
	{
		Print << U"Result";
	}

	void update(MyApp& sceneManager) override
	{
		if (MouseL.down())
		{
			sceneManager.changeScene<Title>(2.0s);
		}
	}

	void draw(const MyApp& sceneManager) const override
	{
		const Font& font = sceneManager.getData().font;
		font(U"Result")
			.drawAt(Scene::Center());
		font(sceneManager.getData().score)
			.drawAt(Scene::Center().movedBy(0, 60));
	}
};

void Main()
{
	Scene::SetBackground(ColorF{ 0.5, 0.6, 0.7 });

	MyApp manager;
	manager
		.setFadeColor(ColorF{ 1.0 })
		.init<Title>();

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
	}
}
