//
//  CurveElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include "CurveElement.hpp"
#include "BaseCalFun.hpp"
#include <cassert>

bool CurveElement::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    assert(dCml >= m_dStartCml);
    if (dCml - m_dStartCml < 0) return false;
    
    //前后标识
    bool bFront = (this->m_eElementType == ElementType::FrontCurve);
    //转向方向(左转true右转false)
    bool bTurnDir = ((bFront && m_bTurnLeft) || (!bFront && !m_bTurnLeft));
    //基准点
    Point2d posBase = (bFront ? m_posStart : m_posEnd);
    //切线角
    double dTanAngle = (bFront ? m_dStartTanAngle : m_dEndTanAngle);
    
    //缓和曲线距离
    double dDeltaLen = (bFront ? dCml - m_dStartCml : m_dTotalLen - (dCml - m_dStartCml));
    Point2d posTrs = TrsCmlToNE_Relative(dDeltaLen);
    Point2d posDist = BaseCalFun::TransferPos(posBase, posTrs, bTurnDir, -dTanAngle);
    
    double dCurveAngle = TrsCmlToAngle_Relative(dDeltaLen);
    dAngle = dTanAngle + (bTurnDir ? dCurveAngle : -dCurveAngle);
    if (!bFront)
        dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);

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

bool CurveElement::TrsNEToCmlDist(const double& dX, const double& dY, double& dCml, double& dDist, double& dAngle)
{
    //前后标识
    bool bFront = (this->m_eElementType == ElementType::FrontCurve);
    //转向方向
    bool bTurnDir = ((bFront && m_bTurnLeft) || (!bFront && !m_bTurnLeft));
    //基准点
    const Point2d& posBase = (bFront ? m_posStart : m_posEnd);
    //切线角
    double dBaseAngle = (bFront ? m_dStartTanAngle : m_dEndTanAngle);
    //转换为缓和曲线相对坐标系
    const Point2d& posTrs = BaseCalFun::TransferPosReversal(posBase, Point2d(dX, dY), bTurnDir, dBaseAngle);
    //二分法搜索投影点
    double dStartCml = 0.0;
    double dEndCml = m_dTotalLen;
    
    do {
        //二分里程
        double dMidCml = (dStartCml + dEndCml) / 2.0;
        //缓和曲线切线角度
        double dCurveAngle = TrsCmlToAngle_Relative(dMidCml);
        //对应坐标
        const Point2d& pos = TrsCmlToNE_Relative(dMidCml);
        //目标点向量
        Vector2d vecCal = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dCurveAngle), sin(dCurveAngle));
        //点乘为0时为垂直
        double dDot = vecTan.dot(vecCal);
        if (vecCal.isZeroVec() || abs(dDot) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml +
                (bFront ? dMidCml : m_dTotalLen - dMidCml);
            //切线角度
            dAngle = dBaseAngle +
                (bTurnDir ? dCurveAngle : -dCurveAngle);
            if (!bFront)
                dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);
            
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
            if (m_dTotalLen - dMidCml < dCalPrecision_1)
            {
                assert(false);
                return false;
            }
            
            dStartCml = dMidCml;
        }
            
    } while (true);
    
    /*
    //累计曲线长度
    double dTotalLen = 0.0;
    do
    {
        //缓和曲线切线角度
        double dCurveAngle = TrsCmlToAngle_Relative(dTotalLen);
        //对应坐标
        const Point2d& pos = TrsCmlToNE_Relative(dTotalLen);
        
#ifdef USE_VECTOR
        
        //目标点向量
        Vector2d vecCal = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dCurveAngle), sin(dCurveAngle));

        double dDot = vecTan.dot(vecCal);
        //点乘为0时为垂直
        if (abs(dDot) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + 
                (bFront ? dTotalLen : m_dTotalLen - dTotalLen);
            //切线角度
            dAngle = dBaseAngle + 
                (bTurnDir ? dCurveAngle : -dCurveAngle);
            if (!bFront)
                dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);
            //左右侧(叉乘判断左右)
            double dCross = vecTan.cross(vecCal);
            bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
            //投影距离
            dDist = vecCal.model() * (bOnLeft ? 1.0 : -1.0);
            
            break;
        }
        dTotalLen += dDot;
        
#else
      
        double dAngleTemp = BaseCalFun::CalAngleX(pos, posTrs) - dCurveAngle;
        BaseCalFun::KeepAngleIn2PI(dAngleTemp);
        
        double dDisTemp = posTrs.distanceTo(pos);
        double dTemp = cos(dAngleTemp) * dDisTemp;
        if (abs(dTemp) < dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + (bFront ? dTotalLen : m_dTotalLen - dTotalLen);
            //切线角度
            dAngle = dTanAngle + (bTurnDir ? dCurveAngle : -dCurveAngle);
            if (!bFront)
                dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);
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
        
    }while(true);
    */
    return true;
}

tagExportLineElement* CurveElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    double dDeltaLen = BaseCalFun::Round(dEndCml - dStartCml);
    double dTotalLenTmp = BaseCalFun::Round(m_dTotalLen);
    assert(dStartCml >= m_dStartCml && dDeltaLen <= dTotalLenTmp);
    if (dStartCml < m_dStartCml || dDeltaLen > dTotalLenTmp)
        return nullptr;
    
    int nPosCount = ceil((dEndCml - dStartCml) / dCurveStep) + 1;
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->eLineType = m_eElementType;
    pRet->nPosCount = nPosCount;
    pRet->pArrPos = new PointExport[nPosCount];
    
    double dCurCml = dStartCml;
    for (int i = 0; i < nPosCount; i++)
    {
        dCurCml = __min(dCurCml, dEndCml);
        TrsCmlDistToNE(dCurCml, dDist, pRet->pArrPos[i].dX, pRet->pArrPos[i].dY, dAngle);
        dCurCml += dCurveStep;
    }
    
    return pRet;
}

Point2d CurveElement::TrsCmlToNE_Relative(const double& dCml)
{
    double dX = (dCml - (pow(dCml, 5) / (40.0 * pow(m_dArcR, 2) * pow(m_dTotalLen, 2))));
    double dY = ((pow(dCml, 3) / (6.0 * m_dArcR * m_dTotalLen)) - (pow(dCml, 7) / (336.0 * pow(m_dArcR, 3) * pow(m_dTotalLen, 3))));
    return Point2d(dX, dY);
}

double CurveElement::TrsCmlToAngle_Relative(const double& dCml)
{
    return dCml * dCml / (2.0 * m_dArcR * m_dTotalLen);
}
