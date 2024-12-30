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
    ///断链数组
    tagDLInfo* m_pDLArr;
    ///断链数组长度
    uint32_t m_nDLCount;
    ///缓存数组
    //unsigned char* m_Buffer;
    
    
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
    ///获取与直线的交点
    Point2d* IntersectWithLine(const double& dAngle, const double& dX, const double& dY, uint32_t& nArrCount);
};

#endif /* HorizontalObject_hpp */
