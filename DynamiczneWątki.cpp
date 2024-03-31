#include <queue> 
#include <chrono> 
#include <thread> 
#include <windows.h> 
#include <iostream> 
#include "mutex" 
#include <list> 
#include <vector>
int iloscOdwiedziających;
int wielkoscWagonika;
int wwagonie = 0;
int stanWagona = 1;
int czasprze;
bool allDone = false;
int tmp1 = 0;
class Pasazer {
public:
    int state;
    int id;
    Pasazer() {}
    Pasazer(int st, int i) :state(st), id(i) {}
};
std::queue<Pasazer> Kolejka;
std::vector<Pasazer> pasazerowie;
std::list<Pasazer*> Wagonik;
std::vector<std::chrono::milliseconds> czos;
std::vector<std::chrono::high_resolution_clock::time_point> start;
std::vector<std::chrono::high_resolution_clock::time_point> stop;
std::mutex DDW;
void Wagon()
{
    while (1)
    {
        if (allDone)
        {
            break;
        }
        if (stanWagona == 0 && wwagonie == wielkoscWagonika)
        {
            Wagonik.clear();
        }
        if (stanWagona == 0 && wwagonie > 0)
        {
            stanWagona = 1;
        }
        else if (wwagonie == 0 && stanWagona == 0)
        {
            stanWagona = 1;

        }
        else if (wwagonie == wielkoscWagonika && stanWagona == 1)
        {
            stanWagona = 0;

        }
        else if (wwagonie == wielkoscWagonika && stanWagona == 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            stanWagona = 1;
        }
    }
}
void PasazerD(int ID)
{
    pasazerowie[ID].state = 1;
    start[ID] = std::chrono::high_resolution_clock::now();
    while (1)
    {
        if (wwagonie < wielkoscWagonika && Kolejka.front().id == ID && stanWagona == 1)
        {
            std::lock_guard<std::mutex> lock(DDW);
            pasazerowie[ID].state = 2;
            Wagonik.push_back(&pasazerowie[ID]);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            wwagonie++;
            Kolejka.pop();
            break;
        }
        else;
    }
    stop[ID] = std::chrono::high_resolution_clock::now();

    while (1)//
    {
        if (stanWagona == 0 && wwagonie == wielkoscWagonika)
        {
            break;
        }
    }
    czos[ID] = std::chrono::duration_cast<std::chrono::milliseconds>(stop[ID] - start[ID]);
    while (1)
    {
        if (stanWagona == 1 && wwagonie > 0)
        {
            pasazerowie[ID].state = 3;
            std::this_thread::sleep_for(std::chrono::seconds(czasprze));
            wwagonie--;
            break;
        }
    }
    pasazerowie[ID].state = 4;

}
void clearConsole() {
    system("cls");
}
void animacja()
{
    for (int i = 0; i < iloscOdwiedziających; i++)
    {
        if (pasazerowie[i].state == 3)
        {
            break;
        }
        if (pasazerowie[iloscOdwiedziających - 1].state != 3)
        {
            return;
        }
    }
    for (int i = 0; i < tmp1; i++)
    {
        std::cout << " ";
    }
}
void printThreadStates()
{
    int ww = 0, kol = 0;
    for (int i = 0; i < iloscOdwiedziających; i++) {
        if (pasazerowie[i].state == 1)
        {
            kol++;
        }
        else if (pasazerowie[i].state == 2 || pasazerowie[i].state == 3)
        {
            ww++;
        }
    }
    std::cout << "Pasażerowie w kolejce: " << kol << "\n" << "Pasażerowie w wagonie: " << ww << std::endl;
    if (tmp1 < 120 - (wielkoscWagonika * 2 + 1))
    {
        animacja();
        for (int i = 0; i < wielkoscWagonika; i++)
        {
            std::cout << "##";
        }
        std::cout << "#" << std::endl;
        animacja();
        for (int i = 0; i < wielkoscWagonika; i++)
        {
            if (ww > 0)
            {
                std::cout << "#P";
                ww--;
            }
            else
            {
                std::cout << "# ";
            }
        }
        std::cout << "#" << std::endl;
        animacja();
        for (int i = 0; i < wielkoscWagonika; i++)
        {
            std::cout << "##";
        }
        std::cout << "#" << std::endl;
        tmp1++;
    }
    else
    {
        std::cout << "\n" << "\n" << std::endl;
        tmp1 = 0;
    }
    for (int i = 0; i < kol; i++)
    {
        std::cout << "   O" << std::endl;
    }
}
int main() {
    SetConsoleOutputCP(1250);
    std::cout << "Wpisz ilosc pasazerów:" << std::endl;
    std::cin >> iloscOdwiedziających;
    std::cout << "Wpisz pojemność wagonika:" << std::endl;
    std::cin >> wielkoscWagonika;
    std::cout << "Wpisz ile sekund ma trwać przejażdżka:" << std::endl;
    std::cin >> czasprze;
    std::vector<std::thread> threads;
    std::thread wagon;
    wagon = std::thread(Wagon);
    for (int i = 0; i < iloscOdwiedziających; i++)
    {
        czos.push_back(std::chrono::milliseconds(0));
        start.push_back(std::chrono::high_resolution_clock::now());
        stop.push_back(std::chrono::high_resolution_clock::now());
    }
    for (int i = 0; i < iloscOdwiedziających; ++i)
    {
        Pasazer pasazer(0, i);
        pasazerowie.push_back(pasazer);
        Kolejka.push(pasazerowie[i]);
    }

    for (int i = 0; i < iloscOdwiedziających; ++i) {
        threads.push_back(std::thread(PasazerD, i));
    }
    while (true) {
        clearConsole();
        printThreadStates();
        Sleep(100);
        if (pasazerowie[iloscOdwiedziających - 1].state == 4)
        {
            allDone = true;
        }
        for (int i = 0; i < iloscOdwiedziających; ++i) {
            if (pasazerowie[i].state != 4) {
                allDone = false;
                break;
            }
        }
        clearConsole();
        printThreadStates();
        if (allDone) {
            break;
        }
    }
    clearConsole();
    for (int i = 0; i < iloscOdwiedziających; i++)
    {
        float tmp = czos[i].count();
        std::cout << "Pasazer nr: " << pasazerowie[i].id << " czekał na wejście do wagonika: " << tmp / 1000 << "s" << std::endl;
    }
    for (int i = 0; i < iloscOdwiedziających; ++i) {
        threads[i].join();
    }
    wagon.join();
    return 0;
}
