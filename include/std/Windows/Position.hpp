#ifndef POSITION_HPP
#define POSITION_HPP

#include <core/Component.hpp>
#include <core/Manager.hpp>
#include <core/Alias.hpp>
#include <iostream>
#include <vector>
#include <tuple>
#include <std/Windows/Pixel.tpp>

namespace EGE::STD::TERMINAL::WINDOWS{


    /**
     * @class Componente de posición para una entidad.
     */
    class Position : public EGE::CORE::Component<Position>{
        private: 

            /**Tamaño del arreglo*/
            int size;
            /**Vector de las tuplas de posicion de la entidad*/
            std::vector<std::tuple<int,int>> position;

            std::tuple<int,int> firstPosition;

        public:

            /**
             * @brief Método constructor de position.
             */
            Position();

            /**
             * @brief Método destructor de position.
             */
            ~Position();

            /**
             * @brief Metodo para iniciar la posicion de una entidad.
             * @param size Tamaño del arreglo de posiciones.
             * @param x Posición inicial de la entidad en x.
             * @param y Posición inicial de la entidad en y.
             */
            void positionInitializer(int size,int x,int y);

            /**
             * @brief Metodo para iniciar la posicion de le entidad con sprite.
             * @param size Tamaño del arreglo de posiciones.
             * @param sprite Vector con el sprite para calcular bien las posiciones en terminal.
             * @param x Posición inicial de la entidad en x.
             * @param y Posición inicial de la entidad en y.
             */
            void positionInitiaizerSprite(int size,std::vector<Pixel> sprite,int x, int y);

            /**
             * @brief Método para obtener la posición de la entidad
             * @return Vector con las coordenas de la entidad
             */
            std::vector<std::tuple<int,int>> getPosition();

            /**
             * @brief Método que retorna la posición inicial en la terminal.
             * @return Tupla con la posición inicial en terminal.
             */
            std::tuple<int,int> *getFisrtPosition();

            /**
             * @brief Método para actualizar la posición a la entidad.
             * @param void Función para cambiar la posicion del elemento.
             */
            void updatePosition(std::vector<Pixel> sprite);

            /**
             * @brief Método que reinicia la posición a partir de las nuevas nuevas coordenadas.
             * @param sprite Vector con el sprite para calcular bien las posiciones en terminal.
             * @param x Posición inicial de la entidad en x.
             * @param y Posición inicial de la entidad en y.
             */
            void resetPosition(std::vector<Pixel> sprite, int x, int y);
    };

    /**
     * @class Clase de posición para objertos sin sprite.
     */
    template <typename mType>
    class mPosition : public EGE::CORE::Manager<mType>{
        public:

            /**
             * @brief Metodo para iniciar la posicion del sprite.
             * 
             * @param size Tamaño del arreglo de posiciones.
             * @param x Posición inicial de la entidad en x.
             * @param y Posición inicial de la entidad en y.
             */
            void positionInitializer(EGE::CORE::EntityId id,int x, int y);

            /**
             * @brief Método que agrega una entidades a entities.
             */
            int addEntity();
    }; 
}

#endif //Position.hpp