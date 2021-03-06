#include "Shapes.h"

namespace rectpack {

Rectangle::Rectangle(const cordType _width, const cordType _height, const int _data) :
    width(_width), height(_height), placed(false), data(_data) {}

Rectangle::Rectangle(const Rectangle &other) :
    width(other.width), height(other.height), placed(other.placed), data(other.data) {}

cordType Rectangle::getArea() const {
    return this->width * this->height;
}

cordType Rectangle::getPerimeter() const {
    return (this->width + this->height) * 2;
}

Rectangle::~Rectangle() {}

std::ostream &operator <<(std::ostream &out, const Rectangle &rect) {
    out << "[" << rect.width << " " << rect.height << " " << rect.placed << " " << rect.data << "]";
    return out;
}

void Rectangle::printToSvg(std::ostream &out) const {
    out << "\t<rect width=\"" << this->width << "\" height=\"" << this->height
        << "\" style=\"fill:rgb(255, 255, 255);stroke-width:1;stroke:rgb(0, 0, 0)\" />" << std::endl;
}

Box::Box(const cordType _x, const cordType _y, const cordType _width, const cordType _height, const float _angle, const int _data) :
    Rectangle(_width, _height, _data), x(_x), y(_y), angle(_angle) {}

Box::Box(const cordType _x, const cordType _y, const Rectangle &_rect, const float _angle) :
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
    out << "[" << box.x << " " << box.y << ", " << box.width << " " << box.height << " " << box.angle << ", " << box.data << "]";
    return out;
}

bool areCollidingAABB(const Box &first, const Box &second) {
    return (first.x < second.x +  second.width && first.x +  first.width > second.x) &&
           (first.y < second.y + second.height && first.y + first.height > second.y);
}

void Box::printToSvg(std::ostream &out) const {
    out << "\t<g transform = \"rotate(" << this->angle << ", " << this->x << ", " << this->y << ")\">" << std::endl;
    out << "\t\t<rect x = \"" << this->x << "\" y = \"" << this->y << "\" width=\"" << this->width << "\" height=\"" << this->height
        << "\" style=\"fill:rgb(255, 255, 210, 255);stroke-width:0.3;stroke:rgb(0, 255, 0)\" />" << std::endl;
    out << "\t</g>";
}

namespace rectangleEvaluators {
    unsigned long long areaEvaluator(const Rectangle &rect) {
        return (unsigned long long)rect.getArea();
    }
    unsigned long long perimeterEvaluator(const Rectangle &rect) {
        return (unsigned long long)rect.getPerimeter();
    }
    unsigned long long widthEvaluator(const Rectangle &rect) {
        return (unsigned long long)rect.width;
    }
    unsigned long long heightEvaluator(const Rectangle &rect) {
        return (unsigned long long)rect.height;
    }
};

};
