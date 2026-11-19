#include <cstdint>
#include "filter.hpp"

double AudioFilter::filterAudio(double input)
{
    // First high pass
    double outputHP1 = aHP1 * (input - previousInputHP1 + previousOutputHP1);

    previousInputHP1 = input;
    previousOutputHP1 = outputHP1;

    // Second high pass
    double outputHP2 = aHP2 * (outputHP1 - previousInputHP2 + previousOutputHP2);

    previousInputHP2 = outputHP1;
    previousOutputHP2 = outputHP2;

    // Low pass

    double outputLP = previousOutputLP + aLP * (outputHP2 - previousOutputLP);
    previousOutputLP = outputLP;

    return outputLP;
}