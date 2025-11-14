//
//  ArcElement.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef ArcElement_hpp
#define ArcElement_hpp

#include "BaseLineElement.hpp"
#include "BaseCalFun.hpp"

class ArcElement : public BaseLineElement
{
public:
    ArcElement();
    virtual ~ArcElement() {}
    
public:
    double dArcR;
    bool bTurnLeft;
    
public:
    bool TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const override;
    
    uint32_t TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) const override;
    
    bool TrsCmlToHeight(double dCml, double& dHeight, double& dFyj) const override;
    
    uint32_t IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) const override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) const override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(double dCml) const override;
    
    inline double TrsCmlToAngle_Relative(double dCml) const override { return dCml / dArcR; }
    
public:
    void InitData() override;
};

#endif /* ArcElement_hpp */
