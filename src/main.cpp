#include <cmath>
#include <complex>
#include <vector>

#include "StormTrack.hpp"

namespace {

    double PI = acos(-1.);

    COLORREF hsv(double h, double s, double v) {
        h = std::fmod(h, 360.0);
        if (h < 0.0) h += 360.0;
        double c = v * s;
        double x = c * (1.0 - std::fabs(std::fmod(h / 60.0, 2.0) - 1.0));
        double m = v - c;
        double r = 0, g = 0, b = 0;
        if (h < 60.0) { r = c; g = x; b = 0; }
        else if (h < 120.0) { r = x; g = c; b = 0; }
        else if (h < 180.0) { r = 0; g = c; b = x; }
        else if (h < 240.0) { r = 0; g = x; b = c; }
        else if (h < 300.0) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return RGB(static_cast<BYTE>((r + m) * 255.0),
            static_cast<BYTE>((g + m) * 255.0),
            static_cast<BYTE>((b + m) * 255.0));
    }

    constexpr size_t N = 100'000;
    constexpr double DRIFT = 0.030;

    constexpr double Y_CHIRP = 0.0;
    constexpr double Y_FM = 3.0;
    constexpr double Y_BEAT = 6.0;
    constexpr double Y_IQ = 9.0;

    constexpr double HUE_CHIRP = 0.0;
    constexpr double HUE_FM = 120.0;
    constexpr double HUE_BEAT = 45.0;
    constexpr double HUE_IQ_RE = 195.0;
    constexpr double HUE_IQ_IM = 305.0;

    std::vector<double> makeChirp(double t) {
        double T = t * DRIFT;
        double f0 = 0.0000180 + 0.0000120 * std::sin(T * 0.10);
        double f1 = 0.0003600 + 0.0001800 * std::sin(T * 0.07 + 1.0);
        std::vector<double> d(N);
        for (size_t i = 0; i < N; ++i) {
            double x = static_cast<double>(i);
            double f = f0 + (f1 - f0) * x / N;
            double phase = 2.0 * PI * (f0 * x + 0.5 * (f1 - f0) / N * x * x);
            double env = std::sin(PI * x / N);
            d[i] = Y_CHIRP + env * std::sin(phase);
        }
        return d;
    }

    std::vector<double> makeFM(double t) {
        double T = t * DRIFT;
        double fc = 0.0002400 + 0.0000480 * std::sin(T * 0.08);
        double beta = 4.0 + 1.0 * std::sin(T * 0.12);
        double fm = 0.0000360 + 0.0000090 * std::sin(T * 0.10);
        std::vector<double> d(N);
        for (size_t i = 0; i < N; ++i) {
            double x = static_cast<double>(i);
            double phase = 2.0 * PI * fc * x + beta * std::sin(2.0 * PI * fm * x);
            d[i] = Y_FM + std::sin(phase);
        }
        return d;
    }

    std::vector<double> makeBeat(double t) {
        double T = t * DRIFT;
        double f1 = 0.0002400;
        double f2 = 0.0002880 + 0.0000240 * std::sin(T * 0.06);
        std::vector<double> d(N);
        for (size_t i = 0; i < N; ++i) {
            double x = static_cast<double>(i);
            d[i] = Y_BEAT + 0.5 * (std::sin(2.0 * PI * f1 * x) +
                std::sin(2.0 * PI * f2 * x));
        }
        return d;
    }

    std::vector<std::complex<double>> makeIQ(double t) {
        double T = t * DRIFT;
        double fc = 0.0000840 + 0.0000240 * std::sin(T * 0.05);
        std::vector<std::complex<double>> d(N);
        for (size_t i = 0; i < N; ++i) {
            double x = static_cast<double>(i);
            double env = std::exp(-x * 0.0000240);
            double phase = 2.0 * PI * fc * x;
            d[i] = std::complex<double>(Y_IQ + env * std::cos(phase),
                Y_IQ + env * std::sin(phase));
        }
        return d;
    }

} // namespace

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    StormTrackInitParameters cfg = { {1280, 720}, {(double)N, 14.0}, {0.0, -2.0} };
    StormTrack window(hInstance, cfg, L"StormTrack: Signal Showcase");

    double t = 0.0;

    while (window.IsActive()) {
        double hue_shift = 25.0 * std::sin(t * 0.008);

        COLORREF col_chirp = hsv(HUE_CHIRP + hue_shift, 0.85, 1.00);
        COLORREF col_fm = hsv(HUE_FM + hue_shift, 0.80, 0.95);
        COLORREF col_beat = hsv(HUE_BEAT + hue_shift, 0.95, 1.00);
        COLORREF col_iq_re = hsv(HUE_IQ_RE + hue_shift, 0.80, 1.00);
        COLORREF col_iq_im = hsv(HUE_IQ_IM + hue_shift, 0.80, 1.00);

        auto chirp_data = makeChirp(t);
        window.UniqueStream(chirp_data, L"Chirp (sweep)", col_chirp);

        auto fm_data = makeFM(t);
        window.UniqueStream(fm_data, L"FM signal", col_fm);

        auto beat_data = makeBeat(t);
        window.UniqueStream(beat_data, L"Beat (interference)", col_beat);

        auto iq_data = makeIQ(t);
        window.UniqueStream(iq_data, L"IQ (complex)", col_iq_re, col_iq_im);

        t += 1.0;
    }

    window.WaitForClose();
    return 0;
}