//
//  HorizontalLogic.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include "LineElementManager.hpp"
#include "BaseCalFun.hpp"

LineElementManager::LineElementManager()
{
    m_arrLineElement = nullptr;
    m_nElementCount = 0;
}

LineElementManager::~LineElementManager()
{
    this->ResetData();
}

//切线长度
double LineElementManager::CalTangentLen(double dFL, double dBL, double dArcR, double dTurnAngle, bool bFront)
{
    if (dArcR == 0.0)
        return 0.0;
    
    //切垂距
    double q1 = dFL / 2.0 - (dFL * dFL * dFL / (240.0 * dArcR * dArcR));
    double q2 = dBL / 2.0 - (dBL * dBL * dBL / (240.0 * dArcR * dArcR));
    //内移量
    double p1 = dFL * dFL / (24.0 * dArcR) - (pow(dFL, 4) / (2688.0 * pow(dArcR, 3)));
    double p2 = dBL * dBL / (24.0 * dArcR) - (pow(dBL, 4) / (2688.0 * pow(dArcR, 3)));
    double dAngle = abs(dTurnAngle);
    
    return (bFront ? (dArcR * tan(dAngle / 2.0) + (p2 - p1 * cos(dAngle)) / sin(dAngle) + q1) : (dArcR * tan(dAngle / 2.0) + (p1 - p2 * cos(dAngle)) / sin(dAngle) + q2));
}

//缓和曲线属性点
Point2d LineElementManager::CalCurvePos(double dL0, double dArcR)
{
    double dX = dL0 - (pow(dL0, 3) / (40.0 * pow(dArcR, 2)));
    double dY = (dL0 * dL0 / (6.0 * dArcR)) - (pow(dL0, 4) / (336.0 * pow(dArcR, 3)));
    return Point2d(dX, dY);
}

void LineElementManager::ResetData()
{
    if (m_arrLineElement)
    {
        for (uint32_t i = 0; i < m_nElementCount; i++)
            if (m_arrLineElement[i])
                delete m_arrLineElement[i];
        
        delete [] m_arrLineElement;
        m_arrLineElement = nullptr;
        m_nElementCount = 0;
    }
}

