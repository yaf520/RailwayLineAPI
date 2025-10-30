//
//  HorizontalObject.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/6.
//

#ifndef HorizontalObject_hpp
#define HorizontalObject_hpp

#include "LineElementManager.hpp"

class HorizontalCurve : public LineElementManager
{
public:
    HorizontalCurve();
    
    virtual ~HorizontalCurve();
    
private:
    ///计算点的投影属于哪一线元
    BaseLineElement* PosBelongTo(Point2d pos) const override;
    ///计算里程属于哪一线元
    BaseLineElement* CmlBelongTo(double dCml) const override;
    
public:
    ///里程+投影计算坐标+切线角
    virtual bool TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const;
    ///坐标计算投影点里程+投影距离+切线角
    virtual bool TrsNEToCmlDist(double dX, double dY, double& dCml, double& dDist, double& dAngle) const;
    ///坐标计算投影点里程+投影距离+切线角集合
    virtual tagCmlDistAngle* TrsNEToCmlDist(double dX, double dY, uint32_t& nArrCount) const;
    ///获取与直线的交点
    virtual Point2d* IntersectWithLine(double dAngle, double dX, double dY, uint32_t& nArrCount) const;
    ///导出偏移曲线交点信息
    virtual tagJDInfo* ExportOffsetJDInfo(double dOffsetDist, int& nJDCount) const;
};

#endif /* HorizontalObject_hpp */
