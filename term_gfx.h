#ifndef TERM_GFX_H
#define TERM_GFX_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define pi 3.14159

typedef struct{
  float x;
  float y;
} vec2;

typedef struct{
  float x;
  float y;
  float z;
} vec3;

typedef struct{
  float x;
  float y;
  float z;
  float w;
} vec4;

typedef union {
  float float_val[16];
  vec4 vec4_val_row[4];
} mat4;

typedef struct{
  vec3 pos;
  vec3 dir;
  float FOV;
  float view_dist;
} camera;

enum axis{
  X,
  Y,
  Z
};

vec3 cross_vec3(vec3 LHS, vec3 RHS){
  return (vec3){(LHS.y * RHS.z - LHS.z * RHS.y), (LHS.z * RHS.x - LHS.x * RHS.z), (LHS.x * RHS.y - LHS.y * RHS.x)};
}
vec3 cross_vec4(vec4 LHS, vec4 RHS){
  return (vec3){(LHS.y * RHS.z - LHS.z * RHS.y), (LHS.z * RHS.x - LHS.x * RHS.z), (LHS.x * RHS.y - LHS.y * RHS.x)};
}

vec3 sub_vec3(vec3 LHS, vec3 RHS){
  return (vec3){LHS.x - RHS.x, LHS.y - RHS.y, LHS.z - RHS.z};
}

float dot_vec3(vec3 LHS, vec3 RHS){
  return (LHS.x * RHS.x + LHS.y * RHS.y + LHS.z * RHS.z);
}
vec3 norm_vec3(vec3 vec){
  float norm_factor = 1/sqrtf(dot_vec3(vec, vec));
  return (vec3){vec.x * norm_factor, vec.y * norm_factor, vec.z * norm_factor};
}
float magnitude_vec3(vec3 vec){
  return sqrtf(dot_vec3(vec, vec));
}

vec2 rotate_vec2(vec2 vec, float by){
  return (vec2){vec.x * cosf(by) - vec.y * sinf(by), vec.x * sinf(by) + vec.y * cosf(by)};
}
vec3 rotate_vec3(vec3 vec, float by, enum axis a){
  switch(a){
    case X: return (vec3){vec.x, vec.y * cosf(by) - vec.z * sinf(by), vec.y * sinf(by) + vec.z * cosf(by)};
    case Y: return (vec3){vec.x * cosf(by) - vec.z * sinf(by), vec.y, vec.x * sinf(by) + vec.z * cosf(by)};
    case Z: return (vec3){vec.x * cosf(by) - vec.y * sinf(by), vec.x * sinf(by) + vec.y * cosf(by), vec.z};
    default: return vec;
  }
}
vec3 rotate_vec3_var_axis(vec3 vec, float by, vec3 a){
  vec4 q = {sinf(by/2) * a.x, sinf(by/2) * a.y, sinf(by/2) * a.z, cosf(by/2)}; // vectors treated as quaternions

  // sandwich product v' = qvq*
  vec3 c = cross_vec3((vec3){q.x, q.y, q.z}, vec);
  float b2 = dot_vec3(vec, (vec3){q.x, q.y, q.z});
  float v2 = dot_vec3(vec, vec);
  return (vec3){vec.x * (q.w * q.w) - v2 + vec.x * b2 * 2.0f + c.x * 2.0f * q.w, vec.y * (q.w * q.w) - v2 + vec.y * b2 * 2.0f + c.y * 2.0f * q.w, vec.z * (q.w * q.w) - v2 + vec.z * b2 * 2.0f + c.z * 2.0f * q.w};
}
float get_rotation_between_vec3(vec3 a, vec3 b){
  return (acosf(dot_vec3(a, b)/(magnitude_vec3(a) * magnitude_vec3(b))));
}

typedef struct{
  vec2 pos;
  char tile;
} frag;

typedef struct{
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  char* frame_buffer;
  char tile_set[5];
  frag cursor;
} term_gfx;

int sgn(float val){
  return (val > 0) - (val < 0);
}

