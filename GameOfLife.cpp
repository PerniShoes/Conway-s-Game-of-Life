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

    // SDL/OpenGl line width limit?
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

    // White starting Cell collor 
    //for (int i{0}; i < m_AmountOfBlocks; i++)
    //{
    //    m_Red[i] = 1.0f;
    //    m_Green[i] = 1.0f;
    //    m_Blue[i] = 1.0f;
    //    m_BlockColorIndex[i] = i;
    //    m_ColorStrength[i] = 1;
    //    m_BlockSpread[i] = false;
    // 
    //}
    
    // Random starting Cell collor 
    for (int i{0}; i < m_AmountOfBlocks; i++)
    {
        m_Red[i] = float((rand() % 101 + 1)) / 100.0f;
        m_Green[i] = float((rand() % 101 + 1)) / 100.0f;
        m_Blue[i] = float((rand() % 101 + 1)) / 100.0f;
        m_BlockColorIndex[i] = i;
        m_ColorStrength[i] = 1;
        m_BlockSpread[i] = false;
    }
    m_BlockSize = (pow(m_CellsPerRow,2) / m_AmountOfBlocks)/10;
}

GameOfLife::~GameOfLife()
{
    m_Cells.clear();
    
}

void GameOfLife::ZoomIn(Point2f mousePos)
{
    m_ScaleFactor += 0.1f;
    m_ZoomTarget = mousePos;
}
void GameOfLife::ZoomOut(Point2f mousePos)
{
    m_ScaleFactor -= 0.1f;
    if (m_ScaleFactor < 0.1f)
    {
        m_ScaleFactor = 0.1f;
    }
    m_ZoomTarget = mousePos;
  
}
void GameOfLife::ResetZoom()
{
    m_ScaleFactor = 1.0f;
}

void GameOfLife::Draw() const
{
    glPushMatrix();

    // The grid is drawn starting from the negative. So the center of it is the origin point (0.0, 0.0)
    glTranslatef(m_StartingPoint.x + m_DrawOffset
        ,m_StartingPoint.y + m_DrawOffset
        ,0.0f);

    glScalef(m_ScaleFactor,m_ScaleFactor,0);

    //DrawGrid(); // Grid makes it easier to see what's going on, but it's uglier 
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
    int blocksPerRow = m_CellsPerRow / m_BlockSize;

    int indexCounter{0};
    for (int i{0}; i < m_Cells.size(); i++)
    {
        int row = i / m_CellsPerRow;
        int col = i % m_CellsPerRow;

        // 
        int blockRow = row / m_BlockSize;
        int blockCol = col / m_BlockSize;
        indexCounter = blockRow * blocksPerRow + blockCol;

        if (indexCounter > m_AmountOfBlocks-1)
        {
            indexCounter = m_AmountOfBlocks - 1;
        }

        // Color spreading, not needed for sim to work (You can change the starting colors in the constructor)
        // Code for colors is probably the most rushed and messy part (it was an afterthought)
        // Just make the if() case never happen to turn it off
        if (m_Paused == false)
        {
            int spreadChance{10000};
            int spreadRandom{(rand() % spreadChance)- m_ColorStrength[indexCounter]};
            if (spreadRandom <= 0)
            {
                if (m_ColorStrength[indexCounter] < spreadChance/10)
                {
                    m_ColorStrength[indexCounter] *= 4;
                }

                int side = rand() % 4; // from 0 to 3 
                switch (side)
                {
                case 0:
                    if (blockRow < blocksPerRow-1)
                    {
                        m_ColorStrength[indexCounter] *= 2;
                        if (m_BlockSpread[indexCounter + blocksPerRow])
                            break;
                        m_BlockColorIndex[indexCounter] = m_BlockColorIndex[indexCounter + blocksPerRow]; // Up
                        m_BlockSpread[indexCounter + blocksPerRow] = true;
                    }
                    break;
                case 1:
                    if (blockRow > 0)
                    {
                        if (m_BlockSpread[indexCounter - blocksPerRow])
                            break;
                        m_BlockColorIndex[indexCounter] = m_BlockColorIndex[indexCounter - blocksPerRow]; // Down
                        m_BlockSpread[indexCounter - blocksPerRow] = true;
                    }
                    break;
                case 2:
                    if (blockCol < blocksPerRow-10)
                    {
                        if (m_BlockSpread[indexCounter + 1])
                            break;
                        m_BlockColorIndex[indexCounter] = m_BlockColorIndex[indexCounter + 1]; // Right
                        m_BlockSpread[indexCounter + 1] = true;
                    }
                    break;
                case 3:
                    if (blockCol > 0)
                    {
                        if (m_BlockSpread[indexCounter - 1])
                            break;
                        m_BlockColorIndex[indexCounter] = m_BlockColorIndex[indexCounter - 1]; // Left
                        m_BlockSpread[indexCounter - 1] = true;
                    }
                    break;
                }
            }
        }

        
        if (m_Cells[i].m_Alive == true)
        {
            SetColor(Color4f{m_Red[m_BlockColorIndex[indexCounter]]
                ,m_Green[m_BlockColorIndex[indexCounter]]
                ,m_Blue[m_BlockColorIndex[indexCounter]]
                ,1.0f});
            FillRect(m_LineWidth + col * (m_CellSize + m_LineWidth) - m_DrawOffset
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

    m_FrameCounter += 1;
    if (m_FrameCounter > 5) // 10 == change color every 10 frames drawn
    {
        m_FrameCounter = 0;
        for (int i{0}; i < m_AmountOfBlocks; i++)
        {
            m_BlockSpread[i] = false;
        }
    }

    // Some cool settings: 6 2 3 3   5 1 3 3  (Can't find more right now)
    float overPopulation { 6 };  // Amount of neighbors too big to survive  (Base: 4)
    float underPopulation{ 2 };  // Amount of neighbors not enough to survive (Base: 1)

    // Reproduction range:
    float minReproduction{ 3 }; // Amount of neighbors big enough to ressurect dead cell (Base: 3)
    float maxReproduction{ 3 }; // Amount of neighbors small enough to ressurect dead cell (Base: 3)

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
            if(neighborsCounter <= maxReproduction && neighborsCounter >= minReproduction)
            {
                m_Cells[i].m_WillBeAlive = true;
            }
            continue;
        }

        // Alive cells
        if (neighborsCounter <= underPopulation)
        {
            m_Cells[i].m_WillBeAlive = false;
            continue;
        }

        if (neighborsCounter >= overPopulation)
        {
            m_Cells[i].m_WillBeAlive = false;
            continue;
        }

        if (neighborsCounter < overPopulation && neighborsCounter > underPopulation)
        {
            m_Cells[i].m_WillBeAlive = true;
            continue;
        }
    }

    m_Population = 0;
    // Changes cells' m_Alive bool
    for (int i{0}; i < m_Cells.size(); i++)
    {
        if (m_Cells[i].m_Alive)
        {
            m_Population += 1;
        }
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

Point2f GameOfLife::GetCenterOfGrid() const
{
    return Point2f{(m_CellsPerRow / 2) * (m_CellSize + m_LineWidth)
    ,(m_CellsPerRow / 2)* (m_CellSize + m_LineWidth)};
}
float GameOfLife::GetScale() const
{
    return m_ScaleFactor;
}