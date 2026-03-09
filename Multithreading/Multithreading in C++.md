# 멀티스레딩


### 프로그램 (Program) 과 프로세스 (Process)

- **프로그램** : 코드와 데이터로 구성
- **프로세스** : 프로그램 실행 시, 들어있는 명렁어가 한 줄씩 실행되면서 ‘활동’ 상태가 됨
    
    
- **프로그램** → 디스크 저장소
- **프로세스** → RAM 메모리

- 프로레스에 독립적인 메모리 공간 존재

### 스레드 (Thread)

- **스레드** : 프로세스처럼 명령어를 한 줄씩 실행하는 기본 단위

### 프로세스 (Process) vs 스레드 (Thread)

- 한 프로세스  안에 여러 개의 스레드
- 한 프로세스 안에 있는 스레드는 프로세스 안에 있는 메모리 공간을 같이 사용할 수 있다
- 스레드마다 스택을 가진다
    - 각 스레드에서 실행되는 함수의 로컬 변수들이 스레드마다 있다

1. **`프로그램을 실행하면 프로세스가 생성`** 
2. **`프로세스 안에는 유일한 스레드가 있고 그 안에서 프로그램이 실행`**

### 언제 멀티스레드 프로그래밍?

1. 오래 걸리는 일 하나와 빨리 끝나는 일 여럿을 같이
2. 어떤 긴 처리를 진행하는 동안 짧은 일을 처리
3. 기기에 있는 CPU를 모두 활용

- Single Thread Program.cpp
    
    ```cpp
    #include <vector>
    #include <iostream>
    #include <chrono>
    
    using namespace std;
    
    const int MaxCount = 150000;
    
    bool IsPrimeNumber (int number)
    {
        if (number == 1)
        {
            return false;
        }
        if (number == 2 || number == 3)
        {
            return true;
        }
        for (int i = 2; i< number -1 ; i++)
        {
            if((number % i) == 0)
                return false;
        }
    
        return true;
        
    }
    
    void PrintNumbers(const vector<int>& primes)
    {
        for(int v: primes)
        {
            cout << v << endl;
        }
    }
    
    int main()
    {
        vector<int> primes;
        
        auto t0 = chrono::system_clock::now();
    
        for(int i = 1; i <= MaxCount; i++)
        {
            if(IsPrimeNumber(i))
            {
                primes.push_back(i);    
            }
    
        }
    
        auto t1 = chrono::system_clock::now();
    
        auto duration = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
        cout << "Took " << duration << " milliseconds." << endl;
    
        //PrintNumbers(primes);
    
        return 0;
    }
    ```
    

CPU를 골고루 사용하는 방법 

- 전역변수 `num` 을 생성
- 각 스레드는 `num` 에서 값을 하나씩 가져온다. 가져온 값이 소수인지 판별
- 소수면 배열 `primes` 에 찾은 숫자를 넣는다
- 모든 스레드가 일을 마치고 나면 소수를 출력

`*Multithreading pseudocode*`

```cpp
main()
{
	Array<Threads> threads;
	for (int i = 0; i< 4 ; i++)
	{
		threads.Add(BeginThread(ThreadProc));
		
		for(int i = 0; i< 4; i++)
		{
			threads.waitForExit();
		}
		printNumbers(primes);
	}
```

### 스레드 정체 (Thread Contention)

- 운영체제는 **여러 프로세스**와 **여러 스레드**를 **`동시`** 에 실행
- **Context Switching** : 각 스레드를 번걸아가면서 실행하는 과정

- **Data Race** : 스레드 2개 이상이 값 하나에 동시에 접근하는 경우
    - Context Switching이 언제 발생하는지 몰라서 생김 (무작위 발생)

### 임계 영역 (Critical Section) 과 뮤텍스 (Mutex)

- 뮤텍스 : 상호 배제 (Mutual Exclusion)의 줄임말로 어떤 정보 X와 어떤 정보 Y를 보호한다.

`Mutex` 사용방법 :

1. X, Y를 보호하는 뮤텍스 MX 생성
2. 스레드는 X와 Y를 건드리기전에  MX에 사용권을 얻겠다 요청
3. 스레드는 X,Y에 엑세스
4. 엑세스가 끝나면 MX에 사용권을 돌려줌

- 매번 뮤텍스는 `lock` 과 `unlock` 이 있는데 매번 실행하기 귀찮다거나, 메인 함수보다 더 빨리 끝나게 되면 오류가 난다.
- `lock_guard`  : C++에서 뮤텍스 잠금 상태를 로컬 변수로 저장하고, 그 변수가 사라질 때 자동으로 잠금 해제가 되는 클래스를 사용

```cpp
std::recursive_mutex mx;
lock_guard<recursive_mutex> lock(mx);
read(x);
write(x);
sum(x);
```

로컬 변수 `lock`  이 사라질때 자동으로 잠금해제 됨

- MultiThread Program.cpp
    
    ```cpp
    #include <vector>
    #include <iostream>
    #include <chrono>
    #include <thread>
    #include <memory>
    #include <mutex>
    
    using namespace std;
    
    const int MaxCount = 150000;
    const int ThreadCount = 4;
    
    bool IsPrimeNumber(int number)
    {
        if (number == 1)
        {
            return false;
        }
        if (number == 2 || number == 3)
        {
            return true;
        }
    
        for(int i = 2; i< number - 1; i++)
        {
            if((number % i ) == 0 )
            {
                return false;
            }
        }
    
        return true;
    }
    
    void PrintNumbers(const vector<int>& primes)
    {
        for (int v: primes)
        {
            cout << v << endl;
        }
    }
    
    int main()
    {
        int num = 1;
        recursive_mutex num_mutex;
    
        vector<int> primes;
        recursive_mutex prime_mutex;
    
        auto t0 = chrono::system_clock::now();
    
        vector<shared_ptr<thread>> threads;
    
        for(int i = 0; i< ThreadCount; i++)
        {
            shared_ptr<thread> thread_new (new thread([&]()
            {
                while(true)
                {
                    int n;
                    {
                        lock_guard<recursive_mutex> num_lock(num_mutex);
                        n = num;
                        num++;
                    }
    
                    if (n >= MaxCount)
                        break;
                        
                    if (IsPrimeNumber(n))
                    {
                        lock_guard<recursive_mutex>primes_lock(prime_mutex);
                        primes.push_back(n);
                    }
                }
            }));
    
            threads.push_back(thread_new);
        }
    
        for(auto thread: threads)
        {
            thread->join();
    
        }
    
        auto t1 = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
        cout << "Took " << duration << " milliseconds." << endl;
    
        return 0;
    
    }
    ```