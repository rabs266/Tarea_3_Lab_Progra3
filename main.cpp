#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "Personaje.h"
#include "Fantasmita.h"
#include "Mina.h"
#include "Cocodrilo.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 60;

Mix_Music *music = NULL;

SDL_Surface *background = NULL;

SDL_Surface *screen = NULL;

SDL_Event event;

TTF_Font *font = NULL;

SDL_Color textColor = { 0, 0, 0 };

SDL_Surface *load_image( std::string filename )
{
    return IMG_Load(filename.c_str());
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    if( screen == NULL )
    {
        return false;
    }

    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }

    SDL_WM_SetCaption( "Llega a la Meta sin que te toquen", NULL );

    return true;
}

bool load_files()
{
    background = load_image( "background.png" );

    if( background == NULL )
    {
        return false;
    }

    music = Mix_LoadMUS( "DissonantWaltz.ogg" );

    if( music == NULL )
    {
        return false;
    }
        return true;
}

void clean_up()
{
    SDL_FreeSurface( background );
    Mix_FreeMusic( music );
    SDL_Quit();
}

bool siChoco(int p_x, int p_y, int c_x, int c_y, int c_w, int c_h)
{
    if(p_x>c_x-c_w/2 &&
       p_x<c_x+c_w/2 &&
       p_y>c_y-c_h/2 &&
       p_y<c_y+c_h/2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool colision(int personaje_x, int personaje_y, int enemigo_x, int enemigo_y)
{
    int width_pichingo = 60;
    int heigh_pichingo = 60;

    int width_enemigo = 60;
    int heigh_enemigo = 60;

    int punto_d_x = personaje_x+width_pichingo/2;
    int punto_a_y = personaje_y+heigh_pichingo/2;
    int punto_i_x = personaje_x-width_pichingo/2;
    int punto_ar_y = personaje_y-heigh_pichingo/2;

    if(siChoco(punto_d_x, punto_a_y, enemigo_x, enemigo_y, width_enemigo, heigh_enemigo))
    {
        return true;
    }
    if(siChoco(punto_d_x, punto_ar_y, enemigo_x, enemigo_y, width_enemigo, heigh_enemigo))
    {
        return true;
    }
    if(siChoco(punto_i_x, punto_a_y, enemigo_x, enemigo_y, width_enemigo, heigh_enemigo))
    {
        return true;
    }
    if(siChoco(punto_i_x, punto_ar_y, enemigo_x, enemigo_y, width_enemigo, heigh_enemigo))
    {
        return true;
    }
        return false;
}

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

int main( int argc, char* args[] )
{
    bool quit = false;

    bool cap = true;

    Timer fps;

    if( init() == false )
    {
        return 1;
    }

    if( load_files() == false )
    {
        return 1;
    }

    Personaje Personaje ( 20, 20, load_image ("Personaje01.png"));

    SDL_Surface* goal = load_image("Ganaste.png");

    vector <Enemigo*> enemigos;
    enemigos.push_back(new Fantasmita(400,550,load_image("Fantasma.png")));
    enemigos.push_back(new Enemigo (100,500,load_image("Fantasma.png")));
    enemigos.push_back(new Mina (600,500,load_image("Mina.png")));
    enemigos.push_back(new Cocodrilo(350, 500, load_image("Cocodrilo01.png")));

    SDL_Surface* meta = load_image ( "Meta.png");

    //Cargo las imagenes de cuando se lo comieron
    SDL_Surface* fantcomio = load_image("FantasmaComio.png");
    SDL_Surface* cococomio = load_image("CocodriloComio.png");
    SDL_Surface* minaboom = load_image("MinaExploto.png");

    bool game_over = false;

    while( quit == false)
    {
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }

    if(game_over == false)
    {
        apply_surface( 0, 0, background, screen );

        apply_surface (Personaje.x-Personaje.surface->w/2,Personaje.y-Personaje.surface->h/2,Personaje.surface, screen);

        for(int i= 0; i<enemigos.size(); i++)
        {
            apply_surface (enemigos[i]->x-enemigos[i]->surface->w/2,enemigos[i]->y-enemigos[i]->surface->h/2, enemigos[i]->surface, screen);
            enemigos[i]->moverse();

            //Aplico la imagen de que solo comio algun Enemigo
            if(colision(Personaje.x, Personaje.y, enemigos[i]->x, enemigos[i]->y))
            {
                if(enemigos[i]->tipo == "cocodrilo")
                {
                    apply_surface(250, 100, cococomio, screen);
                }
                if(enemigos[i]->tipo == "fantasma2")
                {
                    apply_surface(250, 100, fantcomio, screen);
                }
                if(enemigos[i]->tipo == "fantasma1")
                {
                    apply_surface(250, 100, fantcomio, screen);
                }
                if(enemigos[i]->tipo == "mina")
                {
                    apply_surface(250, 100, minaboom, screen);
                }
                    game_over = true;
            }
        }

        apply_surface ( 650, 500, meta, screen );

        Uint8 *keystates = SDL_GetKeyState( NULL );

        if( Mix_PlayingMusic() == 0 )
        {
            if( Mix_PlayMusic( music, -1 ) == -1 )
            {
                return 1;
            }
        }

        if( keystates[ SDLK_UP ] )
        {
            Personaje.y-=2;
        }

        if( keystates[ SDLK_DOWN ] )
        {
            Personaje.y+=2;
        }

        if( keystates[ SDLK_LEFT ] )
        {
            Personaje.x-=2;
        }

        if( keystates[ SDLK_RIGHT ] )
        {
            Personaje.x+=2;
        }

            /////////////////////////////////////////////////IMPLEMENTACION DE FRAMES///////////////////////////////////

        if( ( cap == true ) && ( fps.get_ticks() < 100 / FRAMES_PER_SECOND ) )
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 100 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

        if(Personaje.x > 700 && Personaje.y >550)
        {
            apply_surface(200, 200, goal, screen);
            game_over = true;
        }
    }

        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }
    }

    clean_up();

    return 0;
}