unsigned int get_z_depth_tile_index(term_gfx* screen, float Z, float Znear, float Zfar){
  float Zdiv = (Zfar - Znear)/4.0f;
  if(Z <= Zdiv){
    return 0;
  }
  else if((Z > Zdiv) && (Z <= 2 * Zdiv)){
    return 1;
  }
  else if((Z > 2 * Zdiv) && (Z <= 3 * Zdiv)){
    return 2;
  }
  else if((Z > 3 * Zdiv) && (Z <= 4 * Zdiv)){
    return 3;
  }
  else{
    return 4;
  }
}

vec4 product_vec4_mat4(vec4 pos, mat4 mat){
  return (vec4){(pos.x * mat.float_val[0] + pos.y * mat.float_val[4] + pos.z * mat.float_val[8] + pos.w * mat.float_val[12]),
    (pos.x * mat.float_val[1] + pos.y * mat.float_val[5] + pos.z * mat.float_val[9] + pos.w * mat.float_val[13]),
    (pos.x * mat.float_val[2] + pos.y * mat.float_val[6] + pos.z * mat.float_val[10] + pos.w * mat.float_val[14]),
    (pos.x * mat.float_val[3] + pos.y * mat.float_val[7] + pos.z * mat.float_val[11] + pos.w * mat.float_val[15])};
}
vec4 product_mat4_vec4(mat4 mat, vec4 pos){
  return (vec4){(pos.x * mat.float_val[0] + pos.y * mat.float_val[1] + pos.z * mat.float_val[2] + pos.w * mat.float_val[3]),
    (pos.x * mat.float_val[4] + pos.y * mat.float_val[5] + pos.z * mat.float_val[6] + pos.w * mat.float_val[7]),
    (pos.x * mat.float_val[8] + pos.y * mat.float_val[9] + pos.z * mat.float_val[10] + pos.w * mat.float_val[11]),
    (pos.x * mat.float_val[12] + pos.y * mat.float_val[13] + pos.z * mat.float_val[14] + pos.w * mat.float_val[15])};
}
mat4 product_mat4_mat4(mat4 LHS, mat4 RHS){
  return (mat4){

  };
}

vec2 project_to_screen(term_gfx* screen, vec3 pos, camera cam){
  float aspect = screen->width/screen->height;
  float z_near = cam.pos.z; // TODO if cam rotates z_near =/= pos.z
  mat4 projector = {
    (1/tanf(0.5f * cam.FOV))/aspect, 0, 0, 0,
    0, (1/tanf(0.5f * cam.FOV)), 0, 0,
    0, 0, (cam.view_dist + z_near)/(z_near - cam.view_dist), (2 * cam.view_dist * z_near)/(z_near - cam.view_dist),
    0, 0, -1, 0
  };
  vec4 res = product_mat4_vec4(projector, (vec4){pos.x, pos.y, pos.z, 1});
  
  return (vec2){res.x, res.y};
}

// ================ lines / pixels ========================
void draw_pixel(term_gfx* screen, vec2 pos, char pixel){
  unsigned int x = pos.x;
  unsigned int y = pos.y;
  if(x > screen->width || x < 0 || y > screen->height || y < 0){
    return;
  }
  screen->frame_buffer[y * screen->width + x] = pixel;
}

void draw_line2D(term_gfx* screen, vec2 from, vec2 to, char with){
  float signed_dx = to.x - from.x;
  float signed_dy = to.y - from.y;

  float dx = abs(signed_dx);
  float dy = abs(signed_dy);

  float step;
 
  if(dx >= dy){
    step = dx;
  }
  else{
    step = dy;
  }  
 
  dx /= step;
  dy /= step;
 
  vec2 current = from;
 
  int i = 1;
  while(i <= step)
  {
    draw_pixel(screen, current, with);

    current.x += sgn(signed_dx) * dx;
    current.y += sgn(signed_dy) * dy;
    i++;
  }
}
void draw_projected_line2D(term_gfx* screen, vec3 from, vec3 to){
  float signed_dx = to.x - from.x;
  float signed_dy = to.y - from.y;

  float dx = abs(signed_dx);
  float dy = abs(signed_dy);
  float dz = (to.z - from.z);

  float step;
 
  if(dx >= dy){
    step = dx;
  }
  else{
    step = dy;
  }  
 
  dx /= step;
  dy /= step;
  dz /= step;
 
  vec3 current = from;
 
  int i = 1;
  while(i <= step)
  {
    draw_pixel(screen, (vec2){current.x, current.y}, screen->tile_set[get_z_depth_tile_index(screen, current.z, 0.0f, (float)screen->depth)]);

    current.x += sgn(signed_dx) * dx;
    current.y += sgn(signed_dy) * dy;
    current.z += dz;
    i++;
  }
}
void draw_line3D(term_gfx* screen, camera cam, vec3 from3D, vec3 to3D){
  vec2 to = project_to_screen(screen, to3D, cam);
  vec2 from = project_to_screen(screen, from3D, cam);

  draw_projected_line2D(screen, (vec3){from.x, from.y, from3D.z}, (vec3){to.x, to.y, to3D.z});
}


