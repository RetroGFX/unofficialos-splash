#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "fbsplash.h"
#include "svg_parser.h"
#include "svg_renderer.h"
#include "dt_rotation.h"

/*
 * SVG path data for rendering the logo
 * Contains the path data for each component of the logo
 * Index meaning:
 * 0: "U" in logo
 * 1: "N" in logo
 * 2: "O" in logo
 * 2: "F" in logo
 * 3: "F" in logo
 * 4: "I" in logo
 * 5: "C" in logo
 * 6: "I" in logo
 * 7: "A" in logo
 * 8: "L" in logo
 * 9: "O" in logo
 *10: "S" in logo
 */
const char *svg_paths[] = {
    "M 99.00 274.08 L 99.00 235.26 L 123.06 235.26 L 123.06 79.68 L 187.80 79.68 L 187.80 235.26 L 148.98 274.08 L 99.00 274.08 M 38.94 274.08 L 0 235.26 L 0 79.68 L 64.92 79.68 L 64.92 235.26 L 88.74 235.26 L 88.74 274.08 L 38.94 274.08 Z",
    "M 338.22 274.08 L 220.44 79.68 L 288.84 79.68 L 407.10 274.08 L 338.22 274.08 M 407.10 255.60 L 350.70 162.72 L 350.70 79.68 L 407.10 79.68 L 407.10 255.60 M 220.44 274.08 L 220.44 98.16 L 276.84 190.56 L 276.84 274.08 L 220.44 274.08 Z",
    "M 536.28 274.08 L 536.28 234.54 L 561.06 234.54 L 561.06 119.10 L 536.28 119.10 L 536.28 79.68 L 586.50 79.68 L 625.80 118.50 L 625.80 235.26 L 586.50 274.08 L 536.28 274.08 M 476.22 274.08 L 436.80 235.26 L 436.80 118.50 L 476.22 79.68 L 526.02 79.68 L 526.02 119.10 L 501.72 119.10 L 501.72 234.54 L 526.02 234.54 L 526.02 274.08 L 476.22 274.08 Z",
    "M 655.68 274.08 L 655.68 79.68 L 720.42 79.68 L 720.42 166.56 L 780.18 166.56 L 780.18 204.78 L 720.42 204.78 L 720.42 274.08 L 655.68 274.08 M 764.64 144.12 L 764.64 118.50 L 730.68 118.50 L 730.68 79.68 L 829.38 79.68 L 829.38 144.12 L 764.64 144.12 Z",
    "M 850.32 274.08 L 850.32 79.68 L 915.06 79.68 L 915.06 166.56 L 974.88 166.56 L 974.88 204.78 L 915.06 204.78 L 915.06 274.08 L 850.32 274.08 M 959.34 144.12 L 959.34 118.50 L 925.32 118.50 L 925.32 79.68 L 1024.08 79.68 L 1024.08 144.12 L 959.34 144.12 Z",
    "M 1042.08 274.08 L 1042.08 232.62 L 1045.58 232.62 L 1045.58 121.02 L 1042.08 121.02 L 1042.08 79.68 L 1145.82 79.68 L 1145.82 121.02 L 1142.40 121.02 L 1142.40 232.62 L 1145.82 232.62 L 1145.82 274.08 L 1042.08 274.08 Z",
    "M 1213.20 274.08 L 1174.20 235.26 L 1174.20 118.50 L 1213.20 79.68 L 1238.94 79.68 L 1238.94 274.08 L 1213.20 274.08 M 1292.28 157.62 L 1292.28 118.50 L 1249.20 118.50 L 1249.20 79.68 L 1318.20 79.68 L 1357.02 118.50 L 1357.02 157.62 L 1292.28 157.62 M 1249.20 274.08 L 1249.20 235.26 L 1292.28 235.26 L 1292.28 206.82 L 1357.02 206.82 L 1357.02 235.26 L 1318.20 274.08 L 1249.20 274.08 Z",
    "M 1385.16 274.08 L 1385.16 232.62 L 1404.66 232.62 L 1404.66 121.02 L 1385.16 121.02 L 1385.16 79.68 L 1488.84 79.68 L 1488.84 121.02 L 1469.40 121.02 L 1469.40 232.62 L 1488.84 232.62 L 1488.84 274.08 L 1385.16 274.08 Z",
    "M 1656.00 274.08 L 1647.78 252.84 L 1584.24 252.84 L 1597.68 214.32 L 1632.84 214.32 L 1580.88 79.68 L 1644.00 79.68 L 1721.46 274.08 L 1656.00 274.08 M 1504.38 274.08 L 1573.80 88.50 L 1602.54 165.66 L 1566.96 274.08 L 1504.38 274.08 Z",
    "M 1739.94 274.08 L 1739.94 79.68 L 1804.68 79.68 L 1804.68 274.08 L 1739.94 274.08 M 1814.94 274.08 L 1814.94 235.26 L 1847.04 235.26 L 1847.04 210.66 L 1905.78 210.66 L 1905.78 274.08 L 1814.94 274.08 Z",
    "M 2021.34 274.08 L 2021.34 234.54 L 2046.12 234.54 L 2046.12 119.10 L 2021.34 119.10 L 2021.34 79.68 L 2071.56 79.68 L 2110.86 118.50 L 2110.86 235.26 L 2071.56 274.08 L 2021.34 274.08 M 1961.28 274.08 L 1921.86 235.26 L 1921.86 118.50 L 1961.28 79.68 L 2011.08 79.68 L 2011.08 119.10 L 1986.78 119.10 L 1986.78 234.54 L 2011.08 234.54 L 2011.08 274.08 L 1961.28 274.08 Z",
    "M 2260.86 274.08 L 2260.86 201.24 L 2176.74 201.24 L 2137.80 162.30 L 2137.80 118.50 L 2176.74 79.68 L 2199.76 79.68 L 2199.76 148.68 L 2288.10 148.68 L 2325.72 187.62 L 2325.72 235.26 L 2286.90 274.08 L 2260.86 274.08 M 2260.86 134.16 L 2260.86 114.72 L 2208.96 114.72 L 2208.96 79.68 L 2286.78 79.68 L 2320.44 113.52 L 2320.44 134.16 L 2260.86 134.16 M 2170.44 274.08 L 2137.80 235.26 L 2137.80 215.76 L 2202.54 215.76 L 2202.54 238.92 L 2250.60 238.92 L 2250.60 274.08 L 2170.44 274.08 Z"
};

