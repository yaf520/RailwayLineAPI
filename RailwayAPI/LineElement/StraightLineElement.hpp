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
    
    uint32_t TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxProCount], double arrDist[s_nMaxProCount], double arrAngle[s_nMaxProCount]) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj) override;
    
    uint32_t GetCrossPos(const double& dAngle, const double& dX, const double& dY, Point2d arrCrossPos[s_nMaxProCount]) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    inline Point2d TrsCmlToNE_Relative(const double& dCml) override { return Point2d(cos(m_dStartTanAngle), sin(m_dStartTanAngle)) * dCml; };
    
    double TrsCmlToAngle_Relative(const double& dCml) override { return m_dStartTanAngle; }
    
public:
    void InitData() override {};
    
    void AdjustData(const Point2d& pos) override {};
};

#endif /* StraightLineElement_hpp */
