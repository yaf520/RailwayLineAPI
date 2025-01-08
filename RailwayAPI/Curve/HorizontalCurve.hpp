//
//  HorizontalObject.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#ifndef HorizontalObject_hpp
#define HorizontalObject_hpp

#include "LineElementManager.hpp"
#include "DyArray.hpp"

class HorizontalCurve : public LineElementManager
{
public:
    HorizontalCurve();
    
    virtual ~HorizontalCurve();
    
private:
    ///计算点的投影属于哪一线元
    BaseLineElement* PosBelongTo(Point2d pos) override;
    ///计算里程属于哪一线元
    BaseLineElement* CmlBelongTo(double dCml) override;
    
public:
    ///里程+投影计算坐标+切线角
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle);
    ///坐标计算投影点里程+投影距离+切线角
    bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle);
    ///坐标计算投影点里程+投影距离+切线角集合
    tagCmlDistAngle* TrsNEToCmlDist(const double& dX, const double& dY, uint32_t& nArrCount);
    ///坐标计算投影点里程+投影距离+切线角集合
    DyArray<tagCmlDistAngle> TrsNEToCmlDist(const double& dX, const double& dY);
    ///获取与直线的交点
    Point2d* IntersectWithLine(const double& dAngle, const double& dX, const double& dY, uint32_t& nArrCount);
    ///获取与直线的交点
    DyArray<Point2d> IntersectWithLine(const double& dAngle, const double& dX, const double& dY);
};

#endif /* HorizontalObject_hpp */
