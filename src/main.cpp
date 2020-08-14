#include <iostream>
//#include <thread>

#include <core/Core.hpp>

#include <std/Windows/WindowsTerminal.tpp>
#include <std/Windows/Sprite.tpp>
#include <std/Windows/Position.tpp>
#include <std/Windows/Controller.tpp>
#include <std/Windows/Input.tpp>
#include <std/Windows/keyDefinition.hpp>
#include <std/Windows/visualizeEntity.tpp>
#include <std/Windows/displacementEntity.tpp>
#include <std/Windows/systemGenericCollition.tpp>
#include <std/Windows/systemKeyInverter.tpp>


/*Clase tablero y su administrador.*/
class tablero :public EGE::CORE::Entity<tablero>{
    public:
        tablero(int id): Entity(id){}
};

class managerTablero : public EGE::STD::TERMINAL::WINDOWS::mSprite<tablero>{};

/*Clase bomba y su administrador.*/
class bomba : public EGE::CORE::Entity<bomba>{
    public:
        bomba(int id) : Entity(id){}
};

class managerBomba : public EGE::STD::TERMINAL::WINDOWS::mSprite<bomba>{
    private:
        int bombas[3];
    public:
        void createBombs(){
            for( int i = 0; i < 3; i++ ){
                bombas[i] = this -> addEntity();
                this -> spriteInitializer(bombas[i],3,"bomba");
            }
        }

        int getAvailableBomb(){
            for( auto id : bombas ){
                auto sprite = this -> template getComponent<EGE::STD::TERMINAL::WINDOWS::Sprite>( id );
                if( !sprite->isVisible() ){
                    return id;
                }
            }
            return -1;
        }

        void explodeBomb( int bomb, EGE::STD::TERMINAL::WINDOWS::visualizeEntity<managerBomba> *viewBomba ){
            viewBomba->update( bomb, this);
            Sleep(1000);
            viewBomba->update( bomb, this, false);
        }
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
    EGE::STD::TERMINAL::WINDOWS::visualizeEntity<managerTablero> viewTablero;
    EGE::STD::TERMINAL::WINDOWS::visualizeEntity<managerBomberMan> viewBomberMan;
    EGE::STD::TERMINAL::WINDOWS::visualizeEntity<managerBomba> viewBomba;

    /*Sistemas de desplazamiento para las entidades.*/
    EGE::STD::TERMINAL::WINDOWS::displacementEntity<managerBomberMan> dpBomberMan;

    /*Creamos el sistema de colision entre las entidades*/
    EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<managerTablero,managerBomberMan> sysCollition;
    EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;


    /*Inicalizamos a la  terminal y al metedo de salida del juego*/
    tm.terminalPersonalized(terminal,'-','|');

    viewTablero.update(tablero,&mTablero);

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
                auto position = mBomberMan.getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(bomberman);
                auto firstPosition = position -> getFirstPosition();
            
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
        viewBomberMan.update(bomberman,&mBomberMan);
        viewTablero.update(tablero,&mTablero);

        /*Espera del juego*/
        Sleep(10);
    }
    #endif
}