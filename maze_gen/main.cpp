#include <iostream>
#include <string>
#include <stack>
#include <cassert>
#include <ctime>

using namespace std;

#define Pixels_Per_Wall 3

enum BT {EMPTY, LEFT, RIGHT, TOP, BOTTOM, DIR};

struct Node {
	int _x, _y;
	Node(int x=-1, int y=-1):_x(x),_y(y) {}
};

template <size_t R, size_t C>
void DFS(unsigned char ver_walls[R][C+1], unsigned char hor_walls[R+1][C]) {
	unsigned char visited[R][C] = {0};
	stack<Node> path;
	int rv = rand() % (R*C);
	Node start(rv / C, rv % C);

	memset(ver_walls, 1, sizeof(unsigned char)*R*(C+1));
	memset(hor_walls, 1, sizeof(unsigned char)*(R+1)*C);

	path.push(start);
	visited[start._x][start._y] = 1;
	unsigned int num_visited = 1;
	while ( num_visited!= R*C) {
		Node cur = path.top();
		
		Node avaliable[4];
		BT bts[DIR];
		int cnt = 0;
		// to left;
		if (cur._y - 1 >= 0 && !visited[cur._x][cur._y-1]) {
			avaliable[cnt]._x = cur._x;
			avaliable[cnt]._y = cur._y - 1;
			bts[cnt] = LEFT; 
			cnt++;
		}
		// to right;
		if (cur._y + 1 < C && !visited[cur._x][cur._y+1]) {
			avaliable[cnt]._x = cur._x;
			avaliable[cnt]._y = cur._y + 1;
			bts[cnt] = RIGHT;
			cnt++;
		}
		// to top;
		if (cur._x - 1 >= 0 && !visited[cur._x-1][cur._y]) {
			avaliable[cnt]._x = cur._x - 1;
			avaliable[cnt]._y = cur._y;
			bts[cnt] = TOP;
			cnt++;
		}
		// to bottom;
		if (cur._x + 1 < R && !visited[cur._x+1][cur._y]) {
			avaliable[cnt]._x = cur._x + 1;
			avaliable[cnt]._y = cur._y;
			bts[cnt] = BOTTOM;
			cnt++;
		}
		if (cnt > 0) {
			int iselected = rand() % cnt;
			path.push(avaliable[iselected]);
			visited[path.top()._x][path.top()._y] = 1;
			num_visited++;
			switch (bts[iselected]) {
			case BOTTOM:		hor_walls[cur._x+1][cur._y] = 0;	break;
			case TOP:			hor_walls[cur._x][cur._y]	= 0;	break;
			case LEFT:			ver_walls[cur._x][cur._y]	= 0;	break;
			case RIGHT:			ver_walls[cur._x][cur._y+1] = 0;	break;
			default:	break;
			}
		} else {
			path.pop();
			assert(path.size() > 0);
		} 
	}	
}

template<size_t R,size_t C>
bool _move(int x, int y, unsigned char ver_walls[R][C+1], unsigned char hor_walls[R+1][C], unsigned char visited[R][C],size_t *num_visited) {
	visited[x][y] = 1;
	if (++(*num_visited) == R*C) return true;

	while (true) {
		Node avaliable[4];
		BT bts[DIR];
		int cnt = 0;
		//top
		if (x-1 >= 0 && !visited[x-1][y]) {
			avaliable[cnt]._x = x-1;
			avaliable[cnt]._y = y;
			bts[cnt] = TOP;
			cnt++;
		}
		//bottom
		if (x+1 < R && !visited[x+1][y]) {
			avaliable[cnt]._x = x+1;
			avaliable[cnt]._y = y;
			bts[cnt] = BOTTOM;
			cnt++;
		}
		//left
		if (y-1 >= 0 && !visited[x][y-1]) {
			avaliable[cnt]._x = x;
			avaliable[cnt]._y = y-1;
			bts[cnt] = LEFT;
			cnt++;
		}
		//right
		if (y+1 < C &&!visited[x][y+1]) {
			avaliable[cnt]._x = x;
			avaliable[cnt]._y = y+1;
			bts[cnt] = RIGHT;
			cnt++;
		}
		if (cnt>0) {
			int iselected = rand() % cnt;
			switch (bts[iselected]) {
			case BOTTOM:		hor_walls[x+1][y] = 0;	break;
			case TOP:			hor_walls[x][y]	= 0;	break;
			case LEFT:			ver_walls[x][y]	= 0;	break;
			case RIGHT:			ver_walls[x][y+1] = 0;	break;
			default:	break;
			}
			if (_move<R,C>(avaliable[iselected]._x, avaliable[iselected]._y,ver_walls,hor_walls,visited, num_visited)) return true;
		}else return false;
	}
}
template<size_t R, size_t C>
void DFS_recrusion(unsigned char ver_walls[R][C+1], unsigned char hor_walls[R+1][C]) {
	unsigned char visited[R][C]= {0};
	size_t num_visited = 0;

	memset(ver_walls, 1, sizeof(unsigned char)*R*(C+1));
	memset(hor_walls, 1, sizeof(unsigned char)*(R+1)*C);

	int rv = rand() % (R*C);
	_move<R,C>(rv / C, rv % C, ver_walls, hor_walls, visited, &num_visited);
}

