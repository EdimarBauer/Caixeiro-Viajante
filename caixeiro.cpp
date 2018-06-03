/*
* Created by: Edimar J. Bauer
* Date: June 03, 2018
*
* The Dynamic Programing work well to 20 vertex and admit max 22 vertex before explode the memory
* The Optimized Search work well around to 27 vertex and can to resolve 33 vertex or more depending on the edges
*/


#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;
#define T 50
#define MaxDP 22
#define fi first
#define se second
typedef pair<int,int> ii;

struct ipc{     //ipc (int pointer char)
    int acumulado;
    char *Percurso;
};

const int INF = (1 << 30);
const int TIME_LIMIT = 10;

int TamGrafo, matriz[T][T], G, Dif; //G used to DP. Dif is the extra values obtained for the better answer.
int Posicao[T];
ii melhorAresta[T][2];
char Used[T];
bool visit[T];
ipc dp[MaxDP][(1 << MaxDP)];
vector<ii> adj[T];
map<int, bool> Dependente[T];
vector<int> Provisorio, Percurso;
clock_t Start;

void randomizar(int peso);
void imprimirMatriz();

int forcaBruta(vector<int> &percurso, int no, int cont);
ipc forcaBrutaDP(int no, int S, int cont);

int forcaBrutaOtimizada();
bool salesman(int no, int cont, int extra);
int atualizaConexoes(int no, int &extrax, int mudado[], int cont);
void desfazConexoes(int no, int mudado[]);
void retiraDependente(auto it, int no, int i, int &u, int mudado[]);
void incluiDependente(auto it, int i, int mudado[]);
void salva();
int montarListaAdjacencia();
int caminhoGuloso(int no, int cont);

void imprimirDependentes();
void liberarMemoriaDP();
void resultado();
void resultado(ipc totalDP);

int main(){

    int peso, option;
    ipc totalDP;

    printf("Digite o tamanho do grafo: ");
    scanf("%d", &TamGrafo);
    printf("\nDigite o valor maximo das arestas: ");
    scanf("%d", &peso);

    randomizar(peso);
    imprimirMatriz();

    printf("1: Forca Bruta (Max 12 vertices)\n2: Forca Bruta com PD (Max 22 vertices)\n3: Forca Bruta Otimizada (32 vertices or more)\n");
    scanf("%d", &option);
    G = (1 << TamGrafo) - 1;
    Start = clock();
    switch(option){
    case 1:
        forcaBruta(Percurso, 1, 1); break;
    case 2:
        totalDP = forcaBrutaDP(1, 1, 1); break;
        //liberarMemoriaDP();
    case 3:
        forcaBrutaOtimizada(); break;
    default: return 0;
    }

    if (option != 2)
        resultado();
    else
        resultado(totalDP);
}

//###############################################################################################################

void randomizar(int peso){

    int distancia, tamGrafo = TamGrafo + 1;
    for(int i = 1; i < tamGrafo; i++){
        for(int j = 1; j < tamGrafo; j++){
            if (i != j){
                distancia = (rand() % peso) + 1;
                matriz[i][j] = distancia;
                matriz[j][i] = distancia;
            }
        }
    }
}

void imprimirMatriz(){

    printf("\n     ");
    for(int i = 1; i <= TamGrafo; i++)
        printf("%-7d", i);

    puts("");
    for(int i = 0; i < TamGrafo; i++)
        printf("-------");

    puts("");
    for(int i = 1; i <= TamGrafo; i++){
        printf("%-2d | ", i);
        for(int j = 1; j <= TamGrafo; j++)
            printf("%-2d     ", matriz[i][j]);
        puts("");

        for(int j = 0; j < TamGrafo; j++)
            printf("-------");
        puts("");
    }
    printf("\n\n");
}





int forcaBruta(vector<int> &percurso, int no, int cont){

    if (cont == TamGrafo){
        percurso.push_back(1);
        return matriz[1][no];
    }

    if ( (clock() - Start) / CLOCKS_PER_SEC > TIME_LIMIT ){
        printf("Time liimit excceded %d seconds.\n", TIME_LIMIT);
        exit(0);
    }

    visit[no] = 1;
    int dist, total = INF;
    vector<int> provisorio;
    for(int i = 2; i <= TamGrafo; i++){
        if (!visit[i]){
            dist = min(total, forcaBruta(provisorio, i, cont + 1) + matriz[no][i]);
            if (dist != total){
                percurso.swap(provisorio);
                percurso.push_back(i);
                total = dist;
            }
            provisorio.clear();
        }
    }

    visit[no] = 0;
    return total;
}

