#include <iostream>
#include <chrono>
#include <random>
#include <cmath>
#include <thread>
#include <atomic>

#include "StormTrack.hpp"

// demo generated AI

namespace example_signal {

    const double PI = 3.14159265358979323846;

    std::vector<double> generateChirp(size_t samples, double f0, double f1, double noise_amp = 0.05) {
        static std::mt19937 rng(42);
        std::normal_distribution<double> noise(0.0, noise_amp);
        std::vector<double> data(samples);
        double phi = 0.0;
        double k = (f1 - f0) / samples;
        for (size_t i = 0; i < samples; ++i) {
            double t = static_cast<double>(i);
            double f = f0 + k * t;
            phi += 2 * PI * f * 0.01;
            data[i] = std::sin(phi) + noise(rng);
        }
        return data;
    }

    std::vector<double> generateDampedOscillation(size_t samples, double freq, double damping, double impulse_amplitude = 5.0) {
        std::vector<double> data(samples, 0.0);
        double v = 0.0;
        for (size_t i = 1; i < samples; ++i) {
            if (i == 200) v += impulse_amplitude;
            double a = -damping * v - freq * freq * data[i - 1];
            v += a * 0.01;
            data[i] = v;
        }
        return data;
    }

    void generateIQSignal(size_t samples, double carrier_freq, double mod_freq,
        std::vector<double>& I_out, std::vector<double>& Q_out) {
        I_out.resize(samples);
        Q_out.resize(samples);
        for (size_t i = 0; i < samples; ++i) {
            double t = i * 0.01;
            I_out[i] = std::cos(2 * PI * mod_freq * t) * std::cos(2 * PI * carrier_freq * t);
            Q_out[i] = std::cos(2 * PI * mod_freq * t) * std::sin(2 * PI * carrier_freq * t);
        }
    }

    std::vector<double> generateRandomWalk(size_t steps, double start = 0.0, double step_std = 1.0) {
        static std::mt19937 rng(123);
        std::normal_distribution<double> step(0.0, step_std);
        std::vector<double> data(steps);
        double val = start;
        for (size_t i = 0; i < steps; ++i) {
            val += step(rng);
            data[i] = val;
        }
        return data;
    }

    std::vector<double> generateECG(size_t beats, size_t samples_per_beat = 100) {
        std::vector<double> data;
        data.reserve(beats * samples_per_beat);
        for (size_t b = 0; b < beats; ++b) {
            for (size_t i = 0; i < samples_per_beat; ++i) {
                double t = i / static_cast<double>(samples_per_beat);
                double val = 0.0;
                if (t > 0.0 && t < 0.2) val += 0.15 * std::sin(PI * t / 0.2);
                if (t > 0.15 && t < 0.25) val -= 0.1 * std::sin(PI * (t - 0.15) / 0.1);
                if (t > 0.25 && t < 0.35) val += 1.0 * std::sin(PI * (t - 0.25) / 0.1);
                if (t > 0.35 && t < 0.45) val -= 0.3 * std::sin(PI * (t - 0.35) / 0.1);
                if (t > 0.5 && t < 0.7) val += 0.4 * std::sin(PI * (t - 0.5) / 0.2);
                data.push_back(val);
            }
        }
        return data;
    }

    std::vector<double> generateSeismicNoise(size_t samples) {
        static std::mt19937 rng(456);
        std::normal_distribution<double> white(0.0, 1.0);
        std::vector<double> data(samples);
        double y = 0.0;
        for (size_t i = 0; i < samples; ++i) {
            y = 0.98 * y + 0.02 * white(rng);
            data[i] = y;
        }
        return data;
    }

    std::vector<double> generateSawtooth(size_t samples, double period, double amplitude = 1.0) {
        std::vector<double> data(samples);
        for (size_t i = 0; i < samples; ++i) {
            double phase = std::fmod(i, period) / period;
            data[i] = amplitude * (2.0 * phase - 1.0);
        }
        return data;
    }

    std::vector<double> generateSquareWave(size_t samples, double period, double amplitude = 1.0) {
        std::vector<double> data(samples);
        for (size_t i = 0; i < samples; ++i) {
            data[i] = (std::fmod(i, period) < period / 2) ? amplitude : -amplitude;
        }
        return data;
    }

    std::vector<double> generateAMSignal(size_t samples, double carrier_freq, double mod_freq) {
        std::vector<double> data(samples);
        for (size_t i = 0; i < samples; ++i) {
            double t = i * 0.01;
            data[i] = (1.0 + 0.5 * std::sin(2 * PI * mod_freq * t)) * std::sin(2 * PI * carrier_freq * t);
        }
        return data;
    }

}

namespace demo {


    std::atomic<bool> is_running{ true };


    void RunStreaming() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        StormTrackInitParameters cfg = { {1000, 600}, {100, 100}, {0, -10} };
        StormTrack window(hInstance, cfg, L"StormTrack: High-Speed Frame Streaming");
        window.Show();

