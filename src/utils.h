// Small math/geometry helpers shared across game systems — public interface.

#ifndef UTILS_H
#define UTILS_H

float WrapAngleDelta(float delta);
float gameSeedGenerator(int randomSeed);
float RatioClamped(float value, float max_value);

#endif // UTILS_H
