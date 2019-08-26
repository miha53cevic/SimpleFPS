#include "mihaSimpleSFML.h"

#include <math.h>
#include <stdio.h>

class SimpleFPS : public mihaSimpleSFML
{
public:
    SimpleFPS() {}

private:
    std::string m_map;
    int m_nMapWidth;
    int m_nMapHeight;

    int m_nScreenWidth;
    int m_nScreenHeight;

    float m_fPlayerX;
    float m_fPlayerY;
    float m_fPlayerAngle;

    float m_fFOV;

    float m_fDepth;

    float m_fPixelSizeX;
    float m_fPixelSizeY;

    float m_fSpeed;

    sf::VertexArray m_quads;

private:
    void DrawPixel(int index, sf::Color c)
    {
        // Each quad is has 4 vertices so we have to move the index with * 4 to get the
        // first vertex of a quad, then we go through all 4 of the vertices and change their
        // colour to the one requested
        for (int i = 0; i < 4; i++)
        {
            m_quads[index * 4 + i].color = c;
        }
    }

protected:
    void Event(sf::Event e) override
    {
    }

protected:
    bool OnUserCreate() override
    {
        // Show FPS Counter
        EnableFPSCounter(true);

        // Create map where # represents a wall and . represends free space
        m_map += "################################";
        m_map += "#..............#...............#";
        m_map += "#..............#...............#";
        m_map += "#######........#...####........#";
        m_map += "#..............#...............#";
        m_map += "#..............................#";
        m_map += "#..............................#";
        m_map += "#..............................#";
        m_map += "#..........#####...........#####";
        m_map += "#..............#...............#";
        m_map += "#..............#...............#";
        m_map += "#....#.........#...............#";
        m_map += "#..............#...............#";
        m_map += "#..............................#";
        m_map += "#..............................#";
        m_map += "################################";

        printf("Map Size: %d\n", m_map.size());

        // Map size
        m_nMapWidth     = 32;
        m_nMapHeight    = 32;

        // User window size
        m_nScreenWidth  = 320;
        m_nScreenHeight = 240;

        // Player position and player looking angle
        m_fPlayerX      = 8.0f;
        m_fPlayerY      = 8.0f;
        m_fPlayerAngle  = 0.0f;

        // Field of view = 90 degrees
        m_fFOV          = PI / 2.0f;

        // Max distance to wall
        m_fDepth        = 32.0f;

        // set Pixel Size
        m_fPixelSizeX = (float)ScreenWidth() / (float)m_nScreenWidth;
        m_fPixelSizeY = (float)ScreenHeight() / (float)m_nScreenHeight;

        // Player move and rotation speed
        m_fSpeed = 5.0f;

        // Set primitive type
        m_quads.setPrimitiveType(sf::Quads);

        // Setup QUADS location and size
        for (int y = 0; y < m_nScreenHeight; y++)
        {
            for (int x = 0; x < m_nScreenWidth; x++)
            {
                // Upscaled coordinates
                float sx = x * m_fPixelSizeX;
                float sy = y * m_fPixelSizeY;

                // Append vertices in a clock-wise or counter-clock-wise order
                m_quads.append(sf::Vertex(sf::Vector2f(sx, sy), sf::Color::Black));
                m_quads.append(sf::Vertex(sf::Vector2f(sx + m_fPixelSizeX, sy), sf::Color::Black));
                m_quads.append(sf::Vertex(sf::Vector2f(sx + m_fPixelSizeX, sy + m_fPixelSizeY), sf::Color::Black));
                m_quads.append(sf::Vertex(sf::Vector2f(sx, sy + m_fPixelSizeY), sf::Color::Black));
            }
        }

        return true;
    }

