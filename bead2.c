#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(){
    int numBeads;
        char temp1[705];
    char temp2[705];    

        int i,j,k,lim;

    int count1 = 0;
        int count2 = 0;
    int maxcount1 = 0;

        char virgin = ' ';
    bool flag = false; //flag == true if  virgin doesn't match

        FILE* fin  = fopen("beads.in","r");
    FILE* fout = fopen("beads.out","w");

        fscanf(fin,"%d",&numBeads);
    fscanf(fin,"%s",temp1);

        strcpy(temp2,temp1);
    strcat(temp1,temp2);

        for(i=0,j=numBeads-1;i<numBeads;i++,j++){
        count1 =0;
        count2 = 0;
        flag = false;
        virgin = ' ';
        for(k=i;flag==false && k < (i+numBeads);k++){
            if(temp1[k]=='w'){
                    count1++;
                }
                else if(temp1[k]=='r'){
                if(virgin==' '){
                 virgin = 'r';               
                 }
                if(virgin=='r')
                    count1++;
                else{
                    flag = true;        
                        k--;
                }
            }
            else if(temp1[k]=='b'){
                    if(virgin==' '){
                     virgin = 'b';               
                     }
                    if(virgin=='b')
                        count1++;
                    else{
                        flag = true;        
                    k--;
                    }
                }
            }

        /* Block 2*/
        lim = k;
        flag = false;
        virgin = ' ';       
        for(k=j;flag==false && k < (j+numBeads) && k>=lim;k--){
            if(temp1[k]=='w'){
                    count2++;
                }
                else if(temp1[k]=='r'){
                if(virgin==' '){
                 virgin = 'r';               
                 }
                if(virgin=='r')
                    count2++;
                else{
                    flag = true;        
                        k--;
                }
            }
            else if(temp1[k]=='b'){
                    if(virgin==' '){
                     virgin = 'b';               
                     }
                    if(virgin=='b')
                        count2++;
                    else{
                        flag = true;        
                    k--;
                    }
                }
            }
        if(maxcount1 < (count1+ count2))maxcount1 = count1 + count2;
    }

    fprintf(fout,"%d\n",maxcount1);
        return 0;
}
