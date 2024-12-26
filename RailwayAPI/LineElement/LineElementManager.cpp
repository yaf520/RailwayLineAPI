//
//  HorizontalLogic.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include "LineElementManager.hpp"
#include "BaseCalFun.hpp"

LineElementManager::LineElementManager(CurveType eCurveType)
{
    m_arrLineElement = nullptr;
    m_nElementCount = 0;
    m_eCurveType = eCurveType;
    m_arrJD = nullptr;
    m_nJDCount = 0;
    m_arrCurveElement = nullptr;
    m_nCurveElementCount = 0;
}

LineElementManager::~LineElementManager()
{
    this->ResetData();
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
    
    if (m_arrJD)
    {
        delete m_arrJD;
        m_arrJD = nullptr;
        m_nJDCount = 0;
    }
    
    if (m_arrCurveElement)
    {
        delete [] m_arrCurveElement;
        m_arrCurveElement = nullptr;
        m_nCurveElementCount = 0;
    }
}

Point2d LineElementManager::CalUnitStartPos(uint32_t nIndex, const Vector2d& vecWhole)
{
    //交点坐标
    Point2d posJD1(((m_arrJD + nIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nIndex)->dX, (m_arrJD + nIndex)->dY) : Point2d((m_arrJD + nIndex)->dX_End, (m_arrJD + nIndex)->dY_End)));
    Point2d posJD2((m_arrJD + nIndex + 1)->dX, (m_arrJD + nIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nIndex + 2)->dX, (m_arrJD + nIndex + 2)->dY);
    
    Vector2d vecJD1_JD2 = posJD2 - posJD1;
    Vector2d vecJD2_JD3 = posJD3 - posJD2;
    
    double a = vecJD1_JD2.y;
    double b = -vecJD1_JD2.x;
    double c = vecJD1_JD2.y * posJD1.x - vecJD1_JD2.x * posJD1.y;
    double d = vecJD2_JD3.y;
    double e = -vecJD2_JD3.x;
    double f = vecJD2_JD3.x * (vecWhole.y - posJD2.y) - vecJD2_JD3.y * (vecWhole.x - posJD2.x);
    //世界坐标
    return Point2d((e * c - b * f) / (a * e - b * d), (d * c - a * f) / (b * d - a * e));
}

