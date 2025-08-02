#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include "utils.h"

class Time;
class Texture;

class GameOfLife
{

public:
    GameOfLife(int cellsPerRow);
    GameOfLife(const GameOfLife& gol) = delete;
    GameOfLife& operator=(const GameOfLife& gol) = delete;
    GameOfLife(GameOfLife&& gol) = delete;
    GameOfLife& operator=(GameOfLife&& gol) = delete;

    ~GameOfLife();

    void Draw() const;
    void DrawCells() const;
    void DrawGrid() const;
    void DrawUI(Rectf viewport) const;

    void Update(float elapsedTime);
    void UpdateTextures();

    void Start();
    void Pause();
    void Restart();

    void ZoomIn(Point2f mousePos);
    void ZoomOut(Point2f mousePos);
    void ResetZoom();
    float GetScale() const;
    Point2f GetCenterOfGrid() const;

    void OnMouseUp(const SDL_MouseButtonEvent& e);
    void OnMouseDown(const SDL_MouseButtonEvent& e);  
    void OnMouseMotion(const SDL_MouseMotionEvent& e);


private:

    // Game basics:
    struct Cell
    {
        bool m_Alive;
        bool m_WillBeAlive;
    };

    std::vector<Cell> m_Cells;

    const int m_CellsPerRow;
    const float m_CellSize;
    Point2f m_StartingPoint;
    float m_LineWidth;
    float m_DrawOffset{};

    Point2f m_LastMousePos;

    // Nice counters:
    std::unique_ptr<Texture> m_PopulationTxt;
    std::unique_ptr<Texture> m_GenerationTxt;
    int m_Population;
    int m_Generation;

    // Zooming
    Point2f m_ZoomTarget;
    float m_ScaleFactor;

    // Time (Start, Pause, Restart...)
    std::unique_ptr<Time> m_Time;
    bool m_Paused;

    // Counter to make certain things happen less than 1 time per frame
    int m_FrameCounter{0}; 

    // Code for colors (Wasn't originally planned. It's a bit messy)
    const static int m_AmountOfBlocks{2500}; // (it's actually /10, so if 2500, the amount of blocks is 250)
    int m_BlockSize; 
    mutable bool m_BlockSpread[m_AmountOfBlocks];
    mutable int m_ColorStrength[m_AmountOfBlocks]; // Basically: Strength == chance of spreading
    mutable int m_BlockColorIndex[m_AmountOfBlocks];
    mutable float m_Red[m_AmountOfBlocks]{1.0f};
    mutable float m_Green[m_AmountOfBlocks]{1.0f};
    mutable float m_Blue[m_AmountOfBlocks]{1.0f};
        
            // BASE RULES:
    // Any live cell with fewer than two live neighbours dies,as if caused by underpopulation.
    // Any live cell with two or three live neighbours lives on to the next generation.
    // Any live cell with more than three live neighbours dies,as if by overpopulation.
    // Any dead cell with exactly three live neighbours becomes a live cell,as if by reproduction.
    
            // TODO:
    // 
    // 
    // 
    //
    // 
    // 

            // Lessons for the future:
    //      Too much code repetition (like into OnMouseDown and DrawCells)

    //      There are bad design desicions regarding drawing and translating of the grid. 
    // It should be drawn from 0.0f 0.0f and then translated to origin to scale and then translated back
    // Rn it is drawn from "-half grid size" so the center is on 0.0f 0.0f. It forces me to remember about it being different (than origin)
    // Made it easier to implement some features but overall, often just caused a lot of confusion
     
    //      Do member init-list in the right order from the beginning. It's annoying to fix it up later
    

    //      Making zoom mousePos dependent failure. 
    // Couldn't get it to work with actual math (most likely because of the grid weird drawing position)
    // 
    // (A hacky solution would be to reset m_ScaleFactor everytime m_ZoomTarget is changed)
    // then apply old m_ScaleFactor to new target + zoomIn/zoomOut
    // 
    // This would still require to calculate "Actual" mousePos when zoomed,using this:
    // X+s(P-X)+O Where O=P-[X+s(P-X)]   
    // (O = offset, s = m_ScaleFactor, X = m_ZoomTarget, P = original main point)
    // This formula gives original main point (P) location, after taking zoom into account
};