        const size_t frame_samples = 2000;
        size_t tid_chirp = window.AddTrace(L"Chirp 0.1->5 Hz", RGB(255, 100, 100));
        size_t tid_shock = window.AddTrace(L"Shock Response", RGB(100, 255, 100), 0.01);
        size_t tid_iq_i = window.AddTrace(L"IQ: In-phase", RGB(100, 200, 255));
        size_t tid_iq_q = window.AddTrace(L"IQ: Quadrature", RGB(255, 200, 100));

        while (window.IsActive() && is_running) {
            std::vector<double> chirp = example_signal::generateChirp(frame_samples, 0.1, 5.0, 0.05);
            window.FrameView(chirp, tid_chirp);

            std::vector<double> shock = example_signal::generateDampedOscillation(frame_samples, 2.0, 0.8);
            window.FrameView(shock, tid_shock);

            std::vector<double> I, Q;
            example_signal::generateIQSignal(frame_samples, 2.0, 0.2, I, Q);
            window.FrameView(I, tid_iq_i);
            window.FrameView(Q, tid_iq_q);

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        window.Close();
        window.WaitForClose();
    }

    void RunRealtime() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        StormTrackInitParameters cfg = { {1000, 600}, {200, 10}, {0, -5} };
        StormTrack window(hInstance, cfg, L"StormTrack: Continuous Real-Time Monitoring");
        window.Show();

        size_t tid_walk = window.AddTrace(L"Random Walk", RGB(170, 120, 255));
        size_t tid_ecg = window.AddTrace(L"Live ECG Simulator", RGB(255, 50, 50));
        size_t tid_seism = window.AddTrace(L"Seismic Noise", RGB(50, 180, 180));
        size_t tid_decay = window.AddTrace(L"Exponential Decay", RGB(230, 180, 0));

        std::vector<double> walk_init = example_signal::generateRandomWalk(500, 0.0, 0.02);
        window.RealtimeView(walk_init, tid_walk);
        std::vector<double> ecg_init = example_signal::generateECG(4);
        window.RealtimeView(ecg_init, tid_ecg);
        std::vector<double> seism_init = example_signal::generateSeismicNoise(300);
        window.RealtimeView(seism_init, tid_seism);

        std::vector<double> decay_buffer;
        decay_buffer.reserve(100);
        double amplitude = 1.0;
        std::mt19937 rng(789);
        std::normal_distribution<double> noise(0.0, 0.2);