ipc forcaBrutaDP(int no, int S, int cont){

    if (S == G) {
        ipc noDP;
        noDP.acumulado = matriz[no][1];
        noDP.Percurso = NULL;
        return noDP;
    }
    if (dp[no-1][S].acumulado) return dp[no-1][S];

    if ( (clock() - Start) / CLOCKS_PER_SEC > TIME_LIMIT ){
        printf("Time excceded %d seconds.\n", TIME_LIMIT);
        exit(0);
    }

    visit[no] = 1;
    ipc total, dist;
    total.acumulado = INF;
    total.Percurso = (char*)malloc(sizeof(char)*(TamGrafo-cont));
    if (total.Percurso == NULL){puts("Err of alocation"); exit(0);}
    for (int i = 2; i <= TamGrafo; i++){
        if (!visit[i]){
            dist = forcaBrutaDP(i, S | (1 << (i-1) ), cont+1);
            if (dist.acumulado + matriz[no][i] < total.acumulado){
                total.acumulado = dist.acumulado + matriz[no][i];
                for (int j = 0; j < TamGrafo - cont - 1; j++)
                    total.Percurso[j] = dist.Percurso[j];
                total.Percurso[TamGrafo-cont-1] = (char)i;
            }
        }
    }
    visit[no] = 0;
    return dp[no-1][S] = total;
}





int forcaBrutaOtimizada(){

    int minimo = montarListaAdjacencia();
    //int total = caminhoGuloso(1, TamGrafo) << 1;

    Dif = INF;
    fill_n(visit, T, 0);
    fill_n(Posicao, T, 2);
    Posicao[1] = 1;
    salesman(1, 1, 0);
    return ( (Dif + minimo) >> 1);
}

bool salesman(int no, int cont, int extra){

    //extra represents the excess of the route
    if (Dif <= extra) return 0;
    if (cont == TamGrafo){
        extra += (matriz[no][1] << 1) - melhorAresta[no][0].fi - melhorAresta[1][0].fi;
        if (extra < Dif){
            Dif = extra; salva(); Percurso.push_back(no);
            if (Dif <= 1) return 1; //means not exists a better way, finishes the brute force
        }
        return 0;
    }

    if ( (clock() - Start) / CLOCKS_PER_SEC > TIME_LIMIT ){
        printf("Time excceded %d seconds.\nBetter answer found:\n", TIME_LIMIT);
        return 1;
    }

    visit[no] = 1; Provisorio.push_back(no);
    int x, u = 0, extrax = 0, mudado[T] = {};
    if (no != 1) u = atualizaConexoes(no, extrax, mudado, cont);
    else extrax = melhorAresta[1][0].fi - melhorAresta[1][1].fi;

    if (u > 0){
        //means that exist only one way to be verified
        if (salesman(u, cont+1, extra + extrax + (matriz[no][u] << 1) - melhorAresta[no][0].fi - melhorAresta[u][1].fi))
            return 1;
    }else if (u == 0){
        //means that there are several possible ways
        map<int, bool>::iterator it;
        for (int i = 0; i < adj[no].size(); i++){
            x = adj[no][i].se;
            if (!visit[x]){
                u = matriz[no][x] << 1;

                if (no == 1 && i == 0){
                    u -= extrax * 2;
                    melhorAresta[no][0] = melhorAresta[no][1];
                    Dependente[adj[no][1].se][1] = 0;
                    Posicao[no]++;
                }
                it = Dependente[no].find(x);
                if (it != Dependente[no].end() && no != 1) retiraDependente(it, no, x, u, mudado);
                if (salesman(x, cont+1, extra + extrax + u - melhorAresta[no][0].fi - melhorAresta[x][1].fi)) return 1;
                if (it != Dependente[no].end() && no != 1) incluiDependente(it, x, mudado);
                if (no == 1 && i == 0){
                    melhorAresta[no][0] = adj[no][0];
                    Dependente[adj[no][1].se][1] = 1;
                    Posicao[no]--;
                }
            }
        }
    }
    if (no != 1) desfazConexoes(no, mudado);
    Provisorio.pop_back(); visit[no] = 0;
    return 0;
}

int atualizaConexoes(int no, int &extrax, int mudado[], int cont){

    int x, k, u = 0;
    for (auto it = Dependente[no].begin(); it != Dependente[no].end(); it++){
        x = it->fi;
        if (!visit[x] || (x == 1 && it->se == 0 && cont != 2) ){
            for (k = Posicao[x]; k < adj[x].size(); k++){
                ii y = adj[x][k];
                if (!visit[y.se] || y.se == 1){
                    extrax += y.fi - matriz[x][no];
                    if (no == melhorAresta[x][0].se){
                        melhorAresta[x][0] = y;
                        Dependente[y.se][x] = 0;
                    }else {
                        melhorAresta[x][1] = y;
                        Dependente[y.se][x] = 1;
                    }
                    break;
                }
            }
            if (k == adj[x].size()){
                if (u == 0) u = it->fi;
                else return -1;
            }else{
                mudado[x] = k - Posicao[x] + 1;
                Posicao[x] = k + 1;
            }
        }
    }
    return u;
}