Point2d LineElementManager::CalculateLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    //起始半径
    double dEnterR = (m_arrJD + nCurIndex + 1)->dEnterR;
    double dExitR = (m_arrJD + nCurIndex + 1)->dExitR;
    //单元起点(相对)
    Point2d posCurveStart;
    
    switch (nJDType) {
        case JDType::ThreeUnit:
        case JDType::ThreeUnitBack:
        {
            //圆曲线半径
            double dArcR = (m_arrJD + nCurIndex + 1)->dArcR1;
            //转向角
            double dTurnAngle = (nJDType == JDType::ThreeUnit ? BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3) : BaseCalFun::CalTurnAngle(posJD1, posJD2, posJDRef, posJD3));
            //转向方向
            bool bTurnLeft = ((nJDType == JDType::ThreeUnit && dTurnAngle > 0.0) || (nJDType == JDType::ThreeUnitBack && dTurnAngle < 0.0));
            if (nJDType == JDType::ThreeUnitBack)
                dTurnAngle = MATH_PI * 2.0 - abs(dTurnAngle);
            
            //缓和曲线长度
            double dL1 = (m_arrJD + nCurIndex + 1)->dL1;
            double dL2 = (m_arrJD + nCurIndex + 1)->dL2;
            if (dL1 < 0.0)
            {
                double dC1 = pow((m_arrJD + nCurIndex + 1)->dA1, 2);
                dL1 = (dEnterR == __DBL_MAX__ ? dC1 / dArcR : dC1 * abs(dEnterR - dArcR) / dEnterR / dArcR);
            }

            if (dL2 < 0.0)
            {
                double dC2 = pow((m_arrJD + nCurIndex + 1)->dA2, 2);
                dL2 = (dExitR == __DBL_MAX__ ? dC2 / dArcR : dC2 * abs(dExitR - dArcR) / dExitR / dArcR);
            }
            
            //缓和曲线角
            double dEnterCurveAngle = (abs(dL1) < s_dCalPrecision ? 0.0 : ((dEnterR == __DBL_MAX__) ? dL1 / (dArcR * 2.0) : dL1 * (dEnterR + dArcR) / 2.0 / dEnterR / dArcR));
            double dExitCurveAngle = (abs(dL2) < s_dCalPrecision ? 0.0 : ((dExitR == __DBL_MAX__) ? dL2 / (dArcR * 2.0) : dL2 * (dExitR + dArcR) / 2.0 / dExitR / dArcR));
            //圆弧角度
            double dArcAngle = abs(dTurnAngle) - dEnterCurveAngle - dExitCurveAngle;
            //起点角度
            double dTanAngleStart = BaseCalFun::CalAngleX(posJD1, posJD2);
            //向量定义
            Vector2d vecWhole;
            
            if (nJDType == JDType::ThreeUnitBack)
                posCurveStart = posJD2;
            
            if (abs(dL1) > s_dCalPrecision)
            {
                //前缓和曲线
                SpiralLineElement* pSpiralElement = new SpiralLineElement();
                pSpiralElement->m_posStart = posCurveStart;
                pSpiralElement->m_dStartTanAngle = dTanAngleStart;
                pSpiralElement->m_dTotalLen = dL1;
                pSpiralElement->m_bTurnLeft = bTurnLeft;
                pSpiralElement->m_dEnterR =  dEnterR;
                pSpiralElement->m_dExitR = dArcR;
                pSpiralElement->InitData();
                //保存线元
                arrLineElement[nCurveElementCount++] = pSpiralElement;
                vecWhole += (pSpiralElement->m_posEnd - pSpiralElement->m_posStart);
                
                //变换起点
                posCurveStart = pSpiralElement->m_posEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->m_dEndTanAngle;
            }
            
            //圆曲线
            if (abs(dArcAngle) * dArcR > 0.01)
            {
                ArcElement* pArcElement = new ArcElement();
                pArcElement->m_posStart = posCurveStart;
                pArcElement->m_dStartTanAngle = dTanAngleStart;
                pArcElement->m_dArcR = dArcR;
                pArcElement->m_bTurnLeft = bTurnLeft;
                pArcElement->m_dTotalLen = dArcR * abs(dArcAngle);
                pArcElement->InitData();
                //保存线元
                arrLineElement[nCurveElementCount++] = pArcElement;
                vecWhole += (pArcElement->m_posEnd - pArcElement->m_posStart);
                
                //变换起点
                posCurveStart = pArcElement->m_posEnd;
                //变换角度
                dTanAngleStart = pArcElement->m_dEndTanAngle;
            }
            
            if (abs(dL2) > s_dCalPrecision)
            {
                //后缓和曲线
                SpiralLineElement* pSpiralElement = new SpiralLineElement();
                pSpiralElement->m_posStart = posCurveStart;
                pSpiralElement->m_dStartTanAngle = dTanAngleStart;
                pSpiralElement->m_dTotalLen = dL2;
                pSpiralElement->m_bTurnLeft = bTurnLeft;
                pSpiralElement->m_dEnterR = dArcR;
                pSpiralElement->m_dExitR = dExitR;
                pSpiralElement->InitData();
                //保存线元
                arrLineElement[nCurveElementCount++] = pSpiralElement;
                vecWhole += (pSpiralElement->m_posEnd - pSpiralElement->m_posStart);
            }
            
            //计算曲线起点世界坐标
            posCurveStart = (nJDType == JDType::ThreeUnit ? CalUnitStartPos(nCurIndex, vecWhole) : posJD2);
            
            break;
        }
        case JDType::FiveUnit:
        {
            //起点角度
            double dTanAngleStart = BaseCalFun::CalAngleX(posJD1, posJD2);
            //转向角
            double dTurnAngle = BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3);
            //转向方向
            bool bTurnLeft = (dTurnAngle > 0.0);
            //弧长控制参数
            double dID = (m_arrJD + nCurIndex + 1)->dID;
            //圆弧半径
            double dArcR1 = (m_arrJD + nCurIndex + 1)->dArcR1;
            double dArcR2 = (m_arrJD + nCurIndex + 1)->dArcR2;
            //缓和曲线长度
            double dL1 = (m_arrJD + nCurIndex + 1)->dL1;
            double dL2 = (m_arrJD + nCurIndex + 1)->dL2;
            double dL3 = (m_arrJD + nCurIndex + 1)->dL3;
            if (dL1 < 0.0)
            {
                double dC1 = pow((m_arrJD + nCurIndex + 1)->dA1, 2);
                dL1 = (dEnterR == __DBL_MAX__ ? dC1 / dArcR1 : dC1 * abs(dEnterR - dArcR1) / dEnterR / dArcR1);
            }
            if (dL2 < 0.0)
            {
                double dC2 = pow((m_arrJD + nCurIndex + 1)->dA2, 2);
                dL2 = dC2 * abs(dArcR2 - dArcR1) / dArcR2 / dArcR1;
            }
            if (dL3 < 0.0)
            {
                double dC3 = pow((m_arrJD + nCurIndex + 1)->dA3, 2);
                dL3 = (dExitR == __DBL_MAX__ ? dC3 / dArcR2 : dC3 * abs(dExitR - dArcR2) / dExitR / dArcR2);
            }
            //缓和曲线角度
            double dCurveAngle1 = (abs(dL1) < s_dCalPrecision ? 0.0 : (dEnterR == __DBL_MAX__ ? dL1 / dArcR1 / 2.0 : dL1 * (dEnterR + dArcR1) / 2.0 / dEnterR / dArcR1));
            double dCurveAngle2 = dL2 * (dArcR1 + dArcR2) / 2.0 / dArcR1 / dArcR2;
            double dCurveAngle3 = (abs(dL3) < s_dCalPrecision ? 0.0 : (dExitR == __DBL_MAX__ ? dL3 / dArcR2 / 2.0 : dL3 * (dExitR + dArcR2) / 2.0 / dExitR / dArcR2));
            //圆曲线角度之和
            double dArcAngleTotal = abs(dTurnAngle) - dCurveAngle1 - dCurveAngle2 - dCurveAngle3;
            //圆弧角度
            double dArcAngle1 = (dID == 0.0 ? dArcR2 * dArcAngleTotal / (dArcR1 + dArcR2) : dID / dArcR1);
            double dArcAngle2 = dArcAngleTotal - dArcAngle1;
            //变量定义
            Vector2d vecWhole;
            
            if (dL1 > 0.0)
            {
                //第一段缓和曲线
                SpiralLineElement* pSpiralElement = new SpiralLineElement();
                pSpiralElement->m_posStart = posCurveStart;
                pSpiralElement->m_dStartTanAngle = dTanAngleStart;
                pSpiralElement->m_dTotalLen = dL1;
                pSpiralElement->m_bTurnLeft = bTurnLeft;
                pSpiralElement->m_dEnterR = dEnterR;
                pSpiralElement->m_dExitR = dArcR1;
                pSpiralElement->InitData();
                
                arrLineElement[nCurveElementCount++] = pSpiralElement;
                vecWhole += (pSpiralElement->m_posEnd - pSpiralElement->m_posStart);
                
                //变换起点
                posCurveStart = pSpiralElement->m_posEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->m_dEndTanAngle;
            }
            
            do {
                //第一段圆曲线
                ArcElement* pArcElement = new ArcElement();
                pArcElement->m_posStart = posCurveStart;
                pArcElement->m_dStartTanAngle = dTanAngleStart;
                pArcElement->m_dArcR = dArcR1;
                pArcElement->m_bTurnLeft = bTurnLeft;
                pArcElement->m_dTotalLen = dArcR1 * abs(dArcAngle1);
                pArcElement->InitData();
                
                arrLineElement[nCurveElementCount++] = pArcElement;
                vecWhole += (pArcElement->m_posEnd - pArcElement->m_posStart);
                
                //变换起点
                posCurveStart = pArcElement->m_posEnd;
                //变换角度
                dTanAngleStart = pArcElement->m_dEndTanAngle;
            } while (false);
            
            do {
                //第二段缓和曲线
                SpiralLineElement* pSpiralElement = new SpiralLineElement();
                pSpiralElement->m_posStart = posCurveStart;
                pSpiralElement->m_dStartTanAngle = dTanAngleStart;
                pSpiralElement->m_dTotalLen = dL2;
                pSpiralElement->m_bTurnLeft = bTurnLeft;
                pSpiralElement->m_dEnterR =  dArcR1;
                pSpiralElement->m_dExitR = dArcR2;
                pSpiralElement->InitData();
                
                arrLineElement[nCurveElementCount++] = pSpiralElement;
                vecWhole += (pSpiralElement->m_posEnd - pSpiralElement->m_posStart);
                
                //变换起点
                posCurveStart = pSpiralElement->m_posEnd;
                //变换角度
                dTanAngleStart = pSpiralElement->m_dEndTanAngle;
            } while (false);
            
            do {
                //第二段圆曲线
                ArcElement* pArcElement = new ArcElement();
                pArcElement->m_posStart = posCurveStart;
                pArcElement->m_dStartTanAngle = dTanAngleStart;
                pArcElement->m_dArcR = dArcR2;
                pArcElement->m_bTurnLeft = bTurnLeft;
                pArcElement->m_dTotalLen = dArcR2 * abs(dArcAngle2);
                pArcElement->InitData();
                
                arrLineElement[nCurveElementCount++] = pArcElement;
                vecWhole += (pArcElement->m_posEnd - pArcElement->m_posStart);
                
                //变换起点
                posCurveStart = pArcElement->m_posEnd;
                //变换角度
                dTanAngleStart = pArcElement->m_dEndTanAngle;
            } while (false);
            
            if (dL3 > 0.0)
            {
                //第三段缓和曲线
                SpiralLineElement* pSpiralElement = new SpiralLineElement();
                pSpiralElement->m_posStart = posCurveStart;
                pSpiralElement->m_dStartTanAngle = dTanAngleStart;
                pSpiralElement->m_dTotalLen = dL3;
                pSpiralElement->m_bTurnLeft = bTurnLeft;
                pSpiralElement->m_dEnterR = dArcR2;
                pSpiralElement->m_dExitR = dExitR;
                pSpiralElement->InitData();
                
                arrLineElement[nCurveElementCount++] = pSpiralElement;
                vecWhole += (pSpiralElement->m_posEnd - pSpiralElement->m_posStart);
            }
            
            //计算曲线起点世界坐标
            posCurveStart = CalUnitStartPos(nCurIndex, vecWhole);
            
            break;
        }
        case JDType::FiveUnitBack:
        {
            break;
        }
            
        default:
            break;
    }
    
    return posCurveStart;
}

