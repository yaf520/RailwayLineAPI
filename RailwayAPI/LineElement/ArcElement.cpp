//
//  ArcElement.cpp
//  icon_dg
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <cassert>
#include "ArcElement.hpp"

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
        Vector2d vecCal = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dArcAngle), sin(dArcAngle));
        //点乘为0时为垂直
        double dDot = vecTan.dot(vecCal);
        if (vecCal.isZeroVec() || abs(dDot) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + dMidCml;
            //切线角度
            dAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
            
            if (vecCal.isZeroVec())
                dDist = 0.0;
            else
            {
                //左右侧(叉乘判断左右)
                double dCross = vecTan.cross(vecCal);
                bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
                //投影距离
                dDist = vecCal.model() * (bOnLeft ? 1.0 : -1.0);
            }
            
            break;
        }
        else if (dDot <= -dCalPrecision)
        {
            if (dMidCml < dCalPrecision_1)
            {
                assert(false);
                return false;
            }
            
            dEndCml = dMidCml;
        }
        else if (dDot >= dCalPrecision)
        {
            if (abs(dMidCml - m_dTotalLen) < dCalPrecision_1)
            {
                assert(false);
                return false;
            }
            
            dStartCml = dMidCml;
        }
        
    } while (true);
    /*
    double dTotalLen = 0.0;
    do {
        //曲线切线角度
        double dArcAngle = TrsCmlToAngle_Relative(dTotalLen);
        //对应坐标
        Point2d pos = TrsCmlToNE_Relative(dTotalLen);
        
#ifdef USE_VECTOR
        
        //目标点向量
        Vector2d vecCal = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dArcAngle), sin(dArcAngle));
        
        double dDot = vecTan.dot(vecCal);
        //点乘为0时为垂直
        if (abs(dDot) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + dTotalLen;
            //切线角度
            dAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
            //左右侧
            double dCross = vecTan.cross(vecCal);
            bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
            //投影距离
            dDist = vecCal.model() * (bOnLeft ? 1.0 : -1.0);
            
            break;
        }
        dTotalLen += dDot;
#else
        double dAngleTemp = BaseCalFun::CalAngleX(pos, posTrs) - dArcAngle;
        BaseCalFun::KeepAngleIn2PI(dAngleTemp);
        
        double dDisTemp = posTrs.distanceTo(pos);
        double dTemp = cos(dAngleTemp) * dDisTemp;
        if (abs(dTemp) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + dTotalLen;
            //切线角度
            dAngle = m_dStartTanAngle + (m_bTurnLeft ? dArcAngle : -dArcAngle);
            //左右侧
            assert(abs(dAngleTemp - MATH_PI_2) < 0.0001 || abs(dAngleTemp - MATH_PI * 1.5) < 0.0001);
            bool bOnLeft =
                ((m_bTurnLeft && abs(dAngleTemp - MATH_PI_2) < 0.0001)
                 || (!m_bTurnLeft && abs(dAngleTemp - MATH_PI * 1.5) < 0.0001));
            //投影距离
            dDist = dDisTemp * (bOnLeft ? 1.0 : -1.0);
            
            break;
        }
        dTotalLen += dTemp;
#endif
    
        assert(dTotalLen >= 0.0 && dTotalLen <= m_dTotalLen);
        if (dTotalLen < 0.0 || dTotalLen > m_dTotalLen)
            return false;
        
    } while (true);
     */
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

