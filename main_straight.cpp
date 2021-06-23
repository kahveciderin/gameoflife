#include <stdlib.h>

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <string>

#include <sys/time.h>

using namespace std;

#define GRIDSIZE_X 256
#define GRIDSIZE_Y GRIDSIZE_X
#define SCALE 1

#define RENDER 1
//#define RGB

float timedifference_msec(struct timeval t0, struct timeval t1) {
  return (t1.tv_sec - t0.tv_sec) * 1000.0f +
         (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

int main(int argc, char *argv[]) {

  srand(time(NULL));
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(GRIDSIZE_X * SCALE, GRIDSIZE_Y * SCALE, 0,
                              &window, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  uint8_t survive[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
  uint8_t born[10] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  SDL_RenderSetScale(renderer, SCALE, SCALE);

  bool cells[GRIDSIZE_X * GRIDSIZE_Y];

  for (int i = 0; i < GRIDSIZE_X * GRIDSIZE_Y; i++) {
    cells[i] = 0;
  }

  ifstream file(argv[1]);
  string linefi;
  int lineno = 0;
  string data;
  int xval, yval;

  bool randomData = false;
  while (getline(file, linefi)) {
    if (linefi.at(0) == '#' && linefi.at(1) == '!') {
      randomData = true;
    }
    if (linefi.at(0) != '#') {
      if (lineno == 0) {
        int i = 4;
        char buff[5] = {0, 0, 0, 0, 0};
        for (int j = i; isdigit(linefi.at(i)); i++) {
          buff[i - j] = linefi.at(i);
        }

        sscanf(buff, "%d", &xval);

        i += 6;

        char bufe[5] = {0, 0, 0, 0, 0};
        for (int j = i; isdigit(linefi.at(i)); i++) {
          bufe[i - j] = linefi.at(i);
        }
        sscanf(bufe, "%d", &yval);
        printf("x = %d, y = %d\n\n", xval, yval);

        if (linefi.at(i) == ',') {
          i += 10;

          for (int j = i; isdigit(linefi.at(i)); i++) {
            born[i - j] = linefi.at(i) - '0';
          }
          i += 2;

          for (int j = i; isdigit(linefi.at(i)); i++) {
            survive[i - j] = linefi.at(i) - '0';
          }
        } else {
          born[0] = 3;
          survive[0] = 2;
          survive[1] = 3;
        }

        printf("born: %d %d %d %d %d %d %d %d %d %d survive: %d %d %d %d %d %d "
               "%d %d %d %d\n",
               born[0], born[1], born[2], born[3], born[4], born[5], born[6],
               born[7], born[8], born[9], survive[0], survive[1], survive[2],
               survive[3], survive[4], survive[5], survive[6], survive[7],
               survive[8], survive[9]);
      } else {
        data = data + linefi;
      }

      lineno++;
    }
  }

  // printf("%s", data.c_str());
  int tmpx = (GRIDSIZE_X - xval) / 2;
  int tmpy = (GRIDSIZE_Y - yval) / 2;

  if (randomData) {
    printf("fill with random data\n");
    for (int y_gb = 0; y_gb < GRIDSIZE_X * GRIDSIZE_Y; y_gb++) {
      cells[y_gb] = (0 + (rand() % (1 - 0 + 1)));
    }
  } else {
    for (int i = 0; i < data.length(); i++) {
      if (data.at(i) == '$' || data.at(i) == '!') {
        tmpy++;
        tmpx = (GRIDSIZE_X - xval) / 2;
        // printf("\n");
      } else if (isdigit(data.at(i))) {
        char buff[5] = {0, 0, 0, 0, 0};
        for (int j = i; isdigit(data.at(i)); i++) {
          buff[i - j] = data.at(i);
        }
        int num;
        sscanf(buff, "%d", &num);

        // i++;
        if (data.at(i) == 'o') {
          for (int j = 0; j < num; j++) {
            cells[(tmpy * GRIDSIZE_X) + tmpx] = 1;
            tmpx++;
            // printf("o");
          }
        } else {
          for (int j = 0; j < num; j++) {
            cells[(tmpy * GRIDSIZE_X) + tmpx] = 0;
            tmpx++;
            // printf(".");
          }
        }
        // printf("%d\n", num);
      } else if (data.at(i) == 'o') {
        cells[(tmpy * GRIDSIZE_X) + tmpx] = 1;
        tmpx++;
        // printf("o");
      } else {
        tmpx++;
        // printf(".");
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

  for (int i = 0; i < 10 + 1; i++) {

    sneigh[i] = survive[i];
  }

  for (int i = 0; i < 10 + 1; i++) {

    bneigh[i] = born[i];
  }

  struct timeval oldtime;
  gettimeofday(&oldtime, 0);
  while (run) {

    bool render = step % RENDER == 0;

    bool oldcells[GRIDSIZE_X * GRIDSIZE_Y];

    // printf("%d", bneigh[0]);
    for (int i = 0; i < GRIDSIZE_X * GRIDSIZE_Y; i++) {

      oldcells[i] = cells[i];
    }

    if (render) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
    }
    step++;

    for (int i = 0; i < GRIDSIZE_Y - 1; i++) {
      for (int j = 0; j < GRIDSIZE_X - 1; j++) {
        uint8_t neighbours = 0;

        int a = i - 1;
        int b = i + 1;

        int c = j - 1;
        int d = j + 1;

        if (a < 0)
          a = GRIDSIZE_Y - 2;

        if (b > (GRIDSIZE_Y - 2))
          b = 0;

        if (c < 0)
          c = GRIDSIZE_X - 2;

        if (d > (GRIDSIZE_X - 2))
          d = 0;

        if (oldcells[(a * GRIDSIZE_X) + c])
          neighbours++;
        if (oldcells[(a * GRIDSIZE_X) + j])
          neighbours++;
        if (oldcells[(a * GRIDSIZE_X) + d])
          neighbours++;
        if (oldcells[(i * GRIDSIZE_X) + c])
          neighbours++;
        if (oldcells[(i * GRIDSIZE_X) + d])
          neighbours++;
        if (oldcells[(b * GRIDSIZE_X) + c])
          neighbours++;
        if (oldcells[(b * GRIDSIZE_X) + j])
          neighbours++;
        if (oldcells[(b * GRIDSIZE_X) + d])
          neighbours++;

        if (neighbours == born[0] || neighbours == born[1] ||
            neighbours == born[2] || neighbours == born[3] ||
            neighbours == born[4] || neighbours == born[5] ||
            neighbours == born[6] || neighbours == born[7] ||
            neighbours == born[8] || neighbours == born[9]) {
          cells[(i * GRIDSIZE_Y) + j] = 1;
        } else if (neighbours == survive[0] || neighbours == survive[1] ||
                   neighbours == survive[2] || neighbours == survive[3] ||
                   neighbours == survive[4] || neighbours == survive[5] ||
                   neighbours == survive[6] || neighbours == survive[7] ||
                   neighbours == survive[8] || neighbours == survive[9]) {
        } else {
          cells[(i * GRIDSIZE_Y) + j] = 0;
        }

        if (render && cells[(i * GRIDSIZE_Y) + j]) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderDrawPoint(renderer, j, i);
        }
      }
    }
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_QUIT:
        run = 0;
        break;
      }
    }
    // char a;
    // scanf("%c", &a);
    struct timeval newtime;
    gettimeofday(&newtime, 0);
    printf("Frame time: %f ms\n", timedifference_msec(oldtime, newtime));
    oldtime = newtime;
  }
}
