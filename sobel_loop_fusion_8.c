// This will apply the sobel filter and return the PSNR between the golden sobel and the produced sobel  (3.04 - 3.05)  xwris lu(3.12)
// sobelized image
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SIZE	4096
#define INPUT_FILE	"input.grey"
#define OUTPUT_FILE	"output_sobel.grey"
#define GOLDEN_FILE	"golden.grey"

/* The horizontal and vertical operators to be used in the sobel filter */
char horiz_operator[3][3] = {{-1, 0, 1}, 
                             {-2, 0, 2}, 
                             {-1, 0, 1}};
char vert_operator[3][3] = {{1, 2, 1}, 
                            {0, 0, 0}, 
                            {-1, -2, -1}};

//double sobel(unsigned char *input, unsigned char *output, unsigned char *golden);
//int convolution2D(int posy, int posx, const unsigned char *input, char operator[][3]);

/* The arrays holding the input image, the output image and the output used *
 * as golden standard. The luminosity (intensity) of each pixel in the      *
 * grayscale image is represented by a value between 0 and 255 (an unsigned *
 * character). The arrays (and the files) contain these values in row-major *
 * order (element after element within each row and row after row. 			*/
unsigned char input[SIZE*SIZE], output[SIZE*SIZE], golden[SIZE*SIZE];

/*double M = SIZE*SIZE; Instead of doing this multiple many times inside the code i will do it once and i will just use the result fro my calculations */
/*but as i see the multiple 4096 * 4096 = 2^12 + 2^12 . that means that i can replace it with s shift operation because its faster.*/

double M = 1<< 24;
int sizeMinusOne = SIZE-1;

/* Implement a 2D convolution of the matrix with the operator */
/* posy and posx correspond to the vertical and horizontal disposition of the *
 * pixel we process in the original image, input is the input image and       *
 * operator the operator we apply (horizontal or vertical). The function ret. *
 * value is the convolution of the operator with the neighboring pixels of the*
 * pixel we process.														  */



/* The main computational function of the program. The input, output and *
 * golden arguments are pointers to the arrays used to store the input   *
 * image, the output produced by the algorithm and the output used as    *
 * golden standard for the comparisons.									 */


int main(int argc, char* argv[])
{
//double PSNR;
	double PSNR = 0, t;
	int i, j ,x ,y ,z ,jMinusOne, jPlusOne;
	unsigned int p ,p1 ,p2;
	int res;
	struct timespec  tv1, tv2;
	FILE *f_in, *f_out, *f_golden;

	/* The first and last row of the output array, as well as the first  *
     * and last element of each column are not going to be filled by the *
     * algorithm, therefore make sure to initialize them with 0s.		 */
	memset(output, 0, SIZE*sizeof(unsigned char));
	memset(&output[SIZE*(SIZE-1)], 0, SIZE*sizeof(unsigned char));
	for (i = 1; i < SIZE-1; i+=5) {
		output[i*SIZE] = 0;
		output[i*SIZE + SIZE - 1] = 0;
		output[(i+1)*SIZE] = 0;
		output[(i+1)*SIZE + SIZE -1] = 0;
		output[(i+2)*SIZE] = 0;
		output[(i+2)*SIZE + SIZE -1] = 0;
		output[(i+3)*SIZE] = 0;
		output[(i+3)*SIZE + SIZE -1] = 0;
		output[(i+4)*SIZE] = 0;
		output[(i+4)*SIZE + SIZE -1] = 0;
	}

	/* Open the input, output, golden files, read the input and golden    *
     * and store them to the corresponding arrays.						  */
	f_in = fopen(INPUT_FILE, "r");
	if (f_in == NULL) {
		printf("File " INPUT_FILE " not found\n");
		exit(1);
	}
  
	f_out = fopen(OUTPUT_FILE, "wb");
	if (f_out == NULL) {
		printf("File " OUTPUT_FILE " could not be created\n");
		fclose(f_in);
		exit(1);
	}  
  
	f_golden = fopen(GOLDEN_FILE, "r");
	if (f_golden == NULL) {
		printf("File " GOLDEN_FILE " not found\n");
		fclose(f_in);
		fclose(f_out);
		exit(1);
	}    

	fread(input, sizeof(unsigned char), M, f_in);
	fread(golden, sizeof(unsigned char), M, f_golden);
	fclose(f_in);
	fclose(f_golden);
  
	/* This is the main computation. Get the starting time. */
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv1);
	/* For each pixel of the output image */
	for (i=1; i<sizeMinusOne; i+=1 ) {             
		
		x = (i-1)*SIZE;				/*to reduce time ,the operations between i and SIZE happen many times inside the for(j=1...) loop*/
		y = i*SIZE;					/* i will cal*/
		z = (i+1)*SIZE;

		for (j=1; j<sizeMinusOne; j+=1) {
			/* Apply the sobel filter and calculate the magnitude *
			 * of the derivative.								  */

			jMinusOne = j-1;
			jPlusOne = j+1;
			/*we observ that we have many operations that do not affect the final res, so i will try to make less operations as above ... */
			res = 0;
			res = input[x + jMinusOne]*(-1) + input[x + jPlusOne] + input[y+ jMinusOne]*(-2) + input[y + jPlusOne]*2 + input[z + jMinusOne]*(-1) + input[z + jPlusOne];
			
			p1= res*res;

			res = 0;
			res = input[x + jMinusOne] + input[x +j]*2 + input[x + jPlusOne] + input[z + jMinusOne]*(-1) + input[z + j]*(-2) + input[z + jPlusOne]*(-1);

			p2 = res*res;

			p = p1 +p2;
			res = (int)sqrt(p);
			/* If the resulting value is greater than 255, clip it *
			 * to 255.											   */
			if (res > 255)
				output[y + j] = 255;      
			else
				output[y + j] = (unsigned char)res;

			/*As we see we have the same two nested loops with the same iterations. So with the technique of 'loop fusion' we can unite these by putting *
			* the second one's body inside the first's one           */
			t = (output[y+j] - golden[y+j]) *(output[y+j] - golden[y+j]);
			PSNR += t;
		}
	}


	/* Now run through the output and the golden output to calculate *
	 * the MSE and then the PSNR.									 */
	/*for (i=1; i<sizeMinusOne; i++) {
		y = i*SIZE;	
		for ( j=1; j<sizeMinusOne; j++ ) {
			t = (output[y+j] - golden[y+j]) *(output[y+j] - golden[y+j]);
			PSNR += t;
		}
	}*/
  	
	PSNR /= (double)(M);
	PSNR = 10*log10(65536/PSNR);

	/* This is the end of the main computation. Take the end time,  *
	 * calculate the duration of the computation and report it. 	*/
	clock_gettime(CLOCK_MONOTONIC_RAW, &tv2);

	printf ("Total time = %10g seconds\n",
			(double) (tv2.tv_nsec - tv1.tv_nsec) / 1000000000.0 +
			(double) (tv2.tv_sec - tv1.tv_sec));

  
	/* Write the output file */
	fwrite(output, sizeof(unsigned char), SIZE*SIZE, f_out);
	fclose(f_out);
	//PSNR = sobel(input, output, golden);
	printf("PSNR of original Sobel and computed Sobel image: %g\n", PSNR);
	printf("A visualization of the sobel filter can be found at " OUTPUT_FILE ", or you can run 'make image' to get the jpg\n");

	return 0;
}