/* Color definitions for each path component
 * First 4 paths are red (brand color)
 * Last 3 paths are gray (secondary color)
 */
const char *svg_colors[] = {
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(40,40,180)",  // Blue
    "rgb(85,85,85)",   // Gray
    "rgb(85,85,85)",   // Gray
};

#define NUM_PATHS (sizeof(svg_paths) / sizeof(svg_paths[0]))

/*
 * Main program entry point
 */
int main(void) {
    const char *fb_device = "/dev/fb0";

    // Get rotation from device tree
    int rotation = get_display_rotation();

    // Check framebuffer device accessibility
    if (access(fb_device, R_OK | W_OK) != 0) {
        fprintf(stderr, "Cannot access %s: %s\n", fb_device, strerror(errno));
        return 1;
    }

    // Initialize framebuffer
    Framebuffer *fb = fb_init(fb_device);
    if (!fb) {
        fprintf(stderr, "Failed to initialize framebuffer\n");
        return 1;
    }

    // Calculate display parameters
    DisplayInfo *display_info = calculate_display_info(fb);
    if (!display_info) {
        fprintf(stderr, "Failed to calculate display information\n");
        fb_cleanup(fb);
        return 1;
    }

    // Clear screen to black
    for (uint32_t y = 0; y < fb->vinfo.yres; y++) {
        for (uint32_t x = 0; x < fb->vinfo.xres; x++) {
            set_pixel(fb, x, y, 0x00000000);
        }
    }

    // Process and render each path component
    for (size_t i = 0; i < NUM_PATHS; i++) {
        SVGPath *svg = parse_svg_path(svg_paths[i], svg_colors[i]);
        if (!svg) {
            fprintf(stderr, "Failed to parse SVG path %zu\n", i);
            continue;
        }

        // Apply rotation from device tree if specified
        if (rotation)
            rotate_svg_path(svg, rotation);

        // Render the path
        render_svg_path(fb, svg, display_info);
        free_svg_path(svg);
    }

    // Flush changes to the framebuffer
    fb_flush(fb);

    // Clean up
    free(display_info);
    fb_cleanup(fb);

    return 0;
}
