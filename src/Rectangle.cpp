#include "Rectangle.h"
#include <iostream>

rectangle::rectangle(double a,double b)
{
    length=a;
    width=b;
    area=a*b;
}
double rectangle::getLength()
{
    return length;
}
double rectangle::getWidth()
{
    return width;
}
double rectangle::getArea()
{
    return area;
}