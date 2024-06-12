/*########################################################
# Name: ntToBit
#   - Had tables to convert nucleodtides to an bit value
#     that includes an extra bit to report errors
#   - The error bit can be ignored after checking or if
#     it is known their will be no error
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - guards and defined variables
'   o tbl01: ntToFiveBit
'     - table to convert bases to five bit values, with an
'       extra sixth bit acting as an error
'   o tbl02: ntToFive
'     - table to convert bases to four bit (a/t/g/c only)
'   o tbl03: ntToTwoBit
'     - table to convert bases to four bit (a/t/g/c only)
'       values to two bits with the 3rd bit saved for an
'       error/anonymous base
'   o license:
'     - licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - guards and defined variables
\-------------------------------------------------------*/

#ifndef NUCLEOTIDE_TO_BIT_H
#define NUCLEOTIDE_TO_BIT_H

#define a_fourBit 1
#define c_fourBit 1 << 1
#define g_fourBit 1 << 2
#define t_fourBit 1 << 3
#define n_fiveBit 1 << 4  /*Marks an anonymous base*/
#define err_sixBit 1 << 5

#define err_fiveBit 1 << 4 /*For four bit only*/

#define a_twoBit 0
#define c_twoBit 1
#define g_twoBit 2
#define t_twoBit 3
#define err_threeBit 1 << 2

