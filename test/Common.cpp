#include "Common.hpp"
#include "Math.hpp"
#include "Window.hpp"
#include "Utils.hpp"

#include <fstream>
#include <array>
#include <vector>
#include <lodepng.h> // Saving screenhots

// NOLINTBEGIN

namespace {
    GLenum cScreenshotBuffer = GL_BACK; // Note, could be GL_FRONT

    /**
     * Takes an screenshot
     */
    std::vector<glm::vec<4, unsigned char>> TakeScreenshoot(unsigned width, unsigned height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);                                        // Bind the default frame buffer
        std::vector<glm::vec<4, unsigned char>> result(width * height);              // Avoid reallocations
        glPixelStorei(GL_PACK_ALIGNMENT, 1);                                         // No padding when downloading
        glReadBuffer(cScreenshotBuffer);                                             // Establish where to read pixels from
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, result.data()); // Actual retrieval
        return result;
    }

    void SaveScreenshot(std::string const& filename, unsigned width, unsigned height)
    {
        static_assert(sizeof(glm::vec<4, unsigned char>) == sizeof(unsigned char) * 4, "Incorrect vec4 size assumed");

        auto pixels = TakeScreenshoot(width, height);

        // Flip vertically (inefficiently)
        for (unsigned x = 0; x < width; ++x) {
            for (unsigned y = 0; y < height / 2; ++y) {
                std::swap(pixels.at(y * width + x), pixels.at((height - 1 - y) * width + x));
            }
        }
        // Save to file
        auto err = lodepng::encode(filename, reinterpret_cast<unsigned char*>(pixels.data()), width, height);
        if (err) {
            throw std::runtime_error(fmt::format("Could not save screenshot: {}", lodepng_error_text(err)));
        }
    }
}

void SaveTestScreenshot(CS350::Window& window, size_t id, std::string const& postfix)
{
    window.Update(); // Force a swap buffers
    ivec2 size       = window.size();
    auto  screenshot = TakeScreenshoot(size.x, size.y);
    SaveScreenshot(fmt::format("screenshot_{}_{}{}.png", TestName(), id, postfix), size.x, size.y);
}

namespace testing {
    namespace internal {
        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* absErrorExpr, glm::vec3 const& val1, glm::vec3 const& val2, double absError)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.x, val2.x, absError);
            auto y_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.y, val2.y, absError);
            auto z_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.z, val2.z, absError);

            if (!x_res || !y_res || !z_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << absErrorExpr << ", where\n"
                       << expr1 << " evaluates to " << val1.x << ", " << val1.y << ", " << val1.z << ",\n"
                       << expr2 << " evaluates to " << val2.x << ", " << val2.y << ", " << val2.z << ", and\n"
                       << absErrorExpr << " evaluates to " << absError << ".";
            }

            return AssertionSuccess();
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* absErrorExpr, glm::vec2 const& val1, glm::vec2 const& val2, double absError)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.x, val2.x, absError);
            auto y_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.y, val2.y, absError);

            if (!x_res || !y_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << absErrorExpr << ", where\n"
                       << expr1 << " evaluates to " << val1.x << ", " << val1.y << ",\n"
                       << expr2 << " evaluates to " << val2.x << ", " << val2.y << ", and\n"
                       << absErrorExpr << " evaluates to " << absError << ".";
            }

            return AssertionSuccess();
        }

        AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* absErrorExpr, CS350::Aabb const& val1, CS350::Aabb const& val2, double absError)
        {
            auto a_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.min, val2.min, absError);
            auto b_res = DoubleNearPredFormat(expr1, expr2, absErrorExpr, val1.max, val2.max, absError);

            if (!a_res || !b_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << absErrorExpr << ", where\n"
                       << expr1 << " evaluates to [" << val1.min.x << ", " << val1.min.y << ", " << val1.min.z << "], [" << val1.max.x << ", " << val1.max.y << ", " << val1.max.z << "],\n"
                       << expr2 << " evaluates to [" << val2.min.x << ", " << val2.min.y << ", " << val2.min.z << "], [" << val2.max.x << ", " << val2.max.y << ", " << val2.max.z << "], and\n"
                       << absErrorExpr << " evaluates to " << absError << ".";
            }

            return AssertionSuccess();
        }
    }
}
// NOLINTEND