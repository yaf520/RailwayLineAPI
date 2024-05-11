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
        double dDot = vecTan.dot(vecTarget)/* / vecCal.model()*/;
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
        /*
        else if (dDot < -s_dCalPrecision)
            dEndCml = dMidCml;
        else if (dDot > s_dCalPrecision)
            dStartCml = dMidCml;
         */
        
        else if ((nBelong == 1 && dDot < -s_dCalPrecision) || (nBelong == -1 && dDot > s_dCalPrecision))
            dEndCml = dMidCml;
        else if ((nBelong == 1 && dDot > s_dCalPrecision) || (nBelong == -1 && dDot < -s_dCalPrecision))
            dStartCml = dMidCml;
         
    } while (true);

    return true;
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

bool ArcElement::PosBelongSelf(const double& dX, const double& dY)
{
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_posStart, Point2d(dX, dY), m_bTurnLeft, m_dStartTanAngle);
    
    return PosBelongSelf(posTrs);
}

int ArcElement::PosBelongSelf(const Point2d& pos)
{
    Vector2d vecStart = pos - TrsCmlToNE_Relative(0.0);
    Vector2d vecEnd = pos - TrsCmlToNE_Relative(m_dTotalLen);
    if (vecStart.isZeroVec() || vecEnd.isZeroVec())
        return 1;
    
    Vector2d vecTanStart(cos(TrsCmlToAngle_Relative(0.0)), sin(TrsCmlToAngle_Relative(0.0)));
    Vector2d vecTanEnd(cos(TrsCmlToAngle_Relative(m_dTotalLen)), sin(TrsCmlToAngle_Relative(m_dTotalLen)));
    
    double dDotStart = vecTanStart.dot(vecStart);
    double dDotEnd = vecTanEnd.dot(vecEnd);
    
    //return (dDotStart >= -s_dCalPrecision && dDotEnd <= s_dCalPrecision) ? 1 : 0;
    return (dDotStart >= -s_dCalPrecision && dDotEnd <= s_dCalPrecision) ? 1 : ((dDotStart <= -s_dCalPrecision && dDotEnd >= s_dCalPrecision) ? -1 : 0);
}

Point2d ArcElement::TrsCmlToNE_Relative(const double& dCml)
{
    double dArcAngle = dCml / m_dArcR;
    double dDeltaX = sin(dArcAngle) * m_dArcR;
    double dDeltaY = (1 - cos(dArcAngle)) * m_dArcR;
    return Point2d(dDeltaX, dDeltaY);
}

double ArcElement::TrsCmlToAngle_Relative(const double& dCml)
{
    return dCml / m_dArcR;
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

