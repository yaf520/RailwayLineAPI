//
//  CurveElement.hpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef CurveElement_hpp
#define CurveElement_hpp

#include "BaseLineElement.hpp"
#include "BaseCalFun.hpp"

class CurveElement : public BaseLineElement
{
public:
    CurveElement() {}
    virtual ~CurveElement() {}
    
public:
    double m_dArcR;
    bool m_bTurnLeft;
    
public:
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle) override;
    
    bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dAngle) override { return false;};
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
    tagExportLineElement* ExportVerCurve(double dStartCml, double dEndCml, double dArcStep, double dScaleX, double dScaleY) override { return nullptr;};
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(const double& dCml) override;
    
    double TrsCmlToAngle_Relative(const double& dCml) override;
};


#endif /* CurveElement_hpp */
