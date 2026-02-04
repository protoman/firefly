#include "box2ddebugdraw.h"

#include <iostream>
#include <SDL3/SDL.h>

#include "data/sharedmapdata.h"

SDL_Renderer *Box2dDebugDraw::sdl_renderer = nullptr; // forward declaration and initializationf or static member

Box2dDebugDraw::Box2dDebugDraw() : b2DebugDraw() {
}

void Box2dDebugDraw::DrawPolygonFcn(b2Transform transform, const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context)
{
    std::cout << "Box2dDebugDraw::DrawPolygonFcn" << std::endl;
    DrawPolygon(transform, vertices, vertexCount, color);
}

void Box2dDebugDraw::DrawClosedPolygon(const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context) {
    std::cout << "Box2dDebugDraw::DrawClosedPolygon" << std::endl;
    DrawPolygonWithNoTransform(vertices, vertexCount, color);
}

void Box2dDebugDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
    //std::cout << "Box2dDebugDraw::DrawSolidPolygonFcn" << std::endl;
    DrawPolygon(transform, vertices, vertexCount, color);
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

    // 2. Convert to screen coordinates
    int x1 = p1.x * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.x;
    int y1 = p1.y * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.y;
    int x2 = p2.x * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.x;
    int y2 = p2.y * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.y;

    // 3. Draw with SDL (simplified example - needs a filled circle function)
    DrawCircle(SDL_Point(x1, y1), radius * PIXELS_PER_METER); // Ends
    DrawCircle(SDL_Point(x2, y2), radius * PIXELS_PER_METER);
    //std::cout << "Box2dDebugDraw::DrawSolidCapsuleFcn - p1[" << x1 << "][" << y1 << "], p2[" << x2 << "][" << y2 << "]" << std::endl;
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

void Box2dDebugDraw::DrawPolygon(b2Transform transform, const b2Vec2 *vertices, int vertexCount, const b2HexColor color)
{
    //SDL_SetRenderDrawColor(sdl_renderer, 250, 0, 0);
    SDL_FPoint* sdlPoints = new SDL_FPoint[vertexCount + 1];
    for (int i = 0; i < vertexCount; ++i) {
        b2Vec2 p0 = b2TransformPoint(transform, vertices[i]);
        float x0 = p0.x * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.x;
        float y0 = p0.y * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.y;
        sdlPoints[i] = { x0, y0 };
    }
    sdlPoints[vertexCount] = sdlPoints[0]; // Close the polygon
    SDL_RenderLines(sdl_renderer, sdlPoints, vertexCount + 1);
    delete[] sdlPoints;
}

void Box2dDebugDraw::DrawPolygonWithNoTransform(const b2Vec2 *vertices, int vertexCount, const b2HexColor color) {
    //SDL_SetRenderDrawColor(sdl_renderer, 250, 0, 0);
    SDL_FPoint* sdlPoints = new SDL_FPoint[vertexCount + 1];
    for (int i = 0; i < vertexCount; ++i) {
        b2Vec2 p0 = {vertices[i].x, vertices[i].y};
        float x0 = p0.x * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.x;
        float y0 = p0.y * PIXELS_PER_METER - map_data::SharedMapData::get_instance()->scroll.y;
        sdlPoints[i] = { x0, y0 };
    }
    sdlPoints[vertexCount] = sdlPoints[0]; // Close the polygon
    SDL_RenderLines(sdl_renderer, sdlPoints, vertexCount + 1);
    delete[] sdlPoints;

}

void Box2dDebugDraw::setRenderer(SDL_Renderer *renderer) {
    sdl_renderer = renderer;
}

void Box2dDebugDraw::DrawCircle(SDL_Point center, float radius) {
    // 35 / 49 is a slightly biased approximation of 1/sqrt(2)
    const int arrSize = roundUpToMultipleOfEight( radius * 8 * 35 / 49 );
    SDL_FPoint points[arrSize];
    int       drawCount = 0;

    const int32_t diameter = (radius * 2);

    float x = (radius - 1);
    float y = 0;
    float tx = 1;
    float ty = 1;
    float error = (tx - diameter);

    //std::cout << "Box2dDebugDraw::DrawSolidCapsuleFcn - x[" << x << "], y[" << y << "], radius[" << radius << "]" << std::endl;

    while( x >= y )
    {
        // Each of the following renders an octant of the circle
        points[drawCount+0] = { center.x + x, center.y - y };
        points[drawCount+1] = { center.x + x, center.y + y };
        points[drawCount+2] = { center.x - x, center.y - y };
        points[drawCount+3] = { center.x - x, center.y + y };
        points[drawCount+4] = { center.x + y, center.y - x };
        points[drawCount+5] = { center.x + y, center.y + x };
        points[drawCount+6] = { center.x - y, center.y - x };
        points[drawCount+7] = { center.x - y, center.y + x };

        drawCount += 8;

        if( error <= 0 )
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if( error > 0 )
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }

    SDL_RenderPoints( sdl_renderer, points, drawCount );
}

int Box2dDebugDraw::roundUpToMultipleOfEight(int v) {
    return (v + (8 - 1)) & -8;
}


