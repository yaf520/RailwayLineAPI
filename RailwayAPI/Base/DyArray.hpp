//
//  Array.hpp
//  test
//
//  Created by 易傲飞 on 2025/1/3.
//

#ifndef Array_hpp
#define Array_hpp

#include <assert.h>
#include <new>

#define ARRAY_GROWTH_THRESHOLD 0x10000

template <class T>
class DyArray
{
private:
    int m_nMaxCount;            //最大数目
    int m_nElementCount;        //当前数目
    int m_nGrowCount;           //增长数目
    T* m_pData;                 //缓存指针
    
public:
    DyArray();
    DyArray(const DyArray<T>& src);
    virtual ~DyArray();
    
public:
    /// 获取当前元素数目
    inline int GetCount() {
        return m_nElementCount;
    }
    
public:
    /// 添加元素
    /// - Parameter item: 新元素
    int Add(const T& item);
    
    /// 设置大小
    /// - Parameter nNewSize: 数组大小
    void SetSize(int nNewSize);
    
    /// 设置新元素位置
    /// - Parameters:
    ///   - nIndex: 索引
    ///   - item: 新元素
    void SetAtGrow(int nIndex, const T& item);
    
    /// 移除所有元素
    void RemoveAll();
    
    /// 移除指定位置指定数目元素
    /// - Parameter nIndex: 移除数目
    void RemoveAt(int nIndex, int nCount = 1);
    
    /// 在指定位置插入元素
    /// - Parameters:
    ///   - nIndex: 插入索引
    ///   - item: 新元素
    void InsertAt(int nIndex, const T& item, int nCount = 1);
    
    void Copy(const DyArray<T>& src);
    
    /// 获取指定位置元素
    /// - Parameter nIndex: 索引
    T& GetAt(int nIndex);
    
    /// 获取指定位置元素
    /// - Parameter nIndex: 索引
    const T& GetAt(int nIndex) const;
    
    T* begin() { return m_pData;}
    T* end() { return m_pData + m_nElementCount;}
    
    const T* begin() const { return m_pData;}
    const T* end() const { return m_pData + m_nElementCount;}
    
public:
    virtual T& operator[] (int nIndex);
    virtual const T& operator[] (int nIndex) const;
    virtual DyArray<T>& operator = (const DyArray<T>& src);
    
public:
    /// 申请内存
    /// - Parameter nNewCount: 元素数目
    void AllocMemory(int nNewCount);
};

template <class T>
DyArray<T>::DyArray()
{
    m_pData = nullptr;
    m_nMaxCount = 0;
    m_nElementCount = 0;
    m_nGrowCount = 8;
}

template <class T>
DyArray<T>::DyArray(const DyArray<T>& src)
{
    Copy(src);
}