void LineElementManager::JointLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount, Point2d posCurveStart, double& dCurrentCml)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    
    //拼接线元
    if (nCurIndex != 0 && m_arrLineElement[m_nElementCount - 1]->m_eElementType == ElementType::Line)
    {
        BaseLineElement* pPreLineElement = m_arrLineElement[m_nElementCount - 1];
        //调整前一段直线
        pPreLineElement->m_posEnd = posCurveStart;
        pPreLineElement->m_dTotalLen = pPreLineElement->m_posStart.distanceTo(posCurveStart);
        
        if (pPreLineElement->m_dTotalLen < 0.1)
        {
            delete pPreLineElement;
            m_nElementCount--;
        }
        else
            dCurrentCml += pPreLineElement->m_dTotalLen;
    }
    
    //当前阶段起点
    Point2d posStart = (nCurIndex == 0 ? posJD1 : m_arrLineElement[m_nElementCount - 1]->m_posEnd);
    if (posStart.distanceTo(posCurveStart) >= 0.01)
    {
        //入直线
        StraightLineElement* pStraightLineElement = new StraightLineElement;
        pStraightLineElement->m_posStart = posStart;
        pStraightLineElement->m_posEnd = posCurveStart;
        pStraightLineElement->m_dStartTanAngle = pStraightLineElement->m_dEndTanAngle = BaseCalFun::CalAngleX(posJD1, posJD2);
        pStraightLineElement->m_dTotalLen = pStraightLineElement->m_posStart.distanceTo(pStraightLineElement->m_posEnd);
        pStraightLineElement->m_dStartCml = dCurrentCml;
        pStraightLineElement->m_nIndex = m_nElementCount;
        dCurrentCml += pStraightLineElement->m_dTotalLen;
        m_arrLineElement[m_nElementCount++] = pStraightLineElement;
    }
    
    for (uint8_t j = 0; j < nCurveElementCount; j++)
    {
        if (nJDType == JDType::ThreeUnit || nJDType == JDType::FiveUnit)
            arrLineElement[j]->AdjustData(posCurveStart);
        arrLineElement[j]->m_dStartCml = dCurrentCml;
        arrLineElement[j]->m_nIndex = m_nElementCount;
        m_arrLineElement[m_nElementCount++] = arrLineElement[j];
        
        dCurrentCml += arrLineElement[j]->m_dTotalLen;
    }
    
    if ((m_arrJD + nCurIndex + 1)->dExitR == __DBL_MAX__ || (m_arrJD + nCurIndex + 2)->nJDType == JDType::End)
    {
        BaseLineElement* pPreLineElement = m_arrLineElement[m_nElementCount - 1];
        if (pPreLineElement->m_posEnd.distanceTo(posJD3) >= 0.01)
        {
            //出直线
            StraightLineElement* pStraightLineElement = new StraightLineElement;
            pStraightLineElement->m_posStart = pPreLineElement->m_posEnd;
            pStraightLineElement->m_posEnd = posJD3;
            pStraightLineElement->m_dStartTanAngle = pStraightLineElement->m_dEndTanAngle
                = BaseCalFun::CalAngleX((nJDType == JDType::ThreeUnitBack ? posJDRef : posJD2), posJD3);
            pStraightLineElement->m_dStartCml = dCurrentCml;
            pStraightLineElement->m_nIndex = m_nElementCount;
            pStraightLineElement->m_dTotalLen = pStraightLineElement->m_posStart.distanceTo(pStraightLineElement->m_posEnd);
            m_arrLineElement[m_nElementCount++] = pStraightLineElement;
        }
    }
}

