//
//  CurveElement.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <assert.h>
#include "SpiralLineElement.hpp"
#include "BaseCalFun.hpp"


SpiralLineElement::SpiralLineElement()
    : BaseLineElement()
{
    eElementType = ElementType::SpiralCurve;
}

bool SpiralLineElement::TrsCmlDistToNE(double dCml, double dDist, double& dX, double& dY, double& dAngle) const
{
    assert(dCml >= dStartCml - s_dCalPrecision && dCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dCml < dStartCml - s_dCalPrecision || dCml > dStartCml + dTotalLen + s_dCalPrecision)
        return false;
    
    //转向方向(左转true右转false)
    bool bTurnDir = ((m_bEnter && bTurnLeft) || (!m_bEnter && !bTurnLeft));
    
    //缓和曲线距离
    double dDeltaLen = (m_bEnter ? dCml - dStartCml : dTotalLen - (dCml - dStartCml));
    Point2d posTrs = TrsCmlToNE_Relative(dDeltaLen + m_dHideLen);
    Point2d posDist = BaseCalFun::TransferPos(m_pntBase, posTrs, bTurnDir, -m_dBaseTanAngle);
    
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

uint32_t SpiralLineElement::TrsNEToCmlDist(double dX, double dY, double arrCml[s_nMaxArrCount], double arrDist[s_nMaxArrCount], double arrAngle[s_nMaxArrCount]) const
{
    //转向方向
    bool bTurnDir = ((m_bEnter && bTurnLeft) || (!m_bEnter && !bTurnLeft));
    //转换为缓和曲线相对坐标系
    Point2d posTrs = BaseCalFun::TransferPosReversal(m_pntBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
    //预估根
    double arrEstimateRoot[s_nMaxArrCount] = {0.0};
    uint32_t nEstimateRootCount = EstimateRoot(&SpiralLineElement::f_original_proj, &SpiralLineElement::f_first_deriv_proj, &SpiralLineElement::f_second_deriv_proj, posTrs.x, posTrs.y, arrEstimateRoot);
    if (nEstimateRootCount == 0)
        return false;

    uint32_t nRootCount = 0;
    for (uint32_t nIndex = 0; nIndex < nEstimateRootCount; nIndex++)
    {
        //牛顿迭代法
        double dRoot = 0.0;
        if (NewtonIter(&SpiralLineElement::f_original_proj, &SpiralLineElement::f_first_deriv_proj, arrEstimateRoot[nIndex], posTrs.x, posTrs.y, dRoot))
        {
            if (dRoot < m_dHideLen - s_dCalPrecision || dRoot > dTotalLen + m_dHideLen + s_dCalPrecision)
                continue;
            
            //里程
            arrCml[nRootCount] = dStartCml + (m_bEnter ? (dRoot - m_dHideLen) : dTotalLen - (dRoot - m_dHideLen));
            
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
                bool bOnLeft = ((dCross > 0.0 && bTurnLeft) || (dCross < 0.0 && !bTurnLeft));
                //投影距离
                arrDist[nRootCount] = vecTarget.model() * (bOnLeft ? 1.0 : -1.0);
            }
            
            nRootCount++;
        }
    }
    
    return nRootCount;
}

uint32_t SpiralLineElement::IntersectWithLine(double dAngle, double dX, double dY, Point2d arrCrossPos[s_nMaxArrCount]) const
{
    //转向方向
    bool bTurnDir = ((m_bEnter && bTurnLeft) || (!m_bEnter && !bTurnLeft));
    //世界系方向
    Vector2d vecDir(cos(dAngle), sin(dAngle));
    //转换为缓和曲线相对坐标系
    Vector2d vecRelativeDir = BaseCalFun::TransferPosReversal(Point2d(0, 0), vecDir, bTurnDir, m_dBaseTanAngle);
    //坐标转换
    Point2d posOnLineRelative = BaseCalFun::TransferPosReversal(m_pntBase, Point2d(dX, dY), bTurnDir, m_dBaseTanAngle);
    
    //斜率与截距
    bool bExistSlope = (abs(vecRelativeDir.x) > s_dCalPrecision);
    double k = (bExistSlope ? vecRelativeDir.y / vecRelativeDir.x : __DBL_MAX__);
    double b = (bExistSlope ? posOnLineRelative.y - k * posOnLineRelative.x : posOnLineRelative.x);
    
    //预估根
    double arrEstimateRoot[s_nMaxArrCount] = {0.0};
    uint32_t nEstimateRootCount = EstimateRoot(&SpiralLineElement::f_original_cross, &SpiralLineElement::f_first_deriv_cross, &SpiralLineElement::f_second_deriv_cross, k, b, arrEstimateRoot);
    if (nEstimateRootCount == 0)
        return false;
    
    uint32_t nRootCount = 0;
    for (uint32_t nIndex = 0; nIndex < nEstimateRootCount; nIndex++)
    {
        //牛顿迭代法
        double dRoot = 0.0;
        if (NewtonIter(&SpiralLineElement::f_original_cross, &SpiralLineElement::f_first_deriv_cross, arrEstimateRoot[nIndex], k, b, dRoot))
        {
            if (dRoot < m_dHideLen - s_dCalPrecision || dRoot > dTotalLen + m_dHideLen + s_dCalPrecision)
                continue;
            
            arrCrossPos[nRootCount++] = BaseCalFun::TransferPos(m_pntBase, TrsCmlToNE_Relative(dRoot), bTurnDir, -m_dBaseTanAngle);
        }
    }
        
    return nRootCount;
}

tagExportLineElement* SpiralLineElement::ExportHorCurve(double dStartCml, double dEndCml, double dDist, double dCurveStep) const
{
    assert(dStartCml >= dStartCml - s_dCalPrecision && dEndCml <= dStartCml + dTotalLen + s_dCalPrecision);
    if (dStartCml < dStartCml - s_dCalPrecision || dEndCml > dStartCml + dTotalLen + s_dCalPrecision)
        return nullptr;
    
    int nPosCount = ceil((dEndCml - dStartCml) / dCurveStep) + 1;
    double dAngle = 0.0;
    tagExportLineElement* pRet = new tagExportLineElement;
    pRet->eLineType = eElementType;
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

Point2d SpiralLineElement::TrsCmlToNE_Relative(double dCml) const
{
    double dX = 0.0;
    double dY = 0.0;
    for (int i = 0; i < s_nAddPreCount; i++)
    {
        double x = this->relative_x(dCml, i);
        double y = this->relative_y(dCml, i);
        if (abs(x) < s_dValidPrecision && abs(y) < s_dValidPrecision)
            break;
        
        dX += x;
        dY += y;
    }
    return Point2d(dX, dY);
}

void SpiralLineElement::InitData()
{
    assert(dEnterR != dExitR);
    //入or出
    m_bEnter = (dEnterR > dExitR);
    //转向方向(左转true右转false)
    bool bTurnDir = ((m_bEnter && bTurnLeft) || (!m_bEnter && !bTurnLeft));
    
    if ((m_bEnter && dEnterR != __DBL_MAX__) || (!m_bEnter && dExitR != __DBL_MAX__))
    {
        //计算曲线参数
        dC = dEnterR * dExitR * dTotalLen / abs(dEnterR - dExitR);
        //隐藏长度
        m_dHideLen = dC / (m_bEnter ? dEnterR : dExitR);
        //总计长度
        double dRealLen = m_dHideLen + dTotalLen;
        //隐藏段切线角
        double dAngle_Hide = TrsCmlToAngle_Relative(m_bEnter ? m_dHideLen : dRealLen);
        //基准切线角
        m_dBaseTanAngle = dStartTanAngle + (bTurnDir ? -dAngle_Hide : dAngle_Hide);
        if (!m_bEnter)
        {
            m_dBaseTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(m_dBaseTanAngle);
        }
        //计算基准点
        m_pntBase = BaseCalFun::TransferBasePos(pntStart, TrsCmlToNE_Relative(m_bEnter ? m_dHideLen : dRealLen), bTurnDir, -m_dBaseTanAngle);
        //计算结束点
        pntEnd = BaseCalFun::TransferPos(m_pntBase, TrsCmlToNE_Relative(m_bEnter ? dRealLen : m_dHideLen), bTurnDir, -m_dBaseTanAngle);
        //相对结束点点切线角
        double dEndTanAngle_Relative = TrsCmlToAngle_Relative(m_bEnter ? dRealLen : m_dHideLen);
        //结束点切线角
        dEndTanAngle = m_dBaseTanAngle + (bTurnDir ? dEndTanAngle_Relative : -dEndTanAngle_Relative);
        if (!m_bEnter)
        {
            dEndTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(dEndTanAngle);
        }
    }
    else
    {
        //计算曲线参数
        dC = dTotalLen * (m_bEnter ? dExitR : dEnterR);
        //隐藏长度
        m_dHideLen = 0.0;
        double dAngle_Relative = TrsCmlToAngle_Relative(dTotalLen);
        
        dEndTanAngle = dStartTanAngle + (bTurnLeft ? dAngle_Relative : -dAngle_Relative);
        m_dBaseTanAngle = m_bEnter ? dStartTanAngle : dEndTanAngle;
        if (!m_bEnter)
        {
            m_dBaseTanAngle += MATH_PI;
            BaseCalFun::KeepAngleIn2PI(m_dBaseTanAngle);
        }
        Point2d posRelative = TrsCmlToNE_Relative(dTotalLen);
        pntEnd = (m_bEnter ? BaseCalFun::TransferPos(pntStart, posRelative, bTurnDir, -m_dBaseTanAngle) : BaseCalFun::TransferBasePos(pntStart, posRelative, bTurnDir, -m_dBaseTanAngle));
        
        m_pntBase = (m_bEnter ? pntStart : pntEnd);
    }
}

void SpiralLineElement::AdjustData(const Vector2d& vec)
{
    BaseLineElement::AdjustData(vec);
    m_pntBase += vec;
}

double SpiralLineElement::f_original_proj(double dL0, double dParamX, double dParamY) const
{
    double dTanAngle = dL0 * dL0 / 2.0 / dC;
    double x = 0.0, y = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = this->relative_x(dL0, n);
        double yAdd = this->relative_y(dL0, n);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
    }
    
    return cos(dTanAngle) * (x - dParamX) + sin(dTanAngle) * (y - dParamY);
}

double SpiralLineElement::f_original_cross(double dL0, double k, double b) const
{
    double x = 0.0, y = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = this->relative_x(dL0, n);
        double yAdd = this->relative_y(dL0, n);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
    }
    
    return (k == __DBL_MAX__ ? x - b : k * x + b - y);
}

