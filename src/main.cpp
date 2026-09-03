#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>   
#include "ast.h"
#include "parser.h"

using namespace std;

string leerArchivo(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) throw runtime_error("No se pudo abrir: " + ruta);
    stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    string rutaScript = (argc > 1) ? argv[1] : "ejemplos/script_completo.txt";
    
    try {
        cout << "Iniciando Interprete DSL...\n";
        cout << "Cargando script: " << rutaScript << "\n";
        
        string codigoFuente = leerArchivo(rutaScript);

        Parser parser(codigoFuente);
        unique_ptr<ProgramNode> ast = parser.parseProgram();
        cout << "[OK] Analisis lexico y sintactico finalizado. AST construido.\n\n";

        cout << "--- SALIDA DE EJECUCION ---\n";
        SymbolTable tablaSimbolos;
        ast->execute(tablaSimbolos);
        cout << "---------------------------\n";
        
    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << '\n';
        return 1;
    }
    return 0;
}