void LineElementManager::CurveElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t nCurveElementCount)
{
    //交点坐标
    Point2d posJD1 = ((m_arrJD + nCurIndex)->nJDType != JDType::ThreeUnitBack ? Point2d((m_arrJD + nCurIndex)->dX, (m_arrJD + nCurIndex)->dY) : Point2d((m_arrJD + nCurIndex)->dX_End, (m_arrJD + nCurIndex)->dY_End));
    Point2d posJD2((m_arrJD + nCurIndex + 1)->dX, (m_arrJD + nCurIndex + 1)->dY);
    Point2d posJD3((m_arrJD + nCurIndex + 2)->dX, (m_arrJD + nCurIndex + 2)->dY);
    Point2d posJDRef((m_arrJD + nCurIndex + 1)->dX_End, (m_arrJD + nCurIndex + 1)->dY_End);
    //交点类型
    JDType nJDType = (m_arrJD + nCurIndex + 1)->nJDType;
    
    if (nCurIndex == 0)
    {
        //起点
        m_arrCurveElement[nCurIndex].nJDIndex = nCurIndex;
        m_arrCurveElement[nCurIndex].dN_Y = posJD1.y;
        m_arrCurveElement[nCurIndex].dE_X = posJD1.x;
    }
    else if (nCurIndex + 2 == m_nJDCount - 1)
    {
        //终点
        m_arrCurveElement[nCurIndex + 2].nJDIndex = nCurIndex + 2;
        m_arrCurveElement[nCurIndex + 2].dN_Y = posJD3.y;
        m_arrCurveElement[nCurIndex + 2].dE_X = posJD3.x;
    }

    ///这里暂时只处理三单元曲线
    tagCurveElementItem& item = m_arrCurveElement[nCurIndex + 1];
    item.nJDIndex = nCurIndex + 1;
    item.dN_Y = posJD2.y;
    item.dE_X = posJD2.x;
    item.dTurnAngle = (nJDType == JDType::ThreeUnit || nJDType == JDType::FiveUnitBack ? BaseCalFun::CalTurnAngle(posJD1, posJD2, posJD3) : BaseCalFun::CalTurnAngle(posJD1, posJD2, posJDRef, posJD3));
    if (nJDType == JDType::ThreeUnitBack || nJDType == JDType::FiveUnitBack)
        item.dTurnAngle = MATH_PI * 2.0 - abs(item.dTurnAngle);

    if (nCurveElementCount > 0)
    {
        //切线长
        item.dT1 = arrLineElement[0]->m_posStart.distanceTo(posJD2);
        item.dT2 = arrLineElement[nCurveElementCount - 1]->m_posEnd.distanceTo(posJD2);
        
        for (int i = 0; i < nCurveElementCount; i++)
        {
            //总计长度
            item.dCurveLen += arrLineElement[i]->m_dTotalLen;
            
            if (arrLineElement[i]->m_eElementType == ElementType::Arc)
            {
                //圆曲线
                ArcElement* pArcElement = (ArcElement*)arrLineElement[i];
                //半径
                item.dArcR = pArcElement->m_dArcR;
                
                if (i > 0)
                {
                    item.dL1 = arrLineElement[i - 1]->m_dTotalLen;
                    item.dA1 = pow(((SpiralLineElement*)arrLineElement[i - 1])->m_dC, 0.5);
                }
                if (i + 1 < nCurveElementCount)
                {
                    item.dL2 = arrLineElement[i + 1]->m_dTotalLen;
                    item.dA2 = pow(((SpiralLineElement*)arrLineElement[i + 1])->m_dC, 0.5);
                }
                
                //属性点里程
                item.dHYCml = pArcElement->m_dStartCml;
                item.dYHCml = pArcElement->m_dStartCml + pArcElement->m_dTotalLen;
                item.dQZCml = pArcElement->m_dStartCml + pArcElement->m_dTotalLen / 2.0;
                item.dZHCml = (i > 0 ? arrLineElement[i - 1]->m_dStartCml : item.dHYCml);
                item.dHZCml = (i + 1 < nCurveElementCount ? arrLineElement[i + 1]->m_dStartCml + arrLineElement[i + 1]->m_dTotalLen : item.dYHCml);
                
                //外矢距
                Point2d posQZ;
                double dAngle = 0.0;
                pArcElement->TrsCmlDistToNE(item.dQZCml, 0.0, posQZ.x, posQZ.y, dAngle);
                item.dE = posQZ.distanceTo(posJD2);
            }
        }
    }
}

