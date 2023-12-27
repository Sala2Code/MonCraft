float modFloat(float a, float b){
    float mod;

    mod = (a>0) ? a : -a;
    b = (b>0) ? b : -b;
 
    while (mod >= b)
        mod = mod - b;
 
    return (a > 0) ? mod : -mod+b ;
}