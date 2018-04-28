//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <set>
#include <thread>
#include <vector>
#include <sstream>

#include "ExternLib\MPMCQueue/MPMCQueue.h"

int test_mpmc();
int mpmc_test();
int main_MPMQ(int argc, char* argv[]);

// compile with: /EHa 
#include <iostream>
#include <stdio.h>  
#include <windows.h>  
#include <eh.h>  

int Except_DividByZero();
void Except_MemoryAccessViolation();
void trans_func(unsigned int, EXCEPTION_POINTERS*);

class SE_Exception
{
private:
    unsigned int nSE;
public:
    SE_Exception() {}
    SE_Exception(unsigned int n) : nSE(n) {}
    ~SE_Exception() {}
    unsigned int getSeNumber() { return nSE; }
};

int main(int argc, char* argv[])
{
    _set_se_translator(trans_func);
    try
    {
        int r = Except_DividByZero();
        std::cout << r << std::endl; // prevent removal of optimized release code
    }
    catch (SE_Exception e)
    {
        printf("Caught DividByZero Exception.\n");
    }

    try
    {
        Except_MemoryAccessViolation();
        std::cout << "Except_MemoryAccessViolation" << std::endl;
    }
    catch (SE_Exception e)
    {
        printf("Caught MemoryAccessViolation Exception.\n");
    }

    int btn = MessageBox(0,"The system will terminate.\nWould you like to send a report to COMPANY_NAME ...", "A critical error has occured with the system.",MB_YESNO + MB_ICONQUESTION);
    if (btn == IDYES) {
        std::cout << "yes" << std::endl;
    }
    else 
    {
        std::cout << "no" << std::endl;
    }


    main_MPMQ(argc, argv);

    std::cout << "Press enter..." << std::endl;
    int c = getchar();
    return 0;
}

int  Except_DividByZero()
{
    int x, y = 0;
    x = 5 / y;
    return x;
}

void Except_MemoryAccessViolation()
{
    int* p = NULL;
    p[0] = 0;
}

void trans_func(unsigned int u, EXCEPTION_POINTERS* pExp)
{
    throw SE_Exception(u);
}

int mpmc_test_sum(const uint64_t numOps, const uint64_t numThreads, const uint64_t q_size)
{
    using namespace rigtorp;
    std::cout << "TEST N readers/N Writers , queue of size M:" << q_size << " N:" <<  numThreads << " * n_oper:" << numOps << "\n";
    std::chrono::time_point<std::chrono::system_clock> _start;
    std::chrono::time_point<std::chrono::system_clock> _end;
    _start = std::chrono::system_clock::now();
    
    struct BIG_STRUCT
    {
        BIG_STRUCT() : v(0), d(0)
        {
            c[0] = 0;
        }
        BIG_STRUCT(uint64_t& value, double dv = 0) : v(value), d(dv) { c[0] = 0; }

        uint64_t    v;
        double      d;
        char        c[1024];
    };
    //MPMCQueue<int> q(q_size
    MPMCQueue<BIG_STRUCT> q(q_size);

    std::atomic<bool> flag(false);
    std::vector<std::thread> threads;
    std::atomic<uint64_t> sum(0);

    // writers
    for (uint64_t i = 0; i < numThreads; ++i) 
    {
        threads.push_back(std::thread([&, i] 
        {
            while (!flag);

            for (auto j = i; j < numOps; j += numThreads) 
            {
                q.push(BIG_STRUCT(j, double(.4)) );
                //q.push(j);
            }
        }));
    }

    // readers
    for (uint64_t i = 0; i < numThreads; ++i) 
    {
        threads.push_back(std::thread([&, i] 
        {
            while (!flag);

            uint64_t threadSum = 0;
            for (auto j = i; j < numOps; j += numThreads) 
            {
                uint64_t v;               
                BIG_STRUCT s;
                q.pop(s);
                v = s.v;
                //q.pop(v);
                threadSum += v;
            }
            sum += threadSum;
        }));
    }

    // launch and wait
    flag = true;
    for (auto &thread : threads) 
    {
        thread.join();
    }
    assert(sum == numOps * (numOps - 1) / 2);

    _end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = _end - _start;
    std::cout << "done " << numThreads << "*" << numOps << " = " << sum << "\n";
    {
        std::stringstream ss_detail;
        ss_detail << "Elapsed sec = " << elapsed_seconds.count() << " ";
        ss_detail << std::endl;
        std::cout << ss_detail.str();
    }
    return 0;
}

int main_MPMQ(int argc, char* argv[])
{
    // TEST
    test_mpmc();
    mpmc_test();

    int N = 1;
    const uint64_t numOps = 4000;
    mpmc_test_sum(numOps, N, N * 1);
    mpmc_test_sum(numOps, N, N * 2);
    mpmc_test_sum(numOps, N, N * 4);
    mpmc_test_sum(numOps, N, N * 8);
    mpmc_test_sum(numOps, N, N * 16);
    mpmc_test_sum(numOps, N, N * 32);
    mpmc_test_sum(numOps, N, N * 64);
    mpmc_test_sum(numOps, N, N * 128);
    mpmc_test_sum(numOps, N, N * 256);
    std::cout << std::endl;

    N = 2;
    mpmc_test_sum(numOps, N, N * 1); // 2 cores machine/4 threads
    mpmc_test_sum(numOps, N, N * 2);
    mpmc_test_sum(numOps, N, N * 4);
    mpmc_test_sum(numOps, N, N * 8);
    mpmc_test_sum(numOps, N, N * 16);
    mpmc_test_sum(numOps, N, N * 32);
    mpmc_test_sum(numOps, N, N * 64);
    mpmc_test_sum(numOps, N, N * 128);
    mpmc_test_sum(numOps, N, N * 256);
    mpmc_test_sum(numOps, N, N * 512);
    mpmc_test_sum(numOps, N, N * 1024);
    std::cout << std::endl;

    N = 4;
    mpmc_test_sum(numOps, N, N * 1);
    mpmc_test_sum(numOps, N, N * 2);
    mpmc_test_sum(numOps, N, N * 4);
    mpmc_test_sum(numOps, N, N * 8);
    mpmc_test_sum(numOps, N, N * 16);
    mpmc_test_sum(numOps, N, N * 32);
    mpmc_test_sum(numOps, N, N * 64);
    mpmc_test_sum(numOps, N, N * 128);
    mpmc_test_sum(numOps, N, N * 256);
    mpmc_test_sum(numOps, N, N * 512);
    mpmc_test_sum(numOps, N, N * 1024);
    std::cout << std::endl;

    N = 8;
    mpmc_test_sum(numOps, N, N * 1);
    mpmc_test_sum(numOps, N, N * 2);
    mpmc_test_sum(numOps, N, N * 4);
    mpmc_test_sum(numOps, N, N * 8);
    mpmc_test_sum(numOps, N, N * 16);
    mpmc_test_sum(numOps, N, N * 32);
    mpmc_test_sum(numOps, N, N * 64);
    mpmc_test_sum(numOps, N, N * 128);
    mpmc_test_sum(numOps, N, N * 256);
    mpmc_test_sum(numOps, N, N * 512);
    mpmc_test_sum(numOps, N, N * 1024);
    std::cout << std::endl;

    std::cout << std::endl;

    std::cout << "Press key and enter..." << std::endl;
    int c = getchar();
    return 0;
}
