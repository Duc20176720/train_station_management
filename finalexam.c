#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "dllist.h"
#include "jval.h"

#define INFINITIVE_VALUE 100000
#define filename "airport_1.txt"

typedef struct
{
    JRB edges;
    JRB vertices;
} Graph;

Graph createGraph();
void addVertex(Graph graph, int id, char *name);
char *getVertex(Graph graph, int id);
void addEdge(Graph graph, int v1, int v2, int weight);
int getEdgeValue(Graph graph, int v1, int v2);
int indegree(Graph graph, int v, int *output);
int outdegree(Graph graph, int v, int *output);
void dropGraph(Graph graph);
int shortestPath(Graph graph, int s, int t, int *path, int *length, int check);
void tranverse(JRB root);

Graph createGraph()
{
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
    return g;
}

void addVertex(Graph g, int id, char *name)
{
    JRB node = jrb_find_int(g.vertices, id);
    if (node == NULL) 
        jrb_insert_int(g.vertices, id, new_jval_s(strdup(name)));
}

char *getVertex(Graph g, int id)
{
    JRB node = jrb_find_int(g.vertices, id);
    if (node == NULL)
        return NULL;
    else
        return jval_s(node->val);
}

void addEdge(Graph graph, int v1, int v2, int weight)
{
    JRB node, tree;
    if (getEdgeValue(graph, v1, v2) == INFINITIVE_VALUE)
    {
        node = jrb_find_int(graph.edges, v1);
        if (node == NULL)
        {
            tree = make_jrb();
            jrb_insert_int(graph.edges, v1, new_jval_v(tree));
        }
        else
        {
            tree = (JRB)jval_v(node->val);
        }
        jrb_insert_int(tree, v2, new_jval_i(weight));
    }
}

int getEdgeValue(Graph graph, int v1, int v2)
{
    JRB node, tree;
    node = jrb_find_int(graph.edges, v1);
    if (node == NULL)
        return INFINITIVE_VALUE;
    tree = (JRB)jval_v(node->val);
    node = jrb_find_int(tree, v2);
    if (node == NULL)
        return INFINITIVE_VALUE;
    else
        return jval_i(node->val);
}

int indegree(Graph graph, int v, int *output)
{
    JRB tree, node;
    int total = 0;
    jrb_traverse(node, graph.edges)
    {
        tree = (JRB)jval_v(node->val);
        if (jrb_find_int(tree, v))
        {
            output[total] = jval_i(node->key);
            total++;
        }
    }
    return total;
}

int outdegree(Graph graph, int v, int *output)
{
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.edges, v);
    if (node == NULL)
        return 0;
    tree = (JRB)jval_v(node->val);
    total = 0;
    jrb_traverse(node, tree)
    {
        output[total] = jval_i(node->key);
        total++;
    }
    return total;
}
int shortestPath(Graph g, int s, int t, int *path, int *length, int check)
{
    int visited[1000];
    int distance[1000], previous[1000], tmp[1000];
    int min, u, total, k;
    Dllist queue, ptr, node;
    for (int i = 0; i < 1000; i++)
    {
        distance[i] = INFINITIVE_VALUE;
        visited[i] = 0;
        previous[i] = -1;
    }
    distance[s] = 0;
    previous[s] = s;
    queue = new_dllist();
    dll_append(queue, new_jval_i(s));
    while (!dll_empty(queue))
    {
        min = INFINITIVE_VALUE;
        dll_traverse(ptr, queue)
        {
            u = jval_i(ptr->val);
            visited[u] = 1;
            if (min > distance[u])
            {
                min = distance[u];
                node = ptr;
            }
        }
        u = jval_i(node->val);
        dll_delete_node(node);
        int output[1000];
        int n = outdegree(g, u, output);
        for (int i = 0; i < n; i++)
        {
            int v = output[i];
            int w = getEdgeValue(g, u, v);
            if (distance[v] > distance[u] + w)
            {
                distance[v] = distance[u] + w;
                previous[v] = u;
            }
            if (!visited[v])
                dll_append(queue, new_jval_i(v));
        }
    }
    k = previous[t];
    int n = 0;
    path[n++] = t;
    while (k != s)
    {
        if (k == -1)
            return -1;
        path[n++] = k;
        k = previous[k];
    }
    path[n++] = s;
    *length = n;
    return distance[t];
}
void dropGraph(Graph graph)
{
    JRB node, tree;
    jrb_traverse(node, graph.edges)
    {
        tree = (JRB)jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph.edges);
    jrb_free_tree(graph.vertices);
}

