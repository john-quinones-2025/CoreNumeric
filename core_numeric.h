#ifndef CORE_NUMERIC_H
#define CORE_NUMERIC_H

#include <concepts>     //Para definir concepts Addable, Divisible, Iterable, Comparable
#include <vector>
#include <iterator>     //Para std::begin y std::end
#include <type_traits>  //Para std::is_integral_v , constexpr
#include <cmath> // Para operaciones matematicas

namespace core_numeric {


    //CONCEPTS:

    // Definimos el concept Iterable, verifica que el tipo tenga begin() y end()
    template <typename C>
    concept Iterable = requires (C c) {
        std::begin(c);
        std::end(c);
    };

    // Definimos el concept Addable(), verifica que dos objetos tipo T se puedan sumar y den como resultado un T
    template <typename T>
    concept Addable = requires (T a, T b) {
        { a + b } -> std::same_as<T>;
    };

    // Definimos el concept Divisible, verifica que T se pueda dividir por un size_t(entero sin signo).
    template <typename T>
    concept Divisible = requires (T a, std::size_t n) {
        { a / n } -> std::same_as<T>;
    };

    // Creamos nuestro propio concept, Comparable, verifica comparacion 
    // Se usara para la función max
    template <typename T>
    concept Comparable = requires (T a, T b) {
        { a > b } -> std::convertible_to<bool>;
    };

    // ALGORITMOS GENERICOS:

    

    // Función sum
    // Funcion auxiliar implementada para ser reutilizada en mean y variance
    template <Iterable C>       //Tiene que ser de tipo iterable
    requires Addable<typename C::value_type>    //El tipo de dato contenido debe ser sumable
    auto sum(const C& contenedor) {
        using T = typename C::value_type;
        T resultado{}; // Inicializa en 0 o constructor por defecto

        for (const auto& valor : contenedor) {
            resultado = resultado + valor;
        }
        return resultado;
    }

    // Algoritmo mean (promedio)
    // Reutiliza sum y requiere concept Divisible
    template <Iterable C>
    requires Divisible<typename C::value_type> && Addable<typename C::value_type>
    auto mean(const C& contenedor) {
        // Reutilizamos sum como pide el PDF
        auto suma_total = sum(contenedor);
        std::size_t n = std::size(contenedor);
        
        // Evitamos división por cero 
        if (n == 0) return typename C::value_type{}; 

        // Retornamos la división definida en el concept
        return suma_total / n;
    }

    // Algoritmo variance (varianza)
    // Reutiliza mean y utiliza concept Iterable, Addable y Divisible
    template <Iterable C>
    requires Addable<typename C::value_type> && Divisible<typename C::value_type>
    auto variance(const C& contenedor) {
        auto promedio = mean(contenedor);
        using T = typename C::value_type;
        
        T acumulador{}; 

        // Separamos la logica en tiempo de compilación
        // Usamos if constexpr para distinguir entre tipos enteros y de punto flotante

        if constexpr (std::is_integral_v<T>) {
            // Para los enteros, usamos multiplicacion directa por eficiencia y para evitar casting

             for (const auto& valor : contenedor) {
                auto diff = valor - promedio; 
                acumulador = acumulador + (diff * diff);
            }
        } else {

            // Para float utilizamos libreria de precision std::pow de <cmath>, 
            for (const auto& valor : contenedor) {
                auto diff = valor - promedio;
                acumulador = acumulador + std::pow(diff, 2);
            }
        }

        std::size_t n = std::size(contenedor);
        if (n == 0) return T{};
        
        return acumulador / n;
    }

    // Algoritmo max
    // Busca el maximo elemento
    // Aqui usamos el concept que creamos "Comparable"
    template <Iterable C>
    requires Comparable<typename C::value_type>
    auto max(const C& contenedor) {
        using T = typename C::value_type;
        
        // Manejo de contenedor vacio
        if (std::empty(contenedor)) {
            return T{}; 
        }

        // Itera desde el segundo elemento
        auto it = std::begin(contenedor);
        T maximo = *it;
        ++it;

        for (; it != std::end(contenedor); ++it) {
            if (*it > maximo) {
                maximo = *it;
            }
        }
        return maximo;
    }

    // Algoritmo transform_reduce
    // Recibe una función para transformar cada elemento antes de sumar.
    // Lo utilizaremos en el test.cpp para sumar cuadrados.
    template <Iterable C, typename Func>
    requires Addable<typename C::value_type>
    auto transform_reduce(const C& contenedor, Func funcion) {
        using T = typename C::value_type;
        T resultado{};

        for (const auto& valor : contenedor) {
            resultado = resultado + funcion(valor);
        }
        return resultado;
    }


    // sum_variadic
    //Suma todos los argumentos del pack usando fold expression binaria derecha (...)
    template <typename... Args>
    requires (Addable<Args> && ...) // Todos los argumentos deben ser Addable
    auto sum_variadic(Args... args) {
        return (... + args);
    }

    // mean_variadic
    // Calcula el promedio aprovechando la función sum_variadic y sizeof... para contar argumentos.
    template <typename... Args>
    auto mean_variadic(Args... args) {
        auto suma = sum_variadic(args...);
        return suma / sizeof...(args);
    }


    // variance_variadic:

    // Para calcular la varianza se necesita recorrer los datos dos veces (una para el promedio, otra para diferencias)
    // Como no se puede iterar un parameter pack dos veces facil, se mueven los datos 
    // a un vector temporal y se reutiliza la función variance generica
    template <typename T, typename... Args>
    auto variance_variadic(T first, Args... args) {
        std::vector<T> temp_vec{first, args...};
        return variance(temp_vec);
    }

    // max_variadic
    // Maximo de N argumentos usando fold expressions

    // Usamos una funcion auxiliar para comparar
    template <typename T>
    T max_aux(T a, T b) {
        return (a > b) ? a : b;
    }

    template <typename T, typename... Args>
    requires (Comparable<T> && ... && Comparable<Args>)
    T max_variadic(T first, Args... rest) {
        T max_val = first;
        // Fold expression con operador coma para actualizar max_val
        ((max_val = max_aux(max_val, rest)), ...);
        return max_val;
    }

} // namespace core_numeric

#endif