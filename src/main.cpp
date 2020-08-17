#include <iostream>
#include <windows.h>

#include <core/Core.hpp>
#include <std/Windows/WindowsSTD.hpp>

#define colorTerminal 10
#define colorTablero 6
#define colorBomberMan 9
#define colorBomba 4

/*Clase tablero y su administrador.*/
class tablero :public EGE::CORE::Entity<tablero>{
    public:
        tablero(int id): Entity(id){}
};

class managerTablero : public EGE::STD::TERMINAL::WINDOWS::mSprite<tablero>{};

/*Clase bomba y su administrador.*/
class bomba : public EGE::CORE::Entity<bomba>{
    private:
        bool visible = false;
    public:
        bomba(int id) : Entity(id){}

        bool isVisible(){
            return visible;
        }

        bool setVisible(bool v){
            this->visible = v;
            return visible;
        }
};

class managerBomba : public EGE::STD::TERMINAL::WINDOWS::mSprite<bomba>{
    private:
        int bombas[3];

    public:
        void createBombs(){
            for( int i = 0; i < 3; i++ ){
                bombas[i] = this -> addEntity();
                this -> spriteInitializer(bombas[i],3,"bomba");
                this -> positionInitializer(bombas[i], 1,1);
            }
        }

        int getAvailableBomb(){
            for( auto id : bombas ){
                auto bombEntity = this -> template getEntity<bomba>( id );
                if( !bombEntity->isVisible() ){
                    return id;
                }
            }
            return -1;
        }

        void explodeBomb(int id, EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<managerBomba> *viewBomba ){
            auto bombEntity = this -> template getEntity<bomba>( id );
            viewBomba->viewColor( id, this, colorBomba, bombEntity->setVisible(true) );
            Sleep(1000);
            viewBomba->view( id, this,  bombEntity->setVisible( false ) );
        }
        
        #if 0
        DWORD WINAPI ThreadFunc( LPVOID lpParams ) {
            return 0;
        }
        #endif
};

/*Clase bomberMan y su administrador.*/
class bomberMan : public EGE::CORE::Entity<bomberMan>{
    public:
        bomberMan( int id ) : Entity( id ){}
};

class managerBomberMan : public EGE::STD::TERMINAL::WINDOWS::mSprite<bomberMan>{};


int main(){
    /*Se crea el tablero del juego*/
    EGE::STD::TERMINAL::WINDOWS::mTerminal tm;
    auto terminal = tm.addEntity(22,22);
    
    /*Creamos los manager de las entidades del juego*/
    managerTablero mTablero;
    managerBomberMan mBomberMan;
    managerBomba mBomba;

    /*Creamos a las entidades*/
    auto tablero = mTablero.addEntity();
    auto bomberman = mBomberMan.addEntity();
    mBomba.createBombs();

    /*Agregamos e inicializamos sus componentes*/
    mTablero.spriteInitializer(tablero,21,"tablero");
    mTablero.positionInitializer(tablero,1,1);

    mBomberMan.addComponent<EGE::STD::TERMINAL::WINDOWS::Controller>(bomberman);
    mBomberMan.spriteInitializer(bomberman,3,"bomberMan");
    mBomberMan.positionInitializer(bomberman,1,1);

    /*Sistema de entrada*/
    EGE::STD::TERMINAL::WINDOWS::systemInput entrada;

    /*Sistemas de visualizacion para las entidades.*/
    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<managerTablero> viewTablero;
    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<managerBomberMan> viewBomberMan;
    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<managerBomba> viewBomba;

    /*Sistemas de desplazamiento para las entidades.*/
    EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<managerBomberMan> dpBomberMan;

    /*Creamos el sistema de colision entre las entidades*/
    //EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<managerBomberMan,mTerminal> sysBombaCollition;
    EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<managerTablero,managerBomberMan> sysBomberManCollition;
    EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<managerTablero,managerBomba> sysBombaCollition;
    EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;


    /*Inicalizamos a la  terminal y al metedo de salida del juego*/
    tm.terminalSetColor(terminal, colorTerminal );
    tm.terminalPersonalized(terminal,'-','|');

    viewTablero.viewColor(tablero,&mTablero,colorTablero, true);

    char Tecla = 0;

    #if 1
    /*Bucle del juego*/
    while(Tecla != 'c'){
        char teclasMovimiento[4] = {UP, DOWN, LEFT, RIGHT};
        char lastKey;
        /*Recogemos la tecla*/
        Tecla = entrada.update();

        for(int i = 0; i < 4; i++){
            if( Tecla == teclasMovimiento[i] ){
                lastKey = Tecla;
            }
        }
        /*Movemos al jugador*/
        dpBomberMan.update(Tecla,bomberman,&mBomberMan,ARROWS);  

        /*Uso del sistema de colision*/
        #if 1
        if(Tecla != 0){
            if(sysBomberManCollition.collition(bomberman,&mTablero,&mBomberMan)){
                dpBomberMan.update(inverter.update(Tecla,ARROWS),bomberman,&mBomberMan,ARROWS);
            }
            else if(Tecla == 'e' || Tecla == 'E'){
                auto positionBomberMan = mBomberMan.getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(bomberman);
                auto firstPosition = positionBomberMan -> getFisrtPosition();
            
                int x = std::get<0>(*firstPosition);
                int y = std::get<1>(*firstPosition);
                
                switch (lastKey){
                    case UP:
                        y -= 3;
                        break;
                    case DOWN:
                        y +=3;
                        break;
                    case RIGHT:
                        x += 3;
                        break;
                    case LEFT:
                        x -= 3;
                        break;
                }
                
                #if 0
                HANDLE thread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL );
                    if (thread == NULL){}
                    else{
     	                std::cout<<"Error"<< GetLastError()<< std::endl;
                    }
                CloseHandle( thread );
                #endif

                //mBomba.explodeBomb( &viewBomba );
                
                auto availableBomb = mBomba.getAvailableBomb();
                if( availableBomb != -1 ){
                    mBomba.resetPosition(availableBomb, x, y);
                    if ( !sysBombaCollition.collition(availableBomb, &mTablero, &mBomba) )
                        mBomba.explodeBomb( availableBomb, &viewBomba );
                }
            }
        }
        #endif
        /*Pintamos al jugador*/
        viewBomberMan.viewColor(bomberman,&mBomberMan,colorBomberMan, true);
        viewTablero.viewColor( tablero, &mTablero, colorTablero, true );

        /*Espera del juego*/
        Sleep(10);
    }
    #endif
}