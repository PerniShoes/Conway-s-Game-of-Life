#pragma once
#include "BaseGame.h"
#include <vector>
#include <memory>


class Time;
class GameOfLife;
class Texture;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:
	const Window m_Window;

	Point2f m_LastMousePos;
	std::unique_ptr<Time> m_Time;
	std::unique_ptr<Time> m_AccumulatedTime;
	std::unique_ptr<GameOfLife> m_GOL;

	int m_RenderedFrames;
	std::unique_ptr<Texture> m_FPSCounter;
	float m_TargetFPS;


	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};