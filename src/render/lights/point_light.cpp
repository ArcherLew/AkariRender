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
#include <akari/core/math.h>
#include <akari/core/plugin.h>
#include <akari/core/spectrum.h>
#include <akari/render/light.h>
#include <akari/render/material.h>
#include <akari/render/mesh.h>

namespace akari {
    class PointLight : public Light {
        [[refl]] vec3 color;
        [[refl]] float strength = 1;
        [[refl]] vec3 position;

      public:
        AKR_IMPLS(Light)
        Float pdf_incidence(const Interaction &ref, const vec3 &wi) const override { return 0; }
        void pdf_emission(const Ray &ray, Float *pdfPos, Float *pdfDir) const override {
            *pdfPos = 0;
            *pdfDir = uniform_sphere_pdf();
        }
        void sample_incidence(const vec2 &u, const Interaction &ref, RayIncidentSample *sample,
                              VisibilityTester *tester) const override {
            auto wi = position - ref.p;
            auto dist2 = dot(wi, wi);
            auto dist = std::sqrt(dist2);
            wi /= dist;

            sample->I = Li(-wi, vec2(0)) / dist2;
            sample->wi = wi;
            sample->pdf = 1;
            sample->normal = vec3(0);

            tester->shadowRay = Ray(position, -1.0f * wi, 0, dist * (1 - ShadowEps()));
        }
        void sample_emission(const vec2 &u1, const vec2 &u2, RayEmissionSample *sample) const override {
            sample->ray = Ray(position, uniform_sphere_sampling(u2));
            sample->pdfDir = uniform_sphere_pdf();
            sample->pdfPos = 1;
            sample->normal = vec3(0);
            sample->E = Li(sample->ray.d, vec3());
            sample->uv = vec2();
        }
        Float power() const override { return Spectrum(color).luminance() * strength * 4 * Pi; }
        Spectrum Li(const vec3 &wo, const vec2 &uv) const override {
            return Spectrum(Spectrum(color).luminance() * strength);
        }
        LightType get_light_type() const override { return LightType::EDeltaPosition; }
    };
#include "generated/PointLight.hpp"
    AKR_EXPORT_PLUGIN(p){

    }
} // namespace akari