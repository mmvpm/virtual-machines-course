#include <iostream>
#include <vector>
#include <numeric>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// debug logs
const bool VERBOSE = false;
#define log if (VERBOSE) cout

// constants
static const size_t MAX_MEMORY = 1 << 30; // 1 Gb
static const int MAX_ASSOCIATIVITY = 16;
static const int MAX_WAY_SIZE = MAX_MEMORY / MAX_ASSOCIATIVITY;
static const double DIFF_THRESHOLD = 0.2;

static const size_t ITERATIONS = 50'000'000;
static const size_t BUFFER_MOD = 128;

// memory buffer
static const int PAGE_SIZE = 16384;
alignas(PAGE_SIZE) vector<uint8_t> buffer;

string prettify_bytes(size_t bytes) {
    const char *units[] = {"bytes", "Kb", "Mb", "Gb"};
    int i = 0;
    for (; bytes >= 1024; i++, bytes /= 1024) {}
    return to_string(bytes) + " " + units[i];
}

vector<size_t> generate_permutation(size_t n) {
    vector<size_t> permutation(n);
    iota(permutation.begin(), permutation.end(), 0);

    random_device rd;
    mt19937 g(rd());
    shuffle(permutation.begin(), permutation.end(), g);

    size_t last_index = 0;
    for (size_t i = 0; i < n - 1; ++i) {
        swap(permutation[last_index], permutation[(last_index + 1) % n]);
        last_index = (last_index + 1) % n;
    }

    return permutation;
}

uint8_t **prepare_pointer(size_t stride, size_t spots) {
    vector<size_t> path = generate_permutation(spots);

    uint8_t **pointer;
    for (size_t i = 0; i < spots; ++i) {
        size_t index = path[i];
        pointer = (uint8_t **) (&buffer[index * stride]);
        size_t next_index = path[(i + 1) % spots];
        *pointer = &buffer[next_index * stride];
    }

    return pointer;
}

int64_t run_experiment(size_t stride, size_t spots) {
    uint8_t **pointer = prepare_pointer(stride, spots);

    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        pointer = (uint8_t **) (*pointer);
    }
    int64_t elapsed_ns = (chrono::high_resolution_clock::now() - start).count();

    return elapsed_ns;
}

int main() {
    // init
    buffer = vector<uint8_t>(MAX_MEMORY);
    for (int i = 0; i < MAX_MEMORY; ++i) buffer[i] = i % BUFFER_MOD;

    // determine cache size & associativity
    vector<pair<size_t, set<int>>> jumps_log; // { way_size, jumps }
    for (size_t way_size = 128; way_size < MAX_WAY_SIZE; way_size <<= 1) {
        log << "Times for way size " << prettify_bytes(way_size) << ":" << endl;
        set<int> jumps;
        int64_t previous_ns = -1;
        for (int associativity = 1; associativity <= MAX_ASSOCIATIVITY; ++associativity) {
            int64_t current_ns = run_experiment(way_size, associativity);
            double diff = (double) (current_ns - previous_ns) / (double) current_ns;
            log << "associativity = " << associativity << ", time = " << current_ns << " ns, diff = " << diff * 100
                << "%" << endl;
            if (previous_ns != -1 && diff > DIFF_THRESHOLD) {
                jumps.insert(associativity - 1);
            }
            previous_ns = current_ns;
        }
        jumps_log.emplace_back(way_size, jumps);
        log << endl;
    }

    log << "Jumps for way: " << endl;
    for (auto &[way_size, jumps]: jumps_log) {
        log << "  - " << prettify_bytes(way_size) << ": ";
        for (auto e: jumps) log << e << " ";
        log << endl;
    }
    log << endl;

    vector<pair<size_t, int>> possible_results; // { cache_size, associativity }
    set<int> max_way_jumps = jumps_log[jumps_log.size() - 1].second;
    for (auto &[way_size, jumps]: jumps_log) {
        vector<int> tmp;
        for (auto associativity: max_way_jumps) {
            if (jumps.count(associativity) == 1) {
                tmp.push_back(associativity);
                possible_results.emplace_back(way_size * associativity, associativity);
            }
        }
        for (int associativity: tmp) {
            max_way_jumps.erase(associativity);
        }
    }
    log << "Possible cache params (cache_size, associativity):" << endl;
    for (auto &[cache_size, associativity]: possible_results) {
        log << "  - (" << prettify_bytes(cache_size) << ", " << associativity << ")" << endl;
    }
    log << endl;

    auto &[result_cache_size, result_associativity] = *min_element(possible_results.begin(), possible_results.end());

    // determine cache line size
    set<int> previous_jumps;
    jumps_log.clear();
    int64_t previous_ns = -1;
    size_t result_cache_line_size = -1;
    for (size_t cache_line_size = 16; cache_line_size <= 1024; cache_line_size <<= 1) {
        log << "Times for cache line " << prettify_bytes(cache_line_size) << ":" << endl;
        set<int> jumps;
        for (int spots = 4; spots <= 512; spots <<= 1) {
            int64_t current_ns = run_experiment(result_cache_size + cache_line_size, spots);
            double diff = (double) (current_ns - previous_ns) / (double) current_ns;
            log << "spots = " << spots << ", time = " << current_ns << " ns, diff = " << diff * 100 << "%" << endl;
            if (previous_ns != -1 && diff > DIFF_THRESHOLD) {
                jumps.insert(spots);
            }
            previous_ns = current_ns;
        }
        jumps_log.emplace_back(cache_line_size, jumps);
        log << endl;

        if (!previous_jumps.empty() && jumps != previous_jumps && previous_jumps.size() <= jumps.size()) {
            result_cache_line_size = cache_line_size >> 1; // previous one
            break;
        }
        previous_jumps = jumps;
    }

    log << "Jumps for cache line" << endl;
    for (auto &[cache_line_size, jumps]: jumps_log) {
        log << "  - " << prettify_bytes(cache_line_size) << ": ";
        for (auto e: jumps) log << e << " ";
        log << endl;
    }
    log << endl;

    // results
    cout << "L1 cache: " << prettify_bytes(result_cache_size) << ", "
         << result_associativity << "-ways, "
         << prettify_bytes(result_cache_line_size) << " line" << endl;
}