void LineElementManager::SetJDData(const tagJDInfo* pJDInfo, uint32_t nCount)
{
    if (!pJDInfo || nCount < 2) return;
    
    //重置数据
    ResetData();
    
    //最大线元数量
    uint32_t nMaxCount = nCount * 6 - 11;
    m_arrLineElement = new BaseLineElement*[nMaxCount];
    
    //保存交点
    m_nJDCount = nCount;
    m_arrJD = new tagJDInfo[m_nJDCount];
    memcpy(m_arrJD, pJDInfo, sizeof(tagJDInfo) * m_nJDCount);
    
    ///曲线要素数组
    m_nCurveElementCount = m_nJDCount;
    m_arrCurveElement = new tagCurveElementItem[m_nCurveElementCount];
    
    if (nCount == 2)
    {
        m_arrLineElement = new BaseLineElement*[1];
        
        //两个交点
        Point2d posJD1(pJDInfo->dX, pJDInfo->dY);
        Point2d posJD2((pJDInfo + 1)->dX, (pJDInfo + 1)->dY);
        double dAngle_JD1_JD2 = BaseCalFun::CalAngleX(posJD1, posJD2);
        
        BaseLineElement* pLineElement = new StraightLineElement;
        pLineElement->m_posStart = posJD1;
        pLineElement->m_posEnd = posJD2;
        pLineElement->m_dStartTanAngle = pLineElement->m_dEndTanAngle = dAngle_JD1_JD2;
        pLineElement->m_dStartCml = 0.0;
        pLineElement->m_dTotalLen = posJD1.distanceTo(posJD2);
        pLineElement->m_nIndex = m_nElementCount;
        m_arrLineElement[m_nElementCount++] = pLineElement;
        
        return;
    }
    
    //变量定义
    double dCurrentCml = 0.0;
    for (uint32_t nCurIndex = 0; nCurIndex + 2 < nCount; nCurIndex++)
    {
        //变量定义
        BaseLineElement* arrLineElement[5];
        memset(arrLineElement, 0, sizeof(BaseLineElement*) * 5);
        uint8_t nCurveElementCount = 0;
        
        //计算线元
        Point2d posCurveStart = CalculateLineElement(nCurIndex, arrLineElement, nCurveElementCount);
        
        //拼接线元
        JointLineElement(nCurIndex, arrLineElement, nCurveElementCount, posCurveStart, dCurrentCml);
        
        //计算曲线要素
        CurveElement(nCurIndex, arrLineElement, nCurveElementCount);
    }
    
    //调整线元数组大小
    if (m_nElementCount < nMaxCount)
    {
        BaseLineElement** arrLineElement = new BaseLineElement*[m_nElementCount];
        memcpy(arrLineElement, m_arrLineElement, sizeof(BaseLineElement*) * m_nElementCount);
        delete[] m_arrLineElement;
        m_arrLineElement = arrLineElement;
    }
}

