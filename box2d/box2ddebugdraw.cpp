#include "box2ddebugdraw.h"

#include <iostream>
#include <SDL2/SDL.h>

SDL_Renderer *Box2dDebugDraw::sdl_renderer = nullptr; // forward declaration and initializationf or static member

Box2dDebugDraw::Box2dDebugDraw() : b2DebugDraw() {
}

void Box2dDebugDraw::DrawPolygonFcn(const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawPolygonFcn" << std::endl;
    DrawPolygon(vertices, vertexCount, color);
}

void Box2dDebugDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawSolidPolygonFcn" << std::endl;
    DrawPolygon(vertices, vertexCount, color);
}

void Box2dDebugDraw::DrawCircleFcn(b2Vec2 center, float radius, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawCircleFcn" << std::endl;
}

void Box2dDebugDraw::DrawSolidCircleFcn(b2Transform transform, float radius, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawSolidCircleFcn" << std::endl;
}

void Box2dDebugDraw::DrawSolidCapsuleFcn(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawSolidCapsuleFcn" << std::endl;
}

void Box2dDebugDraw::DrawSegmentFcn(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawSegmentFcn" << std::endl;
}

void Box2dDebugDraw::DrawTransformFcn(b2Transform transform, void *context)
{
    std::cout << "Box2dDebugDraw::DrawTransformFcn" << std::endl;
}

void Box2dDebugDraw::DrawPointFcn(b2Vec2 p, float size, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawPointFcn" << std::endl;
}

void Box2dDebugDraw::DrawStringFcn(b2Vec2 p, const char *s, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawStringFcn" << std::endl;
}

void Box2dDebugDraw::DrawPolygon(const b2Vec2 *vertices, int vertexCount, const b2HexColor color)
{
    //SDL_SetRenderDrawColor(sdl_renderer, 250, 0, 0);
    SDL_Point* sdlPoints = new SDL_Point[vertexCount + 1];
    std::cout << "Box2dDebugDraw::DrawPolygon - vertexCount[" << vertexCount << "]" << std::endl;
    for (int i = 0; i < vertexCount; ++i) {
        sdlPoints[i] = { (int)vertices[i].x*PIXELS_PER_METER, (int)vertices[i].y*PIXELS_PER_METER };
        std::cout << "Box2dDebugDraw::DrawPolygon point[" << i << "][" << sdlPoints[i].x << "][" << sdlPoints[i].y << "]" << std::endl;
    }
    sdlPoints[vertexCount] = sdlPoints[0]; // Close the polygon
    SDL_RenderDrawLines(sdl_renderer, sdlPoints, vertexCount + 1);
    delete[] sdlPoints;
}

void Box2dDebugDraw::setRenderer(SDL_Renderer *renderer) {
    sdl_renderer = renderer;
}


