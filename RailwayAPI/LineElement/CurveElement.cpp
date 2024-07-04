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
#define RELATIVE_DX_1(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n) * pow(x, 4 * n) * (4 * n + 1) / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n)))
#define RELATIVE_DY_1(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n + 1) * pow(x, 4 * n + 2) * (4 * n + 3) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)))
#define RELATIVE_DX_2(n, x) (n == 0 ? 0 : (pow(-1, n) * pow(1.0 / m_dC, 2 * n) * pow(x, 4 * n - 1) * (4 * n + 1) * 4 * n / (BaseCalFun::Factorial(2 * n) * (4 * n + 1) * pow(2, 2 * n))))
#define RELATIVE_DY_2(n, x) (pow(-1, n) * pow(1.0 / m_dC, 2 * n + 1) * pow(x, 4 * n + 1) * (4 * n + 3) * (4 * n + 2) / (BaseCalFun::Factorial(2 * n + 1) * (4 * n + 3) * pow(2, 2 * n + 1)))

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

uint32_t CurveElement::TrsNEToCmlDist(const double& dX, const double& dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount])
{
    //转向方向
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_posBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
    //预估根
    double arrEstimateRoot[s_nMaxArrCount] = {0.0};
    uint32_t nEstimateRootCount = EstimateRoot(&CurveElement::f_original_proj, &CurveElement::f_first_deriv_proj, &CurveElement::f_second_deriv_proj, posTrs.x, posTrs.y, arrEstimateRoot);
    if (nEstimateRootCount == 0)
        return false;

    uint32_t nRootCount = 0;
    for (uint32_t nIndex = 0; nIndex < nEstimateRootCount; nIndex++)
    {
        //牛顿迭代法
        double dRoot = 0.0;
        if (NewtonIter(&CurveElement::f_original_proj, &CurveElement::f_first_deriv_proj, arrEstimateRoot[nIndex], posTrs.x, posTrs.y, dRoot))
        {
            if (dRoot < m_dHideLen - s_dCalPrecision || dRoot > m_dTotalLen + m_dHideLen + s_dCalPrecision)
                continue;
            
            //里程
            arrCml[nRootCount] = m_dStartCml + (m_bEnter ? (dRoot - m_dHideLen) : m_dTotalLen - (dRoot - m_dHideLen));
            
            //切线角
            double dAngleRelative = TrsCmlToAngle_Relative(dRoot);
            arrAngle[nRootCount] = m_dBaseTanAngle + (bTurnDir ? dAngleRelative : -dAngleRelative);
            if (!m_bEnter)
                arrAngle[nRootCount] += (arrAngle[nRootCount] > MATH_PI ? -MATH_PI : MATH_PI);
            
            //投影距离
            Point2d posRet = TrsCmlToNE_Relative(dRoot);
            Vector2d vecTarget = posTrs - posRet;
            if (vecTarget.isZeroVec())
                arrDist[nRootCount] = 0.0;
            else
            {
                Vector2d vecTan(cos(dAngleRelative), sin(dAngleRelative));
                //左右侧(叉乘判断左右)
                double dCross = vecTan.cross(vecTarget);
                bool bOnLeft = ((dCross > 0.0 && m_bTurnLeft) || (dCross < 0.0 && !m_bTurnLeft));
                //投影距离
                arrDist[nRootCount] = vecTarget.model() * (bOnLeft ? 1.0 : -1.0);
            }
            
            nRootCount++;
        }
    }
    
    return nRootCount;
}

