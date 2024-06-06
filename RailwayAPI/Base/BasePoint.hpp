//
//  BasePoint.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef BasePoint_hpp
#define BasePoint_hpp

#include "math.h"

static const double MATH_PI = 3.14159265358979323846264338327950288;
static const double MATH_PI_2 = 1.57079632679489661923132169163975144;

///计算精度
static const double s_dCalPrecision = 1.0e-10;
///有效数字精度
static const double s_dValidPrecision = 1.0e-15;
///线元投影点最大数目
static const int s_nMaxProCount = 5;
///预估根计算分段精细度
static const int s_nEstimateRootSection = 10;
///缓和曲线计算积分精度
static const int s_nAddPreCount = 10;

///基础二维点、向量结构体
typedef struct BasePoint2d
{
    double x;
    double y;
    
    BasePoint2d()
    {
        x = 0.0;
        y = 0.0;
    }

    BasePoint2d(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    
    BasePoint2d(const BasePoint2d& pos)
    {
        this->x = pos.x;
        this->y = pos.y;
    }
    
    void Set(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    
    virtual BasePoint2d operator = (const BasePoint2d& pos)
    {
        this->x = pos.x;
        this->y = pos.y;
        return *this;
    }
    
    virtual BasePoint2d operator + (const BasePoint2d& pos)
    {
        return BasePoint2d(this->x + pos.x, this->y + pos.y);
    }
    
    virtual BasePoint2d operator + (const BasePoint2d& pos) const
    {
        return BasePoint2d(this->x + pos.x, this->y + pos.y);
    }
    
    virtual BasePoint2d operator - (const BasePoint2d& pos)
    {
        return BasePoint2d(this->x - pos.x, this->y - pos.y);
    }
    
    virtual BasePoint2d operator - (const BasePoint2d& pos) const
    {
        return BasePoint2d(this->x - pos.x, this->y - pos.y);
    }
    
    virtual BasePoint2d& operator += (const BasePoint2d& pos)
    {
        this->x += pos.x;
        this->y += pos.y;
        return *this;
    }
    
    virtual BasePoint2d& operator -= (const BasePoint2d& pos)
    {
        this->x -= pos.x;
        this->y -= pos.y;
        return *this;
    }
    
    virtual BasePoint2d operator * (const int& i)
    {
        return BasePoint2d(this->x * i, this->y * i);
    }
    
    virtual BasePoint2d operator * (const double& d)
    {
        return BasePoint2d(this->x * d, this->y * d);
    }
    
    virtual BasePoint2d operator * (const float& f)
    {
        return BasePoint2d(this->x * f, this->y * f);
    }
    
    virtual BasePoint2d operator / (const int& i)
    {
        return BasePoint2d(this->x / i, this->y / i);
    }
    
    virtual BasePoint2d operator / (const double& d)
    {
        return BasePoint2d(this->x / d, this->y / d);
    }
    
    virtual BasePoint2d operator / (const float& f)
    {
        return BasePoint2d(this->x / f, this->y / f);
    }
    
    virtual bool operator == (const BasePoint2d& pos)
    {
        return (this->x == pos.x && this->y == pos.y);
    }
    
    virtual bool operator != (const BasePoint2d& pos)
    {
        return (this->x != pos.x || this->y != pos.y);
    }
    
    virtual double distanceTo(const BasePoint2d& pos)
    {
        return sqrt(pow(this->x - pos.x, 2) + pow(this->y - pos.y, 2));
    }
    
    virtual double distanceTo(const BasePoint2d& pos) const
    {
        return sqrt(pow(this->x - pos.x, 2) + pow(this->y - pos.y, 2));
    }
    
    virtual double dot(const BasePoint2d& pos)
    {
        return this->x * pos.x + this->y * pos.y;
    }
    
    virtual double dot(const BasePoint2d& pos) const
    {
        return this->x * pos.x + this->y * pos.y;
    }
    
    virtual double cross(const BasePoint2d& pos)
    {
        return this->x * pos.y - pos.x * this->y;
    }
    
    virtual double cross(const BasePoint2d& pos) const
    {
        return this->x * pos.y - pos.x * this->y;
    }
    
    virtual double model()
    {
        return sqrt(x * x + y * y);
    }
    
    virtual double model() const
    {
        return sqrt(x * x + y * y);
    }
    
    virtual bool isZeroVec()
    {
        return (abs(x) <= s_dCalPrecision && abs(y) <= s_dCalPrecision);
    }
    
    virtual bool isZeroVec() const
    {
        return (abs(x) <= s_dCalPrecision && abs(y) <= s_dCalPrecision);
    }
}Point2d, Vector2d;

#endif /* BasePoint_hpp */
