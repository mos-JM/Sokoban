/**
 * Esta libreria permite colorear la salida tanto bajo windows como bajo linux
 * 
 * Uso: 
 *   - debe estar incluida en el proyecto
 *   - debe haber un #include "colores.h" al comienzo del fichero
 *   - llama a setColor(color) para cambiar el color actual
 *      usa las constantes Gris, Rojo, Verde, Azul para simplificarte la vida!
 *   - llama a cout << algo para mostrar 'algo' en ese color
 * 
 * Autor: manuel.freire@fdi.ucm.es
 * Licencia: https://creativecommons.org/licenses/by-sa/3.0/
 */

// colores validos
enum Color {
    Gris, Rojo, Verde, Azul, Blanco=15
};

#ifdef _WIN32
// este codigo solo se ejecuta bajo Windows -->

    #include <windows.h>

    void setColor(int color) {
        WORD a = 0;
        switch (color) {
            case Gris: a = FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN; break;
            case Rojo: a = FOREGROUND_RED|FOREGROUND_INTENSITY; break;
            case Verde: a = FOREGROUND_GREEN|FOREGROUND_INTENSITY; break;
            case Azul: a = FOREGROUND_BLUE|FOREGROUND_INTENSITY; break;
            default: a = color;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
    }

// <-- fin de codigo windows-exclusivo
#endif
#ifdef linux
// este codigo solo se ejecuta bajo linux -->

    #include <iostream>
    #include <sstream>
    
    namespace Colores {
        
        // constantes globales para colores
        const char *ANSI_COLOR_RED   =  "\x1b[31;1m";
        const char *ANSI_COLOR_GREEN =  "\x1b[32;1m";
        const char *ANSI_COLOR_YELLOW = "\x1b[33;1m";
        const char *ANSI_COLOR_BLUE =   "\x1b[34;1m";
        const char *ANSI_COLOR_MAGENTA ="\x1b[35;1m";
        const char *ANSI_COLOR_CYAN  =  "\x1b[36;1m";
        const char *ANSI_COLOR_WHITE  = "\x1b[37;1m";
        const char *ANSI_COLOR_RESET  = "\x1b[0m";
        
        // variable global para el color
        const char *g_current_color = ANSI_COLOR_RESET;                
        
        // referencia global al viejo cout
        std::ostream &g_old_cout = std::cout;
        
        typedef std::ostream& (*STRFUNC)(std::ostream&);
        class ColorCout {
        public:
            template<typename T>
            ColorCout& operator<<(T t) {
                g_old_cout << g_current_color << t << ANSI_COLOR_RESET;
                return *this;
            }
            
            // ver http://stackoverflow.com/a/14829836/15472
            ColorCout& operator<<(STRFUNC func) {
                g_old_cout << g_current_color << func;
                return *this;
            }       
        };

        // referencia global al nuevo cout
        ColorCout g_new_cout = ColorCout();                
    }
    
    // cambio de color (modifica la global)
    void setColor(int color) {
        const char *a = Colores::ANSI_COLOR_RESET;
        switch (color) {
            case Gris: a = Colores::ANSI_COLOR_RESET; break;
            case Rojo: a = Colores::ANSI_COLOR_RED; break;
            case Verde: a = Colores::ANSI_COLOR_GREEN; break;
            case Azul: a = Colores::ANSI_COLOR_BLUE; break;
            case 15: a = Colores::ANSI_COLOR_WHITE; break;
            default: a = Colores::ANSI_COLOR_RESET;
        }
        Colores::g_current_color = a;
    }    
    
    #define cout Colores::g_new_cout
    
// <-- fin de codigo linux-exclusivo
#endif
