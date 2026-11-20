#include <cstdint>
#include <vector>
#include <cmath>
#pragma once

class AudioFilter
{
public:
    double previousOutputHP1;
    double previousInputHP1;

    double filteredOutputHP1;

    double previousOutputHP2;
    double previousInputHP2;

    double filteredOutputHP2;

    double previousOutputLP;

    int fs = 44100;
    int fc1 = 90;
    int fc2 = 440;
    int fclp = 14000;

    const double PI = 3.1415926536;
    const double e = 2.7182818285;

    double exp1 = (-2.0 * PI * fc1 / fs);
    double exp2 = (-2.0 * PI * fc2 / fs);
    double expLP = -2.0 * PI * fclp / fs;

    double aHP1 = pow(e, exp1);
    double aHP2 = pow(e, exp2);
    double aLP = 1.0 - pow(e, expLP);

    double filterAudio(double input);
};