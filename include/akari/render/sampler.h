// MIT License
//
// Copyright (c) 2020 椎名深雪
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef AKARIRENDER_SAMPLER_H
#define AKARIRENDER_SAMPLER_H

#include <akari/core/component.h>
#include <akari/core/math.h>

namespace akari {
    // https://en.wikipedia.org/wiki/Permuted_congruential_generator
    class Rng {
        uint64_t state;
        static uint64_t const multiplier = 6364136223846793005u;
        static uint64_t const increment = 1442695040888963407u;

        static uint32_t rotr32(uint32_t x, unsigned r) { return x >> r | x << ((uint32_t)-r & 31u); }

        uint32_t pcg32() {
            uint64_t x = state;
            auto count = (unsigned)(x >> 59ULL); // 59 = 64 - 5

            state = x * multiplier + increment;
            x ^= x >> 18ULL;                              // 18 = (64 - 27)/2
            return rotr32((uint32_t)(x >> 27ULL), count); // 27 = 32 - 5
        }

      public:
        explicit Rng(uint64_t state = 0) : state(state + increment) { pcg32(); }

        uint32_t uniformUint32() { return pcg32(); }

        float uniformFloat() { return float(double(uniformUint32()) / double(std::numeric_limits<uint32_t>::max())); }
    };

    class Sampler : public Component {
      public:
        virtual Float next1d() = 0;
        vec2 next2d() { return vec2(next1d(), next1d()); }
        virtual std::shared_ptr<Sampler> clone() const = 0;
        virtual void set_sample_index(size_t) = 0;
        virtual void start_next_sample() = 0;
        virtual void RoundToEvenDimension() {
            if (current_dimension() & 1u)
                next1d();
        }
        virtual size_t current_dimension() = 0;
    };
} // namespace akari

#endif // AKARIRENDER_SAMPLER_H
