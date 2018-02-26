#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>
#include <thread>
#define MAXSHIPS 10
#define ITERATIONS 20

using namespace std;

void *travel(void *ship);
int count = 0;

struct Ship{
    int id;
    // Max fuel the ship can carry
    int maxn;
    int maxq;
    // Current amount of fuel
    int n;
    int q;

};

struct Supply_ship{
    //Fuel to deliver to the station
    int nit_supply;
    int quant_supply;
    // Amount of fuel needed to leave the station
    int n;
    int q;
};

class SpaceStation{
public:
    int nitrogen;
    int quantum;
    int max_fuel;
    //int vehicles;
    sem_t available_spaces;
    pthread_cond_t nitrogen_cond;
    pthread_cond_t quantum_cond;
    pthread_mutex_t nitrogen_lock;
    pthread_mutex_t quantum_lock;

    SpaceStation(){
        nitrogen = 70;
        quantum = 70;
        max_fuel = 100;
        //vehicles = 5;
        sem_init(&available_spaces, 1, 5);
        pthread_cond_init(&quantum_cond, NULL);
        pthread_cond_init(&nitrogen_cond, NULL);
        pthread_mutex_init(&nitrogen_lock, NULL);
        pthread_mutex_init(&quantum_lock, NULL);
    }

    SpaceStation(int n, int q, int max, int v){
        nitrogen = n;
        quantum = q;
        max_fuel = max;
        //vehicles = v;
        sem_init(&available_spaces, 1, v);
        pthread_cond_init(&quantum_cond, NULL);
        pthread_cond_init(&nitrogen_cond, NULL);
        pthread_mutex_init(&nitrogen_lock, NULL);
        pthread_mutex_init(&quantum_lock, NULL);
    }

    void refuel(Ship &ship){
        //bool done = false;
        int needed;

        sem_wait(&available_spaces);
        if(ship.maxn > 0){
            pthread_mutex_lock(&nitrogen_lock);
            needed = ship.maxn - ship.n;
            if(needed > nitrogen){
                pthread_cond_wait(&nitrogen_cond, &nitrogen_lock);
            }
            nitrogen -= needed;
            ship.n += needed;
            cout << "Ship " << ship.id << "refueled nitrogen." << endl;
            pthread_mutex_unlock(&nitrogen_lock);
        }
        if(ship.maxq > 0){
            pthread_mutex_lock(&quantum_lock);
            needed = ship.maxq - ship.q;
            if(needed > quantum){
                pthread_cond_wait(&quantum_cond, &quantum_lock);
            }
            quantum -= needed;
            ship.q += needed;
            cout << "Ship " << ship.id << "refueled quantum fluid." << endl;
            pthread_mutex_unlock(&quantum_lock);
        }
        sem_post(&available_spaces);
    }

    void deposit(Supply_ship &supply){

        sem_wait(&available_spaces);
        if((max_fuel - nitrogen) >= supply.nit_supply){
            pthread_mutex_lock(&nitrogen_lock);
                if((max_fuel - nitrogen) >= supply.nit_supply){
                    nitrogen += supply.nit_supply - supply.n;
                    supply.nit_supply = 0;
                    pthread_cond_broadcast(&nitrogen_cond);
                }
            pthread_mutex_unlock(&nitrogen_lock);
        }
        if((max_fuel - quantum) >= supply.quant_supply){
            pthread_mutex_lock(&quantum_lock);
                if((max_fuel - quantum) >= supply.quant_supply){
                    quantum += supply.quant_supply - supply.q;
                    supply.quant_supply = 0;
                    pthread_cond_broadcast(&quantum_cond);
                }
            pthread_mutex_unlock(&quantum_lock);
        }
        sem_post(&available_spaces);
    }
};

int main(){

    int i;

    SpaceStation station(50, 50, 100, 4);

    pthread_t ships[MAXSHIPS];

    srand(time(NULL));
    /*
    //Fuel to deliver to the station
    int nit_supply;
    int quant_supply;
    // Amount of fuel needed to leave the station
    int n;
    int q;
    */
    Supply_ship supply;
    supply.

    for(i = 1; i < MAXSHIPS; i++){
        Ship ship;
        ship.id = i;
        ship.maxn = 20;
        ship.maxq = 20;
        ship.n = rand() % 20;
        ship.q = rand() % 20;
        pthread_create(&ships[i], NULL, travel, (void *) ship);
    }
    cout << "nnit: " << station.nitrogen  << endl;

    return 0;
}

void *travel(void *ship){
    while(count <= ITERATIONS){
        this_thread::sleep_for(rand()%10);

    }
}
