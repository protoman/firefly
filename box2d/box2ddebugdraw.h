#ifndef BOX2DDEBUGDRAW_H
#define BOX2DDEBUGDRAW_H

#include <box2d/types.h>
#include <SDL2/SDL.h>

#define PIXELS_PER_METER 40

class Box2dDebugDraw : public b2DebugDraw
{
public:
    Box2dDebugDraw(SDL_Renderer* renderer);
    void DrawPolygonFcn(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);

    /// Draw a solid closed polygon provided in CCW order.
    void DrawSolidPolygonFcn(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color,
                                void* context );

    /// Draw a circle.
    void DrawCircleFcn( b2Vec2 center, float radius, b2HexColor color, void* context );

    /// Draw a solid circle.
    void DrawSolidCircleFcn( b2Transform transform, float radius, b2HexColor color, void* context );

    /// Draw a solid capsule.
    void DrawSolidCapsuleFcn( b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context );

    /// Draw a line segment.
    void DrawSegmentFcn( b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context );

    /// Draw a transform. Choose your own length scale.
    void DrawTransformFcn( b2Transform transform, void* context );

    /// Draw a point.
    void DrawPointFcn( b2Vec2 p, float size, b2HexColor color, void* context );

    /// Draw a string in world space
    void DrawStringFcn( b2Vec2 p, const char* s, b2HexColor color, void* context );

    void DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2HexColor color);

private:
    SDL_Renderer *sdl_renderer;

};


#endif // BOX2DDEBUGDRAW_H
