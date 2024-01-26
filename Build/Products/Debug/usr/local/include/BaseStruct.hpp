//
//  ElementInfo.hpp
//  icon_dg
//
//  Created by 易傲飞 on 2023/10/20.
//

#ifndef ElementInfo_hpp
#define ElementInfo_hpp

#include <string.h>

///交点信息
struct tagJDInfo
{
    double dX;              //X坐标
    double dY;              //Y坐标
    double dFL;             //前缓和曲线长度
    double dBL;             //后缓和曲线长度
    double dArcR;           //圆曲线半径
};

///断链信息
struct tagDLInfo
{
    double dBefLich;        //断前里程
    double dBehLich;        //断后里程
    char strBefNo[12];      //断前冠号
    char strBehNo[12];      //断后冠号
    
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
    char strNo[12];         //冠号
    double dCml;            //变坡点里程
    double dHeight;         //变坡点高程
    double dArcR;           //竖曲线半径
};

//线元类型
enum ElementType
{
    Line = 0,               //直线
    Arc,                    //圆弧
    FrontCurve,             //前缓和曲线
    BackCurve               //后缓和曲线
};

#endif /* ElementInfo_hpp */
