#include <SDL3/SDL.h>

#define BASE_IMPLEMENTATION
#include "base.h"

#include "math.h"
#include "model.h"
#include "renderer.h"
#include "texture.h"

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800
#define FPS_INTERVAL 1
#define DEPTH 255

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool quit;
  SDL_Event e;

  // Temporal buffers
  SDL_FPoint pointsBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
  char textBuffer[600];
  u8 depthBuffer[SCREEN_WIDTH][SCREEN_HEIGHT]; // WARNING: Not sure if it should be +1 or -1 or not add anything?

  // FPS stuff
  size_t frameCount;
  i32 lastFPSUpdate;
  i32 FPS;

  // Positions
  Vec3 lightDir;
  Vec3 eye;
  Vec3 center;
  Vec3 up;
} Renderer;
Renderer renderer = {0};

void RendererQuit(errno_t code) {
  if (renderer.renderer != NULL) {
    SDL_DestroyRenderer(renderer.renderer);
  }
  if (renderer.window != NULL) {
    SDL_DestroyWindow(renderer.window);
  }
  SDL_Quit();
  if (code != -1) {
    exit(code);
  }
}

void InitRenderer(i32 width, i32 height) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LogError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  renderer.window = SDL_CreateWindow("Tiny Renderer", width, height, 0);
  if (!renderer.window) {
    LogError("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  renderer.renderer = SDL_CreateRenderer(renderer.window, NULL);
  if (!renderer.renderer) {
    LogError("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }
}

void EventPoll() {
  while (SDL_PollEvent(&renderer.e) != 0) {
    if (renderer.e.type == SDL_EVENT_QUIT) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_KEY_DOWN) {

      if (renderer.e.key.key == SDLK_RIGHT && renderer.lightDir.x < 2) {
        renderer.lightDir.x += 0.5;
      }
      if (renderer.e.key.key == SDLK_LEFT && renderer.lightDir.x > -2) {
        renderer.lightDir.x -= 0.5;
      }
      if (renderer.e.key.key == SDLK_UP && renderer.lightDir.y < 2) {
        renderer.lightDir.y += 0.5;
      }
      if (renderer.e.key.key == SDLK_DOWN && renderer.lightDir.y > -2) {
        renderer.lightDir.y -= 0.5;
      }
      if (renderer.e.key.key == SDLK_ESCAPE) {
        renderer.quit = true;
      }
    }
  }
}

void DestroyRenderer() {
  RendererQuit(0);
}

void SetDrawColor(Color color) {
  SDL_SetRenderDrawColor(renderer.renderer, color.r, color.g, color.b, color.a);
}

void ClearScreen(Color color) {
  SetDrawColor(color);
  SDL_RenderClear(renderer.renderer);
}

void DrawPoint(i32 x, i32 y, Color color) {
  SetDrawColor(color);
  SDL_RenderPoint(renderer.renderer, x, y);
}

void DrawTextF(i32 x, i32 y, Color color, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  assert(size <= 600 && "Format is bigger than buffer");

  va_start(args, format);
  vsnprintf(renderer.textBuffer, size, format, args);
  va_end(args);

  SetDrawColor(color);
  SDL_RenderDebugText(renderer.renderer, x, y, renderer.textBuffer);
}

void DrawFPS(i32 x, i32 y, Color color) {
  DrawTextF(x, y, color, "FPS: %d", renderer.FPS);
}

Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up) {
  Vec3 z = Vec3Normalize(Vec3Sub(eye, center));
  Vec3 x = Vec3Normalize(Vec3Cross(up, z));
  Vec3 y = Vec3Normalize(Vec3Cross(z, x));

  Mat4 res = Mat4Create();
  res.a[Mat4Get(0, 0)] = x.x;
  res.a[Mat4Get(1, 0)] = y.x;
  res.a[Mat4Get(2, 0)] = z.x;
  res.a[Mat4Get(0, 3)] = -center.x;

  res.a[Mat4Get(0, 1)] = x.y;
  res.a[Mat4Get(1, 1)] = y.y;
  res.a[Mat4Get(2, 1)] = z.y;
  res.a[Mat4Get(1, 3)] = -center.y;

  res.a[Mat4Get(0, 2)] = x.z;
  res.a[Mat4Get(1, 2)] = y.z;
  res.a[Mat4Get(2, 2)] = z.z;
  res.a[Mat4Get(2, 3)] = -center.z;

  return res;
}

Mat4 CreateViewport(i32 x, i32 y, i32 w, i32 h) {
  Mat4 m = Mat4Create();
  m.a[Mat4Get(0, 3)] = x + w / 2.f;
  m.a[Mat4Get(1, 3)] = y + h / 2.f;
  m.a[Mat4Get(2, 3)] = DEPTH / 2.f;

  m.a[Mat4Get(0, 0)] = w / 2.f;
  m.a[Mat4Get(1, 1)] = h / 2.f;
  m.a[Mat4Get(2, 2)] = DEPTH / 2.f;
  return m;
}

Vec3 ProjectVec3(Vec3 vector) {
  return (Vec3){(vector.x + 1.0) * SCREEN_WIDTH / 2, (-vector.y + 1.0) * SCREEN_HEIGHT / 2, (vector.z + 1.0) * DEPTH / 2};
}

void BeginDrawing() {
  for (i32 i = 0; i < SCREEN_WIDTH; i++) {
    for (i32 j = 0; j < SCREEN_HEIGHT; j++) {
      renderer.depthBuffer[i][j] = 0;
    }
  }
  EventPoll();
}

void EndDrawing() {
  SDL_RenderPresent(renderer.renderer);
  renderer.frameCount++;

  i32 currentTime = SDL_GetTicks() / 1000.0f;
  if (currentTime - renderer.lastFPSUpdate >= FPS_INTERVAL) {
    renderer.FPS = renderer.frameCount / (currentTime - renderer.lastFPSUpdate); // NOTE: frameCount / 1 second-ish
    renderer.frameCount = 0;
    renderer.lastFPSUpdate = currentTime;
  }
}

f64 TriangleArea(i32 ax, i32 ay, i32 bx, i32 by, i32 cx, i32 cy) {
  return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

// NOTE: Max X is width, Max Y is height
i32 GetDepthBuffer(i32 x, i32 y) {
  return renderer.depthBuffer[x][y];
}

void SetDepthBuffer(i32 x, i32 y, i32 z) {
  renderer.depthBuffer[x][y] = z;
}

void DrawTriangle(Vec3 a, Vec3 b, Vec3 c, Color color) {
  i32 boxMinX = Min(Min(a.x, b.x), c.x);
  i32 boxMinY = Min(Min(a.y, b.y), c.y);
  i32 boxMaxX = Max(Max(a.x, b.x), c.x);
  i32 boxMaxY = Max(Max(a.y, b.y), c.y);
  f64 totalArea = TriangleArea(a.x, a.y, b.x, b.y, c.x, c.y);

  for (int x = boxMinX; x <= boxMaxX; x++) {
    for (int y = boxMinY; y <= boxMaxY; y++) {
      f64 alpha = TriangleArea(x, y, b.x, b.y, c.x, c.y) / totalArea;
      f64 beta = TriangleArea(x, y, c.x, c.y, a.x, a.y) / totalArea;
      f64 gamma = TriangleArea(x, y, a.x, a.y, b.x, b.y) / totalArea;

      if (alpha < 0 || beta < 0 || gamma < 0) continue;

      f64 z = (alpha * a.z) + (beta * b.z) + (gamma * c.z);
      i32 result = GetDepthBuffer(x, y);

      if (z <= result) continue;
      SetDepthBuffer(x, y, z);
      DrawPoint(x, y, color);
    }
  }
}

void DrawTriangleTexture(Vec3 a, Vec3 b, Vec3 c, Vec2 at, Vec2 bt, Vec2 ct, Vec3 ia, Vec3 ib, Vec3 ic, SDL_Surface *texture) {
  i32 boxMinX = Min(Min(a.x, b.x), c.x);
  i32 boxMinY = Min(Min(a.y, b.y), c.y);
  i32 boxMaxX = Max(Max(a.x, b.x), c.x);
  i32 boxMaxY = Max(Max(a.y, b.y), c.y);
  f64 totalArea = TriangleArea(a.x, a.y, b.x, b.y, c.x, c.y);

  for (int x = boxMinX; x <= boxMaxX; x++) {
    for (int y = boxMinY; y <= boxMaxY; y++) {
      f64 alpha = TriangleArea(x, y, b.x, b.y, c.x, c.y) / totalArea;
      f64 beta = TriangleArea(x, y, c.x, c.y, a.x, a.y) / totalArea;
      f64 gamma = TriangleArea(x, y, a.x, a.y, b.x, b.y) / totalArea;

      if (alpha < 0 || beta < 0 || gamma < 0) continue;

      f64 z = (alpha * a.z) + (beta * b.z) + (gamma * c.z);
      i32 result = GetDepthBuffer(x, y);

      if (z <= result) continue;
      SetDepthBuffer(x, y, z);
      f64 u = (alpha * at.x) + (beta * bt.x) + (gamma * ct.x);
      f64 v = (alpha * at.y) + (beta * bt.y) + (gamma * ct.y);
      u = u * texture->w;
      v = (1.0 - v) * texture->h;

      Color color = GetPixelTexture(texture, u, v);

      // NOTE: Calculates the u and v
      f64 ix = (alpha * ia.x) + (beta * ib.x) + (gamma * ic.x);
      f64 iy = (alpha * ia.y) + (beta * ib.y) + (gamma * ic.y);
      f64 iz = (alpha * ia.z) + (beta * ib.z) + (gamma * ic.z);
      ix = (ix + 1.0) * 0.5;
      iy = (iy + 1.0) * 0.5;
      iz = (iz + 1.0) * 0.5;
      f64 c = (ix + iy + iz) / 3;

      color.r *= c;
      color.g *= c;
      color.b *= c;

      DrawPoint(x, y, color);
    }
  }
}

int main() {
  InitRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);
  renderer.eye = (Vec3){0, -1, 3};
  renderer.center = (Vec3){0, 0, 0};
  renderer.up = (Vec3){0, 1, 0};
  renderer.lightDir = Vec3Normalize((Vec3){1, 1, 1});

  Model model = InitModel(S("./resources/diablo_pose.obj"));
  SDL_Surface *texture = InitTexture(S("./resources/diablo_pose.tga"));
  Mat4 modelView = LookAt(renderer.eye, renderer.center, renderer.up);
  Mat4 projection = Mat4Create();
  Mat4 viewport = CreateViewport(SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8, SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT * 3 / 4);
  projection.a[Mat4Get(3, 2)] = -1.f / Vec3Norm(Vec3Sub(renderer.eye, renderer.center));
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);

      for (i32 i = 0; i < model.faces.length / 3; i++) {
        Mat4 vp = Mat4Mul(Mat4Mul(viewport, projection), modelView);

        Vec3 a = GetVertModel(&model, i, 0);
        Vec3 sca = M2V(Mat4Mul(vp, V2M(a))); // NOTE: Screen Coords a
        Vec2 at = GetVertTextModel(&model, i, 0);
        Vec3 ia = Vec3Mul(GetVertNormModel(&model, i, 0), renderer.lightDir); // NOTE: Intensity a

        Vec3 b = GetVertModel(&model, i, 1);
        Vec3 scb = M2V(Mat4Mul(vp, V2M(b)));
        Vec2 bt = GetVertTextModel(&model, i, 1);
        Vec3 ib = Vec3Mul(GetVertNormModel(&model, i, 1), renderer.lightDir);

        Vec3 c = GetVertModel(&model, i, 2);
        Vec3 scc = M2V(Mat4Mul(vp, V2M(c)));
        Vec2 ct = GetVertTextModel(&model, i, 2);
        Vec3 ic = Vec3Mul(GetVertNormModel(&model, i, 2), renderer.lightDir);

        DrawTriangleTexture(sca, scb, scc, at, bt, ct, ia, ib, ic, texture);
      }

      DrawFPS(10, 10, WHITE);
    }
    EndDrawing();
  }

  DestroyTexture(texture);
  DestroyRenderer();
}
