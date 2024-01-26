//
//  BaseLineElement.hpp
//  icon_dg
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef BaseLineElement_hpp
#define BaseLineElement_hpp

#include "BasePoint.hpp"
#include "BaseStruct.hpp"

#define USE_VECTOR

/// 线元基类
class BaseLineElement
{
public:
    BaseLineElement() {}
    virtual ~BaseLineElement() {}
    
public:
    Point2d m_posStart;               //起点坐标
    Point2d m_posEnd;                 //终点坐标
    double m_dStartCml;               //起点里程
    double m_dTotalLen;               //单段长度
    double m_dStartTanAngle;          //起点角度
    double m_dEndTanAngle;            //终点角度
    ElementType m_eElementType;       //线段类别
    uint32_t m_nIndex;                //线元索引
    
public:
    ///连续里程+投影距离 -> 坐标+切线角
    virtual bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) = 0;
    
    ///坐标 -> 连续里程+投影距离+切线角
    virtual bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle) = 0;
    
    ///里程->高程+俯仰角
    virtual bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj) = 0;
    
protected:
    //相对里程->相对坐标
    virtual Point2d TrsCmlToNE_Relative(const double& dCml) = 0;
    
    //相对里程->相对角度
    virtual double TrsCmlToAngle_Relative(const double& dCml) = 0;
};

#endif /* BaseLineElement_hpp */