    bool OnUserUpdate(sf::Time elapsed) override
    {
        // Keyboard input - ROTATIONS and MOVEMENT
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            m_fPlayerAngle -= (m_fSpeed * 0.4f) * elapsed.asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            m_fPlayerAngle += (m_fSpeed * 0.4f) * elapsed.asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            // Move in unit vector direction
            m_fPlayerX += cosf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
            m_fPlayerY += sinf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();

            // Collision with wall
            if (m_map[(int)m_fPlayerY * m_nMapWidth + (int)m_fPlayerX] == '#')
            {
                m_fPlayerX -= cosf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
                m_fPlayerY -= sinf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            // Move in unit vector direction
            m_fPlayerX -= cosf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
            m_fPlayerY -= sinf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();

            // Collision with wall
            if (m_map[(int)m_fPlayerY * m_nMapWidth + (int)m_fPlayerX] == '#')
            {
                m_fPlayerX += cosf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
                m_fPlayerY += sinf(m_fPlayerAngle) * m_fSpeed * elapsed.asSeconds();
            }
        }

        for (int x = 0; x < m_nScreenWidth; x++)
        {
            // For each column calculate projected ray angle
            float fRayAngle = (m_fPlayerAngle - m_fFOV / 2) + (((float)x / (float)m_nScreenWidth) * m_fFOV);

            // Find distance to wall
            float fStepSize = 0.1f;
            float fDistanceToWall = 0.0f;

            bool bHitWall = false;

            // Unit vector used for stepping
            float fEyeX = cosf(fRayAngle);
            float fEyeY = sinf(fRayAngle);

            while (!bHitWall && fDistanceToWall < m_fDepth)
            {
                /*
                // ---------------------------
                // | Objasnjenje ovoga ispod |
                // ---------------------------
                // cx + cosf(angle) * Radius
                // cy + sinf(angle) * Radius
                */
                fDistanceToWall += fStepSize;
                int nTestX = (int)(m_fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(m_fPlayerY + fEyeY * fDistanceToWall);

                // Test if out of bounds
                if (nTestX < 0 || nTestX >= m_nMapWidth || nTestY < 0 || nTestY >= m_nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = m_fDepth; // Set distance to wall to max distance
                }
                else
                {
                    // Ray is in bound check if the ray hit a wall
                    if (m_map[(nTestY * m_nMapWidth) + nTestX] == '#')
                    {
                        // Ray has hit a wall
                        bHitWall = true;
                    }
                }
            }

            // Calculate distance to ceiling and floor
            int nCeiling = (float)(m_nScreenHeight / 2.0f) - m_nScreenHeight / ((float)fDistanceToWall);
            int nFloor = m_nScreenHeight - nCeiling;

            // Calculate shading value
            float fShade;
            if (fDistanceToWall <= m_fDepth / 4.0f)     fShade = 255;
            else if (fDistanceToWall < m_fDepth / 3.0f) fShade = 200;
            else if (fDistanceToWall < m_fDepth / 2.0f) fShade = 150;
            else if (fDistanceToWall < m_fDepth)        fShade = 100;

            // Do draw routines
            for (int y = 0; y < m_nScreenHeight; y++)
            {
                if (y <= nCeiling)
                {
                    // Draw ceiling
                    DrawPixel((y * m_nScreenWidth) + x, sf::Color(21, 234, 237));
                }
                else if (y > nCeiling && y <= nFloor)
                {
                    // Draw wall
                    DrawPixel((y * m_nScreenWidth) + x, sf::Color(fShade, fShade, fShade));
                }
                else
                {
                    // Draw floor
                    DrawPixel((y * m_nScreenWidth) + x, sf::Color(19, 148, 39));
                }
            }
        }

        // Draw VertexArray of QUADS
        // Calling draw only once per frame gives a huge boost in FPS
        // instead of doing a ton of draw calls with sf::RectangleShape
        Draw(m_quads);

        return true;
    }

};

int main()
{
    SimpleFPS app;
    app.Construct(1280, 720, L"SimpleFPS");
    app.Start();

    return 0;
}