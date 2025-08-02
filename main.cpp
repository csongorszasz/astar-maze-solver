#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <set>
#include <cmath>

using namespace std;

#define INF 1e8
#define EPS 0.000001
#define FELDOLGOZATLAN 0
#define NYITOTT 1
#define ZART 2

struct Pont
{
    int i, j;

    bool operator<(const Pont &a) const
    {
        return i < a.i || (i == a.i && j < a.j);
    }

    Pont() {}
    Pont(int i, int j) : i(i), j(j) {}
};

struct Csomopont
{
    int sorszam;
    Pont poz;
    double f = INF, g = INF, h = INF; // f - teljes tavolsag a kezdoponttol a celig (f = g + h)
                                      // g - tavolsag a kezdoponttol az aktualis pontig
                                      // h - becsult tavolsag az aktualis ponttol a celig
    int elozo = -1;
    int stat = FELDOLGOZATLAN;

    Csomopont() {};
    Csomopont(int sorszam, Pont poz) : sorszam(sorszam), poz(poz) {}
};

void beolvas(vector<vector<int>> &labirintus, Pont &kezdo, Pont &cel)
{
    ifstream fin("labirintus.txt");

    string sor;
    int sorIdx = 0;
    while (!fin.eof())
    {
        getline(fin, sor);
        labirintus.resize(sorIdx + 1);
        for (int i = 0; i < sor.size(); i++)
        {
            int ertek;
            if (sor[i] == '1')
            {
                ertek = 1;
            }
            else if (sor[i] == 'S')
            {
                kezdo.i = sorIdx;
                kezdo.j = i;
                ertek = 0;
            }
            else if (sor[i] == 'F')
            {
                cel.i = sorIdx;
                cel.j = i;
                ertek = 0;
            }
            else
            {
                ertek = 0;
            }
            labirintus[sorIdx].push_back(ertek);
        }
        sorIdx++;
    }

    fin.close();
}

double tavolsag(Pont a, Pont b)
{
    return sqrt(pow(a.i - b.i, 2) + pow(a.j - b.j, 2));
}

// a "csomopontokVek" es "csomopontokMap" erteke valtozik
void init(vector<vector<int>> &labirintus, vector<Csomopont> &csomopontokVek, map<Pont, int> &csomopontokMap, Pont cel)
{
    int sorszam = 0;
    for (int i = 0; i < labirintus.size(); i++)
    {
        for (int j = 0; j < labirintus[i].size(); j++)
        {
            if (labirintus[i][j] == 0)
            {
                csomopontokVek.push_back(Csomopont(sorszam, Pont(i, j)));
                csomopontokMap[Pont(i, j)] = sorszam;
                sorszam++;
            }
        }
    }
}

void beallit(int akt, int elozo, vector<Csomopont> &csomopontok)
{
    csomopontok[akt].elozo = elozo;
    csomopontok[akt].g = csomopontok[elozo].g + tavolsag(csomopontok[akt].poz, csomopontok[elozo].poz);
    csomopontok[akt].f = csomopontok[akt].g + csomopontok[akt].h;
    csomopontok[akt].stat = NYITOTT;
}

void frissit(int akt, int elozo, vector<Csomopont> &csomopontok)
{
    double tav = tavolsag(csomopontok[akt].poz, csomopontok[elozo].poz);
    if (csomopontok[elozo].g + tav < csomopontok[akt].g)
    {
        csomopontok[akt].elozo = elozo;
        csomopontok[akt].g = csomopontok[elozo].g + tav;
        csomopontok[akt].f = csomopontok[akt].g + csomopontok[akt].h;
    }
}

bool lephet(vector<vector<int>> &labirintus, Pont poz)
{
    return ((poz.i >= 0 && poz.i < labirintus.size()) &&
            (poz.j >= 0 && poz.j < labirintus[0].size()) &&
            (labirintus[poz.i][poz.j] == 0));
}

