#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <ctime>

// Parametry problemu
struct Item {
    int weight;
    int value;
};


const int MAX_CAPACITY = 50;
const std::vector<Item> ITEMS = {
    {10, 60}, {20, 100}, {30, 120}, {15, 70}, {5, 30}, 
    {12, 50}, {8, 40}, {7, 30}, {18, 80}, {25, 110}
};

// Parametry Algorytmu Ewolucyjnego
const int POPULATION_SIZE = 50;
const int GENERATIONS = 50;
const double MUTATION_RATE = 0.05;
const double CROSSOVER_RATE = 0.8;
const int TOURNAMENT_SIZE = 3;

// Generator liczb losowych
std::mt19937 rng(time(0));

struct Individual {
    std::vector<int> genes; // 0 lub 1
    int fitness;

    // Obliczanie funkcji przystosowania
    void calculateFitness() {
        int currentWeight = 0;
        int currentValue = 0;
        for (size_t i = 0; i < genes.size(); ++i) {
            if (genes[i] == 1) {
                currentWeight += ITEMS[i].weight;
                currentValue += ITEMS[i].value;
            }
        }
        // Kara za przekroczenie wagi
        if (currentWeight > MAX_CAPACITY) {
            fitness = 0; 
        } else {
            fitness = currentValue;
        }
    }
};

// Inicjalizacja populacji
std::vector<Individual> initializePopulation() {
    std::vector<Individual> population(POPULATION_SIZE);
    std::uniform_int_distribution<int> dist(0, 1);
    
    for (auto& ind : population) {
        ind.genes.resize(ITEMS.size());
        for (int i = 0; i < ITEMS.size(); ++i) {
            ind.genes[i] = dist(rng);
        }
        ind.calculateFitness();
    }
    return population;
}

// Selekcja Turniejowa
Individual tournamentSelection(const std::vector<Individual>& pop) {
    std::uniform_int_distribution<int> dist(0, POPULATION_SIZE - 1);
    Individual best = pop[dist(rng)];
    for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
        Individual contender = pop[dist(rng)];
        if (contender.fitness > best.fitness) {
            best = contender;
        }
    }
    return best;
}

// Krzyżowanie jednopunktowe
std::pair<Individual, Individual> crossover(Individual p1, Individual p2) {
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(rng) < CROSSOVER_RATE) {
        std::uniform_int_distribution<int> point(1, ITEMS.size() - 1);
        int crossoverPoint = point(rng);
        for (int i = crossoverPoint; i < ITEMS.size(); ++i) {
            std::swap(p1.genes[i], p2.genes[i]);
        }
    }
    return {p1, p2};
}

// Mutacja punktowa
void mutate(Individual& ind) {
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (size_t i = 0; i < ind.genes.size(); ++i) {
        if (prob(rng) < MUTATION_RATE) {
            ind.genes[i] = 1 - ind.genes[i]; // Flip 0->1 lub 1->0
        }
    }
}

int main() {
    auto population = initializePopulation();

    // Główna pętla algorytmu
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        std::vector<Individual> newPopulation;

        while (newPopulation.size() < POPULATION_SIZE) {
            // a. Wybór rodziców
            Individual p1 = tournamentSelection(population);
            Individual p2 = tournamentSelection(population);
            
            // b. Krzyżowanie
            auto children = crossover(p1, p2);
            
            // b. Mutacja
            mutate(children.first);
            mutate(children.second);
            
            // c. Obliczanie fitness potomków
            children.first.calculateFitness();
            children.second.calculateFitness();
            
            newPopulation.push_back(children.first);
            if (newPopulation.size() < POPULATION_SIZE) newPopulation.push_back(children.second);
        }

        population = newPopulation;

        // Zbieranie danych do wykresów (najlepszy, średni fitness)
        int maxFit = 0;
        double avgFit = 0;
        for(auto& ind : population) {
            if(ind.fitness > maxFit) maxFit = ind.fitness;
            avgFit += ind.fitness;
        }
        avgFit /= POPULATION_SIZE;
        
        if (gen == 0) std::cout << "Gen;Max;Avg\n"; // Nagłówek
        std::cout << gen << ";" << maxFit << ";" << avgFit << "\n";
    }

    // Zwróć najlepsze rozwiązanie
    auto bestInd = std::max_element(population.begin(), population.end(),
        [](const Individual& a, const Individual& b){ return a.fitness < b.fitness; });
        
    std::cout << "\nNajlepsze rozwiazanie: Wartosc=" << bestInd->fitness << "\nPrzedmioty: ";
    for(size_t i=0; i<bestInd->genes.size(); ++i) {
        if(bestInd->genes[i]) std::cout << i << " ";
    }
    
    return 0;
}