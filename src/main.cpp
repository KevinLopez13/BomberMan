#include <iostream>
#include <windows.h>
#include <time.h>

#include <core/Core.hpp>
#include <std/Windows/Others/WindowsSTD.hpp>

#define ESTW EGE::STD::TERMINAL::WINDOWS

#define colorTerminal 10
#define colorTablero 6
#define colorBomberMan 9
#define colorBomba 4

#define maxBombs 3
#define delayBombs 4

/*Clase tablero y su administrador.*/
class tablero :public EGE::CORE::Entity<tablero>{
    public:
        tablero(int id): Entity(id){}
};

class managerTablero : public ESTW::mSprite<tablero>{};

/*Clase bomba y su administrador.*/
class bomba : public EGE::CORE::Entity<bomba>{
    private:
        bool visible = false;
        time_t inicio, fin;
    public:
        bomba(int id) : Entity(id){}

        bool isVisible(){
            return visible;
        }

        bool setVisible(bool v){
            this->visible = v;
            return visible;
        }

        void initTime(){
            inicio = time(NULL);
        }

        time_t getTime(){
            fin = time(NULL);
            return fin - inicio;
        }
};

class managerBomba : public ESTW::mSprite<bomba>{};

/*Clase bomberMan y su administrador.*/
class bomberMan : public EGE::CORE::Entity<bomberMan>{
    public:
        bomberMan( int id ) : Entity( id ){}
};

class managerBomberMan : public ESTW::mSprite<bomberMan>{};

/*Sistema que regula el movimiento del bomberman.*/
class bomberSystem : public EGE::CORE::System<managerBomberMan>{
    public:
        void move(ESTW::systemDisplacementEntity<managerBomberMan> *dpBomberMan,
                char Tecla, int bomberman, managerBomberMan *mBomberMan){
            for( int i = 0; i < 3; i++){
                dpBomberMan->update( Tecla, bomberman, mBomberMan, ARROWS);
            }
        }
};

/*Sistema que administra las bombas.*/
class bombSystem : public EGE::CORE::System<managerBomba>{
    private:
        int bombas[ maxBombs ];
        bool someVisible = false;
    public:
        void createBombs( managerBomba *mBomba ){
            for( int i = 0; i < maxBombs; i++ ){
                bombas[i] = mBomba -> addEntity();
                mBomba -> spriteInitializer(bombas[i],3,"bomba");
                mBomba -> positionInitializer(bombas[i], 1,1);
            }
        }

        int getAvailableBomb( managerBomba *mBomba ){
            for( auto id : bombas ){
                auto bombEntity = mBomba -> template getEntity<bomba>( id );
                if( !bombEntity->isVisible() ){
                    return id;
                }
            }
            return -1;
        }

        bool isVisible( int id, managerBomba *mBomba){
            auto bombEntity = mBomba -> template getEntity<bomba>( id );
            return bombEntity -> isVisible();
        }

        void updateBombs( managerBomba *mBomba, ESTW::systemVisualizeEntity<managerBomba> *viewBomba){
            for( auto id : bombas ){
                auto bombEntity = mBomba -> template getEntity<bomba>( id );
                time_t timeToExplode = bombEntity->getTime();
                
                if( bombEntity->isVisible() && timeToExplode >= delayBombs ){
                    /*Explode bomb*/
                    viewBomba->view( id, mBomba, bombEntity->setVisible( false ) );
                }
                else{
                    viewBomba->viewColor( id, mBomba, colorBomba, bombEntity->isVisible() );
                }
            }
        }

        void setBomb(int id, managerBomba *mBomba, ESTW::systemVisualizeEntity<managerBomba> *viewBomba ){
            auto bombEntity =  mBomba -> template getEntity<bomba>( id );
            bombEntity->initTime();
            viewBomba->viewColor( id, mBomba, colorBomba, bombEntity->setVisible(true) );
        }
};