uint32_t CurveElement::CalculateCrossNE(const double& dAngle, const double& dX, const double& dY, Point2d arrCrossPos[s_nMaxArrCount])
{
    //转向方向
    bool bTurnDir = ((m_bEnter && m_bTurnLeft) || (!m_bEnter && !m_bTurnLeft));
    //世界系方向
    Vector2d vecDir(cos(dAngle), sin(dAngle));
    //转换为缓和曲线相对坐标系
    Vector2d vecRelativeDir = BaseCalFun::TransferPosReversal(Point2d(0, 0), vecDir, bTurnDir, m_dBaseTanAngle);
    //坐标转换
    Point2d posOnLineRelative = BaseCalFun::TransferPosReversal(m_posBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
    //斜率与截距
    double k = vecRelativeDir.y / vecRelativeDir.x;
    double b = posOnLineRelative.y - k * posOnLineRelative.x;
    
    //预估根
    double arrEstimateRoot[s_nMaxArrCount] = {0.0};
    uint32_t nEstimateRootCount = EstimateRoot(&CurveElement::f_original_cross, &CurveElement::f_first_deriv_cross, &CurveElement::f_second_deriv_cross, k, b, arrEstimateRoot);
    if (nEstimateRootCount == 0)
        return false;
    
    uint32_t nRootCount = 0;
    for (uint32_t nIndex = 0; nIndex < nEstimateRootCount; nIndex++)
    {
        //牛顿迭代法
        double dRoot = 0.0;
        if (NewtonIter(&CurveElement::f_original_cross, &CurveElement::f_first_deriv_cross, arrEstimateRoot[nIndex], k, b, dRoot))
        {
            if (dRoot < m_dHideLen - s_dCalPrecision || dRoot > m_dTotalLen + m_dHideLen + s_dCalPrecision)
                continue;
            
            arrCrossPos[nRootCount++] = BaseCalFun::TransferPos(m_posBase, TrsCmlToNE_Relative(dRoot), bTurnDir, -m_dBaseTanAngle);
        }
    }
        
    return nRootCount;
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
    double dX = 0.0;
    double dY = 0.0;
    for (int i = 0; i < s_nAddPreCount; i++)
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

double CurveElement::f_original_proj(double dL0, const double& dParamX, const double& dParamY)
{
    double dTanAngle = dL0 * dL0 / 2.0 / m_dC;
    double x = 0.0, y = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = RELATIVE_X(n, dL0);
        double yAdd = RELATIVE_Y(n, dL0);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
    }
    
    return cos(dTanAngle) * (x - dParamX) + sin(dTanAngle) * (y - dParamY);
}

double CurveElement::f_original_cross(double dL0, const double& k, const double& b)
{
    double x = 0.0, y = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = RELATIVE_X(n, dL0);
        double yAdd = RELATIVE_Y(n, dL0);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
    }
    
    return k * x + b - y;
}

double CurveElement::f_first_deriv_proj(double dL0, const double& dParamX, const double& dParamY)
{
    double dTanAngle = dL0 * dL0 / 2.0 / m_dC;
    double x = 0.0, y = 0.0, dx = 0.0, dy = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = RELATIVE_X(n, dL0);
        double yAdd = RELATIVE_Y(n, dL0);
        
        double dxAdd = RELATIVE_DX_1(n, dL0);
        double dyAdd = RELATIVE_DY_1(n, dL0);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision
            && abs(dxAdd) < s_dValidPrecision && abs(dyAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
        dx += dxAdd;
        dy += dyAdd;
    }
    return -sin(dTanAngle) * (dL0 / m_dC) * (x - dParamX) + cos(dTanAngle) * dx + cos(dTanAngle) * (dL0 / m_dC) * (y - dParamY) + sin(dTanAngle) * dy;
}

double CurveElement::f_first_deriv_cross(double dL0, const double& k, const double& b)
{
    double dx = 0.0, dy = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double dxAdd = RELATIVE_DX_1(n, dL0);
        double dyAdd = RELATIVE_DY_1(n, dL0);
        
        if (abs(dxAdd) < s_dValidPrecision && abs(dyAdd) < s_dValidPrecision)
            break;
        
        dx += dxAdd;
        dy += dyAdd;
    }
    
    return k * dx - dy;
}

