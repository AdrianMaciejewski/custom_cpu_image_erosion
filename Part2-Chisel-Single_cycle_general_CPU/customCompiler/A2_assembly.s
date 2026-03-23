#can be replaced with ADDI
ADDI x10, x0, 19
ADDI x11, x0, 20
ADDI x12, x0, 1
ADDI x13, x0, 255

#initialize at coordinates (0,0)
ADD x1, x0, x0	#x
ADD x2, x0, x0	#y
beq x0, x0, 14

7:
ADDI x2, x2, 1	#increment y
ADD x1, x0, x0	#reset x
bge x11, x2, 11 #end program if y>=20
END x0, x0, x0
beq x0, x0, 14	#else do another iteration

12:
### Initializing Loop Iteration
ADDI x1, x1, 1	#increment x
bge x1, x11, 7 #exit 12 if x>=20

14:
#save output pixel pointer
MULI x5, x2, 20		#y*20 (computed multiple times, could be optimised)
ADD x6, x5, x1		#x+y*20 (used later as input image memory pointer)
ADDI x3, x6, 400	#offset pointer by 400 to move to output array

### Processing Boarder Pixel
beq x1, x0, 22	#if on border continue
beq x1, x10, 22
beq x2, x0, 22
beq x2, x10, 22
beq x0, x0, 24	#else skip the continue
22:
sw x0, x3	#paint black
beq x0, x0, 12	#continue
#else
24:


### Processing Inner Pixel
lw x5, x6	#load input image pixel
bne x5, x0, 28
sw x0, x3	#if input pixel is black, save output pixel as black
beq x0, x0, 12	#continue
28:

#else (this can definitely be optimized)
#if pixel should be eroded
SUB x6, x1, x12	#x-1
MULI x5, x2, 20	#y*20
ADD x6, x5, x6	#(x-1)+y*20
lw x5, x6
beq x5, x0, 49
#repeat
ADD x6, x1, x12	#x+1
MULI x5, x2, 20	#y*20
ADD x6, x5, x6	#(x+1)+y*20
lw x5, x6
beq x5, x0, 49
#repeat
SUB x6, x2, x12	#y-1
MULI x5, x6, 20	#(y-1)*20
ADD x6, x5, x1	#x+(y-1)*20
lw x5, x6
beq x5, x0, 49
#repeat
ADD x6, x2, x12	#y+1
MULI x5, x6, 20	#(y+1)*20
ADD x6, x5, x1	#x+(y+1)*20
lw x5, x6
beq x5, x0, 49

#else do not 49
beq x0, x0, 51

49:
sw x0, x3
beq x0, x0, 12

51:
sw x13, x3
beq x0, x0, 12
