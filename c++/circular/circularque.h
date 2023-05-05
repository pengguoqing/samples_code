/*
    content:  threa safe circular queue implemented by cplusplus standard library
    
    author:  pengguoqing
    email:   forwardapeng@gmail.com
    date:    2023-05-05
*/


#ifndef   _cplusplus_circularque_h_
#define   _cplusplus_circularque_h_

#include <vector>
#include <mutex>

template<class T>
class CircularQue {
public:
    CircularQue(size_t size = 10);
    ~CircularQue();

    bool IsReadAble() const;
    bool IsWriteAble() const;

    void EnQue(T data);
    void DeQue(T data);


private:
    std::vector<T> m_data;
    std::mutex     m_rwlock;
    int            m_front;
    int            m_rear;
    size_t         m_size;
};

template<class T>
CircularQue<T>::CircularQue(size_t size) {


}

template<typename T>
CircularQue<T>::~CircularQue() {

}





#endif

