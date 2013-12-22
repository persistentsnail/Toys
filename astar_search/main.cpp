#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
using namespace std;

#include "../cave_gen/cave_gen.h"


#define TI(x, y) ((x)*H+(y))

struct OpenedNode{
	int _f, _g;
	int _x, _y;
	int _parent;
	OpenedNode(int f = -1, int g = -1, int x = -1, int y = -1, int parent = -1):_f(f),_g(g),_x(x),_y(y),_parent(parent) {}
	
};

template <size_t W, size_t H>
class OpenedHeap {
	OpenedNode _heap[W*H];
	int _set[W*H];
	int _sz;

	void move_top(int icur) {
		OpenedNode ON = _heap[icur];
		while (icur > 0) {
			int iparent = (icur-1) / 2;
			if (_heap[iparent]._f <= ON._f) break;

			_heap[icur]	   = _heap[iparent];

			// update
			_set[TI(_heap[icur]._x,_heap[icur]._y)] = icur;

			icur = iparent;
		}
		_heap[icur] = ON;
		_set[TI(_heap[icur]._x, _heap[icur]._y)] = icur;
	}

	void move_bottom(int icur) {
		OpenedNode ON = _heap[icur];
		while (true) {
			int ileft = icur * 2 + 1;
			if (ileft >= _sz) break;

			int iright = icur * 2 + 2;

			int ismaller = iright < _sz && _heap[iright]._f < _heap[ileft]._f? iright:ileft;

			if (_heap[ismaller]._f < ON._f) {
				_heap[icur] = _heap[ismaller];
				_set[TI(_heap[icur]._x, _heap[icur]._y)] = icur;
				icur = ismaller;
			}else break;
		}
		_heap[icur] = ON;
		_set[TI(_heap[icur]._x, _heap[icur]._y)] = icur;
	}

public:
	OpenedHeap():_sz(0) {
		memset(_set, -1, sizeof(_set));
	}


	void push(const OpenedNode &ON) {
		_heap[_sz] = ON;
		int icur = _sz++;

		move_top(icur);
	//	ensure();
	}

	void pop(OpenedNode &ret) {
		ret = _heap[0];
		_set[TI(_heap[0]._x,_heap[0]._y)] = -1;
		assert(_sz>0);
		_heap[0] = _heap[--_sz];
		if (_sz > 0)
			move_bottom(0);
//		ensure();
	}

	bool Get(int x, int y, OpenedNode &ret) {
		if (_set[TI(x,y)] >= 0) {
			ret = _heap[_set[TI(x,y)]];
			return true;
		}
		return false;
	}
	
	bool make_smaller(int x, int y, int f, int g, int parent) {
		int icur = _set[TI(x,y)];
		assert(_heap[icur]._x == x && _heap[icur]._y == y);
		if (icur < 0) return false;
		if (f == _heap[icur]._f) return true;
		bool top = false;
		if (f < _heap[icur]._f) top = true;
		_heap[icur]._f = f;
		_heap[icur]._g = g;
		_heap[icur]._parent = parent;

		if (top) {
			move_top(icur);
		}else {
			move_bottom(icur);
		}

		//ensure();
		return true;
	}

	void ensure() {
		for (int i = 0; i < _sz; i++) {
			if (i*2+1 < _sz)
				assert(_heap[i]._f <= _heap[i*2+1]._f);
			if (i*2+2 < _sz)
				assert(_heap[i]._f <= _heap[i*2+2]._f);
		}
	}
};

struct AStarSearcher {

	static int heuristic_cost_estimate(int xcur, int ycur, int xend, int yend) {
		return abs(xcur - xend) + abs(ycur - yend);
	}
	static bool ensure(int cur, int parent, int h) {
		if (parent == -1) return true;
		for (int i = -1; i <=1; i++)
			for (int j = -1; j <= 1; j++)
				if (i*h+j == parent -cur)
					return true;
		return false;
	}
	template<size_t W, size_t H>
	static void Search(int xstart, int ystart, int xend, int yend, vector<vector<uint8>> &map) {
		// closed set
		vector<int> closed(W*H,-1);
		vector<int> g_closed(W*H);
		//opened set
		OpenedHeap<W,H> heap;

		int f = 0 + heuristic_cost_estimate(xstart, ystart, xend, yend);
		heap.push(OpenedNode(f,0,xstart,ystart,-1));

		while (true) {
			OpenedNode smallest;
			heap.pop(smallest);

			// put into closed set
			closed[TI(smallest._x,smallest._y)] = smallest._parent;
			g_closed[TI(smallest._x, smallest._y)] = smallest._f;

			// reach goal
			if (smallest._x == xend && smallest._y == yend) break;

			// Move
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++){
					if (!(i || j)) continue;
					int ni = i+smallest._x;
					int nj = j+smallest._y;

					if (!(ni >= 0 && ni < W && nj >=0 && nj < H && map[ni][nj] != WALL)) continue;
					if (smallest._parent == TI(ni,nj)) continue;
					
					int g = i && j ? smallest._g + 14:smallest._g + 10;
					int f = g + heuristic_cost_estimate(ni,nj,xend,yend);
					OpenedNode exist;
					if (!heap.Get(ni,nj,exist)) {
					
						if (closed[TI(ni,nj)] != -1 && g_closed[TI(ni,nj)] <= f) continue;
						
						exist = OpenedNode(f,g,ni,nj,TI(smallest._x,smallest._y));
						heap.push(exist);
					}else if (exist._f > f) {
						heap.make_smaller(ni,nj,f,g,TI(smallest._x,smallest._y));
					}
				}
		}

		int icur = TI(xend,yend);
		do{
			map[icur/H][icur%H] = PATH;
			icur = closed[icur];
		}while(icur != -1);
	}
};

int main(int argc, char *argv[]) {
	const int W = 35;
	const int H = 100;
	CaveMapGenerator cmg(W,H);
	cmg.generation(0.4, 4, 5);
	cmg.dump();

	vector<vector<uint8>> &map = cmg.get_map();
	int xstart, ystart;
	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++) {
			if (map[i][j] == FLOOR) {
				xstart = i;
				ystart = j;
				goto NEXT_1;
			}
		}
NEXT_1:
	int xend, yend;
	for (int i = W-1; i >= 0; i--)
		for (int j = H-1; j >=0; j--) {
			if (map[i][j] == FLOOR) {
				xend = i;
				yend = j;
				goto NEXT_2;
			}
		}
NEXT_2:
	printf("searching from (%d,%d) to (%d,%d)...\n",xstart,ystart,xend,yend);
	AStarSearcher::Search<W,H>(xstart,ystart,xend,yend,map);
	cmg.dump();
	return 0;
}