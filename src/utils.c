// Small math/geometry helpers shared across game systems.
// See utils.h for the public interface.

#include "math.h"

#include "utils.h"

float WrapAngleDelta(float delta)
{
    delta = fmodf(delta, 360.0f);
    if (delta > 180.0f) delta -= 360.0f;
    else if (delta <= -180.0f) delta += 360.0f;
    return delta;
}
