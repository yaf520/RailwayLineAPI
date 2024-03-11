//
//  StraightLineElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <cassert>
#include "StraightLineElement.hpp"
#include "BaseCalFun.hpp"

bool StraightLineElement::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    assert(dCml >= m_dStartCml);
    if (dCml - m_dStartCml < 0) return false;
    
    double dDeltaLen = dCml - m_dStartCml;
    Point2d posDist = m_posStart + TrsCmlToNE_Relative(dDeltaLen);
    dAngle = m_dStartTanAngle;
    
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

bool StraightLineElement::TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle)
{
    double dPecent = 0.0;
    Point2d posBase(dX, dY);
    Point2d posPrj = BaseCalFun::PointToLineProjection(posBase, m_posStart, m_posEnd, dPecent);
    
    assert(dPecent >= -dCalPrecision_1 && dPecent <= 1.0 + dCalPrecision_1);
    if (dPecent < -dCalPrecision_1 || dPecent > 1.0 + dCalPrecision_1) return false;
    
    dAngle = m_dStartTanAngle;
    dCml = m_dStartCml + posPrj.distanceTo(m_posStart);
    
    //左右判断
    Vector2d vec1 = m_posEnd - posPrj;
    Vector2d vec2 = posBase - posPrj;
    double dCross = vec1.cross(vec2);
    dDist = posPrj.distanceTo(posBase) * (dCross > 0.0 ? 1.0 : -1.0);
    
    return true;
}

tagExportLineElement* StraightLineElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    double dDeltaLen = BaseCalFun::Round(dEndCml - dStartCml);
    double dTotalLenTmp = BaseCalFun::Round(m_dTotalLen);
    assert(dStartCml >= m_dStartCml && dDeltaLen <= dTotalLenTmp);
    if (dStartCml < m_dStartCml || dDeltaLen > dTotalLenTmp)
        return nullptr;
    
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->nPosCount = 2;
    pRet->eLineType = m_eElementType;
    pRet->pArrPos = new PointExport[2];
    TrsCmlDistToNE(dStartCml, dDist, pRet->pArrPos[0].dX, pRet->pArrPos[0].dY, dAngle);
    TrsCmlDistToNE(dEndCml, dDist, pRet->pArrPos[1].dX, pRet->pArrPos[1].dY, dAngle);
    
    return pRet;
}

tagExportLineElement* StraightLineElement::ExportVerCurve(double dStartCml, double dEndCml, double dArcStep, double dScaleX, double dScaleY)
{
    return nullptr;
}

Point2d StraightLineElement::TrsCmlToNE_Relative(const double& dCml)
{
    return Point2d(cos(m_dStartTanAngle), sin(m_dStartTanAngle)) * dCml;
}

bool StraightLineElement::TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj)
{
    double dDeltaCml = dCml - m_posStart.x;
    dHeight = m_posStart.y + tan(m_dStartTanAngle) * dDeltaCml;
    dFyj = m_dStartTanAngle;
    
    return true;
}