double CurveElement::f_second_deriv_proj(double x0, const double& dParamX, const double& dParamY)
{
    double dTanAngle = x0 * x0 / 2.0 / m_dC;
    double dDTanAngle1 = x0 / m_dC;
    double dDTanAngle2 = 1.0 / m_dC;
    
    double x = 0.0, y = 0.0, dx1 = 0.0, dy1 = 0.0, dx2 = 0.0, dy2 = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = RELATIVE_X(n, x0);
        double yAdd = RELATIVE_Y(n, x0);
        
        double d_first_xAdd = RELATIVE_DX_1(n, x0);
        double d_first_yAdd = RELATIVE_DY_1(n, x0);
        
        double d_second_xAdd = RELATIVE_DX_2(n, x0);
        double d_second_yAdd = RELATIVE_DY_2(n, x0);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision
            && abs(d_first_xAdd) < s_dValidPrecision && abs(d_first_yAdd) < s_dValidPrecision
            && abs(d_second_xAdd) < s_dValidPrecision && abs(d_second_yAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
        dx1 += d_first_xAdd;
        dy1 += d_first_yAdd;
        dx2 += d_second_xAdd;
        dy2 += d_second_yAdd;
    }

    return -cos(dTanAngle) * dDTanAngle1 * dDTanAngle1 * (x - dParamX) - sin(dTanAngle) * dDTanAngle2 * (x - dParamX) - sin(dTanAngle) * dDTanAngle1 * dx1 - sin(dTanAngle) * dDTanAngle1 * dx1 + cos(dTanAngle) * dx2 - sin(dTanAngle) * dDTanAngle1 * dDTanAngle1 * (y - dParamY) + cos(dTanAngle) * dDTanAngle2 * (y - dParamY) + cos(dTanAngle) * dDTanAngle1 * dy1 + cos(dTanAngle) * dDTanAngle1 * dy1 + sin(dTanAngle) * dy2;
}

double CurveElement::f_second_deriv_cross(double x0, const double& k, const double& b)
{
    double dx2 = 0.0, dy2 = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double d_second_xAdd = RELATIVE_DX_2(n, x0);
        double d_second_yAdd = RELATIVE_DY_2(n, x0);
        
        if (abs(d_second_xAdd) < s_dValidPrecision && abs(d_second_yAdd) < s_dValidPrecision)
            break;
        
        dx2 += d_second_xAdd;
        dy2 += d_second_yAdd;
    }
    
    return k * dx2 - dy2;
}

