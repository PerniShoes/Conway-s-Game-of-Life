#include <cmath>

#include "GameOfLife.h"
#include "Time.h"
#include <iostream>
#include <print>
#include "Texture.h"
#include "OrientationManager.h"

GameOfLife::GameOfLife(int cellsPerRow)
    :m_CellsPerRow{cellsPerRow}
    ,m_CellSize{10.0f}
    ,m_Time{std::make_unique<Time>(0.0f)}
    ,m_StartingPoint{0.0f,0.0f}
    ,m_LineWidth{1.0f}
    ,m_Paused{true}
    ,m_Generation{0}
    ,m_Population{0}
    ,m_ScaleFactor{1.0f}
    ,m_DrawOffset{(m_CellSize + m_LineWidth) * (m_CellsPerRow / 2)}
    ,m_ZoomTarget{0.0f,0.0f}
{
    m_PopulationTxt = std::make_unique<Texture>("Population: ","Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
    m_GenerationTxt = std::make_unique<Texture>("Generation: ","Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});

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

void GameOfLife::ZoomIn(Point2f mousePos)
{
    m_ScaleFactor += 0.1f;
    m_ZoomTarget = mousePos;

    std::cout << OrientationManager::GetWorldLocation(m_ZoomTarget).x << " <-X  Y->" << OrientationManager::GetWorldLocation(m_ZoomTarget).y << std::endl;
    std::cout << "Scale: " << m_ScaleFactor << std::endl;
}
void GameOfLife::ZoomOut(Point2f mousePos)
{
    m_ScaleFactor -= 0.1f;
    if (m_ScaleFactor < 0.1f)
    {
        m_ScaleFactor = 0.1f;
    }
    m_ZoomTarget = mousePos;

    std::cout << OrientationManager::GetWorldLocation(m_ZoomTarget).x << " <-X  Y->" << OrientationManager::GetWorldLocation(m_ZoomTarget).y << std::endl;
    std::cout << "Scale: " << m_ScaleFactor << std::endl;
}
void GameOfLife::ResetZoom()
{
    m_ScaleFactor = 1.0f;
}

void GameOfLife::Draw() const
{
    glPushMatrix();

    glTranslatef(m_StartingPoint.x + 550.0f
        ,m_StartingPoint.y + 550.0f
        ,0.0f);

    glScalef(m_ScaleFactor,m_ScaleFactor,0);




    // FAILED ATTEMPS at having zoom work at mouse pos.
    // Made it so it works if you start zooming at 0x zoom (m_ScaleFactor == 1.0f)
    // So if you restart zoom each time, it works. Cell detection would break (but that should be an easy fix)
    // There is a lot of confusion since I made some weird design choices. Decided to leave if be for now and work one something else
    // Right now zoom works but it zooms from the middle of the grid. In Game.cpp there are keybinds to center camera on it, so it's fine-ish
     
    // (550.0f is ((m_CellSize + m_LineWidth * m_CellsPerRow)/2)

   /* float offset = 550.0f - (m_ZoomTarget.x + m_ScaleFactor * (550.0f - m_ZoomTarget.x));
    float actualPosX {(m_ZoomTarget.x)- 550.0f};
    float actualPosY {(m_ZoomTarget.y) - 550.0f};

    glTranslatef((actualPosX),(actualPosY),0);
    glScalef(m_ScaleFactor,m_ScaleFactor,0);
    glTranslatef(-(actualPosX),-(actualPosY),0);

    glTranslatef(offset,0.0f,0.0f);*/

    DrawGrid();
    DrawCells();

    glPopMatrix();
}

void GameOfLife::DrawGrid() const
{
    using namespace utils;
    float widthOffset{m_LineWidth / 2.0f};
   
    SetColor(Color4f{1.0f,1.0f,1.0f,0.3f});
    // Draws vertical lines
    for (int i{0}; i <= m_CellsPerRow; i++)
    {
        DrawLine(Point2f{ widthOffset + ((m_CellSize + m_LineWidth) * i) - m_DrawOffset
            , - m_DrawOffset}
                ,Point2f{( widthOffset + (m_CellSize + m_LineWidth) * i) - m_DrawOffset
            , m_LineWidth + ((m_CellSize + m_LineWidth) * m_CellsPerRow) - m_DrawOffset}
        ,m_LineWidth);

    }

    SetColor(Color4f{1.0f,1.0f,1.0f,0.3f});
    // Draws horizontal lines
    for (int i{0}; i <= m_CellsPerRow; i++)
    {

        DrawLine(Point2f{- m_DrawOffset
            ,widthOffset + ((m_CellSize + m_LineWidth) * i) - m_DrawOffset}
                ,Point2f{(+ m_LineWidth + (m_CellSize + m_LineWidth) * m_CellsPerRow) - m_DrawOffset
            , widthOffset + ((m_CellSize + m_LineWidth) * i) - m_DrawOffset}
        ,m_LineWidth);


    }

}
void GameOfLife::DrawCells() const
{
    using namespace utils;
    using namespace std;
    float widthOffset{m_LineWidth / 2.0f};

    SetColor(Color4f{1.0f,1.0f,1.0f,1.0f});
    for (int i{0}; i < m_Cells.size(); i++)
    {
        int row = i / m_CellsPerRow;
        int col = i % m_CellsPerRow;

        if (m_Cells[i].m_Alive == true)
        {
            FillRect(m_LineWidth + col * (m_CellSize + m_LineWidth)- m_DrawOffset
                ,m_LineWidth + row * (m_CellSize + m_LineWidth) - m_DrawOffset
                ,m_CellSize,m_CellSize);
        }

    }

}
void GameOfLife::DrawUI(Rectf viewport) const
{
    m_PopulationTxt->Draw(Point2f{10.0f, viewport.height - m_PopulationTxt->GetHeight()*2 - 4.0f},Rectf{}); 
    m_GenerationTxt->Draw(Point2f{10.0f, viewport.height - m_GenerationTxt->GetHeight()*3 - 4.0f},Rectf{});
}

void GameOfLife::Update(float elapsedTime)
{

    using namespace std;
    m_Time->Update(elapsedTime);
    UpdateTextures();
    if (m_Paused == true)
        return;

    m_Population = 0;
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
            m_Cells[i].m_WillBeAlive = false; 
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
        m_Population += 1;
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

    // Changes cells' m_Alive bool
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

    m_Generation += 1;
}
void GameOfLife::UpdateTextures()
{
    m_PopulationTxt = std::make_unique<Texture>("Population: " + std::to_string(m_Population),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
    m_GenerationTxt = std::make_unique<Texture>("Generation: " + std::to_string(m_Generation),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
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
        m_Population = 0;
        m_Generation = 0;
    }
}

void GameOfLife::OnMouseDown(const SDL_MouseButtonEvent& e)
{
    using namespace utils;
    float widthOffset{m_LineWidth / 2.0f};
  
    m_LastMousePos = OrientationManager::GetWorldLocation(Point2f{float(e.x),float(e.y)});
    std::cout << "MousePos.x : " << m_LastMousePos.x  << std::endl;
    std::cout << "MousePos.y : " << m_LastMousePos.y << std::endl;
    if (m_Paused == true)
    {
        for (int i{0}; i < m_Cells.size(); i++)
        {
            int row = i / m_CellsPerRow;
            int col = i % m_CellsPerRow;

            float center = (float)m_CellsPerRow / 2.0f;

            float colOffset = center - col;
            float rowOffset = center - row;
            float yOffset{};
            float xOffset{};

            if (m_ScaleFactor == 1.0f)
            {
               xOffset = 0.0f;
               yOffset = 0.0f;
            }
            else if (m_ScaleFactor > 1.0f)
            {
                xOffset = -(colOffset * (m_LineWidth + m_CellSize) * (m_ScaleFactor - 1.0f));
                yOffset = -(rowOffset * (m_LineWidth + m_CellSize) * (m_ScaleFactor - 1.0f));
            }
            else
            {
                xOffset = (colOffset * (m_LineWidth + m_CellSize) * (1.0f - m_ScaleFactor));
                yOffset = (rowOffset * (m_LineWidth + m_CellSize) * (1.0f - m_ScaleFactor));
            }

            Rectf cellRect{};
   
            cellRect = Rectf{m_StartingPoint.x + xOffset
                + m_LineWidth + col * (m_CellSize + m_LineWidth)
                ,m_StartingPoint.y + yOffset
                + m_LineWidth + row * (m_CellSize + m_LineWidth)
                ,m_CellSize*m_ScaleFactor,m_CellSize* m_ScaleFactor};
       

            if (IsPointInRect(OrientationManager::GetWorldLocation(e),cellRect))
            {
                if (m_Cells[i].m_Alive == false)
                {
                    m_Cells[i].m_Alive = true;
                    m_Population += 1;
                }
                else
                {
                    m_Cells[i].m_Alive = false;
                    m_Population -= 1;
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

Point2f GameOfLife::GetCenterOfGrid() const
{
    return Point2f{(m_CellsPerRow / 2) * (m_CellSize + m_LineWidth)
    ,(m_CellsPerRow / 2)* (m_CellSize + m_LineWidth)};
}
float GameOfLife::GetScale() const
{
    return m_ScaleFactor;
}