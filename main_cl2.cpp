#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <string>

#include <sys/time.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl2.hpp>
#endif

#define MAX_SOURCE_SIZE (0x100000)
using namespace std;

#define GRIDSIZE_X 512
#define GRIDSIZE_Y GRIDSIZE_X
#define SCALE 1

#define RENDER 1
//#define RGB

const char *getErrorString(cl_int error) {
  // if (error == CL_SUCCESS)
  //   return NULL;
  switch (error) {
  // run-time and JIT compiler errors
  case 0:
    return "CL_SUCCESS";
  case -1:
    return "CL_DEVICE_NOT_FOUND";
  case -2:
    return "CL_DEVICE_NOT_AVAILABLE";
  case -3:
    return "CL_COMPILER_NOT_AVAILABLE";
  case -4:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case -5:
    return "CL_OUT_OF_RESOURCES";
  case -6:
    return "CL_OUT_OF_HOST_MEMORY";
  case -7:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case -8:
    return "CL_MEM_COPY_OVERLAP";
  case -9:
    return "CL_IMAGE_FORMAT_MISMATCH";
  case -10:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case -11:
    return "CL_BUILD_PROGRAM_FAILURE";
  case -12:
    return "CL_MAP_FAILURE";
  case -13:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
  case -14:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
  case -15:
    return "CL_COMPILE_PROGRAM_FAILURE";
  case -16:
    return "CL_LINKER_NOT_AVAILABLE";
  case -17:
    return "CL_LINK_PROGRAM_FAILURE";
  case -18:
    return "CL_DEVICE_PARTITION_FAILED";
  case -19:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

  // compile-time errors
  case -30:
    return "CL_INVALID_VALUE";
  case -31:
    return "CL_INVALID_DEVICE_TYPE";
  case -32:
    return "CL_INVALID_PLATFORM";
  case -33:
    return "CL_INVALID_DEVICE";
  case -34:
    return "CL_INVALID_CONTEXT";
  case -35:
    return "CL_INVALID_QUEUE_PROPERTIES";
  case -36:
    return "CL_INVALID_COMMAND_QUEUE";
  case -37:
    return "CL_INVALID_HOST_PTR";
  case -38:
    return "CL_INVALID_MEM_OBJECT";
  case -39:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case -40:
    return "CL_INVALID_IMAGE_SIZE";
  case -41:
    return "CL_INVALID_SAMPLER";
  case -42:
    return "CL_INVALID_BINARY";
  case -43:
    return "CL_INVALID_BUILD_OPTIONS";
  case -44:
    return "CL_INVALID_PROGRAM";
  case -45:
    return "CL_INVALID_PROGRAM_EXECUTABLE";
  case -46:
    return "CL_INVALID_KERNEL_NAME";
  case -47:
    return "CL_INVALID_KERNEL_DEFINITION";
  case -48:
    return "CL_INVALID_KERNEL";
  case -49:
    return "CL_INVALID_ARG_INDEX";
  case -50:
    return "CL_INVALID_ARG_VALUE";
  case -51:
    return "CL_INVALID_ARG_SIZE";
  case -52:
    return "CL_INVALID_KERNEL_ARGS";
  case -53:
    return "CL_INVALID_WORK_DIMENSION";
  case -54:
    return "CL_INVALID_WORK_GROUP_SIZE";
  case -55:
    return "CL_INVALID_WORK_ITEM_SIZE";
  case -56:
    return "CL_INVALID_GLOBAL_OFFSET";
  case -57:
    return "CL_INVALID_EVENT_WAIT_LIST";
  case -58:
    return "CL_INVALID_EVENT";
  case -59:
    return "CL_INVALID_OPERATION";
  case -60:
    return "CL_INVALID_GL_OBJECT";
  case -61:
    return "CL_INVALID_BUFFER_SIZE";
  case -62:
    return "CL_INVALID_MIP_LEVEL";
  case -63:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
  case -64:
    return "CL_INVALID_PROPERTY";
  case -65:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
  case -66:
    return "CL_INVALID_COMPILER_OPTIONS";
  case -67:
    return "CL_INVALID_LINKER_OPTIONS";
  case -68:
    return "CL_INVALID_DEVICE_PARTITION_COUNT";

  // extension errors
  case -1000:
    return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
  case -1001:
    return "CL_PLATFORM_NOT_FOUND_KHR";
  case -1002:
    return "CL_INVALID_D3D10_DEVICE_KHR";
  case -1003:
    return "CL_INVALID_D3D10_RESOURCE_KHR";
  case -1004:
    return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
  case -1005:
    return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
  default:
    return "Unknown OpenCL error";
  }
}
float timedifference_msec(struct timeval t0, struct timeval t1) {
  return (t1.tv_sec - t0.tv_sec) * 1000.0f +
         (t1.tv_usec - t0.tv_usec) / 1000.0f;
}
cl::Platform findCL2Platform() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (auto &p : platforms) {
    std::string version = p.getInfo<CL_PLATFORM_VERSION>();
    if (version.find("OpenCL 2.") != std::string::npos) {
      return p;
    }
  }
  return cl::Platform();
}
int main(int argc, char *argv[]) {
  cl::Platform platform = findCL2Platform();

  if (0 == platform()) {
    std::cerr << "No OpenCL 2 platform found.\n";
    return EXIT_FAILURE;
  }

  cl::Platform defaultPlatform = cl::Platform::setDefault(platform);
  if (defaultPlatform != platform) {
    std::cerr << "Error setting default platform\n";
    return EXIT_FAILURE;
  }
  FILE *fp;
  char *source_str;
  size_t source_size;
  fp = fopen("game_of_life_kernel.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }
  source_str = (char *)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  const std::string rot13Kernel = R"===(
  kernel void rot13(global char* in, global char* out)
{
  int num = get_global_id(0);
  char c = in[num];
  if ('a' <= c && c <= 'z') {
    out[num] = ((c - 'a') + 13) % 26 + 'a';
  } else if ('A' <= c && c <= 'Z') {
    out[num] = ((c - 'A') + 13) % 26 + 'A';
  } else {
    out[num] = c;
  }
}
)===";
  cl::Program program(rot13Kernel);
  try {
    // Compile
    program.build("-cl-std=CL2.0");
  } catch (cl::BuildError e) {
    // Catch Compile failture and print diagnostics
    std::cerr << "Error building program game_of_life_kernel2.cl: " << e.what() << "\n";
    for (auto &pair : e.getBuildLog()) {
      std::cerr << pair.second << std::endl;
    }

    return EXIT_FAILURE;
  }

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

  struct timeval oldtime;
  gettimeofday(&oldtime, 0);
  while (run) {

    bool render = step % RENDER == 0;

    if (render) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      for (int i = 0; i < GRIDSIZE_X * GRIDSIZE_Y; i++) {
        if (render && cells[i]) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderDrawPoint(renderer, i % GRIDSIZE_X, i / GRIDSIZE_Y);
        }
      }
    }
    step++;

    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_QUIT:
        run = 0;
        break;
      }
    }
    struct timeval newtime;
    gettimeofday(&newtime, 0);
    // printf("Frame time: %f ms\n", timedifference_msec(oldtime, newtime));
    oldtime = newtime;
  }
}
