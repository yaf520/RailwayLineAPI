//
//  CurveElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <cassert>
#include "CurveElement.hpp"
#include "BaseCalFun.hpp"

#define RELATIVE_X(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n) * pow(x, 4 * n + 1) / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n)))
#define RELATIVE_Y(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n + 1) * pow(x, 4 * n + 3) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)))
#define RELATIVE_DX(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n) * pow(x, 4 * n) * (4 * n + 1) / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n)))
#define RELATIVE_DY(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n + 1) * pow(x, 4 * n + 2) * (4 * n + 3) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)))

CurveElement::CurveElement()
    : BaseLineElement()
{
    m_eElementType = ElementType::SpiralCurve;
}

bool CurveElement::TrsCmlDistToNE(const double& dCml, const double& dDist, double& dX, double& dY, double& dAngle)
{
    assert(dCml >= m_dStartCml);
    if (dCml - m_dStartCml < 0) return false;
    
    //转向方向(左转true右转false)
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    
    //缓和曲线距离
    double dDeltaLen = (m_bEnter ? dCml - m_dStartCml : m_dTotalLen - (dCml - m_dStartCml));
    Point2d posTrs = TrsCmlToNE_Relative(dDeltaLen + m_dHideLen);
    Point2d posDist = BaseCalFun::TransferPos(m_posBase, posTrs, bTurnDir, -m_dBaseTanAngle);
    
    double dCurveAngle = TrsCmlToAngle_Relative(dDeltaLen + m_dHideLen);
    dAngle = m_dBaseTanAngle + (bTurnDir ? dCurveAngle : -dCurveAngle);
    if (!m_bEnter)
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
    //转向方向
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_posBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
#define NEWTON_RAPHSON
    
#ifdef NEWTON_RAPHSON
    
    //牛顿迭代法
    double dRoot = 0.0;
    if (Newton_Raphson(posTrs.x, posTrs.x, posTrs.y, dRoot))
    {
        if (dRoot < m_dHideLen || dRoot > m_dTotalLen + m_dHideLen)
            return false;
        
        //里程
        dCml = m_dStartCml + (m_bEnter ? (dRoot - m_dHideLen) : m_dTotalLen - (dRoot - m_dHideLen));
        
        //切线角
        double dAngleRelative = TrsCmlToAngle_Relative(dRoot);
        dAngle = m_dBaseTanAngle + (bTurnDir ? dAngleRelative : -dAngleRelative);
        if (!m_bEnter)
            dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);
        
        //投影距离
        Point2d posRet = TrsCmlToNE_Relative(dRoot);
        Vector2d vecTarget = posTrs - posRet;
        if (vecTarget.isZeroVec())
            dDist = 0.0;
        else
        {
            Vector2d vecTan(cos(dAngleRelative), sin(dAngleRelative));
            //左右侧(叉乘判断左右)
            double dCross = vecTan.cross(vecTarget);
            bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
            //投影距离
            dDist = vecTarget.model() * (bOnLeft ? 1.0 : -1.0);
        }
        
        return true;
    }
    
    return false;
    
#else
    
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
        double dCurveAngle = TrsCmlToAngle_Relative(dMidCml + m_dHideLen);
        //对应坐标
        const Point2d& pos = TrsCmlToNE_Relative(dMidCml + m_dHideLen);
        //目标点向量
        Vector2d vecTarget = posTrs - pos;
        //切线方向向量
        Vector2d vecTan(cos(dCurveAngle), sin(dCurveAngle));
        //点乘为0时为垂直
        double dDot = vecTan.dot(vecTarget);
        if (vecTarget.isZeroVec() || abs(dDot) <= s_dCalPrecision)
        {
            //里程
            dCml = m_dStartCml + (m_bEnter ? dMidCml : m_dTotalLen - dMidCml);
            //切线角度
            dAngle = m_dBaseTanAngle + (bTurnDir ? dCurveAngle : -dCurveAngle);
            if (!m_bEnter)
                dAngle += (dAngle > MATH_PI ? -MATH_PI : MATH_PI);
            
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
        else if (dDot < -s_dCalPrecision)
            dEndCml = dMidCml;
        else if (dDot > s_dCalPrecision)
            dStartCml = dMidCml;
    } while (true);
     
    return true;
    
