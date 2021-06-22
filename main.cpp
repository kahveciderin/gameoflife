#include <stdlib.h>

#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <ctype.h>

using namespace std;

#define GRIDSIZE 512
#define SCALE 1

#define RENDER 8
//#define RGB
int valueinarray(uint8_t val, uint8_t arr[])
{
    int a;
    for (a = 0; a < 11; a++)
    {
        if (arr[a] == val)
            return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    srand(time(NULL));
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(GRIDSIZE * SCALE, GRIDSIZE * SCALE, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    uint8_t survive[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    uint8_t born[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    class line
    {

    public:
        uint32_t chunks[GRIDSIZE / 32];
        void init()
        {
            for (int i = 0; i < (GRIDSIZE / 32) + 1; i++)
            {
                chunks[i] = 0x0;
            }
        }

        bool getbit(uint16_t bit)
        {
            if (bit > GRIDSIZE + 1)
                return 0;
            return (chunks[bit >> 5] & (1 << (bit & 0x1F)));
        }

        void togglebit(uint16_t bit)
        {
            if (bit > GRIDSIZE + 1)
                return;
            chunks[bit >> 5] ^= 1 << (bit & 0x1F);
        }

        void setbit(uint16_t bit, bool value)
        {
            if (getbit(bit) != value)
            {
                togglebit(bit);
            }
        }
    };

    SDL_RenderSetScale(renderer,
                       SCALE,
                       SCALE);

    line lines[GRIDSIZE];

    for (int i = 0; i < GRIDSIZE; i++)
    {
        lines[i].init();
    }

    ifstream file(argv[1]);
    string linefi;
    int lineno = 0;
    string data;
    int xval, yval;

    bool randomData = false;
    while (getline(file, linefi))
    {
        if(linefi.at(0) == '#' && linefi.at(1) == '!'){
            born[0] = 3;
            survive[0] = 2;
            survive[1] = 3;
            randomData = true;
        }
        if (linefi.at(0) != '#')
        {
            if (lineno == 0)
            {
                int i = 4;
                char buff[5] = {0, 0, 0, 0, 0};
                for (int j = i; isdigit(linefi.at(i)); i++)
                {
                    buff[i - j] = linefi.at(i);
                }

                sscanf(buff, "%d", &xval);

                i += 6;

                char bufe[5] = {0, 0, 0, 0, 0};
                for (int j = i; isdigit(linefi.at(i)); i++)
                {
                    bufe[i - j] = linefi.at(i);
                }
                sscanf(bufe, "%d", &yval);
                printf("x = %d, y = %d\n\n", xval, yval);

                if (linefi.at(i) == ',')
                {
                    i += 10;

                    for (int j = i; isdigit(linefi.at(i)); i++)
                    {
                        born[i - j] = linefi.at(i) - '0';
                    }
                    i += 2;

                    for (int j = i; isdigit(linefi.at(i)); i++)
                    {
                        survive[i - j] = linefi.at(i) - '0';
                    }
                }
                else
                {
                    born[0] = 3;
                    survive[0] = 2;
                    survive[1] = 3;
                }

                printf("born: %d %d %d %d %d %d %d %d %d %d survive: %d %d %d %d %d %d %d %d %d %d", born[0], born[1], born[2], born[3], born[4], born[5], born[6], born[7], born[8], born[9], survive[0], survive[1], survive[2], survive[3], survive[4], survive[5], survive[6], survive[7], survive[8], survive[9]);
            }
            else
            {
                data = data + linefi;
            }

            lineno++;
        }
    }

    // printf("%s", data.c_str());
    int tmpx = (GRIDSIZE - xval) / 2;
    int tmpy = (GRIDSIZE - yval) / 2;

    if(randomData){
        printf("fill with random data\n");
        for(int y_gb = 0; y_gb < GRIDSIZE; y_gb++){
            for(int x_gb = 0; x_gb < GRIDSIZE; x_gb++){
                lines[y_gb].setbit(x_gb, (0 + (rand() % (1 - 0 + 1))));
            }
        }
    }else{
    for (int i = 0; i < data.length(); i++)
    {
        if (data.at(i) == '$' || data.at(i) == '!')
        {
            tmpy++;
            tmpx = (GRIDSIZE - xval) / 2;
            //printf("\n");
        }
        else if (isdigit(data.at(i)))
        {
            char buff[5] = {0, 0, 0, 0, 0};
            for (int j = i; isdigit(data.at(i)); i++)
            {
                buff[i - j] = data.at(i);
            }
            int num;
            sscanf(buff, "%d", &num);

            //i++;
            if (data.at(i) == 'o')
            {
                for (int j = 0; j < num; j++)
                {
                    lines[tmpy].setbit(tmpx, 1);
                    tmpx++;
                    //printf("o");
                }
            }
            else
            {
                for (int j = 0; j < num; j++)
                {
                    lines[tmpy].setbit(tmpx, 0);
                    tmpx++;
                    //printf(".");
                }
            }
            //printf("%d\n", num);
        }
        else if (data.at(i) == 'o')
        {
            lines[tmpy].setbit(tmpx, 1);
            tmpx++;
            //printf("o");
        }
        else
        {
            tmpx++;
            //printf(".");
        }
    }
    }

    bool run = 1;
    SDL_Event evt;
    uint64_t step;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    uint8_t sneigh[10];
    uint8_t bneigh[10];

    for (int i = 0; i < 10 + 1; i++)
    {

        sneigh[i] = survive[i];
    }

    for (int i = 0; i < 10 + 1; i++)
    {

        bneigh[i] = born[i];
    }

    while (run)
    {

        bool render = step % RENDER == 0;

        line oldlines[GRIDSIZE];

        // printf("%d", bneigh[0]);
        for (int i = 0; i < GRIDSIZE; i++)
        {

            oldlines[i] = lines[i];
        }

        
        if (render)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }
        step++;

        for (int i = 0; i < GRIDSIZE - 1; i++)
        {
            for (int j = 0; j < GRIDSIZE - 1; j++)
            {
                uint8_t neighbours = 0;

                int a = i - 1;
                int b = i + 1;

                int c = j - 1;
                int d = j + 1;

                if (a < 0)
                    a = GRIDSIZE - 2;

                if (b > (GRIDSIZE - 2))
                    b = 0;

                if (c < 0)
                    c = GRIDSIZE - 2;

                if (d > (GRIDSIZE - 2))
                    d = 0;

                if (oldlines[a].getbit(c))
                    neighbours++;
                if (oldlines[a].getbit(j))
                    neighbours++;
                if (oldlines[a].getbit(d))
                    neighbours++;
                if (oldlines[i].getbit(c))
                    neighbours++;
                if (oldlines[i].getbit(d))
                    neighbours++;
                if (oldlines[b].getbit(c))
                    neighbours++;
                if (oldlines[b].getbit(j))
                    neighbours++;
                if (oldlines[b].getbit(d))
                    neighbours++;

                // if (neighbours < 2 || neighbours > 3)
                //     lines[i].setbit(j, 0);
                // else if (neighbours == 3)
                //     lines[i].setbit(j, 1);

                // if(std::find(std::begin(born), std::end(born), neighbours) != std::end(born)){
                //     lines[i].setbit(j, 1);
                // }else if(std::find(std::begin(survive), std::end(survive), neighbours) != std::end(survive)){

                // }else{
                //     lines[i].setbit(j, 0);
                // }

                if (neighbours == 3)
                {
                    lines[i].setbit(j, 1);
                }
                else if (neighbours == 2 || neighbours == 3)
                {
                }
                else
                {
                    lines[i].setbit(j, 0);
                }

                if (render && lines[i].getbit(j))
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawPoint(renderer, j, i);
                }
            }
        }
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                run = 0;
                break;
            }
        }
        // char a;
        // scanf("%c", &a);
    }
}
