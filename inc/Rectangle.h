#ifndef RECTANGLE_H
#define RECTANGLE_H

class rectangle{
    private:
    double length,width,area;
    public:
    rectangle(double a,double b);
    double getLength();
    double getWidth();    
    double getArea();
};

#endif