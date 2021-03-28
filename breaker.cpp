#pragma warning(disable:4996)
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <numeric>
#include <set>
#define MAX_TEXT_LENGTH 1000
#define MAX_KEY_LENGTH 10
#define RANGE_SEARCH_E 8
#define ALPHABET 26
using namespace std;

typedef struct TRI {
	int cnt;
	int appear[100];
	int idx;
}TRI;

void read_ci(void);
void anal_tri(void);
void anal_mono(void);
void decrypt(void);
void fitness_calc(void);
void find_key(int a);

char text_ci[MAX_TEXT_LENGTH];
char text_pl[MAX_TEXT_LENGTH];

int len_ci;
int len_key;

TRI freq_ci_tri[ALPHABET][ALPHABET][ALPHABET];
int freq_ci_mono_[MAX_KEY_LENGTH][ALPHABET];
set <pair<int, int>, greater<pair<int, char>> > freq_ci_mono[10];
char position_e[MAX_KEY_LENGTH];
char real_position_e[MAX_KEY_LENGTH];

char trigrams_[] = "theandingentionherforthanthinteretioterestersatihatatealleth"; 
char trigrams[ALPHABET][ALPHABET][ALPHABET];

int max_score;

int main(void) {
	for (int i = 0; i < 60; i = i + 3) {
		trigrams[trigrams_[i] - 'a'][trigrams_[i + 1] - 'a'][trigrams_[i + 2] - 'a'] = 1;
	}

	read_ci();
	anal_tri();

	printf("Enter key length: ");
	scanf("%d", &len_key);
	printf("\n");
	getchar();
	anal_mono();
	find_key(len_key);

	printf("----------------message--------------\n");
	for (int i = 0; i < len_ci; i++) {
		text_pl[i] = (text_ci[i] - (real_position_e[i % len_key] - 'a') + 26) % 26 + 'a';
	}
	printf("%s\n\nkey: ", text_pl);
	for (int i = 0; i < len_key; i++) {
		printf("%c", (text_ci[i] - text_pl[i] +'a' + 26) % 26 + 'a');
	}
	
	int flag = 1;
	while (flag) {
		printf("\nDo you want to change the key?  yes = 1 or no = 0 \n");
		scanf("%d", &flag);
		getchar();
		if (flag == 1) {
			printf("Enter your key: ");
			char ch[MAX_KEY_LENGTH] = { 0 };
			for (int i = 0; i < len_key; i++) {
				scanf("%c", ch+i);
			}
			getchar();
			for (int i = 0; i < len_ci; i++) {
				text_pl[i] = (text_ci[i] - ch[i%len_key] + 'a' + 26) % 26 + 'a';
			}
			printf("%s\n\n", text_pl);
		}
		else {
			printf("end..\n");
			break;
		}
	}

	return 0;
}

void fitness_calc(void) {
	int sum = 0;
	for (int i = 2; i < len_ci; i++) {
		sum += trigrams[text_pl[i - 2] - 'a'][text_pl[i - 1] - 'a'][text_pl[i] - 'a'];
	}
	if (max_score < sum) {
		max_score = sum;
		for (int i = 0; i < len_key; i++) {
			real_position_e[i] = position_e[i];
		}
	}
}

void find_key(int a) {
	if (a == 0) {
		decrypt();
		fitness_calc();
	}
	int j = 0;
	for (auto i : freq_ci_mono[len_key - a]) {
		if (j == RANGE_SEARCH_E) return;
		position_e[len_key - a] = i.second;
		find_key(a - 1);
		j++;
	}
}

void decrypt(void) {
	for (int i = 0; i < len_ci; i++) {
		text_pl[i] = (text_ci[i] - (position_e[i%len_key] - 'a') + 26) % 26 + 'a';
	}
}

void anal_mono(void) {
	for (int i = 0; i < len_ci; i++) {
		freq_ci_mono_[i % len_key][text_ci[i]]++;
	}

	for (int i = 0; i < len_key; i++) {
		for (int j = 0; j < 26; j++) {
			freq_ci_mono[i].insert({ freq_ci_mono_[i][j], j });
		}
	}
	
	for (int i = 0; i < len_key; i++) {
		printf("#%d \n", i);
		for (auto j : freq_ci_mono[i]) {
			printf("%c : %d, ", j.second + 'a', j.first);
		}
		printf("\n\n");
	}
} 

void anal_tri(void) {
	for (int i = 2; i < len_ci; i++) {
		freq_ci_tri[text_ci[i - 2]][text_ci[i - 1]][text_ci[i]].cnt++;
		freq_ci_tri[text_ci[i - 2]][text_ci[i - 1]][text_ci[i]].
			appear[freq_ci_tri[text_ci[i - 2]][text_ci[i - 1]][text_ci[i]].idx] = i;
		freq_ci_tri[text_ci[i - 2]][text_ci[i - 1]][text_ci[i]].idx++;
	}

	for (int a = 0; a < 26; a++) {
		for (int b = 0; b < 26; b++) {
			for (int c = 0; c < 26; c++) {
				if (freq_ci_tri[a][b][c].cnt <= 2) continue;
				
				printf("%c%c%c   count = %d,  distance: ", a + 'a', b + 'a', c + 'a', freq_ci_tri[a][b][c].cnt);
				int s = freq_ci_tri[a][b][c].appear[0];
				for (int i = 1; i < freq_ci_tri[a][b][c].cnt; i++) {
					printf("%d ", freq_ci_tri[a][b][c].appear[i] - s);
					s = freq_ci_tri[a][b][c].appear[i];
				}
				printf("\n");
			}
		}
	}
}

void read_ci(void) {
	FILE* fp = fopen("ciphertext.txt", "rt");
	
	char ch;
	while (1) {
		ch = fgetc(fp);

		if (ch == -1) break;
		
		if (ch >= 'A' && ch <= 'Z') {
			text_ci[len_ci] = ch - 'A';
			len_ci++;
		}
	}
	len_ci--;
}