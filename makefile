


.phony = all clean 


all : myScript.out



clean : 
	@rm rm  -i myScript.out



myScript.out : myScript.c
	@gcc myScript.c -o myScript.out

