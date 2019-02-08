/*	Author: 		Shaurya Pratap Singh
	Roll Number:	160102364
	Department: 	B.Tech, Computer Science and Engineering
	Section: 		CSE - D
	University:		DIT University, Dehradun
	Contact:		shauryaraynor@gmail.com

	Objective:		Generation of a Forward / Divided Difference Table in the form of a 'Linked Table' as opposed to
					static/dynamic two dimensional array and writing it to a .csv file.

	Advantage:		The pointers from each cell to the others enable an easier application of methods of interpolation
					such as Gaussian, Stirling and Bessel's Interpolation Techniques. Runtime is also improved since we do
					not have to perform index checks required by a two dimensional array. The main advantage lies over the
					fact that we can soft compute which interpolation technique will give us the best result by	the means 
					of first finding position of 'x' in the data-set and then, accordingly finding the longest chain in 
					linked list traversal.

	Disadvantage:	Each cell takes a space of 64bits as opposed to 32bits in a two dimensional array, because of the meta-
					data of the pointers to other cells. Elements cannot be randomly accessed.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct Node{
	float data;
	struct Node *down;
	struct Node *top_right;
	struct Node *down_right;
}Variable;


typedef struct Table{
	int size;
	float x_data;
	float fx_data;
	Variable *zero;
}Difference_Table;


Variable *create_variable(float);
Difference_Table *create_difference_table(int);
void input_data(Difference_Table **);
void compute_forward_difference(Difference_Table *);
void compute_divided_difference(Difference_Table *);
void display_difference_table(Difference_Table *);
void write_to_file(Difference_Table *, const char*);


int factorial(int);
float power(float, int);
float absolute(float);
float term_calculator(float, int, int);


void newton_forward_interpolation(Difference_Table *);
void newton_backward_interpolation(Difference_Table *);
void newton_divided_difference_interpolation(Difference_Table *D);
void stirling_iterpolation(Difference_Table *);
void bessel_interpolation(Difference_Table *);
void gauss_forward_interolation(Difference_Table *);
void gauss_backward_interolation(Difference_Table *);


Variable *create_variable(float data){
	Variable *new_variable = (Variable *) malloc (sizeof(Variable));
	new_variable -> data = data;
	new_variable -> down = NULL;
	new_variable -> top_right = NULL;
	new_variable -> down_right = NULL;
	return new_variable;
}

	
Difference_Table *create_difference_table(int size){
	Difference_Table *new_difference_table = (Difference_Table *) malloc (sizeof(Difference_Table));
	new_difference_table -> size = size;
	new_difference_table -> x_data = 0;
	new_difference_table -> fx_data = 0;
	new_difference_table -> zero = NULL;
	return new_difference_table;
}


void input_data(Difference_Table **diff_table_ref){
	int i;
	float temp_data;
	Variable *new_variable_x, *new_variable_y, *zero_ref_copy;
	printf("\nInput Column Data x, y:\n");
	for(i = 0; i < (*diff_table_ref) -> size; i ++){
		scanf("%f", &temp_data);
		new_variable_x = create_variable(temp_data);
		scanf("%f", &temp_data);
		new_variable_y = create_variable(temp_data);
		new_variable_x -> down_right = new_variable_y;
		if(i == 0){
			(*diff_table_ref) -> zero = new_variable_x;
			zero_ref_copy = new_variable_x;
		}
		else{
			zero_ref_copy -> down = new_variable_x;
			zero_ref_copy -> down_right -> down = new_variable_y;
			zero_ref_copy = new_variable_x;
		}
	}
}


void compute_forward_difference(Difference_Table *diff_table_ref){
	float temp_data;
	Variable *new_variable, *prev_variable, *zero_y_ref = diff_table_ref -> zero -> down_right;
	Variable *zero_ref_copy = zero_y_ref;
	while(zero_y_ref -> down != NULL){
		prev_variable = NULL;
		while(zero_ref_copy -> down != NULL){
			temp_data = zero_ref_copy -> down -> data -  zero_ref_copy -> data;
			new_variable = create_variable(temp_data);
			if(prev_variable != NULL) prev_variable -> down = new_variable;
			prev_variable = new_variable;
			zero_ref_copy -> down_right = new_variable;
			zero_ref_copy -> down -> top_right = new_variable;
			zero_ref_copy = zero_ref_copy -> down;
		}
		zero_ref_copy = zero_y_ref -> down_right;
		zero_y_ref = zero_y_ref -> down_right;
	}
}


void compute_divided_difference(Difference_Table *diff_table_ref){
	int i = 1, j;
	float temp_data;
	Variable *new_variable, *prev_variable;
	Variable *zero_ref_num = diff_table_ref -> zero -> down_right;
	Variable *zero_ref_copy_num = zero_ref_num;
	Variable *zero_ref_deno = diff_table_ref -> zero;
	Variable *deno_start, *deno_end;
	while(zero_ref_num -> down != NULL){
		j = 1;
		prev_variable = NULL;
		deno_start = zero_ref_deno;
		deno_end = zero_ref_deno;
		while(j <= i){
			deno_end = deno_end -> down;
			j++;
		}
		while(zero_ref_copy_num -> down != NULL){
			temp_data = (zero_ref_copy_num -> down -> data - zero_ref_copy_num -> data) / (deno_end -> data - deno_start -> data);
			new_variable = create_variable(temp_data);
			if(prev_variable != NULL) prev_variable -> down = new_variable;
			prev_variable = new_variable;
			zero_ref_copy_num -> down_right = new_variable;
			zero_ref_copy_num -> down -> top_right = new_variable;
			zero_ref_copy_num = zero_ref_copy_num -> down;
			deno_start = deno_start -> down;
			deno_end = deno_end -> down;
		}
		zero_ref_copy_num = zero_ref_num -> down_right;
		zero_ref_num = zero_ref_num -> down_right;
		i++;
	}
}


void display_difference_table(Difference_Table *diff_table_ref){
	int i;
	char var[10];
	Variable *zero_ref = diff_table_ref -> zero;
	Variable *zero_ref_copy = zero_ref;
	for(i = 0; i <= diff_table_ref -> size; i++){
		if(i == 0) printf("\nSr.%13s", "x");
		else if(i == 1) printf("%15s","y");
		else{
			sprintf(var, "D%dy", i - 2);
			printf("%15s", var);
		}
	}
	printf("\n\n");
	i = 1;
	while(zero_ref != NULL){
		printf("%d",i++);
		while(zero_ref_copy != NULL){
			printf("%15.6f", zero_ref_copy -> data);
			zero_ref_copy = zero_ref_copy -> down_right;
		}
		printf("\n");
		zero_ref = zero_ref -> down;
		zero_ref_copy = zero_ref;
	}
	printf("\nx    = %15.6f", diff_table_ref -> x_data);
	printf("\nf(x) = %15.6f", diff_table_ref -> fx_data);
	printf("\n\n");
}


void write_to_file(Difference_Table *diff_table_ref, const char *mode){
	int i;
	char var[10];
	Variable *zero_ref = diff_table_ref -> zero;
	Variable *zero_ref_copy = zero_ref;
	FILE *fp;
	fp = fopen("Difference_Table.csv", mode);
	for(i = 0; i <= diff_table_ref -> size; i++){
		if(i == 0) fprintf(fp, "Sr.,%13s", "x,");
		else if(i == 1) fprintf(fp, "%15s","y,");
		else{
			sprintf(var, "D%dy", i - 2);
			fprintf(fp, "%15s,", var);
		}
	}
	fprintf(fp, "\n\n");
	i = 1;
	while(zero_ref != NULL){
		fprintf(fp,"%d,",i++);
		while(zero_ref_copy != NULL){
			fprintf(fp, "%15.6f,", zero_ref_copy -> data);
			zero_ref_copy = zero_ref_copy -> down_right;
		}
		fprintf(fp, "\n");
		zero_ref = zero_ref -> down;
		zero_ref_copy = zero_ref;
	}
	fprintf(fp, "\nx, %15.6f", diff_table_ref -> x_data);
	fprintf(fp, "\nf(x), %15.6f", diff_table_ref -> fx_data);
	fprintf(fp, "\n\n");
	fclose(fp);
}


int factorial(int n){
	if(n > 1) return n * factorial(n -1);
	else return 1;
}	


float power(float n, int p){
	if(p > 1) return n * power(n, p - 1);
	else return n;
}


float absolute(float n){
	if(n < 0) return -n;
	else return n;
}


float term_calculator(float a, int iteration, int mode){
	int i = 1, p = 1, q = 0;
	float term = a;	
	switch(mode){
		case 1:	while(i < iteration) term *= (a - i++);
				break;
		case 2: while(i < iteration) term *= (a + i++);
				break;
		case 3: while(i < iteration){
					if(i % 2 != 0) term *= a;
					else term *= (power(a,2) - power(p++,2))/a;
					i++;
				}
				break;
		case 4: term = a - 0.5;
				while(i < iteration){
					if(i % 2 == 0) term *= a - 0.5;
					else term *= (a + q++) * (a - p++) / (a - 0.5);
					i++;
				}
		case 5: while(i < iteration){
					if(i % 2 !=0) term *= a - p;
					else term *= a + p++;
					i++;
				}
				break;
		case 6: while(i < iteration){
					if(i % 2 !=0) term *= a + p;
					else term *= a - p++;
					i++;
				}
				break;
	}	
	return term/factorial(iteration);
}


void newton_forward_interpolation(Difference_Table *D){
	int i = 1;
	float a = (D -> x_data - D -> zero -> data) / (D -> zero -> down -> data - D -> zero -> data);
	Variable *zero_y_ref = D -> zero -> down_right;
	D -> fx_data = zero_y_ref -> data;
	while(zero_y_ref -> down_right != NULL){
		zero_y_ref = zero_y_ref -> down_right;
		D -> fx_data += zero_y_ref -> data * term_calculator(a, i++, 1);
	}
}


void newton_backward_interpolation(Difference_Table *D){
	int i = 1;
	float a;
	Variable *last_x_ref = D -> zero;
	Variable *last_y_ref = D -> zero -> down_right;
	while(last_y_ref -> down != NULL){
		last_x_ref = last_x_ref -> down;
		last_y_ref = last_y_ref -> down;
	}
	a = (D -> x_data - last_x_ref -> data) / (D -> zero -> down -> data - D -> zero -> data);
	D -> fx_data = last_y_ref -> data;
	while(last_y_ref -> top_right != NULL){
		last_y_ref = last_y_ref -> top_right;
		D -> fx_data += last_y_ref -> data * term_calculator(a, i++, 2);
	}
}


void newton_divided_difference_interpolation(Difference_Table *D){
	int i, iteration = 0;
	float temp;
	Variable *zero_y_ref = D -> zero -> down_right;
	Variable *zero_x_ref;
	while(zero_y_ref != NULL){
		i = 0;
		zero_x_ref = D -> zero;
		temp = zero_y_ref -> data;
		while(i < iteration){
			temp *= D -> x_data - zero_x_ref -> data;
			zero_x_ref = zero_x_ref -> down;
			i++;
		}
		D -> fx_data += temp;	
		zero_y_ref = zero_y_ref -> down_right;
		iteration ++;	
	}
}


void stirling_iterpolation(Difference_Table *D){
	int i = 1;
	float a;
	Variable *zero_x_ref = D -> zero;
	Variable *x0, *y0;
	while(zero_x_ref -> data < D -> x_data){
		x0 = zero_x_ref;
		zero_x_ref = zero_x_ref -> down;
	}
	if(absolute(D -> x_data - x0 -> data) > absolute(D -> x_data - x0 -> down -> data)) x0 = x0 -> down;
	a = (D -> x_data - x0 -> data) / (D -> zero -> down -> data - D -> zero -> data);
	y0 = x0 -> down_right;
	D -> fx_data = y0 -> data;
	while(y0 -> top_right != NULL && y0 -> down_right !=  NULL){
		if(i % 2 == 0) D -> fx_data += y0 -> data * term_calculator(a, i, 3);
		else{
			D -> fx_data+= term_calculator(a, i, 3) * (y0 -> top_right -> data + y0 -> down_right -> data) / 2;
			y0 = y0 -> top_right -> down_right;
		}
		i++;
	}
	D -> fx_data += y0 -> data * term_calculator(a, i, 3);
}


void bessel_interpolation(Difference_Table *D){
	int i = 1;
	Variable *x0, *x1, *y0, *y1;
	float a;
	Variable *zero_x_ref = D -> zero;
	while(zero_x_ref -> data < D -> x_data){
		x0 = zero_x_ref;
		zero_x_ref = zero_x_ref -> down;
	}
	x1 = x0 -> down;
	y0 = x0 -> down_right;
	y1 = x1 -> down_right;
	a = (D -> x_data - x0 -> data) / (x1 -> data - x0 -> data);
	D -> fx_data = (y0 -> data + y1 -> data) / 2;
	while(y0 != NULL || y1 != NULL){
		if(i % 2 == 0) D -> fx_data += term_calculator(a, i, 4) * (y0 -> data + y1 -> data) / 2;
		else{
			D -> fx_data += y0 -> down_right -> data * term_calculator(a, i, 4);
			y0 = y0 -> down_right -> top_right;
			y1 = y1 -> top_right -> down_right;
		}
		i++;
	}
}


void gauss_forward_interolation(Difference_Table *D){
	int i = 1;
	float a;
	Variable *zero_x_ref = D -> zero;
	Variable *x0, *y0;
	while(zero_x_ref -> data < D -> x_data){
		x0 = zero_x_ref;
		zero_x_ref = zero_x_ref -> down;
	}
	y0 = x0 -> down_right;
	D -> fx_data = y0 -> data;
	a = (D -> x_data - x0 -> data) / (x0 -> down -> data - x0 -> data);
	y0 = y0 -> down_right;
	while(y0 != NULL){
		if(i % 2 != 0){
			D -> fx_data += y0 -> data * term_calculator(a, i, 5);
			y0 = y0 -> top_right;
		}
		else{
			D -> fx_data += y0 -> data * term_calculator(a, i, 5);
			y0 = y0 -> down_right;
		}
		i++;
	}
}


void gauss_backward_interolation(Difference_Table *D){
	int i = 1;
	float a;
	Variable *zero_x_ref = D -> zero;
	Variable *x0, *y0;
	while(zero_x_ref -> data < D -> x_data){
		x0 = zero_x_ref;
		zero_x_ref = zero_x_ref -> down;
	}
	x0 = x0 -> down;
	a = (D -> x_data - x0 -> data) / (x0 -> down -> data - x0 -> data);
	y0 = x0 -> down_right;
	D -> fx_data = y0 -> data;
	y0 = y0 -> top_right;
	while(y0 != NULL){
		if(i % 2 != 0){
			D -> fx_data += y0 -> data * term_calculator(a, i, 6);
			y0 = y0 -> down_right;
		}
		else{
			D -> fx_data += y0 -> data * term_calculator(a, i, 6);
			y0 = y0 -> top_right;
		}
		i++;
	}
}


int main(int argc, char **argv){
	Difference_Table *D = create_difference_table(atoi(argv[1]));
	D -> x_data = atof(argv[2]);
	input_data(&D);
	compute_forward_difference(D);
	//compute_divided_difference(D);
	//newton_forward_interpolation(D);
	//newton_backward_interpolation(D);
	//newton_divided_difference_interpolation(D);
	stirling_iterpolation(D);
	//bessel_interpolation(D);
	//gauss_forward_interolation(D);
	//gauss_backward_interolation(D);
	display_difference_table(D);
	//write_to_file(D, "w+");
	return 0;
}
