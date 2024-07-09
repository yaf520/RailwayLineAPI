//
//  ArcElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <cassert>
#include "ArcElement.hpp"

ArcElement::ArcElement()
    : BaseLineElement()
{
    m_eElementType = ElementType::Arc;
}

bool ArcElement::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    assert(dCml >= m_dStartCml);
    if (dCml - m_dStartCml < 0) return false;
    
    double dDeltaLen = dCml - m_dStartCml;
    double dArcAngle = TrsCmlToAngle_Relative(dDeltaLen);
    Point2d posDist = BaseCalFun::TransferPos(m_posStart, TrsCmlToNE_Relative(dDeltaLen), m_bTurnLeft, -m_dStartTanAngle);
    dAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
    
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

uint32_t ArcElement::TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount])
{
    //计算圆心坐标
    double dAngleR = m_dStartTanAngle + (m_bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d O = m_posStart + Point2d(cos(dAngleR), sin(dAngleR)) * m_dArcR;
    //目标点
    Point2d P(dX, dY);
    //向量
    Vector2d vecOP = P - O;
    if (vecOP.isZeroVec())
        return 0;
    
    double dPecent = m_dArcR / vecOP.model();
    dPecent = BaseCalFun::Round(dPecent);
    //两个交点
    Point2d A(O.x + vecOP.x * dPecent, O.y + vecOP.y * dPecent);
    Point2d B(O.x - vecOP.x * dPecent, O.y - vecOP.y * dPecent);
    
    Vector2d vecOA = A - O;
    Vector2d vecOB = B - O;
    assert(abs(vecOA.model() - m_dArcR) < 0.01 && abs(vecOB.model() - m_dArcR) < 0.01);
    
    //起点向量
    Vector2d vecStart = m_posStart - O;
    //圆心角
    double dArcAngle = TrsCmlToAngle_Relative(m_dTotalLen);
    
    double dAngleA = BaseCalFun::CalAngleBy2Vec(vecStart, vecOA);
    if (!m_bTurnLeft)
        dAngleA *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleA);
    
    double dAngleB = BaseCalFun::CalAngleBy2Vec(vecStart, vecOB);
    if (!m_bTurnLeft)
        dAngleB *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleB);
    
    //可能会有两个点
    bool bIsA = (dAngleA <= dArcAngle + s_dCalPrecision);
    bool bIsB = (dAngleB <= dArcAngle + s_dCalPrecision);
    uint32_t nCurCount = 0;
    if (bIsA)
    {
        arrCml[nCurCount] = m_dStartCml + m_dArcR * dAngleA;
        //切线角度
        arrAngle[nCurCount] = m_dStartTanAngle + (m_bTurnLeft ? dAngleA : -dAngleA);
        //投影距离
        Vector2d vecTan(cos(arrAngle[nCurCount]), sin(arrAngle[nCurCount]));
        Vector2d vecAP = P - A;
        double dCross = vecTan.cross(vecAP);
        arrDist[nCurCount] = vecAP.model() * (dCross > 0 ? 1.0 : -1.0);
        
        nCurCount++;
    }
    if (bIsB)
    {
        arrCml[nCurCount] = m_dStartCml + m_dArcR * dAngleB;
        //切线角度
        arrAngle[nCurCount] = m_dStartTanAngle + (m_bTurnLeft ? dAngleB : -dAngleB);
        //投影距离
        Vector2d vecTan(cos(arrAngle[nCurCount]), sin(arrAngle[nCurCount]));
        Vector2d vecBP = P - B;
        double dCross = vecTan.cross(vecBP);
        arrDist[nCurCount] = vecBP.model() * (dCross > 0 ? 1.0 : -1.0);
        
        nCurCount++;
    }
    
    return nCurCount;
}

