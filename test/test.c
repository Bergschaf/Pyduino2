#include <stdio.h>

//adds two to an integer and returns the result
int adder2(int a) {
    return a + 2;
}

int getSmallest(int x, int y) {
    int smallest;
    if ( x > y ) { //if (conditional)
        smallest = y; //then statement
    }
    else {
        smallest = x; //else statement
    }
    return smallest;
}

int simplemath() {
    int result = 0;
    result = 2+3;
}


int main(void) {
    int x = 40;
    int y = 2;
    printf("%d" ,simplemath(x,y));
    x = adder2(x) + getSmallest(x,2);
    printf("x is: %d\n", x);
    printf("uuuuuuuuuuuuuuuuuuuuu");
    return 0;
}
