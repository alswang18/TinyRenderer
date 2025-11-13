#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <cmath>
#include <tuple>

constexpr int width = 800;
constexpr int height = 800;

constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color)
{
    // Steep means the algorithm has more vertical than horizontal steps.
    // We always want to iterate left to right on a single axis, so we transpose.
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep)
    { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx)
    { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x = ax; x <= bx; x++)
    {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        // We only have to move vertically by integer for y, so we just keep
        // track of it.

        // The naive solution is to accumulate an error of up to 0.5.
        // so the error would be:
        // float error += std::abs(by - ay) / std::abs(bx - ax);
        // then when error > 0.5, we increment y by 1 and decrement error by 1.
        // instead we check if 1 >= 2 * std::abs(by - ay) > std::abs(bx - ax)
        // simplify more we get bx - ax >= 2 * std::abs(by - ay)
        ierror += std::abs(by - ay);
        if (ierror > bx - ax)
        {
            y += by > ay ? 1 : -1;
            ierror -= (bx - ax);
        }
    }
}

std::tuple<int, int> project(vec3 v)
{ // First of all, (x,y) is an orthogonal projection of the vector (x,y,z).
  return {(v.x + 1.) * width /
              2, // Second, since the input models are scaled to have fit in the
                 // [-1,1]^3 world coordinates,
          (v.y + 1.) * height / 2}; // we want to shift the vector (x,y) and
                                    // then scale it to span the entire screen.
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) +
                 (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, TGAColor color_a, int bx, int by, TGAColor color_b, int cx, int cy, TGAColor color_c,
              TGAImage &framebuffer)
{
    int bbminx =
        std::min(std::min(ax, bx), cx); // bounding box for the triangle
    int bbminy =
        std::min(std::min(ay, by),
                 cy); // defined by its top left and bottom right corners
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    float scale = 0.5f;

    // Calculate centroid (geometric center)
    double center_x = (ax + bx + cx) / 3.0;
    double center_y = (ay + by + cy) / 3.0;

    // Calculate inner triangle vertices by scaling toward center
    int inner_ax = static_cast<int>(center_x + scale * (ax - center_x));
    int inner_ay = static_cast<int>(center_y + scale * (ay - center_y));

    int inner_bx = static_cast<int>(center_x + scale * (bx - center_x));
    int inner_by = static_cast<int>(center_y + scale * (by - center_y));

    int inner_cx = static_cast<int>(center_x + scale * (cx - center_x));
    int inner_cy = static_cast<int>(center_y + scale * (cy - center_y));

    double inner_total_area = signed_triangle_area(
        inner_ax, inner_ay, inner_bx, inner_by, inner_cx, inner_cy);

    if (total_area<1) return; // backface culling + discarding triangles that cover less than a pixel

    for (int x = bbminx; x <= bbmaxx; x++)
    {
        for (int y = bbminy; y <= bbmaxy; y++)
        {
            double alpha =
                signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta =
                signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma =
                signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue; // negative barycentric coordinate => the pixel is
                          // outside the triangle

            // Check inner triangle (the hole)
            double inner_alpha = signed_triangle_area(x, y, inner_bx, inner_by,
                                                      inner_cx, inner_cy) /
                                 inner_total_area;
            double inner_beta = signed_triangle_area(x, y, inner_cx, inner_cy,
                                                     inner_ax, inner_ay) /
                                inner_total_area;
            double inner_gamma = signed_triangle_area(x, y, inner_ax, inner_ay,
                                                      inner_bx, inner_by) /
                                 inner_total_area;

            if (inner_alpha >= 0 && inner_beta >= 0 && inner_gamma >= 0)
                continue; // Inside inner triangle - create hole

            // Calculate actual geometric center
            double center_x = (ax + bx + cx) / 3.0;
            double center_y = (ay + by + cy) / 3.0;

            unsigned char r = static_cast<unsigned char>(
                alpha * color_a.bgra[2] + beta * color_b.bgra[2] +
                gamma * color_c.bgra[2]);

            unsigned char g = static_cast<unsigned char>(
                alpha * color_a.bgra[1] + beta * color_b.bgra[1] +
                gamma * color_c.bgra[1]);

            unsigned char b = static_cast<unsigned char>(
                alpha * color_a.bgra[0] + beta * color_b.bgra[0] +
                gamma * color_c.bgra[0]);

            TGAColor pixel_color = TGAColor();
            pixel_color.bgra[3] = 255;
            pixel_color.bgra[2] = r;
            pixel_color.bgra[1] = g;
            pixel_color.bgra[0] = b;

            framebuffer.set(x, y, pixel_color);

        }
    }
}

int main(int argc, char **argv)
{
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 17, ay =  4, az = 13;
    int bx = 55, by = 39, bz = 128;
    int cx = 23, cy = 59, cz = 255;

    triangle(ax, ay, TGAColor::blue(), bx, by, TGAColor::green(), cx, cy, TGAColor::red(), framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
