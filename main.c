#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>

/**
 * This program should be used after an eventual reverse engineering analysis of the binary file.
 * It should be launched with four arguments:
 *    binary_file      : the name of the target file
 *    PIN_length       : the length of the PIN
 *    iteration_number : the higher this number, the more accurate the PIN code result will be 
 *    latency          : the latency after a number pass   
 *
 * @author: Mohammed Ahmed
 * @version: 1.0
 *
 */


/**
 * Convert an array of integers to an integer
 *
 * @param array
 * @param n size of the array
 * @return x integer result
 */
unsigned long int_arr(unsigned long* array, size_t n) {
  int l;
  unsigned long x;
  x = 0;
  for (l = 0 ; l < n; l++) {
    if (l==0)
      x += array[l];
    else {
      x *= 10;
      x += array[l];
    }
  }
  return x;
}

/**
 * Determines the index number that has the most occurrences in an array
 *
 * @param array
 * @param n          length of the array
 * @return i         the index of the most recurrent number
 */
int arr_occ(unsigned long* array, size_t n) {
  int occ[n];
  int max_occ = 0;
  int max_occ_i, i, k;
  for (i = 0; i < n; i++) {
    occ[i] = 1;
    for (k = i+1; k < n; k++) {
      if (array[i] == array[k])
	occ[i] += 1;
      if (max_occ < occ[i]) {
	max_occ = occ[i];
	max_occ_i = i;
      }
    }
  }
  return max_occ_i;
}
  
int main(int argc, char** argv) {
  

  char res_stringified[50];  
  char *endptr1, *endptr2, *endptr3;  
  unsigned long i, j, k, n, m, latency;
  unsigned long *res, *arr_res;   
  unsigned long normal_time;
  unsigned long min_time_spent = INT_MAX;
  struct timeval begin, end;
  
  
  //Errors management
  if (argc != 5) {
    puts("Please insert 5 arguments: binary_file PIN_length iteration_number latency");
    return EXIT_FAILURE;
  }
  if ( fopen(argv[1],"r") == NULL ) {
    perror("Please specify an existent file");
    return EXIT_FAILURE;
  }  
  n = strtol(argv[2], &endptr1, 10);
  m = strtol(argv[3], &endptr2, 10);
  latency = strtol(argv[4], &endptr3, 10);
  if ( endptr1 == argv[2] || endptr2 == argv[3] || endptr3 == argv[4] ) {
    puts("Please insert integer values for the second and third and fourth arguments");
    return EXIT_FAILURE;
  }
  
  res = (unsigned long *) malloc(n * sizeof(unsigned long));
  arr_res = (unsigned long *) malloc(m * sizeof(unsigned long));
  for (i = 0; i < n ; i++) res[i] = 0;

  //Determination of the minimal execution time of the binary file
  //calibrated with iterations
  for (k = 0; k < 5; k++) {
    char exe[1024] = "echo \"a\"|./";
    strcat(exe, argv[1]);
    gettimeofday(&begin, NULL);
    system(exe);
    gettimeofday(&end, NULL);
    normal_time = (end.tv_usec - begin.tv_usec);
    
    if (normal_time < min_time_spent)
      min_time_spent = normal_time;
  }
  //printf("%ld here\n", min_time_spent);
  
  for (k = 0; k < m; k++) {
    for (i = 0 ; i < n ; i++) {      
      for (j = 0 ; j <= 9 ; j++) {
	res[i] = j;
	
	//Bash command: echo "PIN"|./binary_file	
	sprintf(res_stringified, "%ld",int_arr(res, n));
	char exe[1024] = "echo \"";
	strcat(exe, res_stringified);
	strcat(exe, "\"|./");
	strcat(exe, argv[1]);
	//printf("%s\n",exe);
	
	//Determines if the current number exceed the minimum execution time with latency
	gettimeofday(&begin, NULL);
	system(exe);
	gettimeofday(&end, NULL);
	unsigned long time_spent = end.tv_usec - begin.tv_usec;
	//printf("nb: %d id: %d %d\n", k, j, time_spent);
	if (time_spent >= (min_time_spent + (i+1)*latency)) {
	  break;
	}
      }
    }
    arr_res[k] = int_arr(res,n);
    printf("id: %ld, pin: %ld\n", k, int_arr(res,n));
  }
  
  //Display the PIN code
  printf("Code PIN: %ld\n", arr_res[arr_occ(arr_res, n)]);
  
  return 0;
}
