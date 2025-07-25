#include <cmath>

#include "GameOfLife.h"
#include "Time.h"
#include <iostream>
#include <print>

GameOfLife::GameOfLife(int cellsPerRow)
    :m_CellsPerRow{cellsPerRow}
    ,m_CellSize{5.0f}
    ,m_Time{std::make_unique<Time>(0.0f)}
    ,m_StartingPoint{20.0f,20.0f}
    ,m_LineWidth{1.0f}
    ,m_Paused{true}
{

    if (m_LineWidth > 10.0f)
    {
        m_LineWidth = 10.0f;
    }

    // Reserve space for vector? Should apparently first check if it's actually better performence wise, but with such big size it should be
    m_Cells.reserve(Uint64(pow(m_CellsPerRow,2)));
    for (int i{0}; i < pow(m_CellsPerRow,2); i++)
    {
        m_Cells.push_back(Cell{false, false});
    }
}

GameOfLife::~GameOfLife()
{


}

void GameOfLife::Draw() const
{
    DrawGrid();
    DrawCells();
}

void GameOfLife::DrawGrid() const
{
    using namespace utils;
    float widthOffset{m_LineWidth / 2.0f};
   
    SetColor(Color4f{1.0f,1.0f,1.0f,0.3f});
    // Draws vertical lines
    for (int i{0}; i <= m_CellsPerRow; i++)
    {
        
        DrawLine(Point2f{m_StartingPoint.x + widthOffset + ((m_CellSize + m_LineWidth) * i), m_StartingPoint.y}
            ,Point2f{(m_StartingPoint.x + widthOffset + (m_CellSize + m_LineWidth) * i) , m_StartingPoint.y + m_LineWidth + ((m_CellSize + m_LineWidth) * m_CellsPerRow)}
        ,m_LineWidth);

    }

    // SetColor(Color4f{0.8f,0.0f,0.7f,1.0f}); DEBUG
    // Draws horizontal lines
    for (int i{0}; i <= m_CellsPerRow; i++)
    {

        DrawLine(Point2f{m_StartingPoint.x,m_StartingPoint.y + widthOffset + ((m_CellSize + m_LineWidth) * i)}
            ,Point2f{(m_StartingPoint.x + m_LineWidth + (m_CellSize + m_LineWidth) * m_CellsPerRow),m_StartingPoint.y + widthOffset + ((m_CellSize + m_LineWidth) * i)}
        ,m_LineWidth);


    }

}
void GameOfLife::DrawCells() const
{
    using namespace utils;
    using namespace std;
    float widthOffset{m_LineWidth / 2.0f};
    // Check for column to be able to draw
    SetColor(Color4f{1.0f,1.0f,1.0f,1.0f});


    for (int i{0}; i < m_Cells.size(); i++)
    {
        int row = i / m_CellsPerRow;
        int col = i % m_CellsPerRow;

        if (m_Cells[i].m_Alive == true)
        {
            FillRect(m_StartingPoint.x + m_LineWidth + col * (m_CellSize + m_LineWidth)
                ,m_StartingPoint.y + m_LineWidth + row * (m_CellSize + m_LineWidth)
                ,m_CellSize,m_CellSize);
        }

    }
}

void GameOfLife::Update(float elapsedTime)
{
    using namespace std;
    m_Time->Update(elapsedTime);
    
    if (m_Paused == true)
        return;
    // Updates cells' m_WillBeAlive status (Also sets edge cells as dead)
    for (int i{0}; i < m_Cells.size(); i++)
    {
        int neighborsCounter{0};

        // Checks if a cell is on the edge
        if (i < m_CellsPerRow ||
            i > m_Cells.size()-m_CellsPerRow-1 ||
            i % m_CellsPerRow == m_CellsPerRow-1 ||
            i% m_CellsPerRow == 0 )
        {
            m_Cells[i].m_Alive = false;
            m_Cells[i].m_WillBeAlive = false; // Not yet sure if needed
            continue;
        }

        // Make neighbor offsets into a struct or array or smth and then compare it like that using a loop
        
        if (m_Cells[i - 1].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i + 1].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i - m_CellsPerRow + 1].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i - m_CellsPerRow - 1].m_Alive)
        {
            neighborsCounter += 1;
        }

        if (m_Cells[i + m_CellsPerRow].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i - m_CellsPerRow].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i + m_CellsPerRow + 1].m_Alive)
        {
            neighborsCounter += 1;
        }
        if (m_Cells[i + m_CellsPerRow - 1].m_Alive)
        {
            neighborsCounter += 1;
        }


        // Dead cells
        if (m_Cells[i].m_Alive == false)
        {
            if(neighborsCounter == 3)
            {
                m_Cells[i].m_WillBeAlive = true;
            }
            continue;
        }

        // Alive cells

        if (neighborsCounter < 2)
        {
            m_Cells[i].m_WillBeAlive = false;
            continue;
        }

        if (neighborsCounter > 3)
        {
            m_Cells[i].m_WillBeAlive = false;
            continue;
        }

        if (neighborsCounter == 2 || neighborsCounter == 3)
        {
            m_Cells[i].m_WillBeAlive = true;
            continue;
        }
    }

    for (int i{0}; i < m_Cells.size(); i++)
    {
        if (m_Cells[i].m_WillBeAlive == false)
        {
            m_Cells[i].m_Alive = false;
        }
        else
        {
            m_Cells[i].m_Alive = true;
        }


    }

}

void GameOfLife::Start()
{
    m_Paused = false;
}

void GameOfLife::Pause()
{
    m_Paused = true;
}

void GameOfLife::Restart()
{
    // Pause and set all cells to default
    m_Paused = true;
    for (int i{0}; i < m_Cells.size(); i++)
    {
        m_Cells[i].m_Alive = false;
        m_Cells[i].m_WillBeAlive = false;
    }
}

void GameOfLife::OnMouseDown(const SDL_MouseButtonEvent& e)
{
    using namespace utils;
    float widthOffset{m_LineWidth / 2.0f};
    
    if (m_Paused == true)
    {
        for (int i{0}; i < m_Cells.size(); i++)
        {
            int row = i / m_CellsPerRow;
            int col = i % m_CellsPerRow;
            Rectf cellRect{m_StartingPoint.x + m_LineWidth + col * (m_CellSize + m_LineWidth)
                ,m_StartingPoint.y + m_LineWidth + row * (m_CellSize + m_LineWidth)
                ,m_CellSize,m_CellSize};
        
            if (IsPointInRect(Point2f{float(e.x),float(e.y)},cellRect))
            {
                if (m_Cells[i].m_Alive == false)
                {
                    m_Cells[i].m_Alive = true;
                }
                else
                {
                    m_Cells[i].m_Alive = false;
                }
            }
        }
    }
}

void GameOfLife::OnMouseUp(const SDL_MouseButtonEvent& e)
{

}

void GameOfLife::OnMouseMotion(const SDL_MouseMotionEvent& e)
{

}