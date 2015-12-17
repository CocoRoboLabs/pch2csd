// Function that adds Instr to orc part according to Nord patch
#include <stdio.h>
#include <stdbool.h>

extern FILE *NewFile;
extern FILE *TempFile;

extern unsigned int CCa,CCk;

extern char TempFileName[40];
extern char TempModuleMap[40];
extern char TempModuleIO[40];

extern unsigned int aIOTable[256][6]; // a-cable list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;
extern unsigned int kIOTable[256][6];// k-cable list
                                //0-location (VA or FX); 1-cable ID; 2-module from; 3-port from;
                                //4-module to;  5-port to;


extern unsigned int ParameterCountersVA[128]; // ������� ���������� �� �������� � �����, ����� �� ������������ � �������� �� ���������� �������� ����� ������� ������������ (Tables[...])
                                                //for VA field
extern unsigned int ParameterCountersFX[128]; // same for FX field

extern unsigned int MapTablesVA[128][24]; // ������� �������� � ���� ��� ������� ������������� ������ ������ ������� Tables, �.�. ������ ��������� map-���� ��� ���������������� ������,
                                            // ��� �������� ��������������� ���� ������������ ����������, ��������, 1 - �������
extern unsigned int MapTablesFX[128][24]; // ����������

extern unsigned int ParametersVA[128][64]; // VA parameters
extern unsigned int ParametersFX[128][64]; // FX parameters

extern unsigned int ModuleCounter; // Module Counter

extern unsigned int ModuleIndexListVA[1024]; // VA modules index list
extern unsigned int ModuleIndexListFX[1024]; // same for FX

extern float Tables[128][24]; // Parameter mapping table

extern bool VAFXFlag; // VA or FX flag