#endif

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

bool CurveElement::PosBelongSelf(const double& dX, const double& dY)
{
    //转向方向
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_posBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
    return PosBelongSelf(posTrs);
}

Point2d CurveElement::TrsCmlToNE_Relative(const double& dCml)
{
    //积分常数
    const uint32_t n = 10;

    double dX = 0.0;
    double dY = 0.0;
    for (int i = 0; i < n; i++)
    {
        double x = RELATIVE_X(i, dCml);
        double y = RELATIVE_Y(i, dCml);
        if (abs(x) < s_dValidPrecision && abs(y) < s_dValidPrecision)
            break;
        
        dX += x;
        dY += y;
    }
    return Point2d(dX, dY);
}

double CurveElement::TrsCmlToAngle_Relative(const double& dCml)
{
    return dCml * dCml / 2.0 / m_dC;
}

int CurveElement::PosBelongSelf(const Point2d& pos)
{
    //以下均以曲线自身坐标系计算
    Vector2d vecStart = pos - TrsCmlToNE_Relative(m_dHideLen);
    Vector2d vecEnd = pos - TrsCmlToNE_Relative(m_dHideLen + m_dTotalLen);
    if (vecStart.isZeroVec() || vecEnd.isZeroVec())
        return 1;
    
    double dTanStartAngle = TrsCmlToAngle_Relative(m_dHideLen);
    double dTanEndAngle = TrsCmlToAngle_Relative(m_dHideLen + m_dTotalLen);
    Vector2d vecTanStart(cos(dTanStartAngle), sin(dTanStartAngle));
    Vector2d vecTanEnd(cos(dTanEndAngle), sin(dTanEndAngle));
    
    double dDotStart = vecTanStart.dot(vecStart);
    double dDotEnd = vecTanEnd.dot(vecEnd);
    
    return (dDotStart >= -s_dCalPrecision && dDotEnd <= s_dCalPrecision) ? 1 : 0;
    //return (dDotStart >= -s_dCalPrecision && dDotEnd <= s_dCalPrecision) ? 1 : ((dDotStart <= -s_dCalPrecision && dDotEnd >= s_dCalPrecision) ? -1 : 0);
}

Point2d CurveElement::TrsCmlToCenter_Relative(const double& dCml)
{
    //对应坐标
    Point2d posOnline = TrsCmlToNE_Relative(dCml + m_dHideLen);
    //对应发现角度
    double dNormalAngle = TrsCmlToAngle_Relative(dCml + m_dHideLen) + MATH_PI_2;
    //对应半径
    double dR = m_dC / (dCml + m_dHideLen);
    //圆心坐标
    return posOnline + Vector2d(cos(dNormalAngle), sin(dNormalAngle)) * dR;
}

