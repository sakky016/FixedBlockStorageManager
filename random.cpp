#include "random.h"

/*******************************************************************************************************************/
RandomNumberGenerator::RandomNumberGenerator()
{
    m_bIsSeedGenerated = false;
}

/*******************************************************************************************************************/
RandomNumberGenerator::~RandomNumberGenerator()
{

}

/*******************************************************************************************************************/
void RandomNumberGenerator::generateSeed()
{
    srand((unsigned)time(0));
    m_bIsSeedGenerated = true;
}

/*******************************************************************************************************************/
/* Generate a random number in between [0, rang) */
/*******************************************************************************************************************/
unsigned int RandomNumberGenerator::generateRandom(unsigned int range)
{
    if (m_bIsSeedGenerated)
    {
        return rand() % range;
    }
    else
    {
        generateSeed();
        return rand() % range;
    }
}