int GenInstrumentContent(unsigned int number)
{
    const unsigned int L=40;
    unsigned int tempnumber;
    unsigned int i,j,k;
    char NAME[L];
    unsigned int NAMELength=0;
    char TEMP[L];
    unsigned int counter=1;
    char temp;
    unsigned int valueINT;
    unsigned int mapid;
    float value[64];
    unsigned int IOTemp[100];
    unsigned int IOCount=0;
    unsigned int IOtemp;
    unsigned int IO[50]; // Input = 0; Output = 0;
    unsigned int N;
    unsigned int NIO[50]; // Input = 0; Output = 0;
    unsigned int IOAK[50]; // Audio = 1; Control =0;
    bool FFlag;
    bool PPflag; // �������� �������� �� ��� ��� ��������� � ������ �����������, ��� �� ������ ������� ��� ��� �������

    tempnumber=number;

    //printf("Number = %d\n",number);
    printf("ModuleCounter = %d\n",ModuleCounter);

    while(true)
    {
        tempnumber=(tempnumber-tempnumber%10)/10;
        if(tempnumber==0)
        {
            break;
        }
        counter++;
    }

    for(i=0;i<counter;i++)
    {
        tempnumber=number;

        for(j=0;j<counter-i-1;j++)
        {
            tempnumber=tempnumber/10;
        }
        tempnumber=tempnumber%10;
        TempFileName[8+i]=(char)(48+tempnumber);
        TempModuleMap[5+i]=(char)(48+tempnumber);
        TempModuleIO[3+i]=(char)(48+tempnumber);

    }

    //Module Name
    TempFileName[counter+8]=0x2e;
    TempFileName[counter+9]=0x74;
    TempFileName[counter+10]=0x78;
    TempFileName[counter+11]=0x74;
    TempFileName[counter+12]=0x0;

     //Mapping file name
    TempModuleMap[counter+5]=0x2e;
    TempModuleMap[counter+6]=0x74;
    TempModuleMap[counter+7]=0x78;
    TempModuleMap[counter+8]=0x74;
    TempModuleMap[counter+9]=0x0;

    //������������ ����� ����� � ������������ ������ � �������
    TempModuleIO[counter+3]=0x2e;
    TempModuleIO[counter+4]=0x74;
    TempModuleIO[counter+5]=0x78;
    TempModuleIO[counter+6]=0x74;
    TempModuleIO[counter+7]=0x0;

    PPflag=false;


    //�� ����� ������ ��� �������� ������� ������ ��� ��������� ������ ���
    if((TempFile = fopen(TempFileName,"r")) == NULL)
	{
		printf("Error - ");
		printf(TempFileName);
		printf(" not opened!\n");
		return 0;
	}
	else
	{
	    fseek(TempFile, 0, SEEK_SET);
	    for(i=0;i<L;i++)
        {
            if(fread(&temp,1,1,TempFile) != 0)
            {
                TEMP[i]=temp;
                if(temp==0x2c)
                {
                    for(j=i-1;j>0;j--)
                    {
                        if((TEMP[j]==0x09)||(TEMP[j]==0x20))
                        {
                            for(k=j;k<i;k++)
                            {
                                NAME[k-j]=TEMP[k];
                                NAMELength++;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            else
            {
                break;
            }
        }
        fclose(TempFile);
	}
	fprintf(NewFile,"\t");
	//printf("%d\n",NAMELength);
	// �������� ��� � ������ �������������� �����
	for(i=0;i<NAMELength;i++)
    {
        fprintf(NewFile,"%c",NAME[i]);
    }
    fprintf(NewFile," ");


    // �������� ������� ��������� map-���� � ��������� �� ���� ���� ���������� ��� ������ �� �������� �������
    // ������� ��� �������� ������� ������ �������� ����������, ���������� ��� ������� �� ���������� � ������� ������������, �� ��� ��������� ��������� �������, ������� ����� �� ���-�����
    // ���������� ��������� � ������������� ����
    if(VAFXFlag)
    {

        if((TempFile = fopen(TempModuleMap,"r")) == NULL)
        {
            printf("Error - ");
            printf(TempModuleMap);
            printf(" not opened!\n");
            return 0;
        }
        else
        {
            for(i=0;i<ParameterCountersVA[ModuleCounter];i++)
            {
                fscanf(TempFile, "%d", &mapid);
                MapTablesVA[ModuleCounter][i]=mapid;
                valueINT=ParametersVA[ModuleCounter][i];
                value[i]=Tables[valueINT][mapid];
            }
        }

        printf("Parameter count = ");
        printf("%d\n",ParameterCountersVA[ModuleCounter]);

        for(i=0;i<ParameterCountersVA[ModuleCounter];i++)
        {
            fprintf(NewFile,"%1.3f",value[i]);
            PPflag=true;
            if(i!=ParameterCountersVA[ModuleCounter]-1)
            {
                fprintf(NewFile,", ");
            }
        }
        //fprintf(NewFile,"\n");
    }
    else
    {
        if((TempFile = fopen(TempModuleMap,"r")) == NULL)
        {
            printf("Error - ");
            printf(TempModuleMap);
            printf(" not opened!\n");
            return 0;
        }
        else
        {
            for(i=0;i<ParameterCountersFX[ModuleCounter];i++)
            {
                fscanf(TempFile, "%d", &mapid);
                MapTablesFX[ModuleCounter][i]=mapid;
                valueINT=ParametersFX[ModuleCounter][i];
                value[i]=Tables[valueINT][mapid];
            }
        }

        printf("Parameter count = ");
        printf("%d\n",ParameterCountersFX[ModuleCounter]);

        for(i=0;i<ParameterCountersFX[ModuleCounter];i++)
        {
            fprintf(NewFile,"%1.3f",value[i]);
            PPflag=true;
            if(i!=ParameterCountersFX[ModuleCounter]-1)
            {
                fprintf(NewFile,", ");
            }
        }
        //fprintf(NewFile,"\n");
    }

    // �� � ������ ����� �������� - ����������
    // ����������� ������� ����������� ���� IO �� ������� � ������� � �� ������ ��� ��������� ������

    // ������ ������� ����(0)/�����(1)
    // ������ ������� k(0)/a(1)

    if((TempFile = fopen(TempModuleIO,"r")) == NULL)
    {
        printf("Error - ");
        printf(TempModuleIO);
        printf(" not opened!\n");
        return 0;
    }
    else
    {
        while(true)
        {
            if(fscanf(TempFile, "%d", &IOtemp) == EOF)
            {
                break;
                fclose(TempFile);
            }
            else
            {
                IOTemp[IOCount]=IOtemp;
                IOCount++;
            }
        }
        for(i=0;i<IOCount;i++)
        {
            if(i%2==0)
            {
                IO[i/2]=IOTemp[i];
            }
            else
            {
                IOAK[(i-1)/2]=IOTemp[i];
            }

        }

        N=0;
        for(i=0;i<IOCount/2;i++)
        {
            if(IO[i]==0)
            {
                NIO[i]=N;
                N++;
            }
        }

        N=0;
        for(i=0;i<IOCount/2;i++)
        {
            if(IO[i]==1)
            {
                NIO[i]=N;
                N++;
            }
        }
    }

    if(IOCount>0)
    {
        if(PPflag)
        {
            fprintf(NewFile,", ");
        }
    }

    // ����� ���� ��� ���� ��������� ����� � ������ � ���������� �������� � �������� ������ � ������� NIO, ��� ������� ��������������� ������ ������ � ������
    // ���������� �������� �����, ��� ������������ ��������� ���� ������� � ��������� �� ������� �� ������ � �������
    // ��� ������ ��� ��������� ���������� � ������� � ������, ��� ������������ ������ � ���������� � zak-������������
    // ���� ���������������� ������� �� �����, �� ���� ��� ����, �� ��� ������ �� ������� ������ - �������� ����� (gnd)
    // ���� �� ��� �����, �� �� 1 - ���������

    if(VAFXFlag) //VAArea
    {
        for(i=0;i<IOCount/2;i++)
        {
            FFlag=false;
            if(IOAK[i]==1) // ��� ������ - ��������
            {
                if(IO[i]==0) // ���� ��� ����
                {
                    //printf("IN\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],aIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][5]);
                        if(aIOTable[j][4]==ModuleIndexListVA[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(aIOTable[j][5]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==1) // ������� ������ ���� VA
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // ���� ��� �����
                {
                    //printf("OUT\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],aIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][3]);
                        if(aIOTable[j][2]==ModuleIndexListVA[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(aIOTable[j][3]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==1) // ������� ������ ���� VA
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); //���� ���� �� �������� �� ����� "0"/���� ����� �� � ���� "1"
                    if(i!=IOCount/2-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
            else // ��� ������ - ����������
            {
                if(IO[i]==0) // ���� ��� ����
                {
                    printf("IN\n");
                    for(j=0;j<CCk;j++)
                    {
                        printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],kIOTable[j][4]);
                        printf("Ports %d %d\n",NIO[i],kIOTable[j][5]);
                        if(kIOTable[j][4]==ModuleIndexListVA[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(kIOTable[j][5]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==1) // ������� ������ ���� VA
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // ���� ��� �����
                {
                    printf("OUT\n");
                    for(j=0;j<CCk;j++)
                    {
                        printf("Moduls %d %d\n",ModuleIndexListVA[ModuleCounter],kIOTable[j][2]);
                        printf("Ports %d %d\n",NIO[i],kIOTable[j][3]);
                        if(kIOTable[j][2]==ModuleIndexListVA[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(kIOTable[j][3]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==1) // ������� ������ ���� VA
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); //���� ���� �� �������� �� ����� "0"/���� ����� �� � ���� "1"
                    if(i!=IOCount/2-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
        }
    }
    else // FXArea
    {
        for(i=0;i<IOCount/2;i++)
        {
            FFlag=false;
            if(IOAK[i]==1) // ��� ������ - ��������
            {
                if(IO[i]==0) // ���� ��� ����
                {
                    //printf("IN\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],aIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][5]);
                        if(aIOTable[j][4]==ModuleIndexListFX[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(aIOTable[j][5]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==0) // ������� ������ ���� FX
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // ���� ��� �����
                {
                    printf("OUT\n");
                    for(j=0;j<CCa;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],aIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],aIOTable[j][3]);
                        if(aIOTable[j][2]==ModuleIndexListFX[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(aIOTable[j][3]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==0) // ������� ������ ���� FX
                                {
                                    fprintf(NewFile,"%d",(aIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    //fprintf(NewFile,"%d",IO[i]); //���� ���� �� �������� �� ����� "0"/���� ����� �� � ���� "1"
                    if(i!=IOCount/2-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
            else // ��� ������ - ����������
            {
                if(IO[i]==0) // ���� ��� ����
                {
                    printf("IN\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],kIOTable[j][4]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][5]);
                        if(kIOTable[j][4]==ModuleIndexListFX[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(kIOTable[j][5]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==0) // ������� ������ ���� FX
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else // ���� ��� �����
                {
                    //printf("OUT\n");
                    for(j=0;j<CCk;j++)
                    {
                        //printf("Moduls %d %d\n",ModuleIndexListFX[ModuleCounter],kIOTable[j][2]);
                        //printf("Ports %d %d\n",NIO[i],kIOTable[j][3]);
                        if(kIOTable[j][2]==ModuleIndexListFX[ModuleCounter]) //������ ���� ������ ��������� � ������� ������
                        {
                            if(kIOTable[j][3]==NIO[i]) //���� ������ ��������� � ������� ����� �� IO ����� (�.�. � "i")
                            {
                                if(aIOTable[j][0]==0) // ������� ������ ���� FX
                                {
                                    fprintf(NewFile,"%d",(kIOTable[j][1]+2)); //���������� � ���� ����� ������
                                    if(i!=IOCount/2-1)
                                    {
                                        fprintf(NewFile,", ");
                                    }
                                    FFlag=true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(FFlag==false)
                {
                    fprintf(NewFile,"%d",IO[i]); //���� ���� �� �������� �� ����� "0"/���� ����� �� � ���� "1"
                    if(i!=IOCount/2-1)
                    {
                        fprintf(NewFile,", ");
                    }
                }
            }
        }
    }

    //printf("InstrIO \n");

    return 1;
}
