// Small math/geometry helpers shared across game systems.
// See utils.h for the public interface.

#include "math.h"
#include "time.h"

#include "utils.h"

float WrapAngleDelta(float delta)
{
    delta = fmodf(delta, 360.0f);
    if (delta > 180.0f) delta -= 360.0f;
    else if (delta <= -180.0f) delta += 360.0f;
    return delta;
}

float gameSeedGenerator(int randomSeed)
{
    if (randomSeed == 0) {
        randomSeed = (int)time(NULL);
    }
    return (float)randomSeed;
}

float RatioClamped(float value, float max_value)
{
    if (max_value <= 0.0f) return 0.0f;

    float ratio = value / max_value;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    return ratio;
}
