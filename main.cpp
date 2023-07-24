#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "OBJ_file_loader/OBJLoader.h"
#include "term_gfx.h"

int main(int argc, char* argv[]){
  term_gfx* screen = create_gfx_env(100, 100, 100, 'X');

  mesh3D m2;
  
  int should_quit = 0;

  vec3 rotations = {72, 84, 0};

  enum {CUBE, PYR, FILE} shape = CUBE;

  std::vector<float> file_verts;

  // parse args using getopt
  int opt;
  while ((opt = getopt(argc, argv, "s:f:x:y:z:")) != -1) {
    switch (opt) {
      case 's':
        if(!strcmp(optarg, "cube")){
          shape = CUBE;
        }
        else if(!strcmp(optarg, "pyramid")){
          shape = PYR;
        }
      break;
      case 'f':
        {
            OBJFile::OBJLoader loader;
            loader.load(optarg, file_verts, OBJFile::dataLayout::POS);
            shape = FILE;
            if(file_verts.size() == 0){
                return 0;
            }
        }
      break;
      case 'x':
        rotations.x = atof(optarg);
      break;
      case 'y':
        rotations.y = atof(optarg);
      break;
      case 'z':
        rotations.z = atof(optarg);
      break;
      default:
        fprintf(stderr, "Usage: %s [-sfr]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  float cube_verts[] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

    -0.5f, -0.5f, 0.5f,

    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    -0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,

    0.5f, -0.5f, -0.5f
  };
  float pyramid_verts[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.0f, 0.5f, 0.0f,

    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.0f, 0.5f, 0.0f,

    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    0.0f, 0.5f, 0.0f,

    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    0.0f, 0.5f, 0.0f,

    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f, // base
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f
  };
  switch(shape){
    case CUBE:
      m2 = create_mesh3D(screen, cube_verts, 20, 3, (vec3){0.0f, 0.0f, 0.0f});
    break;
    case PYR:
      m2 = create_mesh3D(screen, pyramid_verts, 21, 3, (vec3){0.0f, 0.0f, 0.0f});
    break;
    case FILE:
      m2 = create_mesh3D(screen, file_verts.data(), file_verts.size()/3, 3, (vec3){0.0f, 0.0f, 0.0f});
    break;
  }

  camera cam1;
  cam1.pos = (vec3){0.0f, 0.0f, 1.0f};
  cam1.dir = (vec3){0.0f, 0.0f, -1.0f};
  cam1.FOV = 90.0f;
  cam1.view_dist = screen->depth;
  
  while(!should_quit){
    frame_begin(screen);
    draw_mesh3D(screen, &cam1, m2);

    if(rotations.x != 0){
      rotate_mesh3D(screen, &m2, (float)((2 * pi) / rotations.x), X);
    }
    
    if(rotations.y != 0){
      rotate_mesh3D(screen, &m2, (float)((2 * pi) / rotations.y), Y);
    }

    if(rotations.z != 0){
      rotate_mesh3D(screen, &m2, (float)((2 * pi) / rotations.z), Z);
    }

    render(screen);

    usleep((1000 * 1000)/15);
  }
  
  destroy_gfx_env(screen);
  return 0;
}
