#pragma once
#include<time.h>
#include <stdlib.h>

using namespace std;

class RandomNumberGenerator
{
private:
    bool m_bIsSeedGenerated;

    /* Private functions */
    void generateSeed();

public:
    RandomNumberGenerator();
    ~RandomNumberGenerator();
    
    unsigned int generateRandom(unsigned int range);
};

