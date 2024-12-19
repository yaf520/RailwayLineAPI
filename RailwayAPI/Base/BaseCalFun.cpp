//
//  BaseCalFun.cpp
//
//  Created by 易傲飞 on 2023/10/27.
//

#include <stdio.h>
#include <string.h>
#include "BaseCalFun.hpp"

double BaseCalFun::CalAngleX(Point2d posStart, Point2d posEnd)
{
    double dDeltaX = posEnd.x - posStart.x;
    double dDeltaY = posEnd.y - posStart.y;
    if (abs(dDeltaX) < s_dCalPrecision)
        return (dDeltaY >= 0.0 ? MATH_PI_2 : MATH_PI * 1.5);
    
    double dAngle = atan(dDeltaY / dDeltaX);
    if (dDeltaY >= 0.0)
    {
        if (dDeltaX < -s_dCalPrecision)
            dAngle += MATH_PI;
    }
    else
    {
        if (dDeltaX < -s_dCalPrecision)
            dAngle += MATH_PI;
        else
            dAngle += MATH_PI * 2.0;
    }
    
    return dAngle;
}

Point2d BaseCalFun::PointToLineProjection(Point2d A, Point2d O, Point2d B, double& dPecent)
{
    Vector2d OA = A - O;
    Vector2d OB = B - O;
    if (OA.isZeroVec())
    {
        dPecent = 0.0;
        return A;
    }
    if (OB.isZeroVec())
    {
        dPecent = 1.0;
        return B;
    }
    
    dPecent = OA.dot(OB) / OB.dot(OB);
    
    return Point2d(O.x + OB.x * dPecent, O.y + OB.y * dPecent);
}

double BaseCalFun::CalAngleBy2Vec(Vector2d vec1, Vector2d vec2)
{
    double dModel1 = vec1.model();
    double dModel2 = vec2.model();
    double dDot = vec1.dot(vec2);
    double dValue = dDot /  (dModel1 * dModel2);
    //限制范围
    dValue = __max(dValue, -1.0);
    dValue = __min(dValue, 1.0);
    double dTurnAngle = acos(dValue);
    double dCross = vec1.cross(vec2);
    return (dCross > 0.0 ? dTurnAngle : -dTurnAngle);
}

double BaseCalFun::CalTurnAngle(Point2d p1, Point2d p2, Point2d p3)
{
    Vector2d vecAO = p2 - p1;
    Vector2d vecOB = p3 - p2;
    
    return CalAngleBy2Vec(vecAO, vecOB);
}

double BaseCalFun::CalTurnAngle(Point2d p1, Point2d p2, Point2d p3, Point2d p4)
{
    Vector2d vecAO = p2 - p1;
    Vector2d vecOB = p4 - p3;
    
    return CalAngleBy2Vec(vecAO, vecOB);
}

Point2d BaseCalFun::TransferPos(Point2d posBase, Point2d posTransfer, bool bTurnDir, double dAngle)
{
    if (!bTurnDir)
        posTransfer.y *= -1.0;
    
    Point2d vecOffset(cos(dAngle) * posTransfer.x + sin(dAngle) * posTransfer.y,
                  -sin(dAngle) * posTransfer.x + cos(dAngle) * posTransfer.y);
    
    return posBase + vecOffset;
}

Point2d BaseCalFun::TransferPosReversal(Point2d posBase, Point2d posTransfer, bool bTurnDir, double dAngle)
{
    Point2d posTemp = posTransfer - posBase;
    Point2d posRet(cos(dAngle) * posTemp.x + sin(dAngle) * posTemp.y,
                   -sin(dAngle) * posTemp.x + cos(dAngle) * posTemp.y);
    
    if (!bTurnDir)
        posRet.y *= -1.0;
    
    return posRet;
}

Point2d BaseCalFun::TransferBasePos(Point2d posTarget, Point2d posRelative, bool bTurnDir, double dAngle)
{
    if (!bTurnDir)
        posRelative.y *= -1.0;
    
    Point2d vecOffset(cos(dAngle) * posRelative.x + sin(dAngle) * posRelative.y,
                  -sin(dAngle) * posRelative.x + cos(dAngle) * posRelative.y);
    
    return posTarget - vecOffset;
}

//角度转换
void BaseCalFun::KeepAngleIn2PI(double& dAngle)
{
    while (dAngle < 0.0 || dAngle > MATH_PI * 2.0) {
        if (dAngle < 0.0)
            dAngle += MATH_PI * 2.0;
        
        if (dAngle > MATH_PI * 2.0)
            dAngle -= MATH_PI * 2.0;
    }
}

double BaseCalFun::Round(const double& dValue, int nPrec)
{
    double dRound = pow(10.0, nPrec);
    return round(dValue * dRound) / dRound;
}

