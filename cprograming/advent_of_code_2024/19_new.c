#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_ways(char * target, char * substrings[]) {
	int len = strlen(target);
	int * counts = calloc(len, sizeof(int));
	counts[0] = 1;

	for (int i = 1; i < len; i++) {
		for (int j = 0; j < 8; j++) {
			int substring_length = strlen(substrings[j]);
			if (substring_length <= i) {
				if (strncmp(target + (i - substring_length), substrings[j], substring_length) == 0) {
					counts[i] += counts[i - substring_length];
				}
			} 
		}
	}

	int result = counts[len-1];
	for (int i = 0; i < len; i++) {
		printf("%d\n", counts[i]);
	}
	free(counts);
	return result;
}

int main() {
	char * substrings[] = {"r", "wr", "b", "g", "bwu", "rb", "gb", "br"};
	char * string = "brwrr";
	printf("Number of ways to form %s: %d\n", string ,count_ways(string, substrings));
}
