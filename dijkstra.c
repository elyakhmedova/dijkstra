#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// вершина списка смежности
struct AdjListNode
{
	int dest;
	struct AdjListNode* next;
};

//список смежности
struct AdjList
{
    struct AdjListNode *head;
};

struct Graph
{
	int V;
	struct AdjList* array;
};

//создание новой вершины в списке смежности
struct AdjListNode* newAdjListNode(int dest)
{
	struct AdjListNode* newNode =(struct AdjListNode*)malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->next = NULL;
	return newNode;
}

struct Graph* createGraph(int V)
{
    int i;
	struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
	graph->V = V;

	//массив списков смежности
	graph->array = (struct AdjList*)malloc((V) * sizeof(struct AdjList));

	for (i = 0; i < V; ++i)
		graph->array[i].head = NULL;

	return graph;
}

void addEdge(struct Graph* graph, int src,int dest)
{
	struct AdjListNode* newNode = newAdjListNode(dest);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}


//Далее - реализация binary heap. храним в виде одномерного массиа
struct MinHeapNode
{
	int v;
	int dist;
};

struct MinHeap
{
	//Количество вершин в куче на данный момент
	int size;
	int capacity;
	int *pos;    //позиция в куче
	struct MinHeapNode **array;
};


struct MinHeapNode* newMinHeapNode(int v,int dist)
{
	struct MinHeapNode* minHeapNode = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}

struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =(struct MinHeapNode**)malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

//функция обмена двух вершин местами
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

//балансирование кучи по принципу min heap
void minHeapify(struct MinHeap* minHeap,int idx)  //idx - индекс вершины, положение которой надо поменять, чтобы получить сбалансированное дерево
{
	int smallest, left, right;
	struct MinHeapNode *smallestNode, *idxNode;
	smallest = idx;
	left = 2*idx + 1;
	right = 2*idx + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->dist <
		minHeap->array[smallest]->dist )
	smallest = left;

	if (right < minHeap->size &&
        minHeap->array[right]->dist <
        minHeap->array[smallest]->dist )
	smallest = right;

    if (smallest != idx)
    {
        //вершины, которые нужно поменять местами
        smallestNode =
             minHeap->array[smallest];
        idxNode =
                 minHeap->array[idx];

        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        swapMinHeapNode(&minHeap->array[smallest],
                         &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }

}

int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;

	struct MinHeapNode* root =
				minHeap->array[0];    //наименьший элемент - в корне

	//меняем меньший элемент на предпоследний
	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;

	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;

	--minHeap->size;
	minHeapify(minHeap, 0);         //упорядочиваем кучу после извлечение элемента

	return root;
}

//функция уменьшения расстояния до данной вершины и изменения позиции этой вершины
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
	int i = minHeap->pos[v];

	//Обновляем расстояние
	minHeap->array[i]->dist = dist;

	//поднимаемся по куче, пока она не будет сбалансирована
	while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
	{
		minHeap->pos[minHeap->array[i]->v] =
									(i-1)/2;
		minHeap->pos[minHeap->array[
							(i-1)/2]->v] = i;
		swapMinHeapNode(&minHeap->array[i],
				&minHeap->array[(i - 1) / 2]);

		i = (i - 1) / 2;
	}
}


int isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return 1;
    return 0;
}

void printPath(int *parent, int j)
{
    if (parent[j]==-1)
        return;
    printPath(parent, parent[j]);
    printf("%d ", j);
}

//Печать расстояния и пути (второе в разработке)
void printArr(int *dist, int n, int dest, int *parent)
{
    if(dist[dest-1] == INT_MAX)
        printf("-1");
    else
    {
        printf("%d\n", dist[dest-1]);
        printPath(parent, dest);
    }
}



//Основная функция
void dijkstra(struct Graph* graph, int src, int dest)
{
    int v, i, *dist, *parent;
	int V = graph->V;
	dist = (int*)malloc((V)*sizeof(int));
	parent = (int*)calloc(V, sizeof(int));
	struct MinHeap* minHeap = createMinHeap(V);

	for (v = 0; v < V; ++v)
	{
        parent[0] = -1;
		dist[v] = INT_MAX;
		minHeap->array[v] = newMinHeapNode(v, dist[v]);
		minHeap->pos[v] = v;
	}

	//начальную вершину нужно извлечь самой первой
	minHeap->array[src] = newMinHeapNode(src, dist[src]);
	minHeap->pos[src] = src;
	dist[src] = 0;

	decreaseKey(minHeap, src, dist[src]);

	minHeap->size = V;

	//в куче - вершины, для которых процесс уменьшения расстояния еще не окончен
	while (!isEmpty(minHeap))
	{
		struct MinHeapNode* minHeapNode = extractMin(minHeap); //извлекаем вершину, до которой расстояние минимально, так как именно ее соседи нас интересуют
		int u = minHeapNode->v;
		if(u == dest)
            break;
        else
        {
            struct AdjListNode* pCrawl = graph->array[u].head;
            while (pCrawl != NULL)  //пока не дошли до конца списка смежности для этой вершины
            {
                int v = pCrawl->dest;

                if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && 1 + dist[u] < dist[v])
                {
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    decreaseKey(minHeap, v, dist[v]);
                }
                pCrawl = pCrawl->next;
            }
        }
    }

	printArr(dist, V, dest, parent);
	free(dist);
	free(parent);
	for(i = 0; i < minHeap->capacity; i++)
    {
        free(minHeap->pos);
        free(minHeap->array[i]);
    }
    free(minHeap);
}


int main()
{

	int NumVert, source, destination, i, vert;
    char inp[100] = {};
    char sep[2] = " ";
    char *p, *list;
    struct Graph* graph;

    fgets(inp, 100, stdin);
    p = strtok(inp, sep);
    NumVert = strtol(p, NULL, 10); //число вершин

    p = strtok(NULL, sep);
    source = strtol(p, NULL, 10);   //вершина, из которой начинается путь

    p = strtok(NULL, sep);         //вершина, в которую приходим
    destination = strtol(p, NULL, 10);

    graph = createGraph(NumVert);
    list = (char*)malloc(NumVert*(sizeof(char)*2)); //динамический массив для считывания вершин
    for(i = 0; i < NumVert; i++)
    {
        fgets(list, NumVert*(sizeof(char))*2, stdin);
        p = strtok(list, sep);

        while(p != NULL)
        {
            vert = strtol(p, NULL, 10);
            if(vert != 0)
                addEdge(graph, i, vert-1);
            p = strtok(NULL, sep);
        }

    }

	dijkstra(graph, source-1, destination);
	for(i = 0; i < NumVert; i++)
        free(graph->array[i].head);
	free(list);
	free(graph);
	free(graph->array);
	return 0;
}
