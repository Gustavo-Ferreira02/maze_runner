#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>

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
std::mutex mtx; // Mutex para proteger o acesso à pilha de posições válidas

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
    std::ifstream inputFile(file_name);
    if (!inputFile) {
        std::cerr << "Erro: Arquivo não foi encontrado!" << std::endl;
        return {-1, -1};
    }

    inputFile >> num_rows >> num_cols;
    if (!num_rows || !num_cols) {
        std::cerr << "Erro: Leitura das linhas e colunas não foi feita!" << std::endl;
    }

    maze.resize(num_rows, std::vector<char>(num_cols));
    Position start_pos = {-1, -1};

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

    inputFile.close();
    return {start_pos};
}

// Função para imprimir o labirinto
void print_maze() {
    for (int a = 0; a < num_rows; a++) {
        for (int b = 0; b < num_cols; b++) {
            std::cout << maze[a][b];
        }
        std::cout << std::endl;
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col) {
    return (row >= 0 && row < num_rows && col >= 0 && col < num_cols && (maze[row][col] == 'x' || maze[row][col] == 's'));
}

// Função auxiliar para explorar o labirinto
void explore(Position pos) {
    if (maze[pos.row][pos.col] == 's') {
        std::cout << "Saída encontrada na posição: (" << pos.row << ", " << pos.col << ")" << std::endl;
        return; // Saída encontrada
    }

    maze[pos.row][pos.col] = '.'; // Marque a posição como visitada
    print_maze();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Verifique as posições adjacentes
    Position moves[] = {
        {pos.row - 1, pos.col},  // Cima
        {pos.row + 1, pos.col},  // Baixo
        {pos.row, pos.col - 1},  // Esquerda
        {pos.row, pos.col + 1}   // Direita
    };

    std::vector<Position> next_positions;

    // Adiciona posições válidas à lista
    for (const auto& move : moves) {
        if (is_valid_position(move.row, move.col)) {
            next_positions.push_back(move);
        }
    }

    // Se houver múltiplos caminhos, cria novas threads para explorá-los
    if (next_positions.size() > 1) {
        std::vector<std::thread> threads;
        for (size_t i = 1; i < next_positions.size(); ++i) {
            threads.emplace_back(explore, next_positions[i]); // Cria uma nova thread para cada caminho
        }

        explore(next_positions[0]); // Explora o primeiro caminho na thread atual

        // Junte todas as threads
        for (auto& t : threads) {
            t.join();
        }
    } else if (next_positions.size() == 1) {
        explore(next_positions[0]); // Apenas um caminho, continue a exploração
    }
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

    explore(initial_pos); // Começa a exploração do labirinto

    return 0;
}
