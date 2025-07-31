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

    Point2f GetCenterOfGrid() const;
    void ZoomIn(Point2f mousePos);
    void ZoomOut(Point2f mousePos);
    void ResetZoom();
    float GetScale() const;

    void OnMouseUp(const SDL_MouseButtonEvent& e);
    void OnMouseDown(const SDL_MouseButtonEvent& e);  // Check for holding to allow "drawing" over the field
    void OnMouseMotion(const SDL_MouseMotionEvent& e);//


private:
    std::unique_ptr<Texture> m_PopulationTxt;
    std::unique_ptr<Texture> m_GenerationTxt;
    Uint64 m_Population;
    Uint64 m_Generation;

    Point2f m_LastMousePos;
    Point2f m_ZoomTarget;
    std::unique_ptr<Time> m_Time;
    bool m_Paused;


    const int m_CellsPerRow;
    const float m_CellSize;
    Point2f m_StartingPoint;
    float m_LineWidth;

    float m_ScaleFactor;
    float m_DrawOffset{};

  
    // A lot of code copied (like into OnMouseDown and DrawCells)

    // There are bad design desicions regarding drawing and translating of the grid. 
    // It should be drawn from 0.0f 0.0f and then translated to origin to scale and then translated back
    // Rn it is drawn from "-half grid size" so the center is on 0.0f 0.0f. Will make it work, but should be changed 


    // HOW TO FIX ZOOM: // Failed
    // Zoom detection not working comes last, once I have proper place zooming (It will depend on the solution)
    // Detection simply doesn't take into account the scale 550 scaled is 550 unscaled for detection (ss in dc)
    // FIRST: When zoom in then "U" (reset scale) it takes my mouse to a different place, that is the problem
    // Discord priv has ss of issue when zooming out
    // Zoom is based on screen (if you zoom in right up corner it zooms into right up corner of grid) (when not camera moved)

    struct Cell
    {
        bool m_Alive;
        bool m_WillBeAlive;
    };

    std::vector<Cell> m_Cells;
     
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
    // CLEAN UP CODE
    // Try bigger grid, maybe improve performance
    // 
    //
    // 
    // 


};

