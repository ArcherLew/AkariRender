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

#include <akari/core/memory_arena.hpp>
#include <akari/render/bsdf.h>
#include <akari/render/interaction.h>
#include <akari/render/material.h>
#include <akari/render/scene.h>

namespace akari {
    SurfaceInteraction::SurfaceInteraction(const MaterialSlot *materialSlot, const glm::vec3 &wo, const glm::vec3 &p,
                                           const akari::Triangle &triangle, const akari::Intersection &intersection,
                                           akari::MemoryArena &arena)
        : Interaction(wo, p, triangle.Ng), materialSlot(materialSlot) {
        sp.texCoords = triangle.interpolated_tex_coord(intersection.uv);
        Ns = triangle.interpolated_normal(intersection.uv);
        Ng = triangle.Ng;
        bsdf = arena.alloc<BSDF>(Ng, Ns);
        handle.meshId = intersection.meshId;
        handle.primId = intersection.primId;
    }
    void SurfaceInteraction::compute_scattering_functions(MemoryArena &arena, TransportMode mode, float scale) {
        if(!bsdf){
            bsdf = arena.alloc<BSDF>(*this);
        }
        materialSlot->material->compute_scattering_functions(this, arena, mode, scale);
    }
    Spectrum SurfaceInteraction::Le(const vec3 &w) {
        if (!materialSlot) {
            return Spectrum(0);
        }
        auto &emission = materialSlot->emission;
        if (dot(w, Ng) > 0 && emission.color && emission.strength) {
            return emission.color->evaluate(sp) * emission.strength->evaluate(sp);
        }
        return Spectrum(0);
    }
} // namespace akari