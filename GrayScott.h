#pragma once
#include "Grid.h"
#include <arm_neon.h>
#include <thread>
#include <vector>
#include <cassert>

class GrayScott {
private:
    static constexpr float diffusionA = 1.0f;
    static constexpr float diffusionB = 0.5f;
    static constexpr float feed = 0.055f;
    static constexpr float kill = 0.062f;

public:
    void Compute(const Grid& readGrid, Grid& writeGrid, float deltaTime) {
        size_t width = readGrid.GetWidth();
        size_t height = readGrid.GetHeight();

        // NEON processes 4 pixels at a time.
        // The inner compute area (width - 2) MUST be a multiple of 4.
        assert((width - 2) % 4 == 0 && "Inner grid width must be a multiple of 4 for NEON SIMD!");

        const float* readA = readGrid.GetRawA();
        const float* readB = readGrid.GetRawB();
        float* writeA = writeGrid.GetRawA();
        float* writeB = writeGrid.GetRawB();

        float32x4_t v_diffA = vdupq_n_f32(diffusionA);
        float32x4_t v_diffB = vdupq_n_f32(diffusionB);
        float32x4_t v_feed = vdupq_n_f32(feed);
        float32x4_t v_kill = vdupq_n_f32(kill);

        float32x4_t v_dt = vdupq_n_f32(deltaTime);
        float32x4_t v_one = vdupq_n_f32(1.0f);
        float32x4_t v_zero = vdupq_n_f32(0.0f);
        float32x4_t v_w_adj = vdupq_n_f32(0.2f);
        float32x4_t v_w_diag = vdupq_n_f32(0.05f);

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        std::vector<std::thread> threads;

        size_t workingHeight = height - 2;
        size_t rowsPerThread = workingHeight / numThreads;


        for (unsigned int i = 0; i < numThreads; ++i) {
            size_t startY = 1 + (i * rowsPerThread);
            size_t endY = (i == numThreads - 1) ? (height - 1) : (startY + rowsPerThread);

            threads.emplace_back([&, startY, endY]() {
                for (size_t y = startY; y < endY; ++y) {
                    size_t centerRow = y * width;
                    size_t upRow = (y - 1) * width;
                    size_t downRow = (y + 1) * width;

                    for (size_t x = 1; x < width - 1; x += 4) {
                        size_t c = centerRow + x;

                        float32x4_t a = vld1q_f32(&readA[c]);
                        float32x4_t b = vld1q_f32(&readB[c]);

                        // Neighbors A
                        float32x4_t a_up = vld1q_f32(&readA[upRow + x]);
                        float32x4_t a_dn = vld1q_f32(&readA[downRow + x]);
                        float32x4_t a_lt = vld1q_f32(&readA[c - 1]);
                        float32x4_t a_rt = vld1q_f32(&readA[c + 1]);
                        float32x4_t a_ul = vld1q_f32(&readA[upRow + x - 1]);
                        float32x4_t a_ur = vld1q_f32(&readA[upRow + x + 1]);
                        float32x4_t a_dl = vld1q_f32(&readA[downRow + x - 1]);
                        float32x4_t a_dr = vld1q_f32(&readA[downRow + x + 1]);

                        // Laplacian A
                        float32x4_t a_adj = vmulq_f32(vaddq_f32(vaddq_f32(a_up, a_dn), vaddq_f32(a_lt, a_rt)), v_w_adj);
                        float32x4_t a_diag = vmulq_f32(vaddq_f32(vaddq_f32(a_ul, a_ur), vaddq_f32(a_dl, a_dr)), v_w_diag);
                        float32x4_t lapA = vsubq_f32(vaddq_f32(a_adj, a_diag), a);

                        // Neighbors B
                        float32x4_t b_up = vld1q_f32(&readB[upRow + x]);
                        float32x4_t b_dn = vld1q_f32(&readB[downRow + x]);
                        float32x4_t b_lt = vld1q_f32(&readB[c - 1]);
                        float32x4_t b_rt = vld1q_f32(&readB[c + 1]);
                        float32x4_t b_ul = vld1q_f32(&readB[upRow + x - 1]);
                        float32x4_t b_ur = vld1q_f32(&readB[upRow + x + 1]);
                        float32x4_t b_dl = vld1q_f32(&readB[downRow + x - 1]);
                        float32x4_t b_dr = vld1q_f32(&readB[downRow + x + 1]);

                        // Laplacian B
                        float32x4_t b_adj = vmulq_f32(vaddq_f32(vaddq_f32(b_up, b_dn), vaddq_f32(b_lt, b_rt)), v_w_adj);
                        float32x4_t b_diag = vmulq_f32(vaddq_f32(vaddq_f32(b_ul, b_ur), vaddq_f32(b_dl, b_dr)), v_w_diag);
                        float32x4_t lapB = vsubq_f32(vaddq_f32(b_adj, b_diag), b);

                        float32x4_t abb = vmulq_f32(a, vmulq_f32(b, b));

                        float32x4_t feed_term_A = vmulq_f32(v_feed, vsubq_f32(v_one, a));
                        float32x4_t diff_term_A = vsubq_f32(vmulq_f32(v_diffA, lapA), abb);
                        float32x4_t nextA = vaddq_f32(a, vmulq_f32(vaddq_f32(diff_term_A, feed_term_A), v_dt));

                        float32x4_t drain_term_B = vmulq_f32(vaddq_f32(v_kill, v_feed), b);
                        float32x4_t diff_term_B = vsubq_f32(vaddq_f32(vmulq_f32(v_diffB, lapB), abb), drain_term_B);
                        float32x4_t nextB = vaddq_f32(b, vmulq_f32(diff_term_B, v_dt));

                        vst1q_f32(&writeA[c], vminq_f32(vmaxq_f32(nextA, v_zero), v_one));
                        vst1q_f32(&writeB[c], vminq_f32(vmaxq_f32(nextB, v_zero), v_one));
                    }
                }
            });
        }

        for (auto& t : threads) t.join();
    }
};
