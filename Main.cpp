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

using MyApp = My::SceneManager<String, GameData>;

class Game;
class Result;

class Title : public MyApp::Scene
{
public:
	explicit Title(const InitData& init)
		: IScene{ init }
	{
		Print << getState();
	}

	void update() override
	{
		if (MouseL.down())
		{
			changeScene(U"Game", 2.0s);
		}
	}

	void draw() const override
	{
		getData().font(U"Title")
			.drawAt(Scene::Center());
	}
};

class Game : public MyApp::Scene
{
public:
	explicit Game(const InitData& init)
		: IScene{ init }
	{
		Print << getState();
		getData().score = 0;
	}
	void update() override
	{
		if (MouseL.down())
		{
			changeScene(U"Result", 2.0s);
		}
		++getData().score;
	}
	void draw() const override
	{
		const Font& font = getData().font;
		font(U"Game")
			.drawAt(Scene::Center());
		font(getData().score)
			.drawAt(Scene::Center().movedBy(0, 60));
	}
};

class Result : public MyApp::Scene
{
public:
	explicit Result(const InitData& init)
		: IScene{ init }
	{
		Print << getState();
	}

	void update() override
	{
		if (MouseL.down())
		{
			changeScene(U"Title", 2.0s);
		}
	}

	void draw() const override
	{
		const Font& font = getData().font;
		font(U"Result")
			.drawAt(Scene::Center());
		font(getData().score)
			.drawAt(Scene::Center().movedBy(0, 60));
	}
};

void Main()
{
	Scene::SetBackground(ColorF{ 0.5, 0.6, 0.7 });

	MyApp manager;
	manager
		.add<Title>(U"Title")
		.add<Game>(U"Game")
		.add<Result>(U"Result")
		.setFadeColor(ColorF{ 1.0 });

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
	}
}
