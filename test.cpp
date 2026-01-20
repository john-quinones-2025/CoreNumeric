#include <iostream>
#include <vector>
#include <string>
#include "core_numeric.h"


struct Vector3D {
    double x, y, z;

    // Constructor por defecto
    Vector3D(double _x = 0, double _y = 0, double _z = 0) : x(_x), y(_y), z(_z) {}

    // Sobrecarga operador + , para el Addable
    Vector3D operator+(const Vector3D& otro) const {
        return Vector3D(x + otro.x, y + otro.y, z + otro.z);
    }

    // Sobrecarga operador - , se utilizara para el variance
    Vector3D operator-(const Vector3D& otro) const {
        return Vector3D(x - otro.x, y - otro.y, z - otro.z);
    }

    // Sobrecarga operador * , multiplicacion componente a componente simple
    Vector3D operator*(const Vector3D& otro) const {
         return Vector3D(x * otro.x, y * otro.y, z * otro.z);
    }

    // Sobrecarga operador / , para el Divisible
    Vector3D operator/(std::size_t n) const {
        return Vector3D(x / n, y / n, z / n);
    }

    // Sobrecarga operador > , para el comparable y el max
    // Criterio: Magnitud al cuadrado
    bool operator>(const Vector3D& otro) const {
        return (x*x + y*y + z*z) > (otro.x*otro.x + otro.y*otro.y + otro.z*otro.z);
    }
    
    // Sobrecarga operador == , para sum_variadic
    bool operator==(const Vector3D& otro) const = default;

    // Impresion
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

int main() {
    

    // Casos de prueba que SI compilan

    std::cout << "Iniciando el Test------\n";

    // Verifico que los algoritmos funcionen con tipos nativos antes de probar objetos
    std::vector<double> v_double = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    auto sum_d = core_numeric::sum(v_double);
    auto mean_d = core_numeric::mean(v_double);
    auto var_d = core_numeric::variance(v_double);  // Aqui entrara al 'else' del if constexpr
    auto max_d = core_numeric::max(v_double);       // Usa mi concept Comparable

    std::cout << "[Double] Suma: " << sum_d << " | Media: " << mean_d 
              << " | Varianza: " << var_d << " | Max: " << max_d << "\n";

    // Hacemos el test con la clase Vector3D
    std::vector<Vector3D> v_vec;
    v_vec.push_back(Vector3D(1, 1, 1));
    v_vec.push_back(Vector3D(2, 2, 2));
    v_vec.push_back(Vector3D(0, 5, 0));

    auto sum_vec = core_numeric::sum(v_vec);
    auto mean_vec = core_numeric::mean(v_vec);
    auto max_vec = core_numeric::max(v_vec); // Usa el concept Comparable personalizado

    std::cout << "[Vector3D] Suma: " << sum_vec << "\n";
    std::cout << "[Vector3D] Media: " << mean_vec << "\n";
    std::cout << "[Vector3D] Max (por magnitud): " << max_vec << "\n";

    // Test con variadic y fold expressions
    auto s1 = core_numeric::sum_variadic(1, 2, 3, 4);       // Enteros
    auto s2 = core_numeric::mean_variadic(1.0, 2.0, 3.0);   // Flotantes
    auto s3 = core_numeric::max_variadic(10, 5, 20, 1);     // Max variadico

    std::cout << "[Variadic] Suma (1,2,3,4): " << s1 << "\n";
    std::cout << "[Variadic] Media (1.0, 2.0, 3.0): " << s2 << "\n";
    std::cout << "[Variadic] Max (10, 5, 20, 1): " << s3 << "\n";

    // transform_reduce test

    // Paso una lambda 'elevar al cuadrado' y luego sumar. 
    // Esto valida que el template acepte funciones como parametros
    auto tr_res = core_numeric::transform_reduce(v_double, [](double x) {
        return x * x; // Suma de cuadrados
    });
    std::cout << "[Transform] Suma de cuadrados: " << tr_res << "\n";


        /*
        
    //Primer caso:     
    std::vector<std::string> palabras = {"hola", "a todos"};
    core_numeric::sum(palabras);
    //ERROR: Esto falla correctamente porque std::string no cumple totalmente con 
    // el concept 'Addable' en el contexto numerico, y definitivamente falla en 'mean' por no ser 'Divisible' (no existe string / int).


    //Segundo caso: 
     struct Cualquier { int val; Cualquier operator+(Cualquier d){return {val+d.val};} };
     std::vector<Cualquier> cualquiers = {{1}, {2}};
     core_numeric::mean(cualquiers);
    // ERROR: Falla el concept 'Divisible'. La libreria core_numeric detecta que falta el operador '/'
    // necesario para calcular el promedio y detiene la compilación con un mensaje claro.

    // Tercer caso
     struct NoComp { int x; };
     std::vector<NoComp> nocomps = {{1}, {2}};
     core_numeric::max(nocomps);
    // ERROR: Falla mi concept 'Comparable'. Como la struct no tiene 
    // sobrecarga de operador '>', la función 'max' no puede instanciarse.
        
        */
    

    return 0;
}