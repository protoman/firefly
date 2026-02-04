//
// Created by iuri on 03/02/2026.
//

#include "TiledMapObjectLayer.hpp"

#include <iostream>

std::vector<st_float_position> TiledMapObjectLayer::init(const tmx::Map &map, unsigned int layerIndex) {
    std::vector<st_float_position> res;
    const auto& layers = map.getLayers();
    assert(layers[layerIndex]->getType() == tmx::Layer::Type::Object);

    const tmx::ObjectGroup& layer = layers[layerIndex]->getLayerAs<tmx::ObjectGroup>();

    for (tmx::Object object : layer.getObjects()) {
        std::cout << "Object[" << object.getName() << "]" << std::endl;
        if (object.getShape() == tmx::Object::Shape::Polygon) {
            std::vector<tmx::Vector2f> points = object.getPoints();
            for (tmx::Vector2f point : points) {
                res.emplace_back(st_float_position(point.x, point.y));
            }
        }
    }
    return res;
}