template <class T>
DyArray<T>::~DyArray()
{
    if (m_pData)
    {
        for (int i = 0; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        
        delete [] (char*)m_pData;
        m_pData = nullptr;
    }
}

template <class T>
int DyArray<T>::Add(const T &item)
{
    int nIndex = m_nElementCount;
    SetAtGrow(nIndex, item);
    return nIndex;
}

template <class T>
void DyArray<T>::SetAtGrow(int nIndex, const T &item)
{
    assert(nIndex >= 0);
    if (nIndex < 0)
        return;
    
    if (nIndex >= m_nElementCount)
        SetSize(m_nElementCount + 1);
    m_pData[nIndex] = item;
}

template <class T>
void DyArray<T>::SetSize(int nNewSize)
{
    assert(nNewSize >= 0);
    if (nNewSize < 0)
        return;
    
    AllocMemory(nNewSize);
    if (nNewSize > m_nElementCount)
    {
        for (int i = m_nElementCount; i < nNewSize; i++)
            new ((void*)(m_pData + i)) T();
    }
    else if (nNewSize < m_nElementCount)
    {
        for (int i = nNewSize; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        memset((void*)(m_pData + nNewSize), 0, m_nElementCount - nNewSize);
    }
    
    m_nElementCount = nNewSize;
}

template <class T>
void DyArray<T>::InsertAt(int nIndex, const T& item, int nCount)
{
    if (nIndex < m_nElementCount)
    {
        int nOldCount = m_nElementCount;
        SetSize(nCount + m_nElementCount);
        for (int i = 0; i < nCount; i++)
            (m_pData + nOldCount + i)->~T();
        
        memmove((void*)(m_pData + nIndex + nCount), (void*)(m_pData + nIndex), sizeof(T) * (nOldCount - nIndex));
        memset((void*)(m_pData + nIndex), 0, sizeof(T) * nCount);
        for (int i = 0; i < nCount; i++)
            new ((void*)(m_pData + nIndex + i)) T();
    }
    else
        SetSize(nIndex + nCount);
    
    for (int i = 0; i < nCount; i++)
        m_pData[nIndex + i] = item;
}

template <class T>
void DyArray<T>::Copy(const DyArray<T>& src)
{
    assert(this != &src);
    if (this == &src)
        return;
    
    if (m_nElementCount > 0)
        RemoveAll();
    
    SetSize(src.m_nElementCount);
    for (int i = 0; i < m_nElementCount; i++)
        m_pData[i] = src[i];
}

template <class T>
void DyArray<T>::RemoveAll()
{
    if (m_pData)
    {
        for (int i = 0; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        
        memset((void*)m_pData, 0, sizeof(T) * m_nElementCount);
        m_nElementCount = 0;
    }
}

template <class T>
void DyArray<T>::RemoveAt(int nIndex, int nCount)
{
    assert(nIndex + nCount <= m_nElementCount);
    if (nIndex + nCount > m_nElementCount)
        return;
    
    for (int i = 0; i < nCount; i++)
        (m_pData + nIndex + i)->~T();
    int nMoveCount = m_nElementCount - (nIndex + nCount);
    if (nMoveCount > 0)
        memmove(m_pData + nIndex, m_pData + nIndex + nCount, sizeof(T) * nMoveCount);
    
    m_nElementCount -= nCount;
}

template <class T>
T& DyArray<T>::GetAt(int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nElementCount);
    if (nIndex < 0 || nIndex >= m_nElementCount)
        throw;
    
    return m_pData[nIndex];
}

template <class T>
const T& DyArray<T>::GetAt(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nElementCount);
    if (nIndex < 0 || nIndex >= m_nElementCount)
        throw;
    
    return m_pData[nIndex];
}

template <class T>
T& DyArray<T>::operator[] (int nIndex)
{
    return GetAt(nIndex);
}

template <class T>
const T& DyArray<T>::operator[](int nIndex) const
{
    return GetAt(nIndex);
}

template <class T>
DyArray<T>& DyArray<T>::operator = (const DyArray<T>& src)
{
    Copy(src);
    return *this;
}

template <class T>
void DyArray<T>::AllocMemory(int nNewCount)
{
    assert(nNewCount > 0);
    if (nNewCount <= 0)
        return;
    
    if (nNewCount > m_nMaxCount)
    {
        const int nGrowCount = (m_nMaxCount * sizeof(T) < ARRAY_GROWTH_THRESHOLD ? m_nMaxCount : ARRAY_GROWTH_THRESHOLD / sizeof(T));
        int minSize = m_nMaxCount + (nGrowCount < m_nGrowCount ? m_nGrowCount : nGrowCount);
        if (nNewCount > minSize)
            minSize = nNewCount;
        m_nMaxCount += minSize;
        
        T* pNewData = (T*)new char[sizeof(T) * m_nMaxCount];
        memcpy((void*)pNewData, (void*)m_pData, sizeof(T) * m_nElementCount);
        memset((void*)(pNewData + m_nElementCount), 0, sizeof(T) * (m_nMaxCount - m_nElementCount));
        
        delete [] m_pData;
        m_pData = pNewData;
    }
}

#endif /* Array_hpp */
