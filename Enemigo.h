#include <iostream>
#ifndef ENEMIGO_H
#define ENEMIGO_H
#include "SDL/SDL_image.h"

using namespace std;

class Enemigo
{
    public:
        int x;
        int y;
        int i;
        string tipo;

        SDL_Surface* surface;
        bool hacia_derecha;

        virtual void moverse();

        Enemigo(){}
        Enemigo(int x, int y, SDL_Surface* surface);
        virtual ~Enemigo();
    protected:
    private:
};

#endif // ENEMIGO_H
