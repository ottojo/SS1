#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#define DEBUG

struct Node {
	struct Node *k[4];
	bool color;
	int size;
};

struct Node* newLeaf(int size, bool color) {
	struct Node *nK = calloc(1, sizeof(struct Node));
	nK->color = color;
	nK->size = size;
	return nK;
}

bool isLeaf(struct Node *k) {
	// One child is 0 <=> every child is 0
	return k->k[0] == 0;
}

void setColor(struct Node *root, int row, int col, bool color) {
#ifdef DEBUG
	printf("setColor called on Node with size %d, to set color at %d, %d to %d.\n", root->size, row, col, color);
#endif
	if (root->size == 1) {
		root->color = color;
		return;
	}

	if (!(root->k[0] || root->k[1] || root->k[2] || root->k[3])) {
		// Current node is a leaf
		if (root->color == color) {
			// We are at leaf that already has new color
			// Dont create a node with 4 equivalent children
			return;
		}

		// At this point, we have to divide the current leaf.
		for (int i = 0; i < 4; ++i) {
			root->k[i] = newLeaf(root->size >> 1, root->color); 
		}
	}	

	// now we are not at size 1 and the current node is not a leaf (anymore)
	// set color recursively	
	struct Node *target;
	if (row < root->size >> 1) {
		if (col < root->size >> 1) {
			// insert top left
			target = root->k[0];
		} else {
			// insert top right
			target = root->k[1];
		}
	} else {
		if (col < root->size >> 1) {
			// insert bottom left
			target = root->k[2];
		} else {
			// insert bottom right
			target = root->k[3];
		}
	}
	
	setColor(target, row % (root->size >> 1), col % (root->size >> 1), color);

	// Merge if all children are leafs with the same color
	bool allChildrenLeaf = true;
	for (int i = 0; i < 4; i++) {
		if (!isLeaf(root->k[i])) {
			allChildrenLeaf = false;
			break;
		}
	}

	if (allChildrenLeaf) {
		bool sameColor = true;
		for (int i = 0; i < 4; ++i) {
			if (root->k[i]->color != color) {
				sameColor = false;
				break;
			}
		}

		if (sameColor) {
#ifdef DEBUG
			printf("All children are leafs with the same color now. Optimizing.\n");
#endif
			root->color = color;
			for (int i = 0; i < 4; ++i) {
				free(root->k[i]);
				root->k[i] = 0;
			}
		}
	}
}

void printNodeText(struct Node *k) {
	if (k->k[0] || k->k[1] || k->k[2] || k->k[3]) {
		// Not a leaf
		printf("[");
		for (int i = 0; i < 4; ++i) {
			printNodeText(k->k[i]);
		}
		printf("]");
	} else {
		// Leaf
		printf(k->color ? "W" : "B");
	}
}

bool colorAt(struct Node *root, int row, int col) {
	if (isLeaf(root)) {
		return root->color;
	} else {
		struct Node *target;
		if (row < root->size >> 1) {
			if (col < root->size >> 1) {
				// top left
				target = root->k[0];
			} else {
				// top right
				target = root->k[1];
			}
		} else {
			if (col < root->size >> 1) {
				// bottom left
				target = root->k[2];
			} else {
				// bottom right
				target = root->k[3];
			}
		}

		return colorAt(target, row % (root->size >> 1), col % (root->size >> 1));
	}
}

void printNodeASCII(struct Node *root) {
	for (int row = 0; row < root->size; ++row) {
		for (int col = 0; col < root->size; ++col) {
			printf(colorAt(root, row, col) ? " " : "#");
		}
		printf("\n");
	}
}

bool hasPrefix(const char* s, const char* prefix) {
#ifdef DEBUG
	printf("testing if \"%s\" has prefix \"%s\".\n", s, prefix);
#endif
	while (*prefix != 0) {
		if (*prefix != *s) {
			return false;
		}
		++prefix;
		++s;
	}
	return true;
}

int main() {
	int size = 0;
	if (scanf("%d", &size) != 1) {
		printf("Size expected.\n");
		exit(1);
	}
	struct Node *root = newLeaf(size, 0);
	while (true) {
		char command[100];
		int status = scanf("%s", command);
		if (status < 1) {
			if (status == EOF) {
				exit(0);
			}
			printf("Error\n");
			continue;
		}
			
		if (hasPrefix(command, "PUT")) {
			int row = 0;
			int col = 0;
			while(scanf("%d %d", &row, &col) != 2) {
				printf("PUT row col\n");
			}
			setColor(root, row, col, 1);
		} else if (hasPrefix(command, "PRINT")) {
			if (scanf("%s", command) != 1){
				printf("Usage: PRINT <IMAGE/TREE>");
				continue;
			}
			if (hasPrefix(command, "TREE")) {
				printNodeText(root);
				printf("\n");
			} else if (hasPrefix(command, "IMAGE")) {
				printNodeASCII(root);
				sleep(1);
			}
		} else if (hasPrefix(command, "EXIT")) {
			exit(0);
		} else {
			printf("Invalid command: \"%s\"\n", command);
		}
	}
}