void desfazConexoes(int no, int mudado[]){

    for (auto it = Dependente[no].begin(); it != Dependente[no].end(); it++){
        if ((!visit[it->fi] || it->fi == 1) && mudado[it->fi]){
            if (it->se){
                Dependente[melhorAresta[it->fi][1].se].erase(it->fi);
                melhorAresta[it->fi][1] = ii(matriz[no][it->fi], no);
            }else{
                Dependente[melhorAresta[it->fi][0].se].erase(it->fi);
                melhorAresta[it->fi][0] = ii(matriz[no][it->fi], no);
            }
            Posicao[it->fi] -= mudado[it->fi];
        }
    }
}

void retiraDependente(auto it, int no, int i, int &u, int mudado[]){

    if (it->se) {
        u -= melhorAresta[i][1].fi - matriz[no][i];
        Dependente[melhorAresta[i][1].se].erase(i);
        melhorAresta[i][1] = ii(matriz[no][i], no);
    }else{
        u -= melhorAresta[i][0].fi - matriz[no][i];
        Dependente[melhorAresta[i][0].se].erase(i);
        melhorAresta[i][0] = ii(matriz[no][i], no);
    }
    Posicao[i] -= mudado[i];
}

void incluiDependente(auto it, int i, int mudado[]){

    Posicao[i] += mudado[i];
    if (it->se) {
        melhorAresta[i][1] = ii(adj[i][Posicao[i]-1]);
        Dependente[melhorAresta[i][1].se][i] = 1;
    }else {
        melhorAresta[i][0] = ii(adj[i][Posicao[i]-1]);
        Dependente[melhorAresta[i][0].se][i] = 0;
    }
}





void salva(){

    Percurso.clear();
    for (int i = 0; i < Provisorio.size(); i++){
        Percurso.push_back(Provisorio[i]);
    }
}

int montarListaAdjacencia(){

    int minimo = 0;
    for (int i = 1; i <= TamGrafo; i++){
        for (int j = 1; j <= TamGrafo; j++)
            if (i != j)
                adj[i].push_back(ii(matriz[i][j], j));
        sort(adj[i].begin(), adj[i].end());
        minimo += adj[i][0].fi + adj[i][1].fi;
        Dependente[adj[i][0].se][i] = 0;
        Dependente[adj[i][1].se][i] = 1;
        melhorAresta[i][0] = adj[i][0];
        melhorAresta[i][1] = adj[i][1];
    }
    return minimo;
}

int caminhoGuloso(int no, int cont){

    int total = 0, menorAresta, k;
    Percurso.push_back(1);
    while(--cont){
        visit[no] = 1;
        menorAresta = INF;
        for (int i = 2; i <= TamGrafo; i++){
            if (!visit[i] && menorAresta > matriz[no][i]){
                k = i;
                menorAresta = matriz[no][i];
            }
        }
        total += menorAresta;
        no = k;
        Percurso.push_back(no);
    }

    return total + matriz[no][1];
}





void imprimirDependentes(){

    for (int i = 1; i <= TamGrafo; i++){
        printf("Dependentes do %d\n", i);
        for (auto it = Dependente[i].begin(); it != Dependente[i].end(); it++){
            printf("%d, ", it->fi);
        }
        puts("");
    }
    puts("");
}

void resultado(){

    printf("\n(0)1 -> ");
    int ant = 1, total = 0;
    for (int i=1; i < Percurso.size(); i++){
        printf("(%d)%d -> ", matriz[ant][Percurso[i]], Percurso[i]);
        total += matriz[ant][Percurso[i]];
        ant = Percurso[i];
    }
    printf("(%d)1\nTotal: %d\n\n", matriz[ant][1], total + matriz[ant][1]);
}

void resultado(ipc totalDP){

    printf("\n(0)1 -> ");
    int ant = 1, x;
    for (int i=1; i < TamGrafo; i++){
        x = (int)totalDP.Percurso[i-1];
        printf("(%d)%d -> ", matriz[ant][x], x);
        ant = x;
    }
    printf("(%d)1\nTotal: %d\n\n", matriz[ant][1], totalDP.acumulado);
}
