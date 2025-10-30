//
//  ArcElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <assert.h>
#include "ArcElement.hpp"

ArcElement::ArcElement()
    : BaseLineElement()
{
    eElementType = ElementType::Arc;
}

bool ArcElement::TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const
{
    assert(dCml >= dStartCml - s_dCalPrecision && dCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dCml < dStartCml - s_dCalPrecision || dCml > dStartCml + dTotalLen + s_dCalPrecision)
        return false;
    
    double dDeltaLen = dCml - dStartCml;
    double dArcAngle = TrsCmlToAngle_Relative(dDeltaLen);
    Point2d posDist = BaseCalFun::TransferPos(pntStart, TrsCmlToNE_Relative(dDeltaLen), bTurnLeft, -dStartTanAngle);
    dAngle = dStartTanAngle + (bTurnLeft ? dArcAngle : -dArcAngle);
    
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

uint32_t ArcElement::TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) const
{
    //计算圆心坐标
    double dAngleR = dStartTanAngle + (bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d O = pntStart + Point2d(cos(dAngleR), sin(dAngleR)) * dArcR;
    //目标点
    Point2d P(dX, dY);
    //向量
    Vector2d vecOP = P - O;
    if (vecOP.isZeroVec())
        return 0;
    
    double dPecent = dArcR / vecOP.model();
    dPecent = BaseCalFun::Round(dPecent);
    //两个交点
    Point2d A(O.x + vecOP.x * dPecent, O.y + vecOP.y * dPecent);
    Point2d B(O.x - vecOP.x * dPecent, O.y - vecOP.y * dPecent);
    
    Vector2d vecOA = A - O;
    Vector2d vecOB = B - O;
    assert(abs(vecOA.model() - dArcR) < 0.01 && abs(vecOB.model() - dArcR) < 0.01);
    
    //起点向量
    Vector2d vecStart = pntStart - O;
    //圆心角
    double dArcAngle = TrsCmlToAngle_Relative(dTotalLen);
    
    double dAngleA = BaseCalFun::CalAngleBy2Vec(vecStart, vecOA);
    if (!bTurnLeft)
        dAngleA *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleA);
    
    double dAngleB = BaseCalFun::CalAngleBy2Vec(vecStart, vecOB);
    if (!bTurnLeft)
        dAngleB *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleB);
    
    //可能会有两个点
    bool bIsA = (dAngleA <= dArcAngle + s_dCalPrecision);
    bool bIsB = (dAngleB <= dArcAngle + s_dCalPrecision);
    uint32_t nCurCount = 0;
    if (bIsA)
    {
        arrCml[nCurCount] = dStartCml + dArcR * dAngleA;
        //切线角度
        arrAngle[nCurCount] = dStartTanAngle + (bTurnLeft ? dAngleA : -dAngleA);
        //投影距离
        Vector2d vecTan(cos(arrAngle[nCurCount]), sin(arrAngle[nCurCount]));
        Vector2d vecAP = P - A;
        double dCross = vecTan.cross(vecAP);
        arrDist[nCurCount] = vecAP.model() * (dCross > 0 ? 1.0 : -1.0);
        
        nCurCount++;
    }
    if (bIsB)
    {
        arrCml[nCurCount] = dStartCml + dArcR * dAngleB;
        //切线角度
        arrAngle[nCurCount] = dStartTanAngle + (bTurnLeft ? dAngleB : -dAngleB);
        //投影距离
        Vector2d vecTan(cos(arrAngle[nCurCount]), sin(arrAngle[nCurCount]));
        Vector2d vecBP = P - B;
        double dCross = vecTan.cross(vecBP);
        arrDist[nCurCount] = vecBP.model() * (dCross > 0 ? 1.0 : -1.0);
        
        nCurCount++;
    }
    
    return nCurCount;
}

