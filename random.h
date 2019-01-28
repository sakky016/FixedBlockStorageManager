#pragma once
#include<time.h>
#include <stdlib.h>
#include<string.h>
using namespace std;

class RandomGenerator
{
private:
    bool m_bIsSeedGenerated;

    /* Private functions */
    void generateSeed();

public:
    RandomGenerator();
    ~RandomGenerator();

    unsigned int generateRandomNumber(unsigned int range);
    char* generateRandomString(int len, bool useUppercase, bool useNumbers);
};
