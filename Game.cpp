#include "pch.h"
#include "Game.h"

#include "utils.h"
#include "GameOfLife.h"
#include "Time.h"
#include "Texture.h"

#include <iostream>
#include <cmath>
#include <print>


Game::Game(const Window& window)
	:BaseGame(window)
	,m_Window{ window }
	,m_RenderedFrames{0}
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	m_FPSCounter = std::make_unique<Texture>("placeHolder","Resources/Fonts/consola.ttf",24,Color4f{1,1,1,1});
	m_TargetFPS = 60.0f;
	m_Time = std::make_unique<Time>(0.0f);
	m_AccumulatedTime = std::make_unique<Time>(0.0f);
	m_GOL = std::make_unique<GameOfLife>(500);
	
}

void Game::Cleanup()
{
	
}

void Game::Update(float elapsedSec)
{
	// Check keyboard state
	using namespace std;

	int m_FPS = int((float)m_RenderedFrames / m_Time->GetTime());

	
	m_Time->Update(elapsedSec);
	m_AccumulatedTime->Update(elapsedSec);
	//m_GOL->Update(elapsedSec);

	const float frameDuration{1.0f / m_TargetFPS};

	if (m_AccumulatedTime->GetTime() >= frameDuration)
	{
		m_GOL->Update(elapsedSec);
		m_AccumulatedTime->Restart();
		m_FPSCounter = std::make_unique<Texture>("FPS: " + std::to_string(m_FPS),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1}); // Has to be better
	}


	m_RenderedFrames += 1;
}

void Game::Draw() const
{
	ClearBackground();
	glPushMatrix();
	{
		m_FPSCounter->Draw(Point2f{10.0f, GetViewPort().height-25.0f},Rectf{}); // Has to be better
		m_GOL->Draw();
	
	}
	glPopMatrix();


}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
	switch (e.keysym.sym)
	{
	case SDLK_p:
		m_GOL->Pause();
		break;
	case SDLK_o:
		m_GOL->Start();
		break;
	case SDLK_y:
		m_GOL->Restart();
		break;
	case SDLK_DOWN:
		m_TargetFPS -= 5.0f;
		if (m_TargetFPS <= 0.0f)
		{
			m_TargetFPS = 5.0f;
		}
		break;
	case SDLK_UP:
		m_TargetFPS += 5.0f;
		if (m_TargetFPS >= 120.0f)
		{
			m_TargetFPS = 120.0f;
		}
		break;


	case SDLK_ESCAPE:
		// Not sure about this, but works ¯\_(^^)_/¯
		SDL_Event e{SDL_QUIT};
		SDL_PushEvent(&e);
		break;
	}
	
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}

	
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	m_LastMousePos = Point2f{ float(e.x),float(e.y) };

	
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	m_GOL->OnMouseDown(e);
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		break;
	case SDL_BUTTON_RIGHT:
		break;
	case SDL_BUTTON_MIDDLE:
		break;
	}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
