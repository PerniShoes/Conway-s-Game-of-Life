#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include "utils.h"

class Time;

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
    void Update(float elapsedTime);

    void Start();
    void Pause();
    void Restart();

    void OnMouseUp(const SDL_MouseButtonEvent& e);
    void OnMouseDown(const SDL_MouseButtonEvent& e);  // Check for holding to allow "drawing" over the field
    void OnMouseMotion(const SDL_MouseMotionEvent& e);//


private:

    std::unique_ptr<Time> m_Time;
    bool m_Paused;

    const int m_CellsPerRow;
    const float m_CellSize;
    Point2f m_StartingPoint;
    float m_LineWidth;


    // Need to store Rectf's of cells probably, to check for mouse overlap
    // A lot of code copied (like into OnMouseDown and DrawCells)


    struct Cell
    {
        bool m_Alive;
        bool m_WillBeAlive;
    };

    std::vector<Cell> m_Cells;

    // Prolly need flags like: Survives, dies/readyToDelete, spawns
    // Also obviously: Dead/Alive
     
    // RULES:
    // Any live cell with fewer than two live neighbours dies,as if caused by underpopulation.
    // Any live cell with two or three live neighbours lives on to the next generation.
    // Any live cell with more than three live neighbours dies,as if by overpopulation.
    // Any dead cell with exactly three live neighbours becomes a live cell,as if by reproduction.

    // https://www.youtube.com/watch?v=wbPgoZ2d0Nw potenatial help
     

    // Main ideas:
    // Loop through the vector with all the cells, check if Cell is m_Alive and if it m_WillBeAlive
    // After going thourgh the entire vector, loop through it again to change their m_Alive status accordingly
    // Loop third time and draw all m_Alive Cell's
    // 
    // There is for sure a way to make it loop less/"work" less, but first make it work, later optimize 
    // Could for example have a vector of arrays and Update only after "activating" and Draw only the grid when unactive


    // TODO:
    // Zooming in and out
    // Panning the camera
    //
    // Generation counter   // Storing bigger numbers, with Uint64 prolly
    // Population counter   //  


};

