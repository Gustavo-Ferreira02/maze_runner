#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <string>

// Representação do labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    // 1. Abra o arquivo especificado por file_name usando std::ifstream
    std::ifstream inputFile(file_name);
    if (!inputFile) {
        std::cerr << "Erro: Arquivo não foi encontrado!" << std::endl;
        return {-1, -1};
    }

    // 2. Leia o número de linhas e colunas do labirinto
    inputFile >> num_rows >> num_cols;
    if (!num_rows || !num_cols) {
        std::cerr << "Erro: Leitura das linhas e colunas não foi feita!" << std::endl;
    }

    // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    maze.resize(num_rows, std::vector<char>(num_cols));

    // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    Position start_pos = {-1, -1}; // -1 serve como posição de erro

    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            inputFile >> maze[i][j];
            if (maze[i][j] == 'e') {
                start_pos = {i, j};
            }
        }
    }

    if (start_pos.row == -1 || start_pos.col == -1) {
        std::cerr << "Erro: Posição inicial 'e' inexistente!" << std::endl;
        return {-1, -1}; 
    }

    // 7. Feche o arquivo após a leitura
    inputFile.close();
    return start_pos; // Retorna a posição inicial correta
}

// Função para imprimir o labirinto
void print_maze() {
    // 1. Percorra a matriz 'maze' usando um loop aninhado
    // 2. Imprima cada caractere usando std::cout
    // 3. Adicione uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n';
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && (maze[row][col] == 'x' || maze[row][col] == 's'));
}

// Função principal para navegar pelo labirinto
bool walk(Position pos) {
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    if (maze[pos.row][pos.col] == 's') {
        return true;
    }

    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    maze[pos.row][pos.col] = '.'; 

    // 2. Chame print_maze() para mostrar o estado atual do labirinto]
    print_maze(); 

    // 3. Adicione um pequeno atraso para visualização:
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Atraso para visualização

    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    Position moves[] = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };

    // Adicione posições válidas à pilha
    for (const auto& move : moves) {
        if (is_valid_position(move.row, move.col)) {
            valid_positions.push(move);
        }
    }

    // Explorar posições válidas
    while (!valid_positions.empty()) {
        Position next_pos = valid_positions.top();
        valid_positions.pop();
        if (walk(next_pos)) { // Chama walk recursivamente
            return true;
        }
    }
    return false; // Se não encontrar saída
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    bool exit_found = walk(initial_pos);
    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}
