//
//  StraightLineElement.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef StraightLineElement_hpp
#define StraightLineElement_hpp

#include "BaseLineElement.hpp"

class StraightLineElement : public BaseLineElement
{
public:
    StraightLineElement();
    virtual ~StraightLineElement() {}
    
public:
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) override;
    
    uint32_t TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj) override;
    
    uint32_t IntersectWithLine(const double& dAngle, const double& dX, const double& dY, Point2d arrCrossPos[s_nMaxArrCount]) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    inline Point2d TrsCmlToNE_Relative(const double& dCml) override { return Point2d(cos(dStartTanAngle), sin(dStartTanAngle)) * dCml; };
    
    double TrsCmlToAngle_Relative(const double& dCml) override { return dStartTanAngle; }
    
public:
    void InitData() override {};
    
    void AdjustData(const Point2d& pos) override {};
};

#endif /* StraightLineElement_hpp */
