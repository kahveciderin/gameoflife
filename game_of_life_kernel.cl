__kernel void game_of_life(__global bool *A, __global bool *C) {
    
    // Get the index of the current element
    int i = get_global_id(0);

    bool debug = false;
    int gridsize = 512;

    int x = i % gridsize;
    int y = i / gridsize;

    int a = y - 1;
    int b = y + 1;

    int c = x - 1;
    int d = x + 1;

    int neighbours = 0;

    if (a < 0) {
        a = gridsize - 1;
    }
    if (b > gridsize - 1) {
        b = 0;
    }
    if (x == 0) {
        c = gridsize - 1;
    }
    if (x == gridsize - 1) {
        d = 0;
    }

    if (A[(a * gridsize) + c])
        neighbours++;
    if (A[(a * gridsize) + x])
        neighbours++;
    if (A[(a * gridsize) + d])
        neighbours++;
    if (A[(y * gridsize) + c])
        neighbours++;
    if (A[(y * gridsize) + d])
        neighbours++;
    if (A[(b * gridsize) + c])
        neighbours++;
    if (A[(b * gridsize) + x])
        neighbours++;
    if (A[(b * gridsize) + d])
        neighbours++;

    
    C[i] = A[i];
    if(neighbours == 3){
        C[i] = 1;
        if(debug) printf("i=%d x=%d y=%d a=%d b=%d c=%d d=%d n=%d stat=born\n", i, x, y, a, b, c, d, neighbours);
    }else if(neighbours == 2){
        if(debug) printf("i=%d x=%d y=%d a=%d b=%d c=%d d=%d n=%d stat=keep\n", i, x, y, a, b, c, d, neighbours);
    }else{
        C[i] = 0;
        if(debug) printf("i=%d x=%d y=%d a=%d b=%d c=%d d=%d n=%d stat=die\n", i, x, y, a, b, c, d, neighbours);
    }
}