///设置数据
void LineElementManager::SetJDData(const tagJDInfo* pJDInfo, uint32_t iCount)
{
    if (!pJDInfo || iCount < 2) return;
    
    //重置数据
    ResetData();
    
    Point2d posJD1, posJD2;
    if (iCount == 2)
    {
        m_arrLineElement = new BaseLineElement*[1];
        
        //两个交点
        posJD1.Set(pJDInfo->dX, pJDInfo->dY);
        posJD2.Set((pJDInfo + 1)->dX, (pJDInfo + 1)->dY);
        double dAngle_JD1_JD2 = BaseCalFun::CalAngleX(posJD1, posJD2);
        
        BaseLineElement* pLineElement = new StraightLineElement;
        pLineElement->m_posStart = posJD1;
        pLineElement->m_posEnd = posJD2;
        pLineElement->m_dStartTanAngle = dAngle_JD1_JD2;
        pLineElement->m_dEndTanAngle = dAngle_JD1_JD2;
        pLineElement->m_eElementType = ElementType::Line;
        pLineElement->m_dStartCml = 0.0;
        pLineElement->m_dTotalLen = posJD1.distanceTo(posJD2);
        pLineElement->m_nIndex = m_nElementCount;
        m_arrLineElement[m_nElementCount++] = pLineElement;
        
        return;
    }
    
    //最大线元数量
    uint32_t nMaxCount = iCount * 4 - 7;
    //m_arrLineElement = (BaseLineElement**)new uint64_t[maxCount];
    m_arrLineElement = new BaseLineElement*[nMaxCount];
    
    //变量定义
    double dCurrentCml = 0.0;
    Point2d posJD3, posZH, posHY, posYH, posHZ;
    for (uint32_t i = 0; i + 2 < iCount; i++)
    {
        //交点坐标
        posJD1.Set((pJDInfo + i)->dX, (pJDInfo + i)->dY);
        posJD2.Set((pJDInfo + i + 1)->dX, (pJDInfo + i + 1)->dY);
        posJD3.Set((pJDInfo + i + 2)->dX, (pJDInfo + i + 2)->dY);
        
        //缓和曲线长度
        double dFL = (pJDInfo + i + 1)->dFL;
        double dBL = (pJDInfo + i + 1)->dBL;
        //圆曲线半径
        double dArcR = (pJDInfo + i + 1)->dArcR;
        //转向角
        double dTurnAngle = BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3);
        bool bTurnLeft = (dTurnAngle > 0.0);
        
        //计算属性点
        //ZH点
        double dAngle_JD1_JD2 = BaseCalFun::CalAngleX(posJD1, posJD2);
        double dTangentLen = CalTangentLen(dFL, dBL, dArcR, abs(dTurnAngle), true);
        posZH = posJD2 - Point2d(cos(dAngle_JD1_JD2), sin(dAngle_JD1_JD2)) * dTangentLen;
        //HY点
        posHY = (dFL > 0.0 ? 
                 BaseCalFun::TransferPos(posZH, CalCurvePos(dFL, dArcR), bTurnLeft, -dAngle_JD1_JD2) : posZH);
        //HZ点
        double dAngle_JD3_JD2 = BaseCalFun::CalAngleX(posJD3, posJD2);
        dTangentLen = CalTangentLen(dFL, dBL, dArcR, abs(dTurnAngle), false);
        posHZ = posJD2 - Point2d(cos(dAngle_JD3_JD2), sin(dAngle_JD3_JD2)) * dTangentLen;
        //YH点
        posYH = (dBL > 0.0 ? 
                 BaseCalFun::TransferPos(posHZ, CalCurvePos(dBL, dArcR), !bTurnLeft, -dAngle_JD3_JD2) : posHZ);
        
        if (m_nElementCount == 0)
        {
            //头段直线
            BaseLineElement* pLineElement = new StraightLineElement;
            pLineElement->m_posStart = posJD1;
            pLineElement->m_posEnd = posZH;
            pLineElement->m_dStartTanAngle = dAngle_JD1_JD2;
            pLineElement->m_dEndTanAngle = dAngle_JD1_JD2;
            pLineElement->m_eElementType = ElementType::Line;
            pLineElement->m_dStartCml = dCurrentCml;
            pLineElement->m_dTotalLen = posJD1.distanceTo(posZH);
            pLineElement->m_nIndex = m_nElementCount;
            m_arrLineElement[m_nElementCount++] = pLineElement;
            dCurrentCml += pLineElement->m_dTotalLen;
        }
        else
        {
            BaseLineElement* backElement = m_arrLineElement[m_nElementCount - 1];
            backElement->m_posEnd = posZH;
            backElement->m_dTotalLen = backElement->m_posStart.distanceTo(posZH);
            dCurrentCml += backElement->m_dTotalLen;
        }
        
        if (dArcR > 0.0)
        {
            //缓和曲线角度
            double dFrontCurveAngle = dFL / (dArcR * 2.0);
            double dBackCurveAngle = dBL / (dArcR * 2.0);
            
            if (dFL > 0.0)
            {
                //前缓和曲线
                CurveElement* pFrontCurveElement = new CurveElement;
                pFrontCurveElement->m_posStart = posZH;
                pFrontCurveElement->m_posEnd = posHY;
                pFrontCurveElement->m_dStartTanAngle = dAngle_JD1_JD2;
                pFrontCurveElement->m_dEndTanAngle = dAngle_JD1_JD2 + (bTurnLeft ? dFrontCurveAngle : -dFrontCurveAngle);
                pFrontCurveElement->m_eElementType = ElementType::FrontCurve;
                pFrontCurveElement->m_dArcR = dArcR;
                pFrontCurveElement->m_dStartCml = dCurrentCml;
                pFrontCurveElement->m_dTotalLen = dFL;
                pFrontCurveElement->m_bTurnLeft = bTurnLeft;
                pFrontCurveElement->m_nIndex = m_nElementCount;
                m_arrLineElement[m_nElementCount++] = pFrontCurveElement;
                dCurrentCml += pFrontCurveElement->m_dTotalLen;
            }
            
            //圆弧段
            ArcElement* pArcElement = new ArcElement;
            pArcElement->m_posStart = posHY;
            pArcElement->m_posEnd = posYH;
            pArcElement->m_dStartTanAngle = dAngle_JD1_JD2 + (bTurnLeft ? dFrontCurveAngle : -dFrontCurveAngle);
            pArcElement->m_eElementType = ElementType::Arc;
            pArcElement->m_dArcR = dArcR;
            pArcElement->m_dStartCml = dCurrentCml;
            pArcElement->m_dTotalLen = dArcR * (abs(dTurnAngle) - dFrontCurveAngle - dBackCurveAngle);
            pArcElement->m_bTurnLeft = bTurnLeft;
            pArcElement->m_nIndex = m_nElementCount;
            m_arrLineElement[m_nElementCount++] = pArcElement;
            dCurrentCml += pArcElement->m_dTotalLen;
            
            if (dBL > 0.0)
            {
                //后缓和曲线
                CurveElement* pBackCurveElement = new CurveElement;
                pBackCurveElement->m_posStart = posYH;
                pBackCurveElement->m_posEnd = posHZ;
                pBackCurveElement->m_dStartTanAngle = dAngle_JD3_JD2 + (bTurnLeft ? -dBackCurveAngle : dBackCurveAngle) + MATH_PI;
                pBackCurveElement->m_dEndTanAngle = dAngle_JD3_JD2;
                pBackCurveElement->m_eElementType = ElementType::BackCurve;
                pBackCurveElement->m_dArcR = dArcR;
                pBackCurveElement->m_dStartCml = dCurrentCml;
                pBackCurveElement->m_dTotalLen = dBL;
                pBackCurveElement->m_bTurnLeft = bTurnLeft;
                pBackCurveElement->m_nIndex = m_nElementCount;
                m_arrLineElement[m_nElementCount++] = pBackCurveElement;
                dCurrentCml += pBackCurveElement->m_dTotalLen;
            }
        }
        
        //后直线
        BaseLineElement* pLineElement = new StraightLineElement;
        pLineElement->m_posStart = posHZ;
        pLineElement->m_posEnd = posJD3;
        pLineElement->m_dStartTanAngle = dAngle_JD3_JD2 + (dAngle_JD3_JD2 > MATH_PI ? -MATH_PI : MATH_PI);
        pLineElement->m_eElementType = ElementType::Line;
        pLineElement->m_dStartCml = dCurrentCml;
        pLineElement->m_dTotalLen = posHZ.distanceTo(posJD3);
        pLineElement->m_nIndex = m_nElementCount;
        m_arrLineElement[m_nElementCount++] = pLineElement;
    }
}