void tranverse(JRB root)
{
    JRB temp;
    jrb_traverse(temp, root)
    {
        printf("%d\n", jval_i(temp->key));
    }
}

int main()
{
    Graph graph = createGraph();
    char temp[30];
    int n, m;
    int numLine = 0;
    int airport1, airport2, price;
    int tiepTuc = 0;
    int start, end;
    int length,w;
    int path[10];
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Cannot open file %s.\n", filename);
        return 0;
    }
    fscanf(file, "%d %d\n", &n, &m);
    while (!feof(file))
    {
        fscanf(file, "%d %d %d\n", &airport1, &airport2, &price);
        addVertex(graph, airport1, "");
        addVertex(graph, airport2, "");
    }
    //tranverse(graph.vertices);
    rewind(file);
    while (!feof(file))
    {
        if (numLine == 0)
            fscanf(file, "%d %d\n", &n, &m);
        else
        {
            fscanf(file, "%d %d %d\n", &airport1, &airport2, &price);
            addEdge(graph, airport1, airport2, price);
            addEdge(graph, airport2, airport1, price);
        }
        numLine++;
    }
    int adjacentGraph[100][100];
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++)
            adjacentGraph[i][j] = 0;
    int output[100];
    JRB tempTranverse;
    jrb_traverse(tempTranverse, graph.vertices)
    {
        int i = jval_i(tempTranverse->key);
        int numAdjacent = outdegree(graph, jval_i(tempTranverse->key), output);
        for (int j = 0; j < numAdjacent; j++)
        {
            adjacentGraph[i][output[j]] =1;
            // getEdgeValue(graph, i, output[j]);
        }
    }
    int x;
    do{
        printf("===============================================\n");
        printf("|Option:                                      |\n");
        printf("|1.Print out adjacency matrix                 |\n");
        printf("|2.List of adjacency airport                  |\n");
        printf("|3.The highest number of adjacent airport     |\n");
        printf("|4.Print out the cheapest path                |\n");
        printf("|5.Exit                                       |\n");
        printf("===============================================\n");
        printf("Enter your choice:");
        scanf("%d",&x);
        while(getchar()!='\n');
        switch(x){
            case 1:
            printf("-- Task 1: Adjacency matrix: \n");
            for (int i = 1; i <= n; i++){
                for (int j = 1; j <= n; j++)
                    printf("%d\t", adjacentGraph[i][j]);
                    printf("\n");
            }
            break;
            case 2:
            printf("-- Task 2: List of adjacency airport: \n");
            jrb_traverse(tempTranverse, graph.vertices){
                int i = jval_i(tempTranverse->key);
                int numAdjacent = outdegree(graph, jval_i(tempTranverse->key), output);
                printf("%d : ", i);
                for (int k = 0; k < numAdjacent; k++)
                printf("%d ", output[k]);
                printf("\n");
            }
            break;
            case 3:
            printf("-- Task 3: The highest number of adjacent airport: \n");
            int input[10];
            int max = 0;
            jrb_traverse(tempTranverse, graph.vertices){
                int i = jval_i(tempTranverse->key);
                int numIndegree = indegree(graph, i, input);
                if (max < numIndegree)
                max = numIndegree;
            }
            int maxIndegree[10];
            int totalMaxDegree = 0;
            jrb_traverse(tempTranverse, graph.vertices){
                int i = jval_i(tempTranverse->key);
                int numIndegree = indegree(graph, i, input);
                if (numIndegree == max){
                    maxIndegree[totalMaxDegree++] = i;
                }
            }
            for (int i = 0; i < totalMaxDegree; i++){
                printf("%d\n", maxIndegree[i]);
            }
            break;
            
            case 4:
            {
            printf("-- Task 4: Print out the cheapest path: \n");
            printf("Enter the start airport: ");
            scanf("%d", &start);
            printf("Enter the target airport: ");
            scanf("%d", &end);
            printf("Index: %d %d \n",start,end);
            if( start == end ){
                printf("The same airport!\n");
                break;
            } 
            w = shortestPath(graph, start, end, path, &length, 0);
            printf("-- Price: %d\n",w);
            if (w != -1){
                printf("-- Ordered list: \n");
                for (int i = length-1; i >=0; i--){
                    printf("%d\n", path[i]);
                }
            }
            else{
                printf("NO PATH\n");
            }
            break;
            }
            case 5:
            printf("Thank you for using the system!\n");
            printf("==========Goodbye==============\n");
            printf("\n");
            exit(0);
        };
    }while(x!=5);
}