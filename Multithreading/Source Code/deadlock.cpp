#include <iostream>
#include <thread>
#include <memory>
#include  <windows.h>

using namespace std;

class CriticalSection
{
private:
    CRITICAL_SECTION m_critSec;

public:
    CriticalSection();
    ~CriticalSection();

    void Lock();
    void UnLock();
};

class CriticalSectionLock
{
private:
    CriticalSection* m_pCritSec;

public:
    CriticalSectionLock(CriticalSection& critSec);
    ~CriticalSectionLock();

};


CriticalSection::CriticalSection()
{
    InitializeCriticalSectionEx(&m_critSec,0,0);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&m_critSec);
}

void CriticalSection::Lock()
{
    EnterCriticalSection(&m_critSec);
}

void CriticalSection::UnLock()
{
    LeaveCriticalSection(&m_critSec);
}

CriticalSectionLock::CriticalSectionLock(CriticalSection& critSec)
{
    m_pCritSec = &critSec;
    m_pCritSec->Lock();
}

CriticalSectionLock::~CriticalSectionLock()
{
    m_pCritSec->UnLock();
}

int a;
CriticalSection a_mutex;
int b;
CriticalSection b_mutex;

int main()
{
    thread t1([]()
    {
        while(1)
        {
            CriticalSectionLock lock(a_mutex);
            a++;
            CriticalSectionLock lock2(b_mutex);
            b++;
            cout << "t1 done.\n";
        }
    }
);

    thread t2([]()
    {
        while(1)
        {
           CriticalSectionLock lock(b_mutex);
           b++;
           CriticalSectionLock lock2(a_mutex);
           a++;
           cout << "t2 done. \n"; 
        }
    }
);

    t1.join();
    t2.join();

    return 0;
}