template<size_t R, size_t C>
void Prim(unsigned char ver_walls[R][C+1], unsigned char hor_walls[R+1][C]) {
	unsigned char used[R][C] = {0};

	Node nodes_avaliable[R*C];
	BT bts[R*C];
	size_t num_avaliable = 0
		;
	int rv = rand()%(R*C);
	nodes_avaliable[num_avaliable] = Node(rv/C, rv%C);
	used[rv/C][rv%C] = 1;
	bts[num_avaliable] = EMPTY;
	num_avaliable++;

	memset(ver_walls, 1, sizeof(unsigned char)*R*(C+1));
	memset(hor_walls, 1, sizeof(unsigned char)*(R+1)*C);

	while(num_avaliable>0){
		int iselected = rand()%num_avaliable;
		Node cur = nodes_avaliable[iselected];
		BT from = bts[iselected];

		nodes_avaliable[iselected] = nodes_avaliable[num_avaliable-1];
		bts[iselected] = bts[num_avaliable-1];
		num_avaliable--;

		switch(from) {
		case LEFT:		ver_walls[cur._x][cur._y+1]= 0; break;
		case RIGHT:     ver_walls[cur._x][cur._y] = 0; break;
		case TOP:		hor_walls[cur._x+1][cur._y] = 0; break;
		case BOTTOM:	hor_walls[cur._x][cur._y] = 0; break;
		default:break;
		}

		// LEFT
		if(cur._y - 1 >=0 && !used[cur._x][cur._y-1]) {
			nodes_avaliable[num_avaliable] = Node(cur._x, cur._y-1);
			bts[num_avaliable] = LEFT;
			num_avaliable++;
			used[cur._x][cur._y-1]=1;
		}
		// RIGHT
		if(cur._y + 1 < C && !used[cur._x][cur._y+1]) {
			nodes_avaliable[num_avaliable] = Node(cur._x, cur._y + 1);
			bts[num_avaliable] = RIGHT;
			num_avaliable++;
			used[cur._x][cur._y+1]=1;
		}
		// TOP
		if (cur._x- 1 >= 0 && !used[cur._x-1][cur._y]) {
			nodes_avaliable[num_avaliable] = Node(cur._x-1, cur._y);
			bts[num_avaliable] = TOP;
			num_avaliable++;
			used[cur._x-1][cur._y]=1;
		}
		// BOTTOM
		if (cur._x+1 < R && !used[cur._x+1][cur._y]) {
			nodes_avaliable[num_avaliable] = Node(cur._x+1, cur._y);
			bts[num_avaliable] = BOTTOM;
			num_avaliable++;
			used[cur._x+1][cur._y]=1;
		}
	}
}

template <size_t R, size_t C>
void graphic_maze(unsigned char ver_walls[R][C+1], unsigned char hor_walls[R+1][C]) {
	unsigned char pixels_map[(Pixels_Per_Wall-1)*R+1][(Pixels_Per_Wall-1)*C+1] = {0};
	for (int i = 0; i < R; i++) {
		for (int j = 0; j < C+1; j++) {
			if (ver_walls[i][j]) {
				for (int k= 0; k< Pixels_Per_Wall; k++) {
					pixels_map[i*(Pixels_Per_Wall-1)+k][j*(Pixels_Per_Wall-1)] = 1;
				}
			}
		}
	}

	for (int i = 0; i < R+1; i++) {
		for (int j = 0; j < C; j++) {
			if (hor_walls[i][j]) {
				for (int k= 0; k< Pixels_Per_Wall; k++) {
					pixels_map[i*(Pixels_Per_Wall-1)][j*(Pixels_Per_Wall-1)+k] = 1;
				}
			}
		}
	}
	for (int i = 0; i < (Pixels_Per_Wall-1)*R+1; i++) {
		for (int j = 0; j < (Pixels_Per_Wall-1)*C+1; j++) {
			if (pixels_map[i][j]) cout << "# "; 
			else cout << "  ";
		}
		cout << endl;
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	const size_t R = 25;
	const size_t C = 25;
	unsigned char ver_walls[R][C+1];
	unsigned char hor_walls[R+1][C];
	//DFS<R,C>(ver_walls,hor_walls);
	//DFS_recrusion<R,C>(ver_walls,hor_walls);
	Prim<R,C>(ver_walls,hor_walls);
	graphic_maze<R,C>(ver_walls, hor_walls);
	return 0;
}