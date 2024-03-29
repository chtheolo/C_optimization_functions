# Makefile. If you change it, remember than in makefiles multiple spaces
# ARE NOT EQUIVALENT to tabs. The line after a rule starts with a tab!

#Add any executable you want to be created here.
EXECUTABLES	= sobel_loop_inter_1  sobel_function_inlining_2  sobel_strength_reduction_3  loopUnrolling_4   elements_Arrays_5  
		  reduce_Arithmetic_Operations_6  sobel_common_sub_7   sobel_loop_fusion_8

#This is the compiler to use
CC = icc

#These are the flags passed to the compiler. Change accordingly
CFLAGS = -Wall -O0

#These are the flags passed to the linker. Nothing in our case
LDFLAGS = -lm


# make all will create all executables
all: $(EXECUTABLES)

# This is the rule to create any executable from the corresponding .c 
# file with the same name.
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# make clean will remove all executables, jpg files and the 
# output of previous executions.
clean:
	rm -f $(EXECUTABLES) *.jpg output_sobel.grey

# make image will create the output_sobel.jpg from the output_sobel.grey. 
# Remember to change this rule if you change the name of the output file.
image: output_sobel.grey
	convert -depth 8 -size 4096x4096 GRAY:golden.grey golden_sobel.jpg 

