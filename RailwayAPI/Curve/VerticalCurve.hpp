//
//  VerticalObject.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#ifndef VerticalObject_hpp
#define VerticalObject_hpp

#include "LineElementManager.hpp"
#include "MileConvert.hpp"

class VerticalCurve : public LineElementManager
{
public:
    VerticalCurve(MileConvert* pMileConvert);
    virtual ~VerticalCurve() { }
    
private:
    ///平面计算指针
    MileConvert* m_pMileConvert;
    
public:
    void SetSlopeData(const tagSlopeInfo* pSlopeInfo, uint32_t iSlopeCount);
    
private:
    ///计算点的投影属于哪一线元
    BaseLineElement* PosBelongTo(Point2d pos) override { return nullptr;}
    ///计算里程属于哪一线元
    BaseLineElement* CmlBelongTo(double dCml) override;
    
public:
    ///里程->高程+俯仰角
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dAngle);
};

#endif /* VerticalObject_hpp */
