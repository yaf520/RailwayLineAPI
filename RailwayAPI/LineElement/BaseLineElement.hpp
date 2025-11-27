//
//  BaseLineElement.hpp
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
    BaseLineElement() : dStartCml(0.0), dTotalLen(0.0), dStartTanAngle(0.0), dEndTanAngle(0.0), eElementType(ElementType::Invalid), nIndex(0) {}
    virtual ~BaseLineElement() {}
    
public:
    Point2d pntStart;               //起点坐标
    Point2d pntEnd;                 //终点坐标
    double dStartCml;               //起点里程
    double dTotalLen;               //单段长度
    double dStartTanAngle;          //起点角度
    double dEndTanAngle;            //终点角度
    ElementType eElementType;       //线段类别
    uint32_t nIndex;                //线元索引
    
public:
    ///连续里程+投影距离 -> 坐标+切线角
    virtual bool TrsCmlDistToNE(double dCml,  double dDist, double& dX, double& dY, double& dAngle) const = 0;
    
    ///坐标 -> 连续里程+投影距离+切线角集合
    virtual uint32_t TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) const = 0;
    
    ///里程->高程+俯仰角
    virtual bool TrsCmlToHeight(double dCml, double& dHeight, double& dFyj) const = 0;
    
    ///计算与任意直线的交点
    virtual uint32_t IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) const = 0;
    
    ///导出平曲线范围里程内的线元数据
    virtual tagExportLineElement ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) const = 0;
    
protected:
    //相对里程->相对坐标
    virtual Point2d TrsCmlToNE_Relative(double dCml) const = 0;
    
    //相对里程->相对角度
    virtual double TrsCmlToAngle_Relative(double dCml) const = 0;
    
public:
    ///初始化数据
    virtual void InitData() = 0;
    
    ///调整数据
    virtual void AdjustData(const Vector2d& vec)
    {
        pntStart += vec;
        pntEnd += vec;
    }
};

#endif /* BaseLineElement_hpp */