void CurveElement::InitData()
{
    assert(m_dEnterR != m_dExitR);
    //入or出
    m_bEnter = (m_dEnterR > m_dExitR);
    //转向方向(左转true右转false)
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    
    if ((m_bEnter && m_dEnterR != __DBL_MAX__) || (!m_bEnter && m_dExitR != __DBL_MAX__))
    {
        //计算曲线参数
        m_dC = m_dEnterR * m_dExitR * m_dTotalLen / abs(m_dEnterR - m_dExitR);
        //隐藏长度
        m_dHideLen = m_dC / (m_bEnter ? m_dEnterR : m_dExitR);
        //总计长度
        double dRealLen = m_dHideLen + m_dTotalLen;
        //隐藏段切线角
        double dAngle_Hide = TrsCmlToAngle_Relative(m_bEnter ? m_dHideLen : dRealLen);
        //基准切线角
        m_dBaseTanAngle = m_dStartTanAngle + (bTurnDir ? -dAngle_Hide : dAngle_Hide);
        if (!m_bEnter)
        {
            m_dBaseTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(m_dBaseTanAngle);
        }
        //计算基准点
        m_posBase = BaseCalFun::TransferBasePos(m_posStart, TrsCmlToNE_Relative(m_bEnter ? m_dHideLen : dRealLen), bTurnDir, -m_dBaseTanAngle);
        //计算结束点
        m_posEnd = BaseCalFun::TransferPos(m_posBase, TrsCmlToNE_Relative(m_bEnter ? dRealLen : m_dHideLen), bTurnDir, -m_dBaseTanAngle);
        //相对结束点点切线角
        double dEndTanAngle_Relative = TrsCmlToAngle_Relative(m_bEnter ? dRealLen : m_dHideLen);
        //结束点切线角
        m_dEndTanAngle = m_dBaseTanAngle + (bTurnDir ? dEndTanAngle_Relative : -dEndTanAngle_Relative);
        if (!m_bEnter)
        {
            m_dEndTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(m_dEndTanAngle);
        }
    }
    else
    {
        //计算曲线参数
        m_dC = m_dTotalLen * (m_bEnter ? m_dExitR : m_dEnterR);
        //隐藏长度
        m_dHideLen = 0.0;
        double dAngle_Relative = TrsCmlToAngle_Relative(m_dTotalLen);
        
        m_dEndTanAngle = m_dStartTanAngle + (m_bTurnLeft ? dAngle_Relative : -dAngle_Relative);
        m_dBaseTanAngle = m_bEnter ? m_dStartTanAngle : m_dEndTanAngle;
        if (!m_bEnter)
        {
            m_dBaseTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(m_dBaseTanAngle);
        }
        Point2d posRelative = TrsCmlToNE_Relative(m_dTotalLen);
        m_posEnd = (m_bEnter ? BaseCalFun::TransferPos(m_posStart, posRelative, bTurnDir, -m_dBaseTanAngle) : BaseCalFun::TransferBasePos(m_posStart, posRelative, bTurnDir, -m_dBaseTanAngle));
        
        m_posBase = (m_bEnter ? m_posStart : m_posEnd);
    }
}

void CurveElement::AdjustData(const Point2d& pos)
{
    m_posStart += pos;
    m_posEnd += pos;
    m_posBase += pos;
}

bool CurveElement::Newton_Raphson(double x, double dX, double dY, double& dRoot)
{
    int nIterCount = 0;
    do {
        double dTanAngle = x * x / 2.0 / m_dC;
        double x1 = 0.0, y1 = 0.0, dx1 = 0.0, dy1 = 0.0;
        for (int n = 0; n < 10; n++)
        {
            double x1Add = RELATIVE_X(n, x);
            double dx1Add = RELATIVE_DX(n, x);
            
            double y1Add = RELATIVE_Y(n, x);
            double dy1Add = RELATIVE_DY(n, x);
            
            if (abs(x1Add) < s_dValidPrecision && abs(y1Add) < s_dValidPrecision && abs(dx1Add) < s_dValidPrecision && abs(dy1Add) < s_dValidPrecision)
                break;
            
            x1 += x1Add;
            y1 += y1Add;
            dx1 += dx1Add;
            dy1 += dy1Add;
        }
        
        double f_x = cos(dTanAngle) * (x1 - dX) + sin(dTanAngle) * (y1 - dY);
        double fd_x = -sin(dTanAngle) * (x / m_dC) * (x1 - dX) + cos(dTanAngle) * dx1 + cos(dTanAngle) * (x / m_dC) * (y1 - dY) + sin(dTanAngle) * dy1;
        
        if (abs(fd_x) < s_dCalPrecision || nIterCount > 50)
            return false;

        double x0 = x - f_x / fd_x;
        if (abs(x0 - x) < s_dCalPrecision)
        {
            dRoot = x0;
            return true;
        }
        
        nIterCount++;
        x = x0;
    } while (true);
}
