#pragma once
#include "globals.h"

class Rectangle
{
    public:
    Rectangle(){}

    Rectangle(int x, int y, int width, int height):
        _x(x),
        _y(y),
        _height(height),
        _width(width)
    {}

    const int getCenterX() const {return this->_x + this->_width / 2;}
    const int getCenterY() const {return this->_y + this->_height / 2;}


    const int getLeft() const {return this->_x;}
    const int getRight() const {return this->_x + this->_width;}
    const int getTop() const {return this->_y;}
    const int getBottom() const {return this->_y + this->_height;}
    

    const int getWidth() const {return this->_width;}
    const int getHeight() const {return this->_height;}

    const int getSide(const sides::Side side) const
    {
        switch (side) {
            case sides::Side::LEFT:
                return this->getLeft();
            case sides::Side::RIGHT:
                return this->getRight();
            case sides::Side::TOP:
                return this->getTop();
            case sides::Side::BOTTOM:
                return this->getBottom();
            case sides::Side::NONE:
                return 0;
        }
    }

    const bool collidesWith(const Rectangle &other) const
    {
        return 
            (this->getRight() >= other.getLeft()) &&            
            (this->getLeft() <= other.getRight()) &&
            (this->getTop() <= other.getBottom()) &&
            (this->getBottom() >= other.getTop());
    }

    const bool isValiadRectangle() const
    {
        return (
            this->_x >= 0 &&
            this->_y >= 0 &&
            this->_width >= 0 &&
            this->_height >= 0       
        );
    }

    private:
        int _x, _y, _width, _height;
};