int main(){
    /*Se crea el tablero del juego*/
    ESTW::mTerminal tm;
    auto terminal = tm.addEntity(22,22);
    
    /*Creamos los manager de las entidades del juego*/
    managerTablero mTablero;
    managerBomberMan mBomberMan;
    managerBomba mBomba;

    /*Creamos a las entidades*/
    auto tablero = mTablero.addEntity();
    auto bomberman = mBomberMan.addEntity();

    /*Creamos el sistema administrador de bomberman*/
    bomberSystem bmSystem;

    /*Creamos el sistema administrador de bombas.*/
    bombSystem bSystem;
    bSystem.createBombs( &mBomba );

    /*Agregamos e inicializamos sus componentes*/
    mTablero.spriteInitializer(tablero,21,"tablero");
    mTablero.positionInitializer(tablero,1,1);

    mBomberMan.addComponent<EGE::STD::TERMINAL::WINDOWS::Controller>(bomberman);
    mBomberMan.spriteInitializer(bomberman,3,"bomberMan");
    mBomberMan.positionInitializer(bomberman,1,1);

    /*Sistema de entrada*/
    EGE::STD::TERMINAL::WINDOWS::systemInput entrada;

    /*Sistemas de visualizacion para las entidades.*/
    ESTW::systemVisualizeEntity<managerTablero> viewTablero;
    ESTW::systemVisualizeEntity<managerBomberMan> viewBomberMan;
    ESTW::systemVisualizeEntity<managerBomba> viewBomba;

    /*Sistemas de desplazamiento para las entidades.*/
    ESTW::systemDisplacementEntity<managerBomberMan> dpBomberMan;

    /*Creamos el sistema de colision entre las entidades*/
    ESTW::systemGenericCollition< managerBomberMan, managerTablero > sysColl_BMan_Tab;
    ESTW::systemGenericCollition< managerBomba, managerTablero > sysColl_Bomb_Tab;
    ESTW::systemGenericCollition< managerBomberMan, managerBomba > sysColl_BMan_Bomb;
    ESTW::systemGenericCollition< managerBomba, managerBomberMan > sysColl_Bomb_BMan;
    ESTW::systemCollitionTerminal< managerBomba > sysColl_Terminal;
    ESTW::systemKeyInverter inverter;

    /*Sistema de reset para las bombas.*/
    ESTW::systemPositionReset<managerBomba> resetBomb; 


    /*Inicalizamos a la  terminal y al metedo de salida del juego*/
    tm.terminalSetColor(terminal, colorTerminal );
    tm.terminalPersonalized(terminal,'-','|');

    viewTablero.viewColor(tablero,&mTablero,colorTablero, true);

    char Tecla = 0;
    int idBombCollition;

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
        //dpBomberMan.update(Tecla,bomberman,&mBomberMan,ARROWS); 
        bmSystem.move( &dpBomberMan, Tecla, bomberman, &mBomberMan);

        /*Uso del sistema de colision*/
        #if 1
        if(Tecla != 0){
            if( sysColl_BMan_Tab.collition( bomberman, &mBomberMan, &mTablero )){
                //dpBomberMan.update(inverter.update(Tecla,ARROWS),bomberman,&mBomberMan,ARROWS);
                bmSystem.move( &dpBomberMan, inverter.update(Tecla,ARROWS), bomberman, &mBomberMan);
            }
            else if( sysColl_BMan_Bomb.collitionId( bomberman, &mBomberMan, &mBomba, &idBombCollition ) &&
                    bSystem.isVisible(idBombCollition, &mBomba ) ){
                //dpBomberMan.update(inverter.update(Tecla,ARROWS),bomberman,&mBomberMan,ARROWS);
                bmSystem.move( &dpBomberMan, inverter.update(Tecla,ARROWS), bomberman, &mBomberMan);

            }
            else if(Tecla == 'e' || Tecla == 'E'){
                auto positionBomberMan = mBomberMan.getComponent<ESTW::Position>(bomberman);
                auto firstPosition = positionBomberMan -> getFirstPosition();
            
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

                auto availableBomb = bSystem.getAvailableBomb( &mBomba );
                if( availableBomb != -1 ){
                    resetBomb.positionResetSprite(availableBomb, &mBomba, x, y);

                    if ( !sysColl_Bomb_Tab.collition(availableBomb, &mBomba, &mTablero) and
                            !sysColl_Terminal.update(availableBomb, &mBomba) ){
                        bSystem.setBomb( availableBomb, &mBomba, &viewBomba );
                    }
                }
            }
        }
        #endif
        /*Actualizamos bombas.*/
        bSystem.updateBombs( &mBomba, &viewBomba );

        /*Se pinta al jugador y al tablero.*/
        viewBomberMan.viewColor(bomberman,&mBomberMan,colorBomberMan, true);
        viewTablero.viewColor( tablero, &mTablero, colorTablero, true );

        /*Espera del juego*/
        Sleep(5);
    }
    #endif
}