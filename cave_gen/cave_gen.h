#ifndef __CAVE_GEN_H__
#define __CAVE_GEN_H__

#include <vector>

using std::vector;
struct Cell {
	int _x, _y;
	Cell(int x=-1, int y=-1):_x(x),_y(y) {}
};

#define CELL_2_INT(c) (c._x * _h + c._y)
#define INT_2_CELL(i) Cell(i/_h, i%_h)
#define VALID_POS(x, y) ((x) >= 0 && (x) < _w && (y) >=0 && (y) < _h)

enum FEATURE {FLOOR, WALL, PATH};

typedef unsigned char uint8;

class CaveSets {
	vector<int> _father;
	int _h,_w;
public:
	CaveSets(int w, int h):_father(w*h, (int)0x80000000),_w(w),_h(h) {}

	void set_union(Cell left, Cell right) {
		int lr = set_find(CELL_2_INT(left));
		int rr = set_find(CELL_2_INT(right));
		if (lr == rr) return;
		// cell first union should init _father to -1
		if (_father[lr] == (int)0x80000000) _father[lr] = -1;
		if (_father[rr] == (int)0x80000000) _father[rr] = -1;

		if (_father[lr] < _father[rr]) {
			_father[rr] = lr;
		}else {
			if (_father[lr] == _father[rr])
				_father[rr]--;
			_father[lr] = rr;
		}
	}

	int set_find(int c) {
		if (_father[c] < 0) return c;
		_father[c] = set_find(_father[c]);
		return _father[c];
	}

	void all_cave_set(vector<int> &caves) {
		vector<int>::iterator iter = _father.begin();
		for (;iter != _father.end();iter++) {
			if (*iter < 0 && *iter != (int)0x80000000) caves.push_back(iter - _father.begin());
		}
	}
};

class CaveMapGenerator {
	vector<vector<uint8>> _cave_map;
	CaveSets _cave_sets;
	int _w, _h;
	Cell _center_cell;
protected:
	void init_map(double init_open_ratio) {
		_cave_map.resize(_w);
		for (int i = 0; i < _w; i++){
			_cave_map[i].resize(_h);
			for(int j = 0; j < _h; j++)
				_cave_map[i][j] = WALL;
		}
		int nopen = (int)(_h * _w * init_open_ratio);

		int ncard = _h*_w - 2*_w - 2*_h + 4;
		nopen = min(nopen, ncard);
		vector<int> cards(ncard);
		for (vector<int>::iterator iter = cards.begin(); iter != cards.end(); iter++)
			*iter = iter - cards.begin();
		for (int i = 0; i < nopen; i++) {
			int iselected = rand() % ncard;
			int ipos = cards[iselected];
			cards[iselected] = cards[ncard-1];
			ncard--;
			_cave_map[1+ipos/(_h-2)][1+ipos%(_h-2)] = FLOOR;
		}
		//dump();
	}

	int neighbour_walls(int x, int y) {
		int total = 0;
		for (int xoffset = -1; xoffset <= 1; xoffset++)
			for (int yoffset = -1; yoffset <= 1; yoffset++){
				if ((xoffset || yoffset) && VALID_POS(x+xoffset, y+yoffset) && _cave_map[x+xoffset][y+yoffset] == WALL) total++;
			}
			return total;
	}

	void cellular_automata(int low_rule_param, int up_rule_param) {
		for (int i = 1; i < _w-1; i++)
			for (int j = 1; j < _h-1; j++) {
				int nwall = neighbour_walls(i,j);
				if (_cave_map[i][j] == FLOOR) {
					if (nwall > up_rule_param) _cave_map[i][j] = WALL;
				}else if (nwall < low_rule_param) {
					_cave_map[i][j] = FLOOR;
				}
			}
			dump();
	}

	void make_cave() {
		for (int i = 0; i < _w; i++)
			for (int j = 0; j < _h; j++) {
				if (_cave_map[i][j] != FLOOR) continue;
				for (int xoffset = -1; xoffset <= 0; xoffset++)
					for (int yoffset = -1; yoffset <= 0; yoffset++) {
						if (!VALID_POS(i+xoffset,j+yoffset) || _cave_map[i+xoffset][j+yoffset] != FLOOR) continue;
						_cave_sets.set_union(Cell(i,j), Cell(i+xoffset,j+yoffset));
					}
			}
	}

	void connection() {
		vector<int> caves;
		_cave_sets.all_cave_set(caves);
		int ncave = caves.size();
		Cell c;

		for (int i = 0; i < ncave; i++){
			c = INT_2_CELL(caves[i]);
			while (true) {
				// move dir choose
				int x_dir, y_dir;
				if (c._x < _center_cell._x) x_dir = 1;
				else if (c._x > _center_cell._x) x_dir = -1;
				else x_dir = 0;
				if (c._y < _center_cell._y) y_dir = 1;
				else if (c._y > _center_cell._y) y_dir = -1;
				else y_dir = 0;

				// random move step
				Cell next_c;
				int r = rand()%3;
				switch (r) {
				case 0: next_c._x = c._x + x_dir; next_c._y = c._y; break;
				case 1: next_c._y = c._y + y_dir; next_c._x = c._x; break;
				case 2: next_c._x = c._x + x_dir; next_c._y = c._y + y_dir; break;
				}

				// whether can stop
				// all caves gather at center 
				if (_cave_map[next_c._x][next_c._y] == WALL) {
					_cave_map[next_c._x][next_c._y] = FLOOR;
					_cave_sets.set_union(c, next_c);
				}else {
					int cur_cave = _cave_sets.set_find(CELL_2_INT(c));
					int center_cave = _cave_sets.set_find(CELL_2_INT(_center_cell));
					if (cur_cave == center_cave) break;
					int next_cave = _cave_sets.set_find(CELL_2_INT(next_c));					
					if (cur_cave != next_cave) {
						_cave_sets.set_union(c, next_c);
						break;
					}
				}
				c = next_c;
			}
		}
	}

public:
	CaveMapGenerator(int w, int h): _w(w), _h(h), _cave_sets(w,h),_center_cell(w/2,h/2) {
	}

	void generation(double init_open_ratio, int low_rule_param, int up_rule_param) {
		if (_w <= 2 || _h <=2) return;
		init_map(init_open_ratio);
		cellular_automata(low_rule_param, up_rule_param);
		make_cave();
		connection();
	}
	void dump() {
		for (int i = 0; i < _w; i++) {
			for (int j = 0; j < _h; j++) {
				if (_cave_map[i][j] == FLOOR)
					cout << " ";
				else if (_cave_map[i][j] == WALL)
					cout << "*";
				else if (_cave_map[i][j] == PATH)
					cout << "-";
			}
			cout << endl;
		}
		cout << endl;
	}
	vector<vector<uint8>> get_map () { return _cave_map;}
};

#endif