  O problema do Caixeiro Viajante é um dos problemas NP-Completos cuja complexidade é exponencial.
  O problema diz respeito a uma pessoa visitar todas as n cidades passando por cada uma delas uma única vez e voltando ao ponto de partida com o menor custo, seja de tempo ou de distância, dependendo o que o valor das arestas (ligações entre cidades) representam.
  O algoritmo em anexo trata três diferentes formas de resolver o problema de forma exata:
  1 - Algoritmo força bruta, complexidade O(n!);
  2 - Algoritmo força bruta com programação dinâmica, complexidade O(n^2 * 2^n);
  3 - Algoritmo de força bruta com técnica de Branch e Bound, complexidade não definida.
  
  O algoritmo 3 não possui complexidade definida nem utiliza programação dinâmica, porém poderia utilizar a programação dinâmica em conjunto para otimização garantindo complexidade O(n^2 * 2^n). A programção dinâmica não é utilizada pois um computador normal não possui memória RAM o suficiente, dado que a técnica de Branch e Bound aplicada consegue resolver o problema para uma quantidade maior de cidades.
  O algoritmo 3 faz um pré-cálculo de todos os possíveis caminhos a partir de um determinado ponto e com isso ele consegue prever de antemão que a partir daquele ponto não existe caminho melhor do que a melhor resposta já encontrada. Essa explicação é um tanto superficial sobre o algoritmo, mas explica bem a ideia.
  
  Uma explicação mais detalhada sobre o algoritmo 3:
  1 - Defini-se um "mapa", número de cidades dado pelo usuário, e o algoritmo cria vértices ligando todas as cidades entre si com valor de aresta aleatório e limite máximo do valor da aresta dado pelo usuário;
  2 - Cria-se uma lista de adjacência ordenada;
  3 - Faz-se um somatório (minimo) das duas melhores adjacências de cada vértice. Este somatório garante que é impossível existir caminho menor do que teto(minimo / 2).
  4 - Calcula-se um caminho qualquer dado por um algoritmo guloso e salva este como melhor resposta (better);
  5 - Calcula-se a distância (dis) do caminho encontrado pelo algoritmo guloso;
  6 - Se dis == teto(minimo / 2) o algoritmo já encontrou a melhor resposta (não significa que não possa existir outro caminho de mesmo tamanho);
  7 - Senão, aplica-se o algoritmo de força bruta;
  8* - Verifica se a partir do estado atual existe caminho possível melhor que better;
  9 - Se sim, segue o algoritmo de força bruta normalmente de forma recursiva;
  10 - Senão, retorna um passo na recursão e evita calcular todos os demais caminhos a partir daquele ponto;
  11 - Sempre que encontrar uma solução melhor, salva-a e continua-se o algoritmo, até testar todas as possibilidades.
  12 - Se, em qualquer momento, encontrar caminho que satisfaça a condição 6, termina o algoritmo.
    
    8* => A verificação se é possível existir caminho melhor a partir de determinado ponto, é dado da seguinte forma:
    Imagina-se selecionar as duas melhores arestas de cada vértice não visitado (selecionar duas arestas, pois cada vértice, ao final, terá duas ligações) e soma-se todos estes valores. Não significa que exista caminho passando pelas duas melhores arestas de cada vértice, mas sabe-se que é impossível obter caminho menor. É necessário dividir este somatório por 2 (ans), pois uma aresta conecta dois vértices e esta aresta está sendo somada duas vezes, uma vez por cada vértice (mesmo que esta aresta não seja escolhida pelos dois vértices, apenas por um deles).
    Soma-se o caminho já passado com ans, se a resposta for maior ou igual a melhor resposta válida já encontrada, significa que não adianta seguir a partir daquele ponto.
    Uma forma eficiente de fazer essa verificação é, ao invés de, ficar calculando ans a cada novo ponto alcançado, é simplesmente ir somando a diferença (dif) de valores entre a aresta escolhida e a primeira e segunda melhor aresta dos vértices conectados por esta aresta. Quando dif >= (better * 2 - minimo), significa que a partir daquele ponto não adianta continuar.
    Este foi o primeiro insight.
    O segundo insight foi eliminar as arestas que não podiam ser escolhidas (pois se conectavam a vértices que já faziam parte do caminho), quando estas arestas eliminadas pertenciam as primeiras escolhas dos vértices que ainda não faziam parte do caminho, o valor de ans aumentava, diminuindo a tolerância a caminhos ruins.
    
    Problema em aberto: definir a complexidade deste algoritmo. 
    Se provado complexidade menor que da programação dinâmica pode render um bom artigo.