uint32_t CurveElement::EstimateRoot(pFunc pf_original, pFunc pf_first_deriv, pFunc pf_second_deriv, const double& dParamX, const double& dParamY, double arrEstimateRoot[s_nMaxArrCount])
{
#define ESTIMATE_TEST
    
#ifdef ESTIMATE_TEST
    
    uint32_t nCurCount = 0;
    
    double x0 = m_dHideLen;
    double x1 = m_dHideLen + m_dTotalLen;
    
    double f0 = (this->*pf_original)(x0, dParamX, dParamY);
    if (abs(f0) < s_dCalPrecision)
        f0 = 0.0;
    
    double f1 = (this->*pf_original)(x1, dParamX, dParamY);
    if (abs(f1) < s_dCalPrecision)
        f1 = 0.0;
    
    double f0_d = (this->*pf_first_deriv)(x0, dParamX, dParamY);
    if (abs(f0_d) < s_dCalPrecision)
        f0_d = 0.0;
    
    double f1_d = (this->*pf_first_deriv)(x1, dParamX, dParamY);
    if (abs(f1_d) < s_dCalPrecision)
        f1_d = 0.0;
    
    if (f0_d * f1_d <= 0.0)
    {
        //存在极值点
        double xLimite = 0.0;
        if (NewtonIter(pf_first_deriv, pf_second_deriv, (x0 + x1) / 2.0, dParamX, dParamY, xLimite))
        {
            //区间极值
            double fLimite = (this->*pf_original)(xLimite, dParamX, dParamY);
            if (abs(fLimite) < s_dCalPrecision)
                arrEstimateRoot[nCurCount++] = fLimite;
            else
            {
                if (f0 * fLimite <= 0.0)
                    arrEstimateRoot[nCurCount++] = (x0 + xLimite) / 2.0;
                if (fLimite * f1 <= 0.0)
                    arrEstimateRoot[nCurCount++] = (xLimite + x1) / 2.0;
            }
        }
    }
    else if (f0 * f1 <= 0.0)
        arrEstimateRoot[nCurCount++] = (x0 + x1) / 2.0;
    
    return nCurCount;
    
#else
    
    //分段数
    double dPerLen = m_dTotalLen / s_nEstimateRootSection;
    double dRealLen = m_dHideLen + m_dTotalLen;
    
    double x0 = m_dHideLen;
    double x1 = 0.0;
    
    uint32_t nCurCount = 0;
    double f0 = 0.0, f1 = 0.0, f0_d = 0.0, f1_d = 0.0;
    do {
        x1 = __min(x0 + dPerLen, dRealLen);
        
        f0 = (this->*pf_original)(x0, dParamX, dParamY);
        if (abs(f0) < s_dCalPrecision)
            f0 = 0.0;
        
        f1 = (this->*pf_original)(x1, dParamX, dParamY);
        if (abs(f1) < s_dCalPrecision)
            f1 = 0.0;
        
        if (f0 == 0.0 && x0 == m_dHideLen)
            arrEstimateRoot[nCurCount++] = x0;
        if (f1 == 0.0)
            arrEstimateRoot[nCurCount++] = x1;

        if (f0 != 0.0 && f1 != 0.0)
        {
            f0_d = (this->*pf_first_deriv)(x0, dParamX, dParamY);
            if (abs(f0_d) < s_dCalPrecision)
                f0_d = 0.0;
            
            f1_d = (this->*pf_first_deriv)(x1, dParamX, dParamY);
            if (abs(f1_d) < s_dCalPrecision)
                f1_d = 0.0;
            
            if (f0_d * f1_d <= 0.0)
            {
                //存在极值点
                double xLimite = 0.0;
                if (NewtonIter(pf_first_deriv, pf_second_deriv, (x0 + x1) / 2.0, dParamX, dParamY, xLimite))
                {
                    //区间极值
                    double fLimite = f_original_proj(xLimite, dParamX, dParamY);
                    if (abs(fLimite) < s_dCalPrecision)
                        arrEstimateRoot[nCurCount++] = fLimite;
                    else
                    {
                        if (f0 * fLimite < 0.0)
                            arrEstimateRoot[nCurCount++] = (x0 + xLimite) / 2.0;
                        if (fLimite * f1 < 0.0)
                            arrEstimateRoot[nCurCount++] = (xLimite + x1) / 2.0;
                    }
                }
            }
            else if (f0 * f1 < 0.0)
                arrEstimateRoot[nCurCount++] = (x0 + x1) / 2.0;
        }
        
        if (x1 >= dRealLen || nCurCount >= s_nMaxArrCount)
            break;
        
        x0 = x1;
    } while (true);
    
    return nCurCount;
    
#endif
    
}

bool CurveElement::NewtonIter(pFunc pf_original, pFunc pf_first_deriv, double dEstimateRoot, const double& dParamX, const double& dParamY, double& dRoot)
{
    //初始值
    double x0 = dEstimateRoot;
    
    int nIterCount = 0;
    do {
        double fx0 = (this->*pf_original)(x0, dParamX, dParamY);
        if (abs(fx0) < s_dCalPrecision)
        {
            dRoot = x0;
            return true;
        }
        double fdx0 = (this->*pf_first_deriv)(x0, dParamX, dParamY);
        
        if (abs(fdx0) < s_dCalPrecision || nIterCount > 50)
            return false;

        nIterCount++;
        x0 = x0 - fx0 / fdx0;
    } while (true);
}
