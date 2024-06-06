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
    double m_dArcR;
    bool m_bTurnLeft;
    
public:
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) override;
    
    bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle) override;
    
    uint32_t TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxProCount], double arrDist[s_nMaxProCount], double arrAngle[s_nMaxProCount]) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(const double& dCml) override;
    
    double TrsCmlToAngle_Relative(const double& dCml) override { return dCml / m_dArcR; };
    
public:
    void InitData() override;
    
    void AdjustData(const Point2d& pos) override;
};

#endif /* ArcElement_hpp */