// ====================== gfx objects ==============================
typedef struct{
  float* verts;
  unsigned int verts_count;
  unsigned int float_count_per_vert;
  vec2 pos; // screen space
} mesh2D;
typedef struct{
  float* verts;
  unsigned int verts_count;
  unsigned int float_count_per_vert;
  vec3 pos; // world space
} mesh3D;
void model_mat(term_gfx* screen, mesh3D mesh, float* verts_cpy){
  for(int i = 0; i < mesh.verts_count * mesh.float_count_per_vert; i += mesh.float_count_per_vert){
    verts_cpy[i] = (mesh.verts[i] + 1.0f) * screen->width/2 + mesh.pos.x;
    verts_cpy[i+1] = (mesh.verts[i+1] + 1.0f) * screen->height/2 + mesh.pos.y;
    verts_cpy[i+2] = (mesh.verts[i+2] + 1.0f) * screen->depth/2 + mesh.pos.z;
  }
}
mesh2D create_mesh2D(term_gfx* screen, float* verts, unsigned int verts_count, unsigned int float_count, vec2 pos){
  for(int i = 0; i < verts_count * float_count; i += float_count){
    verts[i] = (verts[i] + 1.0f) * screen->width/2 + pos.x;
    verts[i+1] = (verts[i+1] + 1.0f) * screen->height/2 + pos.y;
  }
  return (mesh2D){verts, verts_count, float_count, pos};
}
mesh3D create_mesh3D(term_gfx* screen, float* verts, unsigned int verts_count, unsigned int float_count, vec3 pos){
  return (mesh3D){verts, verts_count, float_count, pos};
}

void rotate_mesh2D(mesh2D* mesh, float by){
  for(int i = 0; i < mesh->verts_count * mesh->float_count_per_vert; i += mesh->float_count_per_vert){
    vec2 rotated = rotate_vec2((vec2){mesh->verts[i], mesh->verts[i+1]}, by);
    mesh->verts[i] = rotated.x; // x
    mesh->verts[i+1] = rotated.y; // y
  }
}
void rotate_mesh3D(term_gfx* screen, mesh3D* mesh, float by, enum axis a){
  for(int i = 0; i < mesh->verts_count * mesh->float_count_per_vert; i += mesh->float_count_per_vert){
    vec3 rotated = rotate_vec3((vec3){mesh->verts[i], mesh->verts[i+1], mesh->verts[i+2]}, by, a);
    mesh->verts[i] = rotated.x;
    mesh->verts[i+1] = rotated.y;
    mesh->verts[i+2] = rotated.z;
  }
}