bool ArcElement::TrsCmlToHeight(const double& dCml, double& dHeight, double& dFyj)
{
    //1.计算圆心坐标
    double dAngleR = m_dStartTanAngle + (m_bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d posCenter = m_posStart + Point2d(cos(dAngleR), sin(dAngleR)) * m_dArcR;
    //2.通过圆方程求y的坐标
    double dY1 = posCenter.y + sqrt(pow(m_dArcR, 2) - pow(dCml - posCenter.x, 2));
    double dY2 = posCenter.y - sqrt(pow(m_dArcR, 2) - pow(dCml - posCenter.x, 2));
    //3.排除其中一个
    Point2d pos1(dCml, dY1);
    Point2d pos2(dCml, dY2);
    
    double dAngle1 = MATH_PI - abs(BaseCalFun::CalTurnAngle(m_posStart, posCenter, pos1));
    double dAngle2 = MATH_PI - abs(BaseCalFun::CalTurnAngle(m_posStart, posCenter, pos2));
    double dTurnAngle = MATH_PI - abs(BaseCalFun::CalTurnAngle(m_posStart, posCenter, m_posEnd));
    
    dHeight = (dAngle1 < dTurnAngle ? dY1 : dY2);
    double dArcAngle = (dAngle1 < dTurnAngle ? dAngle1 : dAngle2);
    dFyj = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
    
    return true;
}

uint32_t ArcElement::IntersectWithLine(const double& dAngle, const double& dX, const double& dY, Point2d arrCrossPos[s_nMaxArrCount])
{
    //直线起点
    Point2d posOnlineA(dX, dY);
    //直线方向
    Vector2d vecDir(cos(dAngle), sin(dAngle));
    //直线终点
    Point2d posOnlineB = posOnlineA + vecDir;
    
    //计算圆心坐标
    double dAngleR = m_dStartTanAngle + (m_bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d O = m_posStart + Point2d(cos(dAngleR), sin(dAngleR)) * m_dArcR;
    
    //圆心在直线上的投影坐标
    double dPecent = 0.0;
    Point2d posProj = BaseCalFun::PointToLineProjection(O, posOnlineA, posOnlineB, dPecent);
    double dDis = posProj.distanceTo(O);
    if (dDis > m_dArcR)
        return 0;
    
    double dModel = sqrt(m_dArcR * m_dArcR - dDis * dDis);
    //两个交点
    Point2d A = posProj + vecDir * dModel;
    Point2d B = posProj - vecDir * dModel;
    
    Vector2d vecOA = A - O;
    Vector2d vecOB = B - O;
    
    //起点向量
    Vector2d vecStart = m_posStart - O;
    //圆心角
    double dArcAngle = TrsCmlToAngle_Relative(m_dTotalLen);
    
    double dAngleA = BaseCalFun::CalAngleBy2Vec(vecStart, vecOA);
    if (!m_bTurnLeft)
        dAngleA *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleA);
    
    double dAngleB = BaseCalFun::CalAngleBy2Vec(vecStart, vecOB);
    if (!m_bTurnLeft)
        dAngleB *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleB);
    
    //可能会有两个点
    bool bIsA = (dAngleA <= dArcAngle + s_dCalPrecision);
    bool bIsB = (dAngleB <= dArcAngle + s_dCalPrecision);
    uint32_t nCurCount = 0;
    if (bIsA)
        arrCrossPos[nCurCount++] = A;
    if (bIsB)
        arrCrossPos[nCurCount++] = B;
    
    return nCurCount;
}

tagExportLineElement* ArcElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    double dDeltaLen = BaseCalFun::Round(dEndCml - dStartCml);
    double dTotalLenTmp = BaseCalFun::Round(m_dTotalLen);
    assert(dStartCml >= m_dStartCml && dDeltaLen <= dTotalLenTmp);
    if (dStartCml < m_dStartCml || dDeltaLen > dTotalLenTmp)
        return nullptr;
    
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->nPosCount = 3;
    pRet->eLineType = m_eElementType;
    pRet->pArrPos = new PointExport[3];
    
    TrsCmlDistToNE(dStartCml, dDist, pRet->pArrPos[0].dX, pRet->pArrPos[0].dY, dAngle);
    TrsCmlDistToNE((dStartCml + dEndCml) / 2.0, dDist, pRet->pArrPos[2].dX, pRet->pArrPos[2].dY, dAngle);
    TrsCmlDistToNE(dEndCml, dDist, pRet->pArrPos[1].dX, pRet->pArrPos[1].dY, dAngle);
    
    return pRet;
}

Point2d ArcElement::TrsCmlToNE_Relative(const double& dCml)
{
    double dArcAngle = dCml / m_dArcR;
    double dDeltaX = sin(dArcAngle) * m_dArcR;
    double dDeltaY = (1 - cos(dArcAngle)) * m_dArcR;
    return Point2d(dDeltaX, dDeltaY);
}

void ArcElement::InitData()
{
    m_posEnd = BaseCalFun::TransferPos(m_posStart, TrsCmlToNE_Relative(m_dTotalLen), m_bTurnLeft, -m_dStartTanAngle);
    double dArcAngle = TrsCmlToAngle_Relative(m_dTotalLen);
    m_dEndTanAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
}

void ArcElement::AdjustData(const Point2d& pos)
{
    m_posStart += pos;
    m_posEnd += pos;
}

