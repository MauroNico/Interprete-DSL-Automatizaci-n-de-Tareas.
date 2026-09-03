#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>   
#include "ast.h"
#include "parser.h"

std::string leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) throw std::runtime_error("No se pudo abrir: " + ruta);
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string rutaScript = (argc > 1) ? argv[1] : "ejemplos/script_completo.txt";
    
    try {
        std::cout << "Iniciando Interprete DSL...\n";
        std::cout << "Cargando script: " << rutaScript << "\n";
        
        std::string codigoFuente = leerArchivo(rutaScript);

        Parser parser(codigoFuente);
        std::unique_ptr<ProgramNode> ast = parser.parseProgram();
        std::cout << "[OK] Analisis lexico y sintactico finalizado. AST construido.\n\n";

        std::cout << "--- SALIDA DE EJECUCION ---\n";
        SymbolTable tablaSimbolos;
        ast->execute(tablaSimbolos);
        std::cout << "---------------------------\n";
        std::cout << "\n[OK] Ejecucion finalizada sin errores.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << '\n';
        return 1;
    }
    return 0;
}