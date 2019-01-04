#include <iostream>
#include <chrono>
#include <Windows.h>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV 3.14159 / 4.0;
float fDepth = 16;
int main()
{
    //Create Screen Buffer
    wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;
    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.....##.......#";
    map += L"#.....##.......#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"########.......#";
    map += L"#..............#";
    map += L"#.........######";
    map += L"#..............#";
    map += L"################";

    // Game Loop
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    while(1){

        //get Calculated Framerate;
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();
        //Controls
        //Handle CCW Rotation
        if(GetAsyncKeyState((unsigned short)'A') & 0x8000){
            fPlayer -= (0.8f) * fElapsedTime;
        }
        if(GetAsyncKeyState((unsigned short)'D') & 0x8000){
            fPlayer += (0.8f) * fElapsedTime;
        }
        if(GetAsyncKeyState((unsigned short)'W') & 0x8000){
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            if(map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == "#"){
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }
         if(GetAsyncKeyState((unsigned short)'S') & 0x8000){
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            if(map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == "#"){
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }




        for(int x = 0; x < nScreenWidth; x++){
            //For each column, calculate the projected ray angle into world space
            float fRayAngle = fPlayerA - fFOV / 2.0f) + ((float) x / (float)nScreenWidth) * fFOV);

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;
            fEyeX = sinf(fRayAngle); //Unit vector for ray in player space
            fEyeY = cosf(fRayAngle);

            while(!bHitWall && fDistanceToWall < fDepth){
                fDistanceToWall += 1.0f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                //Test if ray is out of bounds
                if(nTestX < 0 || nTestX > nMapWidth || nTestY < 0 || nTestY > nMapHeight){
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else {
                    //Ray is inbounds so test to see if the ray cell is a wall block
                    if(map[nTestY * nMapWidth + nTestX] == '#'){
                        bHitWall = true;
                        vector<pair<float,float>> p; //distance, dot

                        for(int tx = 0; tx < 2; tx++){
                            for(int ty = 0; ty< 2; ty++){
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vy*vy + vx*vx);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy/d);
                                p.push_back(make_pair(d,dot));

                            }
                        }
                        //Sort pairs from closest to farthest

                        sort(p.begin(), p.end(),
                        [](const pair<float, float> &left, const pair<float, float> &right) {
                            return left.first < right.first;
                        });

                        float fBound = 0.01;
                        if(acos(p.at(0).second) < fBound) bBoundary = true;
                        if(acos(p.at(1).second) < fBount) bBoundary = true;
                    }
                }
            }
            //Calculate distance to ceiling and floor
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;
            short nShade == ' ';
            if(fDistanceToWall <= fDepth /4.0f)         nShade = 0x2588;
            else if(fDistanceToWall <= fDepth /3.0f)    nShade = 0x2593;
            else if(fDistanceToWall <= fDepth /2.0f)    nShade = 0x2592;
            else if(fDistanceToWall <= fDepth)          nShade = 0x2591;
            else                                        nShade = ' ';
            if(bBoundary)   nSgade = 'I';


            for(int y = 0; y < nScreenHeight; y++){
                if(y < nCeiling)
                    screen[y*nScreenWidth + x] = ' ';
                else if(y > nCeiling && y <= nFloor)
                    screen[y*nScreenWidth + x] = nShade;
                else{
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if(b < .25)         nShade = '#';
                    else if(b < .5)     nShade = 'x';
                    else if(b < .75)    nShade = '.';
                    else if(b < .9)     nShade = '-';
                    else                nShade = ' ';

                    screen[y*nScreenWidth + x] = ' ';
                }
            }
        }
        //DisplayStats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS =%3.2f ",fPlayerX,fPlayerY,fPlayerA, 1.0f /fElapsedTime)
        //DisplayMap
        for( int nx = 0; nx < nMapWidth; nx++){
            for(int ny = 0; ny <nMapWidth; ny++){
                screen[(ny+1)*nScreenWidth + nx] = map[ny*nMapWidth+nx];
            }

        }
        screen[((int)fPlayerY + 1) *n ScreenWidth + (int)fPlayerX] = "P";
        screen[nScreenWidth * nScreenHeight - 1] = "\0";
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
    }

    return 0;
}
