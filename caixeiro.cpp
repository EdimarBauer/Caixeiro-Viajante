/*
* Created by: Edimar J. Bauer
* Date: May 27, 2018
*
* The Dinamic Programing function well to 20 vertices and admit max 22 vertices untill explode the memory
* The Optimized Search function well to 24 vertices
*/


#include <bits/stdc++.h>

using namespace std;
#define T 50
#define fi first
#define se second
typedef pair<int,int> ii;

struct ipi{     //ipi (int pointer int)
    int Fi;     //first
    int *Se;    //second
};

const int INF (1 << 30);

int TamGrafo, matriz[T][T], G, Dif; //G used to DP. Dif is the difference between the minimum way and the actual answer * 2.
bool visit[T];
ipi dp[23][(1<<22)];
vector<ii> adj[T];
vector<int> Provisorio;

void randomizar(int peso);
void inserirAresta(int i, int j, int d);
void imprimirMatriz();

int forcaBruta(vector<int> &percurso, int no, int cont);
ipi forcaBrutaDP(int no, int S, int cont);
int forcaBrutaOtimizada(vector<int> &percurso);
int montarListaAdjacencia();
int menorCaminhoEncontrado(vector<int> &percurso, int no, int cont);
int caminhoGuloso(vector<int> &percurso, int no, int cont);
void salesman(vector<int> &percurso, int no, int cont, int extra);

void liberarMemoriaDP();
void resultado(vector<int> &percurso, int total);
void resultado(ipi totalDP);

int main(){

    int peso, option, total;
    ipi totalDP;
    vector<int> percurso;
    printf("Digite o tamanho do grafo: ");
    scanf("%d", &TamGrafo);
    printf("\nDigite o valor maximo das arestas: ");
    scanf("%d", &peso);

    randomizar(peso);
    imprimirMatriz();

    printf("1: Forca Bruta\n2: Forca Bruta com PD (Max 22 vertices)\n3: Forca Bruta Otimizada\n");
    scanf("%d", &option);
    G = (1 << TamGrafo) - 1;
    switch(option){
    case 1:
        total = forcaBruta(percurso, 1, 1); break;
    case 2:
        totalDP = forcaBrutaDP(1, 1, 1); break;
        //liberarMemoriaDP();
    case 3:
        total = forcaBrutaOtimizada(percurso); break;
    default: break;
    }

    if (option != 2)
        resultado(percurso, total);
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
                inserirAresta(i, j, distancia);
            }
        }
    }
}

void inserirAresta(int i, int j, int d){
    matriz[i][j] = d;
    matriz[j][i] = d;
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

ipi forcaBrutaDP(int no, int S, int cont){

    if (S == G) {
        ipi noDP;
        noDP.Fi = matriz[no][1];
        noDP.Se = NULL;
        return noDP;
    }
    if (dp[no][S].Fi) return dp[no][S];

    visit[no] = 1;
    ipi total, dist;
    total.Fi = INF;
    total.Se = (int*)malloc(sizeof(int)*(TamGrafo-cont));
    if (total.Se == NULL){puts("Err of alocation"); exit(0);}
    for (int i = 2; i <= TamGrafo; i++){
        if (!visit[i]){
            dist = forcaBrutaDP(i, S | (1 << (i-1) ), cont+1);
            if (dist.Fi + matriz[no][i] < total.Fi){
                total.Fi = dist.Fi + matriz[no][i];
                for (int j = 0; j < TamGrafo - cont - 1; j++)
                    total.Se[j] = dist.Se[j];
                total.Se[TamGrafo-cont-1] = i;
            }
        }
    }
    visit[no] = 0;
    return dp[no][S] = total;
}

int forcaBrutaOtimizada(vector<int> &percurso){

    int minimo = montarListaAdjacencia();
    int total = caminhoGuloso(percurso, 1, TamGrafo);

    Dif = (total - minimo) << 1;
    memset(visit, 0, sizeof visit);

    salesman(percurso, 1, 1, 0);
    return (Dif >> 1) + minimo;
}

void salesman(vector<int> &percurso, int no, int cont, int extra){

    //extra represents the excess of the route
    if (Dif <= extra) return;
    if (cont == TamGrafo){
        extra += matriz[no][1] - adj[no][0].fi + matriz[no][1] - adj[1][1].fi;
        if (extra < Dif){
            Dif = extra;
            percurso.swap(Provisorio);
            percurso.push_back(no);
        }
        return;
    }

    visit[no] = 1;
    Provisorio.push_back(no);
    int u;
    for (int i = 2; i <= TamGrafo; i++){
        if (!visit[i]){
            u = matriz[no][i] << 1;
            //update extra with the excess of the edge
            salesman(percurso, i, cont+1, extra + u - adj[no][0].fi - adj[i][1].fi);
        }
    }
    Provisorio.pop_back();
    visit[no] = 0;
}

int montarListaAdjacencia(){

    int minimo = 0;
    for (int i = 1; i <= TamGrafo; i++){
        for (int j = 1; j <= TamGrafo; j++)
            if (i != j)
                adj[i].push_back(ii(matriz[i][j], j));
        sort(adj[i].begin(), adj[i].end());
        minimo += adj[i][0].fi + adj[i][1].fi;
    }
    return (minimo + 1) / 2;
}

int caminhoGuloso(vector<int> &percurso, int no, int cont){

    int total = 0, menorAresta, k;
    percurso.push_back(1);
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
        percurso.push_back(no);
    }

    return total + matriz[no][1];
}

void resultado(vector<int> &percurso, int total){

    printf("\n1(0) -> ");
    int ant = 1;
    for (int i=1; i < percurso.size(); i++){
        printf("%d(%d) -> ", percurso[i], matriz[ant][percurso[i]]);
        ant = percurso[i];
    }
    printf("1(%d)\nTotal: %d\n\n", matriz[ant][1], total);
}

void resultado(ipi totalDP){

    printf("\n1(0) -> ");
    int ant = 1, x;
    for (int i=1; i < TamGrafo; i++){
        x = totalDP.Se[i-1];
        printf("%d(%d) -> ", x, matriz[ant][x]);
        ant = x;
    }
    printf("1(%d)\nTotal: %d\n\n", matriz[ant][1], totalDP.Fi);
}
