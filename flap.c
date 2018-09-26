#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <SDL.h>

//Frames and window settings
#define BITDEPTH 32 //color depth
#define WINPOS_X 1000 //game window's x position on monitor
#define WINPOS_Y 100 //game window's y position on monitor
#define WIN_WIDTH 400 //game window's pixel width
#define WIN_HEIGHT 650 //game window's pixel height
#define FRAMELAG 2 //the lag between frames
#define FRAMESPEED 11 //how fast the screen scrolls

// Bird settings
#define BIRD_GRAV 0.6 //the effect of gravity, the higher the value, the greater the gravity
#define GRAV_ACCEL 20 //acceleration of gravity
#define BIRD_SIZE 30 //size of the rectangle bird, in this case it's 30 pixels x 30 pixels
#define BIRD_POW -8 //determines how much speed and height the bird will gain with each input; the lower the value, the greater power each input has

// Game states
#define MENU 0
#define PLAY 1

// Pipe settings
#define GAP 120 //pixel gap between top pipe and bottom pipe
#define TOP_MIN 30 //the minimum amount of pixels the top pipe can be
#define TOP_MAX 400 //the maximum amount of pixels the top pipe can be
#define PIPE_START 700 //where the bird starts from the first pipe
#define PIPE_W 50 //width of each pipe
#define PIPE_VEL -8.5 // Speed of pipe loop
#define PIPE_DIST 300 // Distance between pipes horizontally


struct Pipe {
    int x; // Horizontal position of the pipe
    int y; // Top of pipe
    struct Pipe* next; // The next pipe
};


struct Pipe* make_pipe(int x)
{
    //returns the Pipe* p pointer to new allocated memory as to keep the data needed to form the pipes in succession
    struct Pipe* p = malloc(sizeof(struct Pipe));

    p->x = x;
    p->y = rand() % (TOP_MAX - TOP_MIN) + TOP_MIN;

    return p;
}

/*
draw a pipe onto a surface
'x' parameter is the left edge of the pipe
'y' parameter is the top of the gap in the pipe
*/
void drawpipe(SDL_Surface* surface, int x, int y)
{
    SDL_Rect pipe;

    // Top pipe:
    pipe.w = PIPE_W;
    pipe.h = y;
    pipe.x = x;
    pipe.y = 0;

    SDL_FillRect(surface, &pipe, SDL_MapRGB(surface->format, 121, 191, 39));

    // Bottom pipe:
    pipe.y = y + GAP;
    pipe.h = WIN_HEIGHT - pipe.y;

    SDL_FillRect(surface, &pipe, SDL_MapRGB(surface->format, 121, 191, 39));
}

/*
 Draw the bird onto a surface
 'surface' parameter forms the surface on which to draw the bird
 'x' parameter is the x-coordinate of the center of the bird
 'y' parameter is the y-coordinate of the center of the bird
*/
void drawbird(SDL_Surface* surface, int x, int y)
{
    SDL_Rect bird; //creates a rectangle for the bird, the preprocessor "BIRD_SIZE" set the size to 30x30
    bird.w = BIRD_SIZE;
    bird.h = BIRD_SIZE;
    bird.x = x - (bird.w /2);
    bird.y = y - (bird.h /2);

    SDL_FillRect(surface, &bird, SDL_MapRGB(surface->format, 255, 255, 0));
}

//Function will check for collision between bird and pipes
SDL_bool check_collision(struct Pipe* p, int bird_x, int bird_y)
{
    SDL_Rect bird;
    bird.w = BIRD_SIZE;
    bird.h = BIRD_SIZE;
    bird.x = bird_x - (bird.w / 2);
    bird.y = bird_y - (bird.h / 2);

    // Check for collision with top pipe
    SDL_Rect pipe;
    pipe.w = PIPE_W;
    pipe.h = p->y;
    pipe.x = p->x;
    pipe.y = 0;

    if (SDL_HasIntersection(&bird, &pipe))
        {
            return SDL_TRUE;
        }

    // Check for collision with bottom pipe:
    pipe.y = p->y + GAP;
    pipe.h = WIN_HEIGHT - pipe.y;

    return SDL_HasIntersection(&bird, &pipe);
}

int main(int argc, char** argv) {
    // Game Setup:
    const int bird_start_x = WIN_WIDTH / 4;
    const int bird_start_y = WIN_HEIGHT / 2;
    int bird_y;
    // Start velocity downward
    const double bird_start_vel = 0;
    double bird_vel ;
    int score;
    int alive;

    // Create first pipe
    struct Pipe* first_pipe;
    // Create next pipe
    struct Pipe* next_pipe;

    // SDL Setup:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow
    (
        "Flappy Bird Clone",
        WINPOS_X,
        WINPOS_Y,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* ren = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);


    // Main program loop:
    clock_t last_frame_time = clock();
    int run = 1;
    SDL_Event event;
    int screen = MENU;

    printf("Press any key or click the game window to the right to play. Be ready!\n");

    while (run)
        {
        // Process events:
        while (SDL_PollEvent(&event))
            {
                //play with mouse or keyboard:
                if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN)
                {
                    // A click on the game window begins the game
                    if (screen == MENU)
                    {
                        screen = PLAY;
                    }
                    else if (screen == PLAY)
                    {
                        // Vel changes with each jump
                        bird_vel = BIRD_POW;
                    }
                }
            }

        // Process and render the frames:
        if (clock() - last_frame_time >= FRAMESPEED * FRAMELAG)
            {
            // Draw the frame and allocate a new RGB surface
            SDL_Surface* surface = SDL_CreateRGBSurface
            (0, WIN_WIDTH, WIN_HEIGHT, BITDEPTH, 0, 0, 0, 0);

            if (screen == PLAY)
                {
                // Accelerate bird
                bird_vel = fmin(BIRD_GRAV + bird_vel, GRAV_ACCEL);
                bird_y += bird_vel;

                // Loop each pipe:
                struct Pipe* p = first_pipe;
                for (p = first_pipe; p != 0; p = p->next)
                {
                    p->x += PIPE_VEL;

                    // Check for collision to see whether the bird has died:
                    if (check_collision(p, bird_start_x, bird_y))
                        {
                            if (alive) printf("Game Over\n");
                            printf("Your final score count is: %d\n", score);
                            alive = 0;
                            return(0);
                        }
                        //draw the pipe:
                        drawpipe(surface, p->x, p->y);
                        //draw the next pipe:
                        if (next_pipe->next == 0)
                        {
                            next_pipe->next = make_pipe(next_pipe->x + PIPE_DIST);
                        }
                    }

                //Draw the bird:
                drawbird(surface, bird_start_x, bird_y);

                // Check for score:
                if (alive && bird_start_x + BIRD_SIZE / 2 > next_pipe->x)
                    {
                        score++;
                        printf("Your score count is: %d\n", score);
                        //stop counting until bird passes next pipe:
                        next_pipe = next_pipe->next;
                    }
                }
                else if (screen == MENU)
                {
                    // Reset bird:
                    bird_y = bird_start_y;
                    bird_vel = bird_start_vel;
                    score = 0;
                    alive = 1;
                    // Set up pipe loop:
                    next_pipe = first_pipe = make_pipe(PIPE_START);
                }


            // Render the frames with SDL library:
            SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surface);
            SDL_RenderCopy(ren, tex, 0, 0);
            SDL_RenderPresent(ren);

            // Start the next frame:
            last_frame_time = clock();
        }
    }
    return 0;
}