double SpiralLineElement::f_first_deriv_proj(double dL0, double dParamX, double dParamY) const
{
    double dTanAngle = dL0 * dL0 / 2.0 / dC;
    double x = 0.0, y = 0.0, dx = 0.0, dy = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = this->relative_x(dL0, n);
        double yAdd = this->relative_y(dL0, n);
        
        double dxAdd = this->relative_dx(dL0, n);
        double dyAdd = this->relative_dy(dL0, n);
        
        if (abs(xAdd) < s_dValidPrecision && abs(yAdd) < s_dValidPrecision
            && abs(dxAdd) < s_dValidPrecision && abs(dyAdd) < s_dValidPrecision)
            break;
        
        x += xAdd;
        y += yAdd;
        dx += dxAdd;
        dy += dyAdd;
    }
    return -sin(dTanAngle) * (dL0 / dC) * (x - dParamX) + cos(dTanAngle) * dx + cos(dTanAngle) * (dL0 / dC) * (y - dParamY) + sin(dTanAngle) * dy;
}

double SpiralLineElement::f_first_deriv_cross(double dL0, double k, double b) const
{
    double dx = 0.0, dy = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double dxAdd = this->relative_dx(dL0, n);
        double dyAdd = this->relative_dy(dL0, n);
        
        if (abs(dxAdd) < s_dValidPrecision && abs(dyAdd) < s_dValidPrecision)
            break;
        
        dx += dxAdd;
        dy += dyAdd;
    }
    
    return (k == __DBL_MAX__ ? dx : k * dx - dy);
}

