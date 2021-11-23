#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

// Constants for screen height and width
int screen_height = 30;
int screen_width = 120;

// Constants for the torus
const float R1 = 1.0f; // thickness of the torus
const float R2 = 2.0f; // radius of the torus
const float theta_step = 0.014f;
const float phi_step = 0.004f;

// Constants for viewing frame
float K1 = 1.0f;
const float K2 = 5.0f; // how far away the torus is from the viewer

const float pi = (float)M_PI;

char* lum_to_ascii = ".,-~:;=!*#$@"; 

void render_frame(float A, float B)
{
    K1 = screen_width*K2*3/(8*(R1+R2));
    // printf("%d, %d\n", screen_height, screen_width);

    char* output = (char*)malloc(screen_height*screen_width*sizeof(char));
    float* zbuffer = (float*)malloc(screen_height*screen_width*sizeof(float));

    memset(output, 32, screen_height*screen_width*sizeof(char)); // 32 is the character for space
    memset(zbuffer, 0, screen_height*screen_width*sizeof(float));

    // precomputing sine and cosine of A and B
    float sinA = sinf(A);
    float sinB = sinf(B);
    float cosA = cosf(A);
    float cosB = cosf(B);

    // generating torus
    for(float theta=0.0f; theta < 2*pi; theta += theta_step)
    {
        // precomputing sine and cosine of theta
        float sintheta = sinf(theta);
        float costheta = cosf(theta);

        for(float phi=0.0f; phi < 2*pi; phi += phi_step)
        {
            // computing sine and cosine of phi
            float sinphi = sinf(phi);
            float cosphi = cosf(phi);

            /// Calculating location of pixel
            // Calculating 3d point on circle
            float circlex = R2+R1*costheta;
            float circley = R1*sintheta;
            float x = circlex*(cosB*cosphi + sinA*sinB*sinphi) - circley*cosA*sinB;
            float y = circlex*(sinB*cosphi - sinA*cosB*sinphi) + circley*cosA*cosB;
            float z = K2 + cosA*circlex*sinphi + circley*sinA;
            float invz = 1/z;

            // Projection
            int xp = (int) (screen_width/2.0f + K1*invz*x);
            int yp = (int) (screen_height/2.0f - K1*invz*y); // inverting y because increasing indexes are lower on the screen

            // Only care if the donut is on the screen
            if(yp < screen_height && yp >= 0 && xp < screen_width && xp >= 0)
            {
                /// Calculating luminance
                // (Nx, Ny, Nz) * (0, 1, -1). This ranges from +- sqrt(2)
                float L = cosphi*costheta*sinB - cosA*costheta*sinphi - sinA*sintheta + cosB*(cosA*sintheta - costheta*sinA*sinphi);
                if(L > 0) // we don't care if the normals point away from the camera as its already hidden from the screen.
                {
                    if(invz > zbuffer[yp*screen_width + xp])
                    {
                        zbuffer[yp*screen_width + xp] = invz;
                        int luminence = (int)(L*8.0f);

                        // Converting luminence to an ASCII character
                        // fprintf(stderr, "%d\n", luminence);
                        output[yp*screen_width + xp] = lum_to_ascii[luminence];
                        // output[yp*screen_width + xp] = 'p';

                    }
                }
            }
        }
    }

    // output to screen
    erase();
    for(int i=0; i<screen_height; ++i)
    {
        for(int j=0; j<screen_width; ++j)
        {
            mvaddch(i,j, output[i*screen_width + j]);
            // mvaddch(i,j, 'p');
        }
    }

    free(output);
    free(zbuffer);
}


int main(int argc, char* argv[])
{
    initscr();
    cbreak();
    noecho();
    getmaxyx(stdscr, screen_height, screen_width);
    // printw("%i, %i", screen_height, screen_width);
    timeout(0);
    keypad(stdscr, TRUE);

    float a = 0.0f;
    while(true)
    {
        if(getch() != -1) 
        {
            break;
        }

        render_frame(a,a);
        a += 0.01f;
        if(a > 2*pi)
        {
            a = 0.0f;
        }
        usleep(10000);
    }

    endwin();
    // render_frame(a,a);
}