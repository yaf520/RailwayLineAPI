//
//  BasePoint.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef BasePoint_hpp
#define BasePoint_hpp

#include <math.h>

static const double MATH_PI = 3.1415926535897931;
static const double MATH_PI_2 = 1.5707963267948966;

///计算精度
static const double s_dCalPrecision = 1.0e-10;
///有效数字精度
static const double s_dValidPrecision = 1.0e-15;
///结果数组最大数目
static const int s_nMaxArrCount = 2;
///预估根计算分段精细度
static const int s_nEstimateRootSection = 10;
///缓和曲线计算积分精度
static const int s_nAddPreCount = 10;
///长度有效值
static const double s_dLenPrecision = 1.0e-5;

///基础二维点、向量结构体
typedef struct BasePoint2d
{
    double x;
    double y;
    
    BasePoint2d()
        :x(0.0), y(0.0)
    {
    }

    BasePoint2d(double xx, double yy)
        :x(xx), y(yy)
    {
    }
    
    BasePoint2d(const BasePoint2d& pos)
        :x(pos.x), y(pos.y)
    {
    }
    
    void Set(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    
    BasePoint2d operator + (const BasePoint2d& pos) const
    {
        return BasePoint2d(this->x + pos.x, this->y + pos.y);
    }
    
    BasePoint2d operator - (const BasePoint2d& pos) const
    {
        return BasePoint2d(this->x - pos.x, this->y - pos.y);
    }
    
    BasePoint2d& operator += (const BasePoint2d& pos)
    {
        this->x += pos.x;
        this->y += pos.y;
        return *this;
    }
    
    BasePoint2d& operator -= (const BasePoint2d& pos)
    {
        this->x -= pos.x;
        this->y -= pos.y;
        return *this;
    }
    
    BasePoint2d operator * (const int& i) const
    {
        return BasePoint2d(this->x * i, this->y * i);
    }
    
    BasePoint2d operator * (const double& d) const
    {
        return BasePoint2d(this->x * d, this->y * d);
    }
    
    BasePoint2d operator * (const float& f) const
    {
        return BasePoint2d(this->x * f, this->y * f);
    }
    
    BasePoint2d operator / (const int& i) const
    {
        return BasePoint2d(this->x / i, this->y / i);
    }
    
    BasePoint2d operator / (const double& d) const
    {
        return BasePoint2d(this->x / d, this->y / d);
    }
    
    BasePoint2d operator / (const float& f) const
    {
        return BasePoint2d(this->x / f, this->y / f);
    }
    
    bool operator == (const BasePoint2d& pos) const
    {
        return (this->x == pos.x && this->y == pos.y);
    }
    
    bool operator != (const BasePoint2d& pos) const
    {
        return (this->x != pos.x || this->y != pos.y);
    }
    
    double distanceTo(const BasePoint2d& pos) const
    {
        return sqrt(pow(this->x - pos.x, 2) + pow(this->y - pos.y, 2));
    }
    
    double dot(const BasePoint2d& pos) const
    {
        return this->x * pos.x + this->y * pos.y;
    }
    
    double cross(const BasePoint2d& pos) const
    {
        return this->x * pos.y - pos.x * this->y;
    }
    
    double model() const
    {
        return sqrt(x * x + y * y);
    }
    
    bool isZeroVec() const
    {
        return (abs(x) <= s_dCalPrecision && abs(y) <= s_dCalPrecision);
    }
    
    double angle() const
    {
        if (abs(x) < s_dCalPrecision)
            return (y >= 0.0 ? MATH_PI_2 : MATH_PI * 1.5);
        
        double dAngle = atan(y / x);
        if (y >= 0.0)
        {
            if (x < -s_dCalPrecision)
                dAngle += MATH_PI;
        }
        else
        {
            if (x < -s_dCalPrecision)
                dAngle += MATH_PI;
            else
                dAngle += MATH_PI * 2.0;
        }
        
        return dAngle;
    }
}Point2d, Vector2d;

#endif /* BasePoint_hpp */
