#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <map>

const int N = 4; // Board size

struct State {
    std::vector<std::vector<int>> board;
    int cost;
    int heuristic;
    int moves;
    State* parent;

    State(std::vector<std::vector<int>> b, int c, int h, int m, State* p) {
        board = b;
        cost = c;
        heuristic = h;
        moves = m;
        parent = p;
    }

    bool operator<(const State& other) const {
        return cost + heuristic > other.cost + other.heuristic;
    }
};

// Calculate the Manhattan distance heuristic
int calculateHeuristic(const std::vector<std::vector<int>>& board) {
    int heuristic = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int num = board[i][j];
            if (num != 0) {
                int num_i = (num - 1) / N;
                int num_j = (num - 1) % N;
                heuristic += abs(i - num_i) + abs(j - num_j);
            }
        }
    }
    return heuristic;
}

// Check if the board is solvable
bool isSolvable(const std::vector<std::vector<int>>& board) {
    int inversions = 0;
    std::vector<int> flattened;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            flattened.push_back(board[i][j]);
        }
    }
    for (int i = 0; i < N * N - 1; i++) {
        for (int j = i + 1; j < N * N; j++) {
            if (flattened[j] && flattened[i] && flattened[i] > flattened[j]) {
                inversions++;
            }
        }
    }

    int blankRow = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 0) {
                blankRow = i;
                break;
            }
        }
    }

    if (N % 2 == 1) {
        return inversions % 2 == 0;
    } else {
        return (inversions + blankRow) % 2 == 1;
    }
}

// Generate the goal state
std::vector<std::vector<int>> generateGoalState() {
    int num = 1;
    std::vector<std::vector<int>> goal(N, std::vector<int>(N, 0));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            goal[i][j] = num++;
        }
    }
    goal[N - 1][N - 1] = 0;
    return goal;
}

// Print the board state
void printBoard(const std::vector<std::vector<int>>& board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Check if the board state is the goal state
bool isGoalState(const std::vector<std::vector<int>>& board) {
    std::vector<std::vector<int>> goal = generateGoalState();
    return board == goal;
}

// Generate next possible moves from the current state
std::vector<std::vector<std::vector<int>>> generateNextMoves(const std::vector<std::vector<int>>& board) {
    std::vector<std::vector<std::vector<int>>> nextMoves;
    int xBlank, yBlank;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == 0) {
                xBlank = i;
                yBlank = j;
                break;
            }
        }
    }

    // Generate next moves by swapping the blank space with adjacent tiles
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (const auto& dir : directions) {
        int x = xBlank + dir.first;
        int y = yBlank + dir.second;
        if (x >= 0 && x < N && y >= 0 && y < N) {
            std::vector<std::vector<int>> newBoard = board;
            std::swap(newBoard[xBlank][yBlank], newBoard[x][y]);
            nextMoves.push_back(newBoard);
        }
    }

    return nextMoves;
}

// Solve the puzzle using A* algorithm
void solvePuzzle(std::vector<std::vector<int>>& initialBoard) {
    int heuristic = calculateHeuristic(initialBoard);
    State initialState = State(initialBoard, 0, heuristic, 0, nullptr);

    std::priority_queue<State> openList;
    openList.push(initialState);

    std::map<std::vector<std::vector<int>>,bool> visited;
    visited[initialBoard] = false;

    while (!openList.empty()) {
        State currentState = openList.top();
        openList.pop();
        visited[currentState.board] = true;

        if (isGoalState(currentState.board)) {
            // Generate the path by backtracking through the parents
            std::vector<State*> path;
            State* current = &currentState;
            while (current != nullptr) {
                path.push_back(current);
                current = current->parent;
            }

            // Print the step-by-step solution
            int step = 0;
            for (int i = path.size() - 1; i >= 0; i--) {
                std::cout << "Step " << step++ << ":" << std::endl;
                printBoard(path[i]->board);
            }
            std::cout << "Solver finished." << std::endl;
            return;
        }

        std::vector<std::vector<std::vector<int>>> nextMoves = generateNextMoves(currentState.board);
        for (const auto& move : nextMoves) {
            bool isVisited = visited.find(move) != visited.end();
            if (isVisited && visited[move]) continue;
            int newCost = currentState.cost + 1;

            if (!isVisited) {
                visited[move] = false;
                int newHeuristic = calculateHeuristic(move);
                State *parent = new State(currentState);
                State nextState = State(move, newCost, newHeuristic, currentState.moves + 1, parent);
                openList.push(nextState);
            }
        }
    }

    std::cout << "No solution found." << std::endl;
}

int main() {
    // Initialize the initial state of the puzzle
    std::vector<std::vector<int>> initialBoard(N, std::vector<int>(N));
    initialBoard = {{0, 1, 2, 3},
                    {6, 7, 8, 4},
                    {5, 9, 10, 11},
                    {13, 14, 15, 12}};

    // Check if the puzzle is solvable
    if (isSolvable(initialBoard)) {
        std::cout << "Solving the puzzle..." << std::endl;
        solvePuzzle(initialBoard);
    } else {
        std::cout << "The given puzzle is unsolvable." << std::endl;
    }

    return 0;
}