// ============ rendering ====================
term_gfx* create_gfx_env(unsigned int width, unsigned int height, unsigned int depth, char cursor_char){
  term_gfx* terminal_buffer = (term_gfx*)malloc(sizeof(term_gfx));
  terminal_buffer->width = width * 2;
  terminal_buffer->height = height;
  terminal_buffer->depth = depth;
  terminal_buffer->frame_buffer = (char*)malloc(terminal_buffer->width * terminal_buffer->height + 1);
  terminal_buffer->frame_buffer[terminal_buffer->width * terminal_buffer->height] = '\0';
  terminal_buffer->tile_set[0] = 'W';
  terminal_buffer->tile_set[1] = '#';
  terminal_buffer->tile_set[2] = '-';
  terminal_buffer->tile_set[3] = '.';
  terminal_buffer->tile_set[4] = ' ';
  terminal_buffer->cursor = (frag){(vec2){0.0f, 0.0f}, cursor_char};
  memset(terminal_buffer->frame_buffer, terminal_buffer->tile_set[4], terminal_buffer->width * terminal_buffer->height - 1);
  return terminal_buffer;
}
void bind_tile_set(term_gfx* screen, char foreground, char d1, char d2, char d3, char background){
  screen->tile_set[0] = foreground;
  screen->tile_set[1] = d1;
  screen->tile_set[2] = d2;
  screen->tile_set[3] = d3;
  screen->tile_set[4] = background;
}
void change_cursor_tile(term_gfx* screen, char new_tile){
  screen->cursor.tile = new_tile;
}
char get_compressed_version(char* tile_set, char c0, char c1){
  // i01234 
  // |W#-. |W#-. |W#-. |W#-. |W#-. |
  // |     |.....|-----|#####|WWWWW|
  //    a     b     c     d     e
  // |^*`` |%?!:.|47>;_|@#+=_|86&=_|
  const char* sub_tile_sets[] = {
                  "^*`` ",
                  "%?!:.",
                  "47>;_",
                  "@#+=_",
                  "86&=_"
                  };

  int index = 0;
  switch(c0){
    case 'W':
    break;
    case '#':
      index = 1;
    break;
    case '-':
      index = 2;
    break;
    case '.':
      index = 3;
    break;
    case ' ':
      index = 4;
    break;
  }

  switch(c1){
    case 'W':
      return sub_tile_sets[4][index];
    break;
    case '#':
      return sub_tile_sets[3][index];
    break;
    case '-':
      return sub_tile_sets[2][index];
    break;
    case '.':
      return sub_tile_sets[1][index];
    break;
    case ' ':
      return sub_tile_sets[0][index];
    break;
  }
  return ' ';
}

void poll_input(term_gfx* screen){

}
void frame_begin(term_gfx* screen){
  memset(screen->frame_buffer, screen->tile_set[4], screen->width * screen->height);

  // using escape codes to move cursor back to top left corner of terminal display
  // UP = \033[{n}A // n = number of char positions to move
  // DOWN = \033[{n}B
  // RIGHT = \033[{n}C
  // LEFT = \033[{n}D
  // MOVE TO = \033[{y};{x}H // move cursor to (x, y)
  printf("\033[%d;%dH", 0, 0);

  screen->frame_buffer[(unsigned int)(screen->cursor.pos.y * screen->width + screen->cursor.pos.x)] = screen->cursor.tile;
}
void draw_mesh3D(term_gfx* screen, camera* cam, mesh3D object){
  float verts[object.verts_count * 3];
  model_mat(screen, object, verts);  
  for(int i = 0; i < (object.verts_count-1) * 3; i += 3){
    draw_line3D(screen, *cam, (vec3){verts[i], verts[i+1], verts[i+2]}, (vec3){verts[i + 3], verts[i + 4], verts[i + 5]});
  }
  draw_line3D(screen, *cam, (vec3){verts[(object.verts_count * 3) - 3], verts[(object.verts_count * 3) - 2], verts[(object.verts_count * 3) - 1]}, (vec3){verts[0], verts[1], verts[2]});
}
void draw_mesh2D(term_gfx* screen, mesh2D object){
  for(int i = 0; i < (object.verts_count-1)*object.float_count_per_vert; i += object.float_count_per_vert){
    draw_line2D(screen, (vec2){object.verts[i], object.verts[i+1]}, (vec2){object.verts[i + object.float_count_per_vert], object.verts[i + object.float_count_per_vert + 1]}, screen->tile_set[0]);
  }
  draw_line2D(screen, (vec2){object.verts[(object.verts_count-1) * object.float_count_per_vert], object.verts[((object.verts_count-1) * object.float_count_per_vert) + 1]}, (vec2){object.verts[0], object.verts[1]}, screen->tile_set[0]);
}
void render(term_gfx* screen){
  for(int i = 0; i < (screen->height); i+=2){
    const char* row0 = &screen->frame_buffer[i * screen->width];
    const char* row1 = &screen->frame_buffer[(i + 1) * screen->width];
    char row[screen->width];
    for(int j = 0; j < (screen->width); ++j){   
      row[j] = get_compressed_version(screen->tile_set, row0[j], row1[j]);
    }
    fwrite(&row, screen->width, 1, stdout);
    fputc('\n', stdout);
  }
}
void destroy_gfx_env(term_gfx* term_env){
  free(term_env->frame_buffer);
  free(term_env);
}

#endif