bool ArcElement::TrsCmlToHeight(double dCml, double& dHeight, double& dFyj) const
{
    //1.计算圆心坐标
    double dAngleR = dStartTanAngle + (bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d posCenter = pntStart + Point2d(cos(dAngleR), sin(dAngleR)) * dArcR;
    //2.通过圆方程求y的坐标
    double dY1 = posCenter.y + sqrt(pow(dArcR, 2) - pow(dCml - posCenter.x, 2));
    double dY2 = posCenter.y - sqrt(pow(dArcR, 2) - pow(dCml - posCenter.x, 2));
    //3.排除其中一个
    Point2d pos1(dCml, dY1);
    Point2d pos2(dCml, dY2);
    
    double dAngle1 = MATH_PI - abs(BaseCalFun::CalTurnAngle(pntStart, posCenter, pos1));
    double dAngle2 = MATH_PI - abs(BaseCalFun::CalTurnAngle(pntStart, posCenter, pos2));
    double dTurnAngle = MATH_PI - abs(BaseCalFun::CalTurnAngle(pntStart, posCenter, pntEnd));
    
    dHeight = (dAngle1 < dTurnAngle ? dY1 : dY2);
    double dArcAngle = (dAngle1 < dTurnAngle ? dAngle1 : dAngle2);
    dFyj = dStartTanAngle + (bTurnLeft ? dArcAngle : -dArcAngle);
    
    return true;
}

uint32_t ArcElement::IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) const
{
    //直线起点
    Point2d posOnlineA(dX, dY);
    //直线方向
    Vector2d vecDir(cos(dAngle), sin(dAngle));
    //直线终点
    Point2d posOnlineB = posOnlineA + vecDir;
    
    //计算圆心坐标
    double dAngleR = dStartTanAngle + (bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d O = pntStart + Point2d(cos(dAngleR), sin(dAngleR)) * dArcR;
    
    //圆心在直线上的投影坐标
    double dPecent = 0.0;
    Point2d posProj = BaseCalFun::PointToLineProjection(O, posOnlineA, posOnlineB, dPecent);
    double dDis = posProj.distanceTo(O);
    if (dDis > dArcR)
        return 0;
    
    double dModel = sqrt(dArcR * dArcR - dDis * dDis);
    //两个交点
    Point2d A = posProj + vecDir * dModel;
    Point2d B = posProj - vecDir * dModel;
    
    Vector2d vecOA = A - O;
    Vector2d vecOB = B - O;
    
    //起点向量
    Vector2d vecStart = pntStart - O;
    //圆心角
    double dArcAngle = TrsCmlToAngle_Relative(dTotalLen);
    
    double dAngleA = BaseCalFun::CalAngleBy2Vec(vecStart, vecOA);
    if (!bTurnLeft)
        dAngleA *= -1.0;
    BaseCalFun::KeepAngleIn2PI(dAngleA);
    
    double dAngleB = BaseCalFun::CalAngleBy2Vec(vecStart, vecOB);
    if (!bTurnLeft)
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

tagExportLineElement* ArcElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) const
{
    assert(dStartCml >= dStartCml - s_dCalPrecision && dEndCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dStartCml < dStartCml - s_dCalPrecision || dEndCml > dStartCml + dTotalLen + s_dCalPrecision)
        return nullptr;
    
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->nPosCount = 3;
    pRet->eLineType = eElementType;
    pRet->pArrPos = new PointExport[3];
    
    TrsCmlDistToNE(dStartCml, dDist, pRet->pArrPos[0].dX, pRet->pArrPos[0].dY, dAngle);
    TrsCmlDistToNE((dStartCml + dEndCml) / 2.0, dDist, pRet->pArrPos[2].dX, pRet->pArrPos[2].dY, dAngle);
    TrsCmlDistToNE(dEndCml, dDist, pRet->pArrPos[1].dX, pRet->pArrPos[1].dY, dAngle);
    
    return pRet;
}

Point2d ArcElement::TrsCmlToNE_Relative(double dCml) const
{
    double dArcAngle = dCml / dArcR;
    double dDeltaX = sin(dArcAngle) * dArcR;
    double dDeltaY = (1 - cos(dArcAngle)) * dArcR;
    return Point2d(dDeltaX, dDeltaY);
}

void ArcElement::InitData()
{
    pntEnd = BaseCalFun::TransferPos(pntStart, TrsCmlToNE_Relative(dTotalLen), bTurnLeft, -dStartTanAngle);
    double dArcAngle = TrsCmlToAngle_Relative(dTotalLen);
    dEndTanAngle = dStartTanAngle + (bTurnLeft ? dArcAngle : -dArcAngle);
}

void ArcElement::AdjustData(const Point2d& pos)
{
    pntStart += pos;
    pntEnd += pos;
}

