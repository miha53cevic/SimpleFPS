#include "mihaSimpleSFML.h"

#define _USE_MATH_DEFINES
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

private:
    void DrawPixel(int x, int y, sf::Color c)
    {
        sf::RectangleShape pixel;
        pixel.setPosition(x, y);
        pixel.setSize({ m_fPixelSizeX, m_fPixelSizeY });
        pixel.setFillColor(c);

        Draw(pixel);
    }

protected:
    void Event(sf::Event e) override
    {
    }

protected:
    bool OnUserCreate() override
    {
        // Create map where # represents a wall and . represends free space
        m_map += "################";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#######........#";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#..........#####";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#....#.........#";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "#..............#";
        m_map += "################";

        printf("Map Size: %d\n", m_map.size());

        // Map size
        m_nMapWidth     = 16;
        m_nMapHeight    = 16;

        // User window size
        m_nScreenWidth  = 120;
        m_nScreenHeight = 40;

        // Player position and player looking angle
        m_fPlayerX      = 8.0f;
        m_fPlayerY      = 8.0f;
        m_fPlayerAngle  = 0.0f;

        // Field of view = 90 degrees
        m_fFOV          = M_PI / 2.0f;

        // Max distance to wall
        m_fDepth        = 16.0f;

        // set Pixel Size
        m_fPixelSizeX = (float)ScreenWidth() / (float)m_nScreenWidth;
        m_fPixelSizeY = (float)ScreenHeight() / (float)m_nScreenHeight;

        // Player move and rotation speed
        m_fSpeed = 5.0f;

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
                }
                else if (y > nCeiling && y <= nFloor)
                {
                    // Draw wall
                    DrawPixel(x * m_fPixelSizeX, y * m_fPixelSizeY, sf::Color(fShade, fShade, fShade));
                }
                else
                {
                    // Draw floor
                    DrawPixel(x * m_fPixelSizeX, y * m_fPixelSizeY, sf::Color(51, 51, 51));
                }
            }
        }

        // Draw info
        #ifdef _WIN32
                system("cls");
        #endif

        #ifdef __linux__
                system("clear");
        #endif

        printf("X = %3.2f, Y = %3.2f\n", m_fPlayerX, m_fPlayerY);

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