void BaseCalFun::SplitMilepost(const char strMilepost[64], double& dMileValue, char strMark[16])
{
    char strTmp[64] = {0};
    int dPos_K = 0;
    int dLen = 0;
    bool bExitingPlus = false;
    int dMinusFind = 0;
    
    strcpy(strTmp, strMilepost);
    while (true) {
        if (strMilepost[dLen] == '+')
        {
            dMileValue = atof(strMilepost + dLen + 1);
            strTmp[dLen] = '\0';
            bExitingPlus = true;
            break;
        }
        else if (strMilepost[dLen] == '-')
        {
            if (dMinusFind == 0)
            {
                char cTmp = strMilepost[dLen - 1];
                if ((cTmp >= 'a' && cTmp <= 'z') || (cTmp >= 'A' && cTmp <= 'Z'))
                    dMinusFind++;
                else
                {
                    dMileValue = atof(strMilepost + dLen + 1);
                    strTmp[dLen] = '\0';
                    bExitingPlus = true;
                    dMinusFind++;
                    break;
                }
            }
            else
            {
                dMileValue = atof(strMilepost + dLen + 1);
                strTmp[dLen] = '\0';
                bExitingPlus = true;
                dMinusFind++;
                break;
            }
        }
        else if ((strMilepost[dLen] >= 'a' && strMilepost[dLen] <= 'z') || (strMilepost[dLen] >= 'A' && strMilepost[dLen] <= 'Z'))
        {
            dPos_K = dLen + 1;
        }
        else if (strMilepost[dLen] == '\0')
        {
            break;
        }
        dLen++;
    }
    
    if (bExitingPlus)
    {
        if (dMinusFind != 0)
        {
            if (strTmp[dPos_K] == '-')
            {
                if (dMinusFind == 1)
                    dMileValue = -1 * (atof(strTmp + dPos_K + 1) * 1000.0 - dMileValue);
                else if (dMinusFind == 2)
                    dMileValue = -1 * (atof(strTmp + dPos_K + 1) * 1000.0 + dMileValue);
                else
                    dMileValue = 0.0;
            }
            else
                dMileValue = -1 * (atof(strTmp + dPos_K) * 1000.0 + dMileValue);
        }
        else
            dMileValue = atof(strTmp + dPos_K) * 1000.0 + dMileValue;
    }
    else
        dMileValue = atof(strTmp + dPos_K);
    
    if (dPos_K > 0)
    {
        strTmp[dPos_K] = '\0';
        strcpy(strMark, strTmp);
    }
    else
    {
        strMark[0] = '\0';
    }
}

void BaseCalFun::MileCharacterStr(const char strMilepost[16], double dMileVal, int nPrec, bool bDelZero, char strOut[64])
{
    int nKilo = 0;
    double dHundred = 0.0;
    char strTmpMl[16] = {0};
    char strFormat[8] = {0};
    int nMinus = 1;
    
    if (dMileVal < -0.00000001)
    {
        nMinus = -1;
        dMileVal = fabs(dMileVal);
    }
    
    nKilo = powl(10, nPrec);
    dMileVal = floor(dMileVal * nKilo + 0.5) / nKilo;
    
    //公里标
    nKilo = floor((dMileVal + 0.000001) / 1000.0);
    //百米标
    dHundred = dMileVal - nKilo * 1000.0 + 1000.0;
    //格式
    snprintf(strFormat, 8, "%%0.%dlf", nPrec);
    snprintf(strTmpMl, 16, strFormat, dHundred);
    
    strTmpMl[0] = (nMinus == 1 ? '+' : '-');
    
    if (bDelZero)
    {
        int shLngth = strlen(strTmpMl) - 1;
        int sh0Pos = -1;
        while (shLngth > 0) {
            if (sh0Pos < 0 && strTmpMl[shLngth] > 48 && strTmpMl[shLngth] < 58)
            {
                sh0Pos = shLngth + 1;
                
                shLngth--;
                continue;;
            }
            else if (strTmpMl[shLngth] == '.')
            {
                if (sh0Pos < 0)
                    sh0Pos = shLngth;
                
                break;
            }
            else
                shLngth--;
        }
        
        if (shLngth > 0 && sh0Pos > 0)
            strTmpMl[sh0Pos] = '\0';
    }
    
    snprintf(strOut, 64, "%s%d%s", strMilepost, nKilo * nMinus, strTmpMl);
}

int BaseCalFun::CalDecimalDigits(double dValue)
{
    int i = 0;
    double dTmpPre = 0.0000001;
    dValue = fabs(dValue);
    if (fabs(dValue - floor(dValue)) <= dTmpPre)
        return i;
    else
    {
        dValue = dValue - floor(dValue) + 0.000000001;
        while (true) {
            i++;
            dValue = dValue * 10.01 + 0.000000001;
            dTmpPre = dTmpPre * 10.01 + 0.0000000;
            if (fabs(dValue) - floor(dValue) <= dTmpPre)
                break;
            else if (i == 7)
                break;
        }
    }
    
    return i;
}

uint32_t BaseCalFun::Factorial(uint32_t n)
{
    if (n == 0 || n == 1)
        return 1;
    
    uint32_t nResult = 1;
    for (int m = 1; m <= n; m++)
        nResult *= m;
    
    return nResult;
    
    /*
    //避免使用递归
    if (i == 0 || i == 1)
        return 1;
    else
        return i * Factorial(i - 1);
     */
}
