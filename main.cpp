#include<iostream>
#include<Windows.h>
#include "random.h"
#include<thread>
#include<map>


using namespace std;

void th_function(int val, int &check)
{
    int i = 0;
    printf("\Thread [%u] spawned with Val: %u\n\n", this_thread::get_id(), val);
    while (i < val)
    {
        printf("T%u: %d\n", this_thread::get_id(), i);
        check = i;
        i++;
    }
    printf("\n");
}

/*******************************************************************************************/
/*                                      M A I N                                            */
/*******************************************************************************************/
int main()
{
    RandomNumberGenerator rn;
    /* Start time tracking now */
    DWORD count = GetTickCount();

    int val = rn.generateRandom(100);
    int check = 1;
    map<int, bool> valMap;

    unsigned int threadSupport = std::thread::hardware_concurrency();
    printf("Thread supports: %u\n", threadSupport);
    Sleep(1000);

    std::thread th1(th_function, val, std::ref(check));
    while (check != val)
    {
        if (check % 10 == 0 && valMap[check] == false)
        {
            valMap[check] = true;
            printf("  >> Reached %d\n", check);
        }
    }

    //std::thread th2(th_function, rn.generateRandom(100));

    th1.join();
    //th2.join();   

    DWORD timetaken = GetTickCount() - count;
    cout << "\n\n** Interval = " << timetaken << " ms ****" << endl;



    getchar(); 
    return 0;
}