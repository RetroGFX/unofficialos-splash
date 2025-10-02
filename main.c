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
    "M 23.41 9.47 L 28.15 9.47 L 28.15 30.15 Q 28.15 35.55 26.93 38.72 Q 25.71 41.89 22.52 43.88 Q 19.34 45.87 14.16 45.87 Q 9.13 45.87 5.93 44.14 Q 2.73 42.41 1.37 39.12 Q 0 35.84 0 30.15 L 0 9.47 L 4.74 9.47 L 4.74 30.13 Q 4.74 34.79 5.60 37.00 Q 6.47 39.21 8.58 40.41 Q 10.69 41.60 13.75 41.60 Q 18.97 41.60 21.19 39.23 Q 23.41 36.87 23.41 30.13 L 23.41 9.47 Z",
    "M 35.99 45.26 L 35.99 9.47 L 40.84 9.47 L 59.64 37.57 L 59.64 9.47 L 64.18 9.47 L 64.18 45.26 L 59.33 45.26 L 40.53 17.14 L 40.53 45.26 L 35.99 45.26 Z",
    "M 70.70 27.83 Q 70.70 18.92 75.49 13.88 Q 80.27 8.84 87.84 8.84 Q 92.80 8.84 96.78 11.21 Q 100.76 13.57 102.84 17.81 Q 104.93 22.05 104.93 27.42 Q 104.93 32.86 102.73 37.16 Q 100.54 41.46 96.51 43.66 Q 92.48 45.87 87.82 45.87 Q 82.76 45.87 78.78 43.43 Q 74.80 40.99 72.75 36.77 Q 70.70 32.54 70.70 27.83 M 75.59 27.91 Q 75.59 34.38 79.06 38.10 Q 82.54 41.82 87.79 41.82 Q 93.14 41.82 96.59 38.06 Q 100.05 34.30 100.05 27.39 Q 100.05 23.02 98.57 19.76 Q 97.09 16.50 94.25 14.71 Q 91.41 12.92 87.87 12.92 Q 82.84 12.92 79.21 16.37 Q 75.59 19.82 75.59 27.91 Z",
    "M 111.28 45.26 L 111.28 9.47 L 135.42 9.47 L 135.42 13.70 L 116.02 13.70 L 116.02 24.78 L 132.81 24.78 L 132.81 29.00 L 116.02 29.00 L 116.02 45.26 L 111.28 45.26 Z",
    "M 141.82 45.26 L 141.82 9.47 L 165.97 9.47 L 165.97 13.70 L 146.56 13.70 L 146.56 24.78 L 163.35 24.78 L 163.35 29.00 L 146.56 29.00 L 146.56 45.26 L 141.82 45.26 Z",
    "M 172.92 45.26 L 172.92 9.47 L 177.66 9.47 L 177.66 45.26 L 172.92 45.26 Z",
    "M 211.55 32.71 L 216.28 33.91 Q 214.79 39.75 210.93 42.81 Q 207.06 45.87 201.46 45.87 Q 195.68 45.87 192.05 43.52 Q 188.43 41.16 186.54 36.69 Q 184.64 32.23 184.64 27.10 Q 184.64 21.51 186.78 17.35 Q 188.92 13.18 192.86 11.02 Q 196.80 8.86 201.54 8.86 Q 206.91 8.86 210.57 11.60 Q 214.23 14.33 215.67 19.29 L 211.01 20.39 Q 209.77 16.48 207.40 14.70 Q 205.03 12.92 201.44 12.92 Q 197.31 12.92 194.54 14.89 Q 191.77 16.87 190.65 20.20 Q 189.53 23.54 189.53 27.08 Q 189.53 31.64 190.86 35.05 Q 192.19 38.45 195.00 40.14 Q 197.80 41.82 201.07 41.82 Q 205.05 41.82 207.81 39.53 Q 210.57 37.23 211.55 32.71 Z",
    "M 222.92 45.26 L 222.92 9.47 L 227.66 9.47 L 227.66 45.26 L 222.92 45.26 Z",
    "M 232.08 45.26 L 245.83 9.47 L 250.93 9.47 L 265.58 45.26 L 260.18 45.26 L 256.01 34.42 L 241.04 34.42 L 237.11 45.26 L 232.08 45.26 M 242.41 30.57 L 254.54 30.57 L 250.81 20.65 Q 249.10 16.14 248.27 13.23 Q 247.58 16.67 246.34 20.07 L 242.41 30.57 Z",
    "M 269.17 45.26 L 269.17 9.47 L 273.90 9.47 L 273.90 41.04 L 291.53 41.04 L 291.53 45.26 L 269.17 45.26 Z",
    "M 295.73 27.83 Q 295.73 18.92 300.51 13.88 Q 305.30 8.84 312.87 8.84 Q 317.82 8.84 321.80 11.21 Q 325.78 13.57 327.87 17.81 Q 329.96 22.05 329.96 27.42 Q 329.96 32.86 327.76 37.16 Q 325.56 41.46 321.53 43.66 Q 317.50 45.87 312.84 45.87 Q 307.79 45.87 303.81 43.43 Q 299.83 40.99 297.78 36.77 Q 295.73 32.54 295.73 27.83 M 300.61 27.91 Q 300.61 34.38 304.09 38.10 Q 307.57 41.82 312.82 41.82 Q 318.16 41.82 321.62 38.06 Q 325.07 34.30 325.07 27.39 Q 325.07 23.02 323.60 19.76 Q 322.12 16.50 319.27 14.71 Q 316.43 12.92 312.89 12.92 Q 307.86 12.92 304.24 16.37 Q 300.61 19.82 300.61 27.91 Z",
    "M 334.45 33.76 L 338.92 33.37 Q 339.23 36.06 340.39 37.78 Q 341.55 39.50 343.99 40.56 Q 346.44 41.63 349.49 41.63 Q 352.20 41.63 354.27 40.82 Q 356.35 40.01 357.36 38.61 Q 358.37 37.21 358.37 35.55 Q 358.37 33.86 357.40 32.60 Q 356.42 31.35 354.17 30.49 Q 352.73 29.93 347.80 28.75 Q 342.87 27.56 340.89 26.51 Q 338.33 25.17 337.07 23.18 Q 335.82 21.19 335.82 18.73 Q 335.82 16.02 337.35 13.66 Q 338.89 11.30 341.85 10.08 Q 344.80 8.86 348.41 8.86 Q 352.39 8.86 355.43 10.14 Q 358.47 11.43 360.11 13.92 Q 361.74 16.41 361.87 19.56 L 357.32 19.90 Q 356.96 16.50 354.85 14.77 Q 352.73 13.04 348.61 13.04 Q 344.31 13.04 342.35 14.61 Q 340.38 16.19 340.38 18.41 Q 340.38 20.34 341.77 21.58 Q 343.14 22.83 348.91 24.13 Q 354.69 25.44 356.84 26.42 Q 359.96 27.86 361.45 30.07 Q 362.94 32.28 362.94 35.16 Q 362.94 38.01 361.30 40.54 Q 359.67 43.07 356.60 44.47 Q 353.54 45.87 349.71 45.87 Q 344.85 45.87 341.56 44.46 Q 338.28 43.04 336.41 40.20 Q 334.55 37.35 334.45 33.76 Z"
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
