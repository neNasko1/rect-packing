#include "Shapes.h"

namespace rectpack {

Rectangle::Rectangle(const float _width, const float _height, const int _data) :
    width(_width), height(_height), placed(false), data(_data) {}

Rectangle::Rectangle(const Rectangle &other) :
    width(other.width), height(other.height), placed(other.placed), data(other.data) {}

float Rectangle::getArea() const {
    return this->width * this->height;
}

Rectangle Rectangle::flip() const {
    return Rectangle(this->height, this->width);
}

std::ostream &operator <<(std::ostream &out, const Rectangle &rect) {
    out << "[" << rect.width << " " << rect.height << " " << rect.placed << " " << rect.data << "]";
    return out;
}

void Rectangle::printToSvg(std::ostream &out) const {
    out << "\t<rect width=\"" << this->width << "\" height=\"" << this->height
        << "\" style=\"fill:rgb(255, 255, 255);stroke-width:1;stroke:rgb(0, 0, 0)\" />" << std::endl;
}

Rectangle::~Rectangle() {}

Box::Box(const float _x, const float _y, const float _width, const float _height, const float _angle, const int _data) :
    Rectangle(_width, _height, _data), x(_x), y(_y), angle(_angle) {}

Box::Box(const float _x, const float _y, const float _angle, const Rectangle &_rect) :
    Rectangle(_rect), x(_x), y(_y), angle(_angle) {}

Box::Box(const Box &other) :
    Rectangle(other.width, other.height, other.data), x(other.x), y(other.y), angle(other.angle) {}

Box::~Box() {}

bool Box::containsAABB(const Box &other) const {
    return this->x <= other.x && this->y <= other.y &&
           this->x + this->width >= other.x + other.width &&
           this->y + this->height >= other.y + other.height;
}

std::ostream &operator <<(std::ostream &out, const Box &box) {
    out << "[" << box.x << " " << box.y << ", " << box.width << " " << box.height << " " << box.data << "]";
    return out;
}

bool areCollidingAABB(const Box &first, const Box &second) {
    return (first.x <= second.x +  second.width && first.x +  first.width >= second.x) &&
           (first.y <= second.y + second.height && first.y + first.height >= second.y);
}

void Box::printToSvg(std::ostream &out) const {
    out << "\t<g transform = \"rotate(" << this->angle << ", " << this->x << ", " << this->y << ")\">" << std::endl;
    out << "\t\t<rect x = \"" << this->x << "\" y = \"" << this->y << "\" width=\"" << this->width << "\" height=\"" << this->height
        << "\" style=\"fill:rgb(255, 255, 255, 0);stroke-width:0.3;stroke:rgb(0, 255, 0)\" />" << std::endl;
    out << "\t\t<text x = \"" << this->x + 2 << "\" y = \"" << this->y + 8 << "\" fill = \"red\" style=\"font-size: 4pt;\">" << this->data << "</text>" << std::endl;
    out << "\t</g>";
}

};