        int step = 0;
        while (window.IsActive() && is_running) {
            {
                std::vector<double> walk_chunk = example_signal::generateRandomWalk(10, 0.0, 0.02);
                window.RealtimeView(walk_chunk, tid_walk);
            }
            if (step % 100 == 0) {
                std::vector<double> ecg_chunk = example_signal::generateECG(1);
                window.RealtimeView(ecg_chunk, tid_ecg);
            }
            {
                std::vector<double> seism_chunk = example_signal::generateSeismicNoise(15);
                window.RealtimeView(seism_chunk, tid_seism);
            }
            {
                double val = amplitude * std::exp(-step * 0.01) + noise(rng);
                decay_buffer.push_back(val);
                if (decay_buffer.size() >= 50) {
                    window.RealtimeView(decay_buffer, tid_decay);
                    decay_buffer.clear();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            ++step;
        }
        window.Close();
        window.WaitForClose();
    }

    void RunMultiTrace() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        StormTrackInitParameters cfg = { {1100, 700}, {50, 25}, {0, -1} };
        StormTrack window(hInstance, cfg, L"StormTrack: Multi-Trace Stress Test");
        window.Show();

        const size_t samples = 1000;
        const double y_shift_step = 3.0;
        double y_shift = 0.0;

        size_t tid_saw = window.AddTrace(L"Sawtooth", RGB(255, 80, 80));
        size_t tid_square = window.AddTrace(L"Square Wave", RGB(80, 255, 80));
        size_t tid_am = window.AddTrace(L"AM Signal", RGB(80, 80, 255));
        size_t tid_fast = window.AddTrace(L"Fast Sine", RGB(255, 255, 80));
        size_t tid_slow = window.AddTrace(L"Slow Drift", RGB(255, 80, 255));
        size_t tid_noisy = window.AddTrace(L"Noisy Measurement", RGB(80, 255, 255));
        size_t tid_chirp2 = window.AddTrace(L"Chirp (wideband)", RGB(255, 160, 80));
        size_t tid_impulse = window.AddTrace(L"Impulse Train", RGB(200, 200, 200));

        while (window.IsActive() && is_running) {
            std::vector<double> saw = example_signal::generateSawtooth(samples, 40.0, 1.5);
            for (auto& v : saw) v += y_shift;
            window.FrameView(saw, tid_saw);
            y_shift += y_shift_step;

            std::vector<double> sq = example_signal::generateSquareWave(samples, 60.0, 1.2);
            for (auto& v : sq) v += y_shift;
            window.FrameView(sq, tid_square);
            y_shift += y_shift_step;

            std::vector<double> am = example_signal::generateAMSignal(samples, 0.5, 0.05);
            for (auto& v : am) v += y_shift;
            window.FrameView(am, tid_am);
            y_shift += y_shift_step;

            std::vector<double> fast(samples);
            for (size_t i = 0; i < samples; ++i) fast[i] = std::sin(i * 0.3) + y_shift;
            window.FrameView(fast, tid_fast);
            y_shift += y_shift_step;

            std::vector<double> slow(samples);
            for (size_t i = 0; i < samples; ++i) slow[i] = 0.5 * std::sin(i * 0.01) + 0.2 * (i / 1000.0) + y_shift;
            window.FrameView(slow, tid_slow);
            y_shift += y_shift_step;

            static std::mt19937 rng_noise(111);
            std::normal_distribution<double> n(0.0, 0.3);
            std::vector<double> noisy(samples);
            for (size_t i = 0; i < samples; ++i) noisy[i] = std::cos(i * 0.1) + n(rng_noise) + y_shift;
            window.FrameView(noisy, tid_noisy);
            y_shift += y_shift_step;

            std::vector<double> chirp2 = example_signal::generateChirp(samples, 0.01, 0.5, 0.02);
            for (auto& v : chirp2) v += y_shift;
            window.FrameView(chirp2, tid_chirp2);
            y_shift += y_shift_step;

            std::vector<double> impulse(samples, 0.0);
            for (size_t i = 0; i < samples; i += 150)
                if (i + 3 < samples) impulse[i] = impulse[i + 1] = impulse[i + 2] = 2.0;
            for (auto& v : impulse) v += y_shift;
            window.FrameView(impulse, tid_impulse);
            y_shift = 0.0;

            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        window.Close();
        window.WaitForClose();
    }

    void RunMillionPoints() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        StormTrackInitParameters cfg = { {1000, 600}, {1000000, 6}, {0, -3} };
        StormTrack window(hInstance, cfg, L"StormTrack: 1 Million Points & Auto-Scale (press A)");
        window.Show();

        const size_t N = 1000000;
        std::vector<double> hugeData(N);
        {
            static std::mt19937 rng(999);
            std::normal_distribution<double> noise(0.0, 0.1);
            for (size_t i = 0; i < N; ++i) {
                hugeData[i] = std::sin(i * 0.0001) + noise(rng);
            }
        }

        window.JustView(hugeData, L"Sin + Noise (1M pts)", RGB(255, 180, 80));

        std::vector<double> trend(N);
        for (size_t i = 0; i < N; ++i) trend[i] = 0.5 * std::cos(i * 0.00005);
        window.JustView(trend, L"Slow cosine trend", RGB(120, 120, 255));

        while (window.IsActive() && is_running) { ; }

        window.Close();
        window.WaitForClose();
    }
}

void RunNoiseTest() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    StormTrack window(hInstance, L"Noise Points");
    window.Show();

    const size_t N = 2048;
    std::vector<double> noise(N);
    {
        static std::mt19937 rng(123);
        std::normal_distribution<double> dist(0.0, 1.0);
        for (size_t i = 0; i < N; ++i)
            noise[i] = dist(rng);
    }

    window.JustView(noise, L"White noise", RGB(200, 200, 100));

    window.WaitForClose();
}

void RunChangeSizeData() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    StormTrack window(hInstance, L"Change Data Size");
	auto trace = window.AddTrace(L"Change Data Size Trace", RGB(200, 200, 100));

    window.Show();

    for (;;) {
        size_t N = std::rand() & 0xfffff;
        std::vector<double> data = example_signal::generateRandomWalk(N, 0.0, 0.1);
        window.FrameView(data, trace);
    }

    window.WaitForClose();
}

void RunEmptyData() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    StormTrack window(hInstance, L"Empty data");
    window.Show();

    const size_t N = 0;
    std::vector<double> data_empty(0);
    std::vector<double> data_no_empty(1);
    std::vector<double> data_two_points(2);

    window.JustView(data_empty, L"Empty data", RGB(200, 200, 100));
    window.JustView(data_no_empty, L"Data with one point", RGB(100, 100, 200));
    window.JustView(data_two_points, L"Data with two points", RGB(100, 200, 100));

    window.WaitForClose();
}

int main() {
    std::thread streamingDemo(demo::RunStreaming);
    std::thread realtimeDemo(demo::RunRealtime);
    std::thread multiTraceDemo(demo::RunMultiTrace);
    std::thread millionDemo(demo::RunMillionPoints);

    std::cout << "Four demos are running. Close all windows to exit, or press Enter.\n";
    std::cin.get();

    demo::is_running = false;

    streamingDemo.join();
    realtimeDemo.join();
    multiTraceDemo.join();
    millionDemo.join();

    std::cout << "All windows closed cleanly.\n";
    return 0;
}