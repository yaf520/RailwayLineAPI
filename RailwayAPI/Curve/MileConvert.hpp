//
//  MileConvert.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2024/12/25.
//

#ifndef MileConvert_hpp
#define MileConvert_hpp

#include "BaseStruct.hpp"

class MileConvert
{
public:
    MileConvert();
    virtual ~MileConvert();
    
private:
    tagDLInfo* m_pDLArr;
    int m_nDLCount;
    
public:
    bool SetDLData(const tagDLInfo* pDLInfo, int nCount);
    
public:
    ///计算外业测量里程
    double GetMilepost(double dConitinueMile, char strMark[16], int& iReliability, char strErr[64]);
    ///计算累计曲线长度
    double GetSumLengthOfMile(char strMark[16], double dMileValue, int& iReliability, char strErr[64]);
    ///连续里程->现场里程
    bool TrsCmltoCkml(double cml, char ckml[64], int nPrec);
    ///现场里程->连续里程
    bool TrsCkmlToCml(char ckml[64], double& cml, char strErr[64]);
};

#endif /* MileConvert_hpp */
