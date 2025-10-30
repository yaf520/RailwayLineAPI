//
//  StraightLineElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <assert.h>
#include "StraightLineElement.hpp"
#include "BaseCalFun.hpp"

StraightLineElement::StraightLineElement()
{
    eElementType = ElementType::Line;
}

bool StraightLineElement::TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const
{
    assert(dCml >= dStartCml - s_dCalPrecision && dCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dCml < dStartCml - s_dCalPrecision || dCml > dStartCml + dTotalLen + s_dCalPrecision)
        return false;
    
    double dDeltaLen = dCml - dStartCml;
    Point2d posDist = pntStart + TrsCmlToNE_Relative(dDeltaLen);
    dAngle = dStartTanAngle;
    
    //投影点
    if (dDist != 0.0)
    {
        double dDistAngle = dAngle + (dDist > 0 ? MATH_PI_2 : -MATH_PI_2);
        posDist += Point2d(cos(dDistAngle), sin(dDistAngle)) * abs(dDist);
    }
    
    dX = posDist.x;
    dY = posDist.y;
    
    return true;
}

uint32_t StraightLineElement::TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) const
{
    double dPecent = 0.0;
    Point2d posBase(dX, dY);
    Point2d posPrj = BaseCalFun::PointToLineProjection(posBase, pntStart, pntEnd, dPecent);
    
    if (dPecent < -s_dCalPrecision || dPecent > 1.0 + s_dCalPrecision) return 0;
    
    arrAngle[0] = dStartTanAngle;
    arrCml[0] = dStartCml + posPrj.distanceTo(pntStart);
    
    //左右判断
    Vector2d vec1 = pntEnd - posPrj;
    Vector2d vec2 = posBase - posPrj;
    double dCross = vec1.cross(vec2);
    arrDist[0] = posPrj.distanceTo(posBase) * (dCross > 0.0 ? 1.0 : -1.0);
    
    return 1;
}

uint32_t StraightLineElement::IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) const
{
    if (abs(dAngle - dStartTanAngle) < s_dCalPrecision)
        return 0;
    
    Vector2d vec1(cos(dAngle), sin(dAngle));
    Vector2d vec2(cos(dStartTanAngle), sin(dStartTanAngle));
    Point2d p1(dX, dY);
    Point2d p2 = pntStart;
    
    double t = (p2 - p1).cross(vec2) / vec1.cross(vec2);
    Point2d posCross = p1 + vec1 * t;
    
    double dPecent = 0.0;
    BaseCalFun::PointToLineProjection(posCross, pntStart, pntEnd, dPecent);
    if (dPecent < -s_dCalPrecision || dPecent > 1.0 + s_dCalPrecision)
        return 0;
    
    arrCrossPos[0] = posCross;
    
    return 1;
}

tagExportLineElement* StraightLineElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) const
{
    assert(dStartCml >= dStartCml - s_dCalPrecision && dEndCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dStartCml < dStartCml - s_dCalPrecision || dEndCml > dStartCml + dTotalLen + s_dCalPrecision)
        return nullptr;
    
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->nPosCount = 2;
    pRet->eLineType = eElementType;
    pRet->pArrPos = new PointExport[2];
    TrsCmlDistToNE(dStartCml, dDist, pRet->pArrPos[0].dX, pRet->pArrPos[0].dY, dAngle);
    TrsCmlDistToNE(dEndCml, dDist, pRet->pArrPos[1].dX, pRet->pArrPos[1].dY, dAngle);
    
    return pRet;
}

bool StraightLineElement::TrsCmlToHeight(double dCml, double& dHeight, double& dFyj) const
{
    double dDeltaCml = dCml - pntStart.x;
    dHeight = pntStart.y + tan(dStartTanAngle) * dDeltaCml;
    dFyj = dStartTanAngle;
    
    return true;
}
