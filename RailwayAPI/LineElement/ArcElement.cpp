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

bool ArcElement::TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle)
{
    //计算圆心坐标
    double dAngleR = m_dStartTanAngle + (m_bTurnLeft ? MATH_PI_2 : -MATH_PI_2);
    Point2d O = m_posStart + Point2d(cos(dAngleR), sin(dAngleR)) * m_dArcR;
    //目标点
    Point2d P(dX, dY);
    //向量
    Vector2d vecOP = P - O;
    double dPecent = m_dArcR / vecOP.model();
    dPecent = BaseCalFun::Round(dPecent);
    //两个交点
    Point2d A(O.x + vecOP.x * dPecent, O.y + vecOP.y * dPecent);
    Point2d B(O.x - vecOP.x * dPecent, O.y - vecOP.y * dPecent);
    
    Vector2d vecOA = A - O;
    Vector2d vecOB = B - O;
    assert(abs(vecOA.model() + vecOB.model() - 2.0 * m_dArcR) < 0.01);
    
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
    
    //可能会有两个点，取投影距离小的
    double arrCml[2] = {0};
    double arrDist[2] = {0};
    double arrAngle[2] = {0};
    bool bIsA = (dAngleA <= dArcAngle + s_dCalPrecision);
    bool bIsB = (dAngleB <= dArcAngle + s_dCalPrecision);
    if (bIsA)
    {
        arrCml[0] = m_dStartCml + m_dArcR * dAngleA;
        //切线角度
        arrAngle[0] = m_dStartTanAngle + (m_bTurnLeft ? dAngleA : -dAngleA);
        //投影距离
        Vector2d vecTan(cos(arrAngle[0]), sin(arrAngle[0]));
        Vector2d vecAP = P - A;
        double dCross = vecTan.cross(vecAP);
        arrDist[0] = vecAP.model() * (dCross > 0 ? 1.0 : -1.0);
    }
    if (bIsB)
    {
        arrCml[1] = m_dStartCml + m_dArcR * dAngleB;
        //切线角度
        arrAngle[1] = m_dStartTanAngle + (m_bTurnLeft ? dAngleB : -dAngleB);
        //投影距离
        Vector2d vecTan(cos(arrAngle[1]), sin(arrAngle[1]));
        Vector2d vecBP = P - B;
        double dCross = vecTan.cross(vecBP);
        arrDist[1] = vecBP.model() * (dCross > 0 ? 1.0 : -1.0);
    }
    if (bIsA && !bIsB)
    {
        dCml = arrCml[0];
        dAngle = arrAngle[0];
        dDist = arrDist[0];
    }
    else if (!bIsA && bIsB)
    {
        dCml = arrCml[1];
        dAngle = arrAngle[1];
        dDist = arrDist[1];
    }
    else if (bIsA && bIsB)
    {
        int nIndex = (abs(arrDist[0]) < abs(arrDist[1]) ? 0 : 1);
        dCml = arrCml[nIndex];
        dAngle = arrAngle[nIndex];
        dDist = arrDist[nIndex];
    }
    else
        return false;
    
    return true;
    
    /*
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_posStart, Point2d(dX, dY), m_bTurnLeft, m_dStartTanAngle);
    int nBelong = PosBelongSelf(posTrs);
    if (!nBelong)
        return false;
    
    //二分法搜索投影点
    double dStartCml = 0.0;
    double dEndCml = m_dTotalLen;
    
    do {
        //二分里程
        double dMidCml = (dStartCml + dEndCml) / 2.0;
        //缓和曲线切线角度
        double dArcAngle = TrsCmlToAngle_Relative(dMidCml);
        //对应坐标
        const Point2d& pos = TrsCmlToNE_Relative(dMidCml);
        //目标点向量
        Vector2d vecTarget = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dArcAngle), sin(dArcAngle));
        //点乘为0时为垂直
        double dDot = vecTan.dot(vecTarget);
        if (vecTarget.isZeroVec() || abs(dDot) <= s_dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + dMidCml;
            //切线角度
            dAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
            
            if (vecTarget.isZeroVec())
                dDist = 0.0;
            else
            {
                //左右侧(叉乘判断左右)
                double dCross = vecTan.cross(vecTarget);
                bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
                //投影距离
                dDist = vecTarget.model() * (bOnLeft ? 1.0 : -1.0);
            }
            
            break;
        }
        else if ((nBelong == 1 && dDot < -s_dCalPrecision) || (nBelong == -1 && dDot > s_dCalPrecision))
            dEndCml = dMidCml;
        else if ((nBelong == 1 && dDot > s_dCalPrecision) || (nBelong == -1 && dDot < -s_dCalPrecision))
            dStartCml = dMidCml;
         
    } while (true);
     
    return true;
    */
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

