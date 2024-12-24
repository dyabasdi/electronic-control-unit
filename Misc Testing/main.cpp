#include <SDL2/SDL.h>
#include <iostream>
#include "bno055_data.h"

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("G-Force Graph", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Sensor data processing loop
    float accel_x, accel_y, accel_z;
    while (true) {
        // Read and process accelerometer data
        read_accel_data(&accel_x, &accel_y, &accel_z);
        process_accel_data(&accel_x, &accel_y, &accel_z);

        // Render graph (this is a simplified placeholder)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Example rendering: draw a simple line (you can replace this with actual graph plotting)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 100, 100, 300, 300);
        SDL_RenderPresent(renderer);

        // Add delay to make rendering smoother (e.g., 30 FPS)
        SDL_Delay(33);  // 30 FPS
    }

    // Clean up SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
