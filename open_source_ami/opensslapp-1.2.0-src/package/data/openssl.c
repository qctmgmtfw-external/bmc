#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "apps.h"
int req_main(int argc, char **argv);
int x509_main(int argc, char **argv);

void print_usage()
{
    printf("Usage\n");
    printf(" ./openssl req  : Generate a CSR (Certificate Signing Request)\n");
    printf(" ./openssl x509 : Generating a Self-Signed Certificate\n");
    printf(" ./openssl -h   : Help\n");
    printf("\nCommand Help\n");
    printf(" Eg: ./openssl req -h\n");
}
int main(int argc, char **argv)
{
    int ret;
    
    if(argc == 1) {
        print_usage();
        return 0;
    }
    argc--;
    argv++;
    if(strcmp(argv[0],"req")==0)
        ret=req_main(argc,argv);
    else
        if(strcmp(argv[0],"x509")==0)
            ret=x509_main(argc,argv);
        else
        {   
            if(strcmp(argv[0],"-h")!=0)
                printf("\nInvalid command:\n");
            print_usage();
        }
    return 0;
}