double SpiralLineElement::f_second_deriv_proj(double x0, double dParamX, double dParamY) const
{
    double dTanAngle = x0 * x0 / 2.0 / dC;
    double dDTanAngle1 = x0 / dC;
    double dDTanAngle2 = 1.0 / dC;
    
    double x = 0.0, y = 0.0, dx1 = 0.0, dy1 = 0.0, dx2 = 0.0, dy2 = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double xAdd = this->relative_x(x0, n);
        double yAdd = this->relative_y(x0, n);
        
        double d_first_xAdd = this->relative_dx(x0, n);
        double d_first_yAdd = this->relative_dy(x0, n);
        
        double d_second_xAdd = this->relative_d2x(x0, n);
        double d_second_yAdd = this->relative_d2y(x0, n);
        
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

double SpiralLineElement::f_second_deriv_cross(double x0, double k, double b) const
{
    double dx2 = 0.0, dy2 = 0.0;
    for (int n = 0; n < s_nAddPreCount; n++)
    {
        double d_second_xAdd = this->relative_d2x(x0, n);
        double d_second_yAdd = this->relative_d2y(x0, n);
        
        if (abs(d_second_xAdd) < s_dValidPrecision && abs(d_second_yAdd) < s_dValidPrecision)
            break;
        
        dx2 += d_second_xAdd;
        dy2 += d_second_yAdd;
    }
    
    return (k == __DBL_MAX__ ? dx2 : k * dx2 - dy2);
}

uint32_t SpiralLineElement::EstimateRoot(pFunc pf_original, pFunc pf_first_deriv, pFunc pf_second_deriv, double dParamX, double dParamY, double arrEstimateRoot[s_nMaxArrCount]) const
{
//#define ESTIMATE_TEST
    
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
    double dPerLen = dTotalLen / s_nEstimateRootSection;
    double dRealLen = m_dHideLen + dTotalLen;
    
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

bool SpiralLineElement::NewtonIter(pFunc pf_original, pFunc pf_first_deriv, double dEstimateRoot, double dParamX, double dParamY, double& dRoot) const
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
