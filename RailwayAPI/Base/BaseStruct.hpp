//
//  ElementInfo.hpp
//
//  Created by 易傲飞 on 2023/10/20.
//

#ifndef ElementInfo_hpp
#define ElementInfo_hpp

#include <string.h>

enum class JDType : int
{
    Invaild = -3,           //无效
    End,                    //终点
    Start,                  //起点
    TurnPoint,              //转折点
    ThreeUnit = 3,          //三单元曲线
    ThreeUnitBack,          //三单元回头曲线
    FiveUnit,               //五单元曲线
    FiveUnitBack            //五单元回头曲线
};

///交点信息
struct tagJDInfo
{
    JDType nJDType;         //交点类型(-1:起点 -2:终点 3:三单元平曲线 4:三单元回头曲线 5:五单元平曲线 6:五单元回头曲线 0:转折点坐标)
    double dX;              //X坐标
    double dY;              //Y坐标
    double dX_End;          //X结束坐标
    double dY_End;          //Y结束坐标
    double dL1;             //第一缓和曲线长度
    double dA1;             //第一缓和曲线参数
    double dL2;             //第二缓和曲线长度
    double dA2;             //第二缓和曲线参数
    double dL3;             //第三缓和曲线长度
    double dA3;             //第三缓和曲线参数
    double dEnterR;         //入缓和曲线半径(__DBL_MAX__代表无穷大,即直线)
    double dExitR;          //出缓和曲线半径(__DBL_MAX__代表无穷大,即直线)
    double dArcR1;          //第一圆曲线半径
    double dArcR2;          //第二圆曲线半径
    double dID;             //弧长控制参数(0为两段圆弧相等，非0则为第一段圆弧长度)
    //int nBelongTo;          //所属线元索引
    
    ///初始化值
    tagJDInfo()
    {
        nJDType = JDType::Invaild;
        dX = 0.0;
        dY = 0.0;
        dL1 = -1.0;
        dA1 = -1.0;
        dL2 = -1.0;
        dA2 = -1.0;
        dL3 = -1.0;
        dA3 = -1.0;
        dX_End = 0.0;
        dY_End = 0.0;
        dArcR1 = 0.0;
        dArcR2 = 0.0;
        dID = 0.0;
        dEnterR = __DBL_MAX__;
        dExitR = __DBL_MAX__;
        //nBelongTo = -1;
    }
};

///断链信息
struct tagDLInfo
{
    double dBefLich;        //断前里程
    double dBehLich;        //断后里程
    char strBefNo[16];      //断前冠号
    char strBehNo[16];      //断后冠号
    
    tagDLInfo()
    {
        dBefLich = 0.0;
        dBehLich = 0.0;
        memset(strBefNo, 0, sizeof(strBefNo));
        memset(strBehNo, 0, sizeof(strBehNo));
    }
    
    virtual tagDLInfo& operator = (const tagDLInfo& info)
    {
        this->dBefLich = info.dBefLich;
        this->dBehLich = info.dBehLich;
        strcpy(this->strBefNo, info.strBefNo);
        strcpy(this->strBehNo, info.strBehNo);
        return *this;
    }
};

///坡度信息
struct tagSlopeInfo
{
    char strNo[16];         //冠号
    double dCml;            //变坡点里程
    double dHeight;         //变坡点高程
    double dArcR;           //竖曲线半径
};

//线元类型
enum class ElementType : int
{
    Invalid = -1,
    Line,                   //直线
    Arc,                    //圆弧
    SpiralCurve             //缓和曲线
};

struct PointExport
{
    double dX;
    double dY;
    
    PointExport()
    {
        dX = 0.0;
        dY = 0.0;
    }
    
    PointExport(double dX, double dY)
    {
        this->dX = dX;
        this->dY = dY;
    }
};

//导出子项
struct tagExportLineElement
{
    ElementType eLineType;
    PointExport* pArrPos;
    int nPosCount;
    
    tagExportLineElement()
    {
        eLineType = (ElementType)-1;
        pArrPos = nullptr;
        nPosCount = 0;
    }
    
    virtual ~tagExportLineElement()
    {
        if (pArrPos)
        {
            delete[] pArrPos;
            pArrPos = nullptr;
            nPosCount = 0;
        }
    }
    
    virtual tagExportLineElement& operator = (const tagExportLineElement& data)
    {
        this->eLineType = data.eLineType;
        this->nPosCount = data.nPosCount;
        this->pArrPos = new PointExport[this->nPosCount];
        for (int i = 0; i < nPosCount; i++)
            *(pArrPos + i) = data.pArrPos[i];
        
        return *this;
    }
};

///里程+投影距离+方位角
struct tagCmlDistAngle
{
    double dCml;
    double dDist;
    double dFwj;
    
    tagCmlDistAngle()
    {
        dCml = 0.0;
        dDist = 0.0;
        dFwj = 0.0;
    }
};

///曲线要素子项
struct tagCurveElementItem
{
    int nJDIndex;
    double dN_Y;
    double dE_X;
    double dTurnAngle;
    double dArcR;
    double dL1;
    double dL2;
    double dT1;
    double dT2;
    double dA1;
    double dA2;
    double dE;
    double dCurveLen;
    
    //属性点里程
    double dJDCml;
    double dZHCml;
    double dHYCml;
    double dQZCml;
    double dYHCml;
    double dHZCml;
    
    tagCurveElementItem()
    {
        nJDIndex = 0;
        dN_Y = 0.0;
        dE_X = 0.0;
        dTurnAngle = 0.0;
        dArcR = 0.0;
        dL1 = 0.0;
        dL2 = 0.0;
        dT1 = 0.0;
        dT2 = 0.0;
        dA1 = 0.0;
        dA2 = 0.0;
        dE = 0.0;
        dCurveLen = 0.0;
        dJDCml = 0.0;
        dZHCml = 0.0;
        dHYCml = 0.0;
        dQZCml = 0.0;
        dYHCml = 0.0;
        dHZCml = 0.0;
    }
};

#endif /* ElementInfo_hpp */