tagExportLineElement* LineElementManager::ExportHorCurve(int& nArrCount, double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    nArrCount = 0;
    if (dEndCml - dStartCml < 0.0)
        return nullptr;
    
    BaseLineElement* pStartElement = CmlBelongTo(dStartCml);
    if (!pStartElement)
        return nullptr;
    
    BaseLineElement* pEndElement = CmlBelongTo(dEndCml);
    if (!pEndElement)
        return nullptr;
    
    int nStartIndex = pStartElement->m_nIndex;
    int nEndIndex = pEndElement->m_nIndex;
    
    nArrCount = nEndIndex - nStartIndex + 1;
    tagExportLineElement* pRet = new tagExportLineElement[nArrCount];
    int nExportIndex = 0;
    for (int nCurInex = nStartIndex; nCurInex <= nEndIndex; nCurInex++)
    {
        BaseLineElement* pCurLineElement = m_arrLineElement[nCurInex];
        
        double dCurStartCml = (nCurInex == nStartIndex ? dStartCml : pCurLineElement->m_dStartCml);
        double dCurEndCml = (nCurInex == nEndIndex ? dEndCml : pCurLineElement->m_dStartCml + pCurLineElement->m_dTotalLen);
        tagExportLineElement* pExportInfo = pCurLineElement->ExportHorCurve(dCurStartCml, dCurEndCml, dDist, dCurveStep);
        if (!pExportInfo)
        {
            nArrCount = 0;
            delete[] pRet;
            return nullptr;
        }
        *(pRet + nExportIndex++) = *pExportInfo;
        delete pExportInfo;
    }
    
    return pRet;
}

const tagJDInfo* LineElementManager::ExportJDInfo(int& nCount)
{
    nCount = m_nJDCount;
    return m_arrJD;
}

double LineElementManager::GetLength()
{
    if (m_nElementCount == 0 || !m_arrLineElement || m_nJDCount < 2 || !m_arrJD)
        return 0.0;
    
    switch (m_eCurveType) {
        case CurveType::HorizontalCurve:
        {
            double dTotalLen = m_arrLineElement[m_nElementCount - 1]->m_dStartCml + m_arrLineElement[m_nElementCount - 1]->m_dTotalLen;
            return BaseCalFun::Round(dTotalLen);
        }
        case CurveType::VerticalCurve:
        {
            double dTotalLen = m_arrJD[m_nJDCount - 1].dX - m_arrJD[0].dX;
            return BaseCalFun::Round(dTotalLen);
        }
        default:
        {
            return 0.0;
        }
    }
}
