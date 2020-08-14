#include <iostream>
//#include <thread>

#include <core/Core.hpp>

#include <std/Windows/WindowsTerminal.tpp>
#include <std/Windows/Sprite.tpp>
#include <std/Windows/Position.tpp>
#include <std/Windows/Controller.tpp>
#include <std/Windows/Input.tpp>
#include <std/Windows/keyDefinition.hpp>
#include <std/Windows/systemVisualizeEntity.tpp>
#include <std/Windows/systemDisplacementEntity.tpp>
#include <std/Windows/systemGenericCollition.tpp>
#include <std/Windows/systemKeyInverter.tpp>

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
        #if 1
        void createBombs(){
            for( int i = 0; i < 3; i++ ){
                bombas[i] = this -> addEntity();
                this -> spriteInitializer(bombas[i],3,"bomba");
                //this -> positionInitializer(bombas[i], 1,1);
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

        void explodeBomb( int id, EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<managerBomba> *viewBomba ){
            auto bombEntity = this -> template getEntity<bomba>( id );
            viewBomba->viewColor( id, this, colorBomba, bombEntity->setVisible(true) );
            Sleep(1000);
            viewBomba->view( id, this,  bombEntity->setVisible( false ) );
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
    EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<managerTablero,managerBomberMan> sysCollition;
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
            if(sysCollition.collition(bomberman,&mTablero,&mBomberMan)){
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
                
                auto availableBomb = mBomba.getAvailableBomb();
                if( availableBomb != -1 ){
                    mBomba.positionInitializer(availableBomb, x, y);
                    //std::thread t1( mBomba.explodeBomb, availableBomb, &viewBomba );
                    mBomba.explodeBomb( availableBomb, &viewBomba );
                }
            }
        }
        #endif
        /*Pintamos al jugador*/
        viewBomberMan.viewColor(bomberman,&mBomberMan,colorBomberMan, true);
        viewTablero.viewColor(tablero,&mTablero,colorTablero,true);

        /*Espera del juego*/
        Sleep(10);
    }
    #endif
}