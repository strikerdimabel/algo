#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <chrono>
#include <queue>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;
using namespace std::chrono;

typedef long long CompressedState; // CS stands for CompressedState
typedef vector<string> Storehouse; // SH stands for Storehouse

const size_t MAX_SIZE = 8;

const long long CS_CONTAINER_BITS = 0xF'FFFF'FFFFL;
const long long CS_CONTAINER_MASKS[8][8] = {
    {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
    {0L, 0x1L, 0x2L, 0x4L, 0x8L, 0x10L, 0x20L, 0L},
    {0L, 0x40L, 0x80L, 0x100L, 0x200L, 0x400L, 0x800L, 0L},
    {0L, 0x1000L, 0x2000L, 0x4000L, 0x8000L, 0x1'0000L, 0x2'0000L, 0L},
    {0L, 0x4'0000L, 0x8'0000L, 0x10'0000L, 0x20'0000L, 0x40'0000L, 0x80'0000L, 0L},
    {0L, 0x100'0000L, 0x200'0000L, 0x400'0000L, 0x800'0000L, 0x1000'0000L, 0x2000'0000L, 0L},
    {0L, 0x4000'0000L, 0x8000'0000L, 0x1'0000'0000L, 0x2'0000'0000L, 0x4'0000'0000L, 0x8'0000'0000L, 0L},
    {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
};

const size_t CS_PLAYER_START_BIT = 36;
const long long CS_PLAYER_BITS = 0x3F0'0000'0000L;
const long long CS_PLAYER_X_BITS = 0x70'0000'0000L;
const long long CS_PLAYER_Y_BITS = 0x380'0000'0000L;

const int movements[4][2] = {
    {0, 1},
    {1, 0},
    {0, -1},
    {-1, 0},
};
const char movement_chars[4][2] = {
    {'r', 'R'},
    {'d', 'D'},
    {'l', 'L'},
    {'u', 'U'},
};


struct Position
{
    size_t x;
    size_t y;

    Position(size_t x, size_t y): x(x), y(y)
    {
    }
};

struct PositionAndPath
{
    size_t x;
    size_t y;
    string path;

    PositionAndPath(size_t x, size_t y, string path): x(x), y(y), path(path)
    {
    }
};

struct PositionAndWeight
{
    size_t x;
    size_t y;
    size_t weight;

    PositionAndWeight(size_t x, size_t y, size_t weight): x(x), y(y), weight(weight)
    {
    }

    bool operator<(const PositionAndWeight& other) const
    {
        if (weight != other.weight)
        {
            return weight < other.weight;
        }
        if (x != other.x)
        {
            return x < other.x;
        }
        return y < other.y;
    }
};

struct PositionAndOptions
{
    size_t x;
    size_t y;
    set<PositionAndWeight> options;
    PositionAndWeight best_option;

    PositionAndOptions(
        size_t x,
        size_t y,
        set<PositionAndWeight> options,
        PositionAndWeight best_option
    ): x(x), y(y), options(options), best_option(best_option)
    {
    }

    bool operator<(const PositionAndOptions& other) const
    {
        if (best_option.weight != other.best_option.weight)
        {
            return best_option.weight < other.best_option.weight;
        }
        if (x != other.x)
        {
            return x < other.x;
        }
        return y < other.y;
    }
};

struct Stats
{
    long long total_actor_steps = 0;
    long long total_actor_container_movements = 0;
    size_t visited_states_max_memory = 0;
    size_t active_states_max_memory = 0;
    size_t active_states_left = 0;
};
Stats stats;
long long start_time_millis;

size_t n, m;
Storehouse target_sh_containers;
CompressedState target_cs_containers;
vector<Position> target_container_positions;
size_t weight_matrix[MAX_SIZE][MAX_SIZE];

void init_target_container_positions() {
    for (size_t x = 1; x < n - 1; ++x)
    {
        for(size_t y = 1; y < m - 1; ++y)
        {
            if (target_sh_containers[x][y] == '$')
            {
                target_container_positions.push_back(Position(x, y));
            }
        }
    }
}

void init_weight_matrix() {
    // Floyd–Warshall algorithm
    size_t dist[8][8][8][8];
    size_t INF_DIST = 10'000;
    memset(dist, INF_DIST, sizeof(dist));
    for (size_t x = 1; x < n - 1; ++x)
    {
        for (size_t y = 1; y < m - 1; ++y)
        {
            if (target_sh_containers[x][y] == '#')
            {
                continue;
            }
            if (target_sh_containers[x][y + 1] != '#')
            {
                dist[x][y][x][y + 1] = 1;
                dist[x][y + 1][x][y] = 1;
            }
            if (target_sh_containers[x + 1][y] != '#')
            {
                dist[x][y][x + 1][y] = 1;
                dist[x + 1][y][x][y] = 1;
            }
            dist[x][y][x][y] = 0;
        }
    }
    for (size_t x1 = 1; x1 < n - 1; ++x1)
    {
        for (size_t y1 = 1; y1 < m - 1; ++y1)
        {
            if (target_sh_containers[x1][y1] == '#')
            {
                continue;
            }
            for (size_t x2 = 1; x2 < n - 1; ++x2)
            {
                for (size_t y2 = 1; y2 < m - 1; ++y2)
                {
                    if (target_sh_containers[x2][y2] == '#')
                    {
                        continue;
                    }
                    for (size_t x3 = 1; x3 < n - 1; ++x3)
                    {
                        for (size_t y3 = 1; y3 < m - 1; ++y3)
                        {
                            if (target_sh_containers[x3][y3] == '#')
                            {
                                continue;
                            }
                            if (dist[x1][y1][x2][y2] > dist[x1][y1][x3][y3] + dist[x3][y3][x2][y2])
                            {
                                dist[x1][y1][x2][y2] = dist[x1][y1][x3][y3] + dist[x3][y3][x2][y2];
                            }
                        }
                    }
                }
            }
        }
    }

    // Weight matrix
    memset(weight_matrix, 0, sizeof(weight_matrix));
    for (size_t x = 1; x < n - 1; ++x)
    {
        for (size_t y = 1; y < m - 1; ++y)
        {
            if (target_sh_containers[x][y] == '#')
            {
                continue;
            }
            for (Position target_pos : target_container_positions)
            {
                weight_matrix[x][y] += dist[x][y][target_pos.x][target_pos.y];
            }
        }
    }
}

size_t calculate_weight(const Storehouse& sh_containers) {
    size_t total_weight = 0;
    for (size_t x = 1; x < n - 1; ++x)
    {
        for (size_t y = 1; y < m - 1; ++y)
        {
            if (sh_containers[x][y] != '$')
            {
                continue;
            }
            total_weight += weight_matrix[x][y];
        }
    }
    return total_weight;
}

struct ActiveState
{
    Storehouse sh_containers;
    size_t actor_x = 1;
    size_t actor_y = 1;
    string path;
    size_t weight = 0;

    bool operator<(const ActiveState& other_state) const
    {
        return weight < other_state.weight;
    }
};

Storehouse read()
{
    Storehouse sh;
    string t;
    while (getline(cin, t))
    {
        sh.push_back(t);
    }
    return sh;
}

ActiveState get_initial_state(const Storehouse& input_sh)
{
    ActiveState active_state;
    for (size_t x = 0; x < n; ++x)
    {
        string row = input_sh[x];
        replace(row.begin(), row.end(), '+', '@');
        replace(row.begin(), row.end(), '*', '$');
        replace(row.begin(), row.end(), '.', ' ');
        size_t actor_y = row.find('@');
        if (actor_y != string::npos)
        {
            active_state.actor_x = x;
            active_state.actor_y = actor_y;
            replace(row.begin(), row.end(), '@', ' ');
        }
        active_state.sh_containers.push_back(row);
    }
    return active_state;
}

Storehouse get_target_sh_containers(const Storehouse& input_sh)
{
    Storehouse storehouse;
    for (size_t x = 0; x < n; ++x)
    {
        string row = input_sh[x];
        replace(row.begin(), row.end(), '@', ' ');
        replace(row.begin(), row.end(), '$', ' ');
        replace(row.begin(), row.end(), '+', '$');
        replace(row.begin(), row.end(), '*', '$');
        replace(row.begin(), row.end(), '.', '$');
        storehouse.push_back(row);
    }
    return storehouse;
}

void print_sh(const Storehouse& sh)
{
    for (string row : sh)
    {
        cout << row << endl;
    }
}

void print_stats()
{
    cout << "total_actor_steps: " << stats.total_actor_steps << endl;
    cout << "total_actor_container_movements: " << stats.total_actor_container_movements << endl;
    cout << "visited_states_max_memory: " << stats.visited_states_max_memory << endl;
    cout << "stack_states_max_memory: " << stats.active_states_max_memory << endl;
    cout << "stack_states_left: " << stats.active_states_left << endl;
    cout << "time: " << (duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - start_time_millis) / 1000.0 << endl;
}

CompressedState compress_containers(const Storehouse& sh_containers)
{
    CompressedState cs = 0;
    for (size_t x = 1; x < n - 1; ++x)
    {
        const string& row = sh_containers[x];
        for(size_t y = 1; y < m - 1; ++y)
        {
            if (row[y] == '$') {
                long long box_pos = CS_CONTAINER_MASKS[x][y];
                cs |= box_pos;
            }
        }
    }
    return cs;
}

Position normalize_actor_position(const ActiveState& active_state)
{
    Position best_position(active_state.actor_x, active_state.actor_y);
    bool visited[MAX_SIZE][MAX_SIZE];
    memset(visited, 0, sizeof(visited));
    queue<Position> lookup_queue;

    size_t x = active_state.actor_x;
    size_t y = active_state.actor_y;
    lookup_queue.push(Position(x, y));
    visited[x][y] = true;

    while (lookup_queue.size() > 0)
    {
        Position position = lookup_queue.front();
        lookup_queue.pop();
        for (size_t i = 0; i < 4; ++i)
        {
            x = position.x + movements[i][0];
            y = position.y + movements[i][1];
            stats.total_actor_steps += 1;
            if (!visited[x][y] && active_state.sh_containers[x][y] == ' ')
            {
                lookup_queue.push(Position(x, y));
                visited[x][y] = true;
                if (x < best_position.x || x == best_position.x && y < best_position.y)
                {
                    best_position.x = x;
                    best_position.y = y;
                }
            }
        }
    }
    return best_position;
}

void init_working_area(Storehouse& sh_containers, size_t actor_x, size_t actor_y)
{
    bool visited[MAX_SIZE][MAX_SIZE];
    memset(visited, 0, sizeof(visited));
    queue<Position> lookup_queue;

    size_t x = actor_x;
    size_t y = actor_y;
    lookup_queue.push(Position(x, y));
    visited[x][y] = true;

    while (lookup_queue.size() > 0)
    {
        Position position = lookup_queue.front();
        lookup_queue.pop();
        for (size_t i = 0; i < 4; ++i)
        {
            x = position.x + movements[i][0];
            y = position.y + movements[i][1];
            if (!visited[x][y]
                && sh_containers[x][y] != '#'
                && x > 0 && x < n - 1 && y > 0 && y < m - 1)
            {
                lookup_queue.push(Position(x, y));
                visited[x][y] = true;
            }
        }
    }

    for (size_t x = 0; x < n; ++x)
    {
        for (size_t y = 0; y < m; ++y)
        {
            if (!visited[x][y])
            {
                sh_containers[x][y] = '#';
            }
        }
    }
}

CompressedState compress_containers_and_actor(const ActiveState& active_state)
{
    Position normalized_actor_position = normalize_actor_position(active_state);
    long long normalized_actor_cs = ((long long) (normalized_actor_position.x | (normalized_actor_position.y << 3)))
        << CS_PLAYER_START_BIT;
    CompressedState cs = compress_containers(active_state.sh_containers) | normalized_actor_cs;
    return cs;
}


string bfs(const ActiveState& initial_state)
{
    set<CompressedState> visited_states;
    multiset<ActiveState> active_states;

    CompressedState initial_cs_containers = compress_containers(initial_state.sh_containers);
    if (initial_cs_containers == target_cs_containers)
    {
        stats.active_states_left = active_states.size();
        return string();
    }
    CompressedState initial_cs = compress_containers_and_actor(initial_state);
    active_states.insert(initial_state);
    visited_states.insert(initial_cs);
    stats.active_states_max_memory = 1;
    stats.visited_states_max_memory = 1;

    while(active_states.size() > 0)
    {
        ActiveState active_state = *active_states.begin();
        active_states.erase(active_states.begin());

        bool visited[MAX_SIZE][MAX_SIZE];
        memset(visited, 0, sizeof(visited));
        queue<PositionAndPath> lookup_queue;

        size_t x = active_state.actor_x;
        size_t y = active_state.actor_y;
        lookup_queue.push(PositionAndPath(x, y, string()));
        visited[x][y] = true;

        while (lookup_queue.size() > 0)
        {
            PositionAndPath positionAndPath = lookup_queue.front();
            lookup_queue.pop();
            for (size_t i = 0; i < 4; ++i)
            {
                x = positionAndPath.x + movements[i][0];
                y = positionAndPath.y + movements[i][1];
                stats.total_actor_steps += 1;
                if (!visited[x][y] && active_state.sh_containers[x][y] == ' ')
                {
                    string newPath = positionAndPath.path + movement_chars[i][0];
                    lookup_queue.push(PositionAndPath(x, y, newPath));
                    visited[x][y] = true;
                } else if (!visited[x][y]
                    && active_state.sh_containers[x][y] == '$'
                    && active_state.sh_containers[x + movements[i][0]][y + movements[i][1]] == ' '
                )
                {
                    string newPath = positionAndPath.path + movement_chars[i][1];
                    ActiveState new_active_state(active_state);
                    new_active_state.path += newPath;
                    new_active_state.actor_x = x;
                    new_active_state.actor_y = y;
                    new_active_state.sh_containers[x][y] = ' ';
                    new_active_state.sh_containers[x + movements[i][0]][y + movements[i][1]] = '$';
                    CompressedState cs = compress_containers_and_actor(new_active_state);
                    stats.total_actor_container_movements += 1;
                    if (visited_states.find(cs) == visited_states.end())
                    {
                        CompressedState cs_containers = cs & CS_CONTAINER_BITS;
                        if (cs_containers == target_cs_containers)
                        {
                            stats.active_states_left = active_states.size();
                            return new_active_state.path;
                        }
                        new_active_state.weight = calculate_weight(new_active_state.sh_containers);
                        active_states.insert(new_active_state);
                        visited_states.insert(cs);
                        stats.active_states_max_memory = max(stats.active_states_max_memory, active_states.size());
                        stats.visited_states_max_memory += 1;
                        stats.active_states_left = active_states.size();
                        if (stats.visited_states_max_memory % 10000 == 0)
                        {
                            print_stats();
                            cout << endl;
                        }
                    }
                }
            }
        }
    }
    return string("Not found");
}

string internal_run(const Storehouse& input)
{
    start_time_millis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    n = input.size();
    m = input[0].size();
    ActiveState initial_state = get_initial_state(input);
    target_sh_containers = get_target_sh_containers(input);
    init_working_area(initial_state.sh_containers, initial_state.actor_x, initial_state.actor_y);
    init_working_area(target_sh_containers, initial_state.actor_x, initial_state.actor_y);
    init_target_container_positions();
    init_weight_matrix();
    target_cs_containers = compress_containers(target_sh_containers);
    string result_path = bfs(initial_state);
    return result_path;
}

bool check_result(const Storehouse& input, const string& result)
{
    ActiveState state = get_initial_state(input);
    target_sh_containers = get_target_sh_containers(input);
    target_cs_containers = compress_containers(target_sh_containers);
    size_t x = state.actor_x;
    size_t y = state.actor_y;
    Storehouse sh_containers = state.sh_containers;
    if (result.size() > 10000)
    {
        cout << "ERROR: path longer than 10000" << endl;
    }
    map<char, size_t> lower_movements = {
        {'r', 0},
        {'d', 1},
        {'l', 2},
        {'u', 3},
    };
    map<char, size_t> upper_movements = {
        {'R', 0},
        {'D', 1},
        {'L', 2},
        {'U', 3},
    };

    for (char step : result)
    {
        if (lower_movements.find(step) != lower_movements.end())
        {
            size_t movement = lower_movements[step];
            x += movements[movement][0];
            y += movements[movement][1];
            if (sh_containers[x][y] == '#' || sh_containers[x][y] == '$')
            {
                cout << "ERROR: illegal movement: " << step << endl;
                return false;
            }
            if (x <= 0 || x >= n - 1 || y <= 0 || y >= m - 1)
            {
                cout << "ERROR: index out of bound: (" << x << ", " << y << ")" << endl;
                return false;
            }
        }
        if (upper_movements.find(step) != upper_movements.end())
        {
            size_t movement = upper_movements[step];
            x += movements[movement][0];
            y += movements[movement][1];
            size_t next_x = x + movements[movement][0];
            size_t next_y = y + movements[movement][1];
            if (sh_containers[next_x][next_y] == '#'
                || sh_containers[next_x][next_y] == '$'
                || sh_containers[x][y] != '$')
            {
                cout << "ERROR: illegal container movement: " << step << endl;
                return false;
            }
            if (next_x <= 0 || next_x >= n - 1 || next_y <= 0 || next_y >= m - 1)
            {
                cout << "ERROR: container index out of bound: (" << next_x << ", " << next_y << ")" << endl;
                return false;
            }
            sh_containers[next_x][next_y] = '$';
            sh_containers[x][y] = ' ';
        }
    }
    CompressedState cs_containers = compress_containers(sh_containers);
    if (cs_containers != target_cs_containers)
    {
        cout << "ERROR: wrong result:" << endl;
        print_sh(sh_containers);
        return false;
    }
    return true;
}

void run_tests()
{
    vector<Storehouse> test_inputs = {
        {
            "###",
            "#@#",
            "###",
        },
        {
            "###",
            "###",
            "###",
        },
        {
            "###",
            "#@#",
            "#*#",
            "###",
        },
        {
            "#####",
            "#@$.#",
            "#####",
        },
        {
            "#######",
            "#@$.#*#",
            "#######",
        },
        {
            "#####",
            "#@$.*",
            "#####",
        },
        {
            "########",
            "#@  $ .#",
            "########",
        },
        {
            " ######",
            "##   .#",
            "#@  ###",
            "#   * #",
            "#   $ #",
            "#     #",
            "#######",
        },
        {
            "#######",
            "#@$  .#",
            "#$ $  #",
            "#. .  #",
            "#######",
        },
        {
            " ######",
            "##  . #",
            "# * # #",
            "# .$  #",
            "#  #$##",
            "## @ # ",
            " ##### ",
        },
        {
            "########",
            "#...   #",
            "#      #",
            "#***   #",
            "#      #",
            "#$$$   #",
            "#     @#",
            "########",
        },
        // {
        //     "########",
        //     "#......#",
        //     "#      #",
        //     "#***** #",
        //     "#      #",
        //     "#$$$$$$#",
        //     "#     @#",
        //     "########",
        // },
        // {
        //     // result: ruurrdLrdLruurrddLUrddDuululldRdRluurrdDllDLruulDlDurrrDluuurDDlluuullDDDDrRlluuRdrruulDDDuuuulDldRurDDrrruuuLLLL
        //     "########",
        //     "#.     #",
        //     "#$ $ $ #",
        //     "#@ $ $ #",
        //     "#  $ $ #",
        //     "#  ****#",
        //     "#......#",
        //     "########",
        // },
        // {
        //     "########",
        //     "#..... #",
        //     "#$$$$$@#",
        //     "# .....#",
        //     "#$$$$$$#",
        //     "#    $.#",
        //     "#. $.$.#",
        //     "########",
        // }
        // {
        //     "########",
        //     "##.* $.#",
        //     "#  * . #",
        //     "#   *. #",
        //     "## $.$ #",
        //     "# $*$$$#",
        //     "#..   @#",
        //     "########",
        // },       
        // {
        //     "########",
        //     "#..... #",
        //     "#  ##  #",
        //     "# *    #",
        //     "#  # # #",
        //     "#$$$$$$#",
        //     "#     +#",
        //     "########",        
        // },
        // {
        //     "########",
        //     "#@ $.$.#",
        //     "#  $.$.#",
        //     "#$ $.$.#",
        //     "#. $.$.#",
        //     "#$ $.$.#",
        //     "#.   $.#",
        //     "########",
        // },
        // {
        //     "########",
        //     "#.     #",
        //     "#$ $ $ #",
        //     "#  $ $ #",
        //     "#@ $ $ #",
        //     "###*** #",
        //     "#......#",
        //     "########",
        // }
    };
    size_t success = 0;
    size_t fail = 0;
    for (Storehouse input : test_inputs)
    {
        stats = Stats();
        string result = internal_run(input);
        print_sh(input);
        print_stats();
        cout << "result: " << result << endl;
        if (check_result(input, result))
        {
            success += 1;
        }
        else
        {
            fail += 1;
        }
        cout << endl;
    }
    cout << "success: " << success << endl;
    cout << "fail: " << fail << endl;
}

void run_solution()
{
    Storehouse input = read();
    string result_path = internal_run(input);
    cout << result_path << endl;
}

int main()
{
    run_tests();
    return 0;
}

/*122992QB*/
