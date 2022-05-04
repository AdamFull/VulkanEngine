#pragma once
#include "PerlinNoise.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Noise
		{
			// Fractal noise generator based on perlin noise above
			template <typename T>
			class FractalNoise
			{
			private:
				PerlinNoise<float> perlinNoise;
				uint32_t octaves;
				T frequency;
				T amplitude;
				T persistence;

			public:
				FractalNoise(const PerlinNoise<T> &perlinNoise, uint32_t oct, T pers)
				{
					this->perlinNoise = perlinNoise;
					octaves = oct;
					persistence = pers;
				}

				T noise(T x, T y, T z)
				{
					T sum = 0;
					T frequency = (T)1;
					T amplitude = (T)1;
					T max = (T)0;
					for (uint32_t i = 0; i < octaves; i++)
					{
						sum += perlinNoise.noise(x * frequency, y * frequency, z * frequency) * amplitude;
						max += amplitude;
						amplitude *= persistence;
						frequency *= (T)2;
					}

					sum = sum / max;
					return (sum + (T)1.0) / (T)2.0;
				}
			};
		}
	}
}