// return: csomopont index
int keresMinF(set<int> &nyitott, vector<Csomopont> &csomopontok)
{
    int minim = -1;
    for (auto csp : nyitott)
    {
        if (minim == -1)
            minim = csp;
        else if (csomopontok[csp].f < csomopontok[minim].f)
            minim = csp;
        else if (csomopontok[csp].f - csomopontok[minim].f <= EPS)
            if (csomopontok[csp].h < csomopontok[minim].h)
                minim = csp;
    }
    return minim;
}

void aStar(vector<vector<int>> &labirintus, vector<Csomopont> &csomopontokVek, map<Pont, int> &csomopontokMap, Pont kezdo, Pont cel)
{
    set<int> nyitott;

    csomopontokVek[csomopontokMap[kezdo]].g = 0;
    csomopontokVek[csomopontokMap[kezdo]].f = csomopontokVek[csomopontokMap[kezdo]].h;
    csomopontokVek[csomopontokMap[kezdo]].stat = NYITOTT;
    nyitott.insert(csomopontokMap[kezdo]);

    while (!nyitott.empty())
    {
        int akt = keresMinF(nyitott, csomopontokVek);

        if (akt == csomopontokVek[csomopontokMap[cel]].sorszam)
            return;

        csomopontokVek[akt].stat = ZART;
        nyitott.erase(akt);

        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i != 0 || j != 0)
                {
                    Pont ujPoz(csomopontokVek[akt].poz.i + i, csomopontokVek[akt].poz.j + j);
                    if (lephet(labirintus, ujPoz) && csomopontokVek[csomopontokMap[ujPoz]].stat != ZART)
                    {
                        int szomszed = csomopontokMap[ujPoz];
                        if (csomopontokVek[szomszed].stat == FELDOLGOZATLAN)
                        {
                            nyitott.insert(szomszed);
                            beallit(szomszed, akt, csomopontokVek);
                        }
                        else if (csomopontokVek[szomszed].stat == NYITOTT)
                        {
                            frissit(szomszed, akt, csomopontokVek);
                        }
                    }
                }
            }
        }
    }
}

void meghatMegoldas(int akt, vector<Csomopont> &csomopontok, vector<Pont> &megoldas)
{
    if (csomopontok[akt].elozo == -1)
    {
        megoldas.push_back(csomopontok[akt].poz);
        return;
    }
    megoldas.push_back(csomopontok[akt].poz);
    meghatMegoldas(csomopontok[akt].elozo, csomopontok, megoldas);
}

void kiirLab(vector<vector<int>> &labirintus, Pont kezdo, Pont cel)
{
    ofstream fout("out.txt");

    for (int i = 0; i < labirintus.size(); i++)
    {
        for (int j = 0; j < labirintus[i].size(); j++)
        {
            if (i == kezdo.i && j == kezdo.j)
                fout << "S";
            else if (i == cel.i && j == cel.j)
                fout << "F";
            else if (labirintus[i][j] == 0)
                fout << " ";
            else if (labirintus[i][j] == 1)
                fout << 1;
            else if (labirintus[i][j] == 2)
                fout << "█";
        }
        fout << endl;
    }

    fout.close();
}

void kiirMegoldas(vector<vector<int>> &labirintus, vector<Csomopont> csomopontok, map<Pont, int> &csomopontokMap, Pont kezdo, Pont cel)
{
    vector<Pont> megoldas;
    meghatMegoldas(csomopontokMap[cel], csomopontok, megoldas);

    for (int i = 0; i < megoldas.size(); i++)
    {
        labirintus[megoldas[i].i][megoldas[i].j] = 2;
    }

    kiirLab(labirintus, kezdo, cel);
}

int main()
{
    vector<vector<int>> labirintus;
    Pont kezdo, cel;
    beolvas(labirintus, kezdo, cel);

    vector<Csomopont> csomopontokVek;
    map<Pont, int> csomopontokMap;
    init(labirintus, csomopontokVek, csomopontokMap, cel);

    aStar(labirintus, csomopontokVek, csomopontokMap, kezdo, cel);
    kiirMegoldas(labirintus, csomopontokVek, csomopontokMap, kezdo, cel);
}