/*-------------------------------------------------------\
| Tbl-01: ntToFiveBit
|  - table to convert bases to five bit values (a/c/g/t/
|    is anonymous), with an extra sixth bit acting as an
|    error flag
|    o 1st bit is A (a_fourBit)
|    o 2nd bit is C (c_fourBit)
|    o 3rd bit is G (g_fourBit)
|    o 4th bit is T/U (t_fourBit)
|    o 5th bit marks anonymous base (n_fiveBit)
|    o 6th bit is error (err_sixBit)
\-------------------------------------------------------*/
static
unsigned char ntToFiveBit[] =
{ /*ntToFiveBit*/
   /*White space/invisible charactes block*/
   err_sixBit, /*0   = Null character*/

   err_sixBit, /*1   = Start of Heading*/
   err_sixBit, /*2   = Start of Text*/
   err_sixBit, /*3   = End of Text*/
   err_sixBit, /*4   = End of Transmission*/
   err_sixBit, /*5   = Enquiry*/
   err_sixBit, /*6   = Acknowledge*/
   err_sixBit, /*7   = Bell*/
   err_sixBit, /*8   = Backspace*/

   err_sixBit, /*9   =  tab (horizontal)*/
   err_sixBit, /*10  = New line*/

   err_sixBit, /*11  = Vertical Tab (not key)*/
   err_sixBit, /*12  = Form Feed*/

   err_sixBit, /*13  = Carriage Return*/

   err_sixBit, /*14  = Shift Out*/
   err_sixBit, /*15  = Shift In*/
   err_sixBit, /*16  = Data Link Escape*/
   err_sixBit, /*17  = Device Control One*/
   err_sixBit, /*18  = Device Control Two*/
   err_sixBit, /*19  = Device Contol Three*/
   err_sixBit, /*20  = Device Control Four*/
   err_sixBit, /*21  = Negative Acknowledge*/
   err_sixBit, /*22  = Synchronous Idle*/
   err_sixBit, /*23  = End Transmission Block*/
   err_sixBit, /*24  = Cancle*/
   err_sixBit, /*25  = End of medium*/
   err_sixBit, /*26  = Substitute*/
   err_sixBit, /*27  = escape*/
   err_sixBit, /*28  = File Separator*/
   err_sixBit, /*29  = Group Separator*/
   err_sixBit, /*30  = Record Separator*/
   err_sixBit, /*31  = Unit Separator*/

   /*symbol/number block*/
   err_sixBit, /*32  = space*/
   err_sixBit, /*33  = !*/
   err_sixBit, /*34  = "*/
   err_sixBit, /*35  = #*/
   err_sixBit, /*36  = $*/
   err_sixBit, /*37  = %*/
   err_sixBit, /*38  = &*/
   err_sixBit, /*39  = '*/
   err_sixBit, /*40  = (*/
   err_sixBit, /*41  = )*/
   err_sixBit, /*42  = **/
   err_sixBit, /*43  = +*/
   err_sixBit, /*44  = ,*/
   err_sixBit, /*45  = -*/
   err_sixBit, /*46  = .*/
   err_sixBit, /*47  = /*/
   err_sixBit, /*48  = 0*/
   err_sixBit, /*49  = 1*/
   err_sixBit, /*50  = 2*/
   err_sixBit, /*51  = 3*/
   err_sixBit, /*52  = 4*/
   err_sixBit, /*53  = 5*/
   err_sixBit, /*54  = 6*/
   err_sixBit, /*55  = 7*/
   err_sixBit, /*56  = 8*/
   err_sixBit, /*57  = 9*/
   err_sixBit, /*58  = :*/
   err_sixBit, /*59  = ;*/
   err_sixBit, /*60  = <*/
   err_sixBit, /*61  = =*/
   err_sixBit, /*62  = >*/
   err_sixBit, /*63  = ?*/
   err_sixBit, /*64  = @*/

   /*Uppercase letters*/
   a_fourBit,   /*65  = A*/

   /*66=B; C/G/T*/
   (n_fiveBit | c_fourBit | g_fourBit | t_fourBit),

   c_fourBit,   /*67  = C*/

   /*68=D; A/G/T*/
   (n_fiveBit | a_fourBit | g_fourBit | t_fourBit),

   err_sixBit, /*69  = E not nucleotide*/
   err_sixBit, /*70  = F not nucleotide*/
   g_fourBit,   /*71  = G*/

   /*72=H; A/G/T*/
   (n_fiveBit | a_fourBit | c_fourBit | t_fourBit),

   err_sixBit, /*73  = I not nucleotide*/
   err_sixBit, /*74  = J not nucleotide*/
   (n_fiveBit | g_fourBit | t_fourBit), /*75=K; G/T*/
   err_sixBit, /*76  = L not nucleotide*/
   (n_fiveBit | a_fourBit | c_fourBit), /*77=M; A/C*/

   /*78=N; any base*/
   (   n_fiveBit
     | a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*Any base (N)*/

   err_sixBit, /*79  = O not nucleotide*/
   err_sixBit, /*80  = P not nucleotide*/
   err_sixBit, /*81  = Q not nucleotide*/
   (n_fiveBit | a_fourBit | g_fourBit),   /*82=R A/G*/
   (n_fiveBit | c_fourBit | g_fourBit),   /*83=S C/G*/
   t_fourBit,   /*84  = T*/
   t_fourBit,   /*85  = U*/

   /*86  = V (ACG), treat as N*/
   (n_fiveBit | a_fourBit | c_fourBit | g_fourBit),

   (n_fiveBit | a_fourBit | t_fourBit), /*87=W A\T*/

   /*88=X; any aa (I will hold same for nucelotides)*/
   (   n_fiveBit
     | a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*X*/

   (n_fiveBit | c_fourBit | t_fourBit),   /*89=Y C/T*/
   err_sixBit, /*90  = Z not nucleotide*/

   /*Special characters after upercase letters*/
   err_sixBit, /*91  = [*/
   err_sixBit, /*92  = \*/
   err_sixBit, /*93  = ]*/
   err_sixBit, /*94  = ^*/
   err_sixBit, /*95  = _*/
   err_sixBit, /*96  = `*/

   /*lower case letters*/
   a_fourBit,   /*97=a*/

   /*98=b; C/G/T*/
   (n_fiveBit | c_fourBit | g_fourBit | t_fourBit),

   c_fourBit,   /*99  = c*/

   /*100=d; A/G/T*/
   (n_fiveBit | a_fourBit | g_fourBit | t_fourBit),

   err_sixBit, /*101 = e not nucleotide*/
   err_sixBit, /*102 = f not nucleotide*/
   g_fourBit,   /*103 = g*/

   /*104=h; A/G/T*/
   (n_fiveBit | a_fourBit | c_fourBit | t_fourBit),

   err_sixBit, /*105 = i not nucleotide*/
   err_sixBit, /*106 = j not nucleotide*/
   (n_fiveBit | g_fourBit | t_fourBit), /*107=k; G/T*/
   err_sixBit, /*108  = l not nucleotide*/
   (n_fiveBit | a_fourBit | c_fourBit), /*109=m; A/C*/

   /*110=n; any base*/
   (   n_fiveBit
     | a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*Any base (n)*/

   err_sixBit, /*111 = o not nucleotide*/
   err_sixBit, /*112 = p not nucleotide*/
   err_sixBit, /*113 = q not nucleotide*/
   (n_fiveBit | a_fourBit | g_fourBit),   /*114=r A/G*/
   (n_fiveBit | c_fourBit | g_fourBit),   /*115=s C/G*/
   t_fourBit,   /*116 = t*/
   t_fourBit,   /*117 = u*/

   /*118 = v (ACG), treat as N*/
   (n_fiveBit | a_fourBit | c_fourBit | g_fourBit),

   (n_fiveBit | a_fourBit | t_fourBit), /*119=w A\T*/

   /*120=x; any aa (I will hold same for nucelotides)*/
   (   n_fiveBit
     | a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*x*/

   (n_fiveBit | c_fourBit | t_fourBit),   /*121=y C/T*/
   err_sixBit, /*122 = z not nucleotide*/

   /*Special characters after lowercase letters*/
   err_sixBit, /*123 = {*/
   err_sixBit, /*124 = |*/
   err_sixBit, /*125 = }*/
   err_sixBit, /*126 = ~*/
   err_sixBit, /*127 = Del*/
}; /*ntToFiveBit*/

/*-------------------------------------------------------\
| Tbl-02: ntToFourBit
|  - table to convert bases to four bit (a/t/g/c only)
|    values, with the extra five bit acting as an error
|    o 1st bit is A (a_fourBit)
|    o 2nd bit is C (c_fourBit)
|    o 3rd bit is G (g_fourBit)
|    o 4th bit is T/U (t_fourBit)
\-------------------------------------------------------*/
static
unsigned char ntToFourBit[] =
{ /*ntToFourBit*/
   /*White space/invisible charactes block*/
   err_fiveBit, /*0   = Null character*/

   err_fiveBit, /*1   = Start of Heading*/
   err_fiveBit, /*2   = Start of Text*/
   err_fiveBit, /*3   = End of Text*/
   err_fiveBit, /*4   = End of Transmission*/
   err_fiveBit, /*5   = Enquiry*/
   err_fiveBit, /*6   = Acknowledge*/
   err_fiveBit, /*7   = Bell*/
   err_fiveBit, /*8   = Backspace*/

   err_fiveBit, /*9   =  tab (horizontal)*/
   err_fiveBit, /*10  = New line*/

   err_fiveBit, /*11  = Vertical Tab (not key)*/
   err_fiveBit, /*12  = Form Feed*/

   err_fiveBit, /*13  = Carriage Return*/

   err_fiveBit, /*14  = Shift Out*/
   err_fiveBit, /*15  = Shift In*/
   err_fiveBit, /*16  = Data Link Escape*/
   err_fiveBit, /*17  = Device Control One*/
   err_fiveBit, /*18  = Device Control Two*/
   err_fiveBit, /*19  = Device Contol Three*/
   err_fiveBit, /*20  = Device Control Four*/
   err_fiveBit, /*21  = Negative Acknowledge*/
   err_fiveBit, /*22  = Synchronous Idle*/
   err_fiveBit, /*23  = End Transmission Block*/
   err_fiveBit, /*24  = Cancle*/
   err_fiveBit, /*25  = End of medium*/
   err_fiveBit, /*26  = Substitute*/
   err_fiveBit, /*27  = escape*/
   err_fiveBit, /*28  = File Separator*/
   err_fiveBit, /*29  = Group Separator*/
   err_fiveBit, /*30  = Record Separator*/
   err_fiveBit, /*31  = Unit Separator*/

   /*symbol/number block*/
   err_fiveBit, /*32  = space*/
   err_fiveBit, /*33  = !*/
   err_fiveBit, /*34  = "*/
   err_fiveBit, /*35  = #*/
   err_fiveBit, /*36  = $*/
   err_fiveBit, /*37  = %*/
   err_fiveBit, /*38  = &*/
   err_fiveBit, /*39  = '*/
   err_fiveBit, /*40  = (*/
   err_fiveBit, /*41  = )*/
   err_fiveBit, /*42  = **/
   err_fiveBit, /*43  = +*/
   err_fiveBit, /*44  = ,*/
   err_fiveBit, /*45  = -*/
   err_fiveBit, /*46  = .*/
   err_fiveBit, /*47  = /*/
   err_fiveBit, /*48  = 0*/
   err_fiveBit, /*49  = 1*/
   err_fiveBit, /*50  = 2*/
   err_fiveBit, /*51  = 3*/
   err_fiveBit, /*52  = 4*/
   err_fiveBit, /*53  = 5*/
   err_fiveBit, /*54  = 6*/
   err_fiveBit, /*55  = 7*/
   err_fiveBit, /*56  = 8*/
   err_fiveBit, /*57  = 9*/
   err_fiveBit, /*58  = :*/
   err_fiveBit, /*59  = ;*/
   err_fiveBit, /*60  = <*/
   err_fiveBit, /*61  = =*/
   err_fiveBit, /*62  = >*/
   err_fiveBit, /*63  = ?*/
   err_fiveBit, /*64  = @*/

   /*Uppercase letters*/
   a_fourBit,   /*65  = A*/

   /*66=B; C/G/T*/
   (c_fourBit | g_fourBit | t_fourBit),

   c_fourBit,   /*67  = C*/

   /*68=D; A/G/T*/
   (a_fourBit | g_fourBit | t_fourBit),

   err_fiveBit, /*69  = E not nucleotide*/
   err_fiveBit, /*70  = F not nucleotide*/
   g_fourBit,   /*71  = G*/

   /*72=H; A/G/T*/
   (a_fourBit | c_fourBit | t_fourBit),

   err_fiveBit, /*73  = I not nucleotide*/
   err_fiveBit, /*74  = J not nucleotide*/
   (g_fourBit | t_fourBit), /*75=K; G/T*/
   err_fiveBit, /*76  = L not nucleotide*/
   (a_fourBit | c_fourBit), /*77=M; A/C*/

   /*78=N; any base*/
   (
       a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*Any base (N)*/

   err_fiveBit, /*79  = O not nucleotide*/
   err_fiveBit, /*80  = P not nucleotide*/
   err_fiveBit, /*81  = Q not nucleotide*/
   (a_fourBit | g_fourBit),   /*82=R A/G*/
   (c_fourBit | g_fourBit),   /*83=S C/G*/
   t_fourBit,   /*84  = T*/
   t_fourBit,   /*85  = U*/

   /*86  = V (ACG), treat as N*/
   (a_fourBit | c_fourBit | g_fourBit),

   (a_fourBit | t_fourBit), /*87=W A\T*/

   /*88=X; any aa (I will hold same for nucelotides)*/
   (
       a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*X*/

   (c_fourBit | t_fourBit),   /*89=Y C/T*/
   err_fiveBit, /*90  = Z not nucleotide*/

   /*Special characters after upercase letters*/
   err_fiveBit, /*91  = [*/
   err_fiveBit, /*92  = \*/
   err_fiveBit, /*93  = ]*/
   err_fiveBit, /*94  = ^*/
   err_fiveBit, /*95  = _*/
   err_fiveBit, /*96  = `*/

   /*lower case letters*/
   a_fourBit,   /*97=a*/

   /*98=b; C/G/T*/
   (c_fourBit | g_fourBit | t_fourBit),

   c_fourBit,   /*99  = c*/

   /*100=d; A/G/T*/
   (a_fourBit | g_fourBit | t_fourBit),

   err_fiveBit, /*101 = e not nucleotide*/
   err_fiveBit, /*102 = f not nucleotide*/
   g_fourBit,   /*103 = g*/

   /*104=h; A/G/T*/
   (a_fourBit | c_fourBit | t_fourBit),

   err_fiveBit, /*105 = i not nucleotide*/
   err_fiveBit, /*106 = j not nucleotide*/
   (g_fourBit | t_fourBit), /*107=k; G/T*/
   err_fiveBit, /*108  = l not nucleotide*/
   (a_fourBit | c_fourBit), /*109=m; A/C*/

   /*110=n; any base*/
   ( 
       a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*Any base (n)*/

   err_fiveBit, /*111 = o not nucleotide*/
   err_fiveBit, /*112 = p not nucleotide*/
   err_fiveBit, /*113 = q not nucleotide*/
   (a_fourBit | g_fourBit),   /*114=r A/G*/
   (c_fourBit | g_fourBit),   /*115=s C/G*/
   t_fourBit,   /*116 = t*/
   t_fourBit,   /*117 = u*/

   /*118 = v (ACG), treat as N*/
   (a_fourBit | c_fourBit | g_fourBit),

   (a_fourBit | t_fourBit), /*119=w A\T*/

   /*120=x; any aa (I will hold same for nucelotides)*/
   ( 
       a_fourBit
     | c_fourBit
     | g_fourBit
     | t_fourBit
   ), /*x*/

   (c_fourBit | t_fourBit),   /*121=y C/T*/
   err_fiveBit, /*122 = z not nucleotide*/

   /*Special characters after lowercase letters*/
   err_fiveBit, /*123 = {*/
   err_fiveBit, /*124 = |*/
   err_fiveBit, /*125 = }*/
   err_fiveBit, /*126 = ~*/
   err_fiveBit, /*127 = Del*/
}; /*ntToFourBit*/

/*-------------------------------------------------------\
| Tbl-03: ntToTwoBit
|  - table to convert bases to four bit (a/t/g/c only)
|    values to two bits with the 3rd bit saved for an
|    error/anonymous base
|    o 1st bit is A (a_fourBit)
|    o 2nd bit is C (c_fourBit)
|    o 3rd bit is G (g_fourBit)
|    o 4th bit is T/U (t_fourBit)
\-------------------------------------------------------*/
static
unsigned char ntToTwoBit[] =
{  /*ntToTwoBit*/
   /*White space/invisible charactes block*/
   err_threeBit, /*0   = Null character*/

   err_threeBit, /*1   = Start of Heading*/
   err_threeBit, /*2   = Start of Text*/
   err_threeBit, /*3   = End of Text*/
   err_threeBit, /*4   = End of Transmission*/
   err_threeBit, /*5   = Enquiry*/
   err_threeBit, /*6   = Acknowledge*/
   err_threeBit, /*7   = Bell*/
   err_threeBit, /*8   = Backspace*/

   err_threeBit, /*9   =  tab (horizontal)*/
   err_threeBit, /*10  = New line*/

   err_threeBit, /*11  = Vertical Tab (not key)*/
   err_threeBit, /*12  = Form Feed*/

   err_threeBit, /*13  = Carriage Return*/

   err_threeBit, /*14  = Shift Out*/
   err_threeBit, /*15  = Shift In*/
   err_threeBit, /*16  = Data Link Escape*/
   err_threeBit, /*17  = Device Control One*/
   err_threeBit, /*18  = Device Control Two*/
   err_threeBit, /*19  = Device Contol Three*/
   err_threeBit, /*20  = Device Control Four*/
   err_threeBit, /*21  = Negative Acknowledge*/
   err_threeBit, /*22  = Synchronous Idle*/
   err_threeBit, /*23  = End Transmission Block*/
   err_threeBit, /*24  = Cancle*/
   err_threeBit, /*25  = End of medium*/
   err_threeBit, /*26  = Substitute*/
   err_threeBit, /*27  = escape*/
   err_threeBit, /*28  = File Separator*/
   err_threeBit, /*29  = Group Separator*/
   err_threeBit, /*30  = Record Separator*/
   err_threeBit, /*31  = Unit Separator*/

   /*symbol/number block*/
   err_threeBit, /*32  = space*/
   err_threeBit, /*33  = !*/
   err_threeBit, /*34  = "*/
   err_threeBit, /*35  = #*/
   err_threeBit, /*36  = $*/
   err_threeBit, /*37  = %*/
   err_threeBit, /*38  = &*/
   err_threeBit, /*39  = '*/
   err_threeBit, /*40  = (*/
   err_threeBit, /*41  = )*/
   err_threeBit, /*42  = **/
   err_threeBit, /*43  = +*/
   err_threeBit, /*44  = ,*/
   err_threeBit, /*45  = -*/
   err_threeBit, /*46  = .*/
   err_threeBit, /*47  = /*/
   err_threeBit, /*48  = 0*/
   err_threeBit, /*49  = 1*/
   err_threeBit, /*50  = 2*/
   err_threeBit, /*51  = 3*/
   err_threeBit, /*52  = 4*/
   err_threeBit, /*53  = 5*/
   err_threeBit, /*54  = 6*/
   err_threeBit, /*55  = 7*/
   err_threeBit, /*56  = 8*/
   err_threeBit, /*57  = 9*/
   err_threeBit, /*58  = :*/
   err_threeBit, /*59  = ;*/
   err_threeBit, /*60  = <*/
   err_threeBit, /*61  = =*/
   err_threeBit, /*62  = >*/
   err_threeBit, /*63  = ?*/
   err_threeBit, /*64  = @*/

   /*Uppercase letters*/
   a_twoBit,   /*65  = A*/

   err_threeBit, /*66=B; C/G/T*/

   c_twoBit,   /*67  = C*/


   err_threeBit, /*68=D; A/G/T*/
   err_threeBit, /*69  = E not nucleotide*/
   err_threeBit, /*70  = F not nucleotide*/

   g_twoBit,     /*71  = G*/

   err_threeBit, /*72=H; A/G/T*/
   err_threeBit, /*73  = I not nucleotide*/
   err_threeBit, /*74  = J not nucleotide*/
   err_threeBit, /*75=K; G/T*/
   err_threeBit, /*76  = L not nucleotide*/
   err_threeBit, /*77=M; A/C*/

   err_threeBit, /*78=N; any base*/
   err_threeBit, /*79  = O not nucleotide*/
   err_threeBit, /*80  = P not nucleotide*/
   err_threeBit, /*81  = Q not nucleotide*/
   err_threeBit, /*82=R A/G*/
   err_threeBit, /*83=S C/G*/

   t_twoBit,   /*84  = T*/
   t_twoBit,   /*85  = U*/

   err_threeBit, /*86  = V (ACG), treat as N*/
   err_threeBit, /*87=W A\T*/
   err_threeBit, /*88=X; any aa*/
   err_threeBit,   /*89=Y C/T*/
   err_threeBit, /*90  = Z not nucleotide*/

   /*Special characters after upercase letters*/
   err_threeBit, /*91  = [*/
   err_threeBit, /*92  = \*/
   err_threeBit, /*93  = ]*/
   err_threeBit, /*94  = ^*/
   err_threeBit, /*95  = _*/
   err_threeBit, /*96  = `*/

   /*lower case letters*/
   a_twoBit,   /*97=a*/

   err_threeBit, /*98=b; C/G/T*/

   c_twoBit,   /*99  = c*/

   err_threeBit, /*100=d; A/G/T*/
   err_threeBit, /*101 = e not nucleotide*/
   err_threeBit, /*102 = f not nucleotide*/
   g_twoBit,   /*103 = g*/
   err_threeBit, /*104=h; A/G/T*/
   err_threeBit, /*105 = i not nucleotide*/
   err_threeBit, /*106 = j not nucleotide*/
   err_threeBit, /*107=k; G/T*/
   err_threeBit, /*108  = l not nucleotide*/
   err_threeBit, /*109=m; A/C*/

   err_threeBit, /*110=n; any base*/

   err_threeBit, /*111 = o not nucleotide*/
   err_threeBit, /*112 = p not nucleotide*/
   err_threeBit, /*113 = q not nucleotide*/
   err_threeBit, /*114=r A/G*/
   err_threeBit, /*115=s C/G*/
   t_twoBit,     /*116 = t*/
   t_twoBit,     /*117 = u*/

   
   err_threeBit, /*118 = v (ACG), treat as N*/
   err_threeBit, /*119=w A\T*/
   err_threeBit, /*120 = x*/
   err_threeBit,   /*121=y C/T*/
   err_threeBit, /*122 = z not nucleotide*/

   /*Special characters after lowercase letters*/
   err_threeBit, /*123 = {*/
   err_threeBit, /*124 = |*/
   err_threeBit, /*125 = }*/
   err_threeBit, /*126 = ~*/
   err_threeBit, /*127 = Del*/
}; /*ntToTwoBit*/

#endif

/*=======================================================\
: License:
: 
: This code is under the unlicense (public domain).
:   However, for cases were the public domain is not
:   suitable, such as countries that do not respect the
:   public domain or were working with the publikc domain
:   is inconvient / not possible, this code is under the
:   MIT license.
: 
: Public domain:
: 
: This is free and unencumbered software released into the
:   public domain.
: 
: Anyone is free to copy, modify, publish, use, compile,
:   sell, or distribute this software, either in source
:   code form or as a compiled binary, for any purpose,
:   commercial or non-commercial, and by any means.
: 
: In jurisdictions that recognize copyright laws, the
:   author or authors of this software dedicate any and
:   all copyright interest in the software to the public
:   domain. We make this dedication for the benefit of the
:   public at large and to the detriment of our heirs and
:   successors. We intend this dedication to be an overt
:   act of relinquishment in perpetuity of all present and
:   future rights to this software under copyright law.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO
:   EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM,
:   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
:   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
:   IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
:   DEALINGS IN THE SOFTWARE.
: 
: For more information, please refer to
:   <https://unlicense.org>
: 
: MIT License:
: 
: Copyright (c) 2024 jeremyButtler
: 
: Permission is hereby granted, free of charge, to any
:   person obtaining a copy of this software and
:   associated documentation files (the "Software"), to
:   deal in the Software without restriction, including
:   without limitation the rights to use, copy, modify,
:   merge, publish, distribute, sublicense, and/or sell
:   copies of the Software, and to permit persons to whom
:   the Software is furnished to do so, subject to the
:   following conditions:
: 
: The above copyright notice and this permission notice
:   shall be included in all copies or substantial
:   portions of the Software.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
:   EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
:   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
:   AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
:   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
:   USE OR OTHER DEALINGS IN THE SOFTWARE.
\=======================================================*/
