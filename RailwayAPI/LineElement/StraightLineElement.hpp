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
    
    bool TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle) override;
    
    bool TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj) override;
    
    tagExportLineElement* ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) override;
    
    bool PosBelongSelf(const double& dX, const double& dY) override;
    
protected:
    //相对里程->相对坐标
    Point2d TrsCmlToNE_Relative(const double& dCml) override;
    
    double TrsCmlToAngle_Relative(const double& dCml) override { return 0.0;}
    
    int PosBelongSelf(const Point2d& pos) override;
    
public:
    void InitData() override {};
    
    void AdjustData(const Point2d& pos) override {};
};

#endif /* StraightLineElement_hpp */
