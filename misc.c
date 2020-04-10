#include "project.h"


/*
* kbhit() -- a keyboard lookahead monitor
*
* returns the number of characters available to read
*/
int kbhit ( void )
{
	struct timeval tv;
	struct termios old_termios, new_termios;
	int            error;
	int            count = 0;

	tcgetattr( 0, &old_termios );
	new_termios              = old_termios;
	/*
	* raw mode
	*/
	new_termios.c_lflag     &= ~ICANON;
	/*
	* disable echoing the char as it is typed
	*/
	new_termios.c_lflag     &= ~ECHO;
	/*
	* minimum chars to wait for
	*/
	new_termios.c_cc[VMIN]   = 1;
	/*
	* minimum wait time, 1 * 0.10s
	*/
	new_termios.c_cc[VTIME]  = 1;
	error                    = tcsetattr( 0, TCSANOW, &new_termios );
	tv.tv_sec                = 0;
	tv.tv_usec               = 100;
	/*
	* insert a minimal delay
	*/
	select( 1, NULL, NULL, NULL, &tv );
	error                   += ioctl( 0, FIONREAD, &count );
	error                   += tcsetattr( 0, TCSANOW, &old_termios );
	return( error == 0 ? count : -1 );
}  /* end of kbhit */

char *formatNumber (u64 value)
   {
   static char buffer [8][30];
   static int cycle;
   char *position;
   
   position = buffer [cycle];
   if (++cycle == DIMENSION (buffer)) cycle = 0;
   printw (position, "%I64X", value);
   if (value > 9) strcat (position, "h");
   return position;
   }



char *pciDeviceDescription (int vendor, int device)
   {
   static char buffer [80];

   *buffer = '\0';

	if (vendor == 0x003D ) strcpy (buffer, "Lockheed Martin" );                       
	else if (vendor == 0x0E11 ) strcpy (buffer, "Compaq" );                                
	else if (vendor == 0x1000 ) strcpy (buffer, "SYM" );                                   
	else if (vendor == 0x1002 ) strcpy (buffer, "ATI" );                                   
	else if (vendor == 0x1003 ) strcpy (buffer, "ULSI" );                                  
	else if (vendor == 0x1004 ) strcpy (buffer, "VLSI" );                                  
	else if (vendor == 0x1005 ) strcpy (buffer, "Avance" );                                
	else if (vendor == 0x1006 ) strcpy (buffer, "Reply" );                                 
	else if (vendor == 0x1007 ) strcpy (buffer, "NetFrame" );                              
	else if (vendor == 0x1008 ) strcpy (buffer, "Epson" );                                 
	else if (vendor == 0x100A ) strcpy (buffer, "Phoenix" );                               
	else if (vendor == 0x100B ) strcpy (buffer, "NSC" );                                   
	else if (vendor == 0x100C ) strcpy (buffer, "Tseng" );                                 
	else if (vendor == 0x100D ) strcpy (buffer, "AST" );                                   
	else if (vendor == 0x100E ) strcpy (buffer, "Weitek" );                                
	else if (vendor == 0x1010 ) strcpy (buffer, "VLogic" );                                
	else if (vendor == 0x1011 ) strcpy (buffer, "DEC" );                                   
	else if (vendor == 0x1012 ) strcpy (buffer, "Micronics" );                             
	else if (vendor == 0x1013 ) strcpy (buffer, "Cirrus" );                                
	else if (vendor == 0x1014 ) strcpy (buffer, "IBM" );                                   
	else if (vendor == 0x1015 ) strcpy (buffer, "LSIL" );                                  
	else if (vendor == 0x1016 ) strcpy (buffer, "Fujitsu ICL" );                           
	else if (vendor == 0x1017 ) strcpy (buffer, "Spea" );                                  
	else if (vendor == 0x1018 ) strcpy (buffer, "Unisys" );                                
	else if (vendor == 0x1019 ) strcpy (buffer, "ECS" );                                   
	else if (vendor == 0x101A ) strcpy (buffer, "NCR" );                                   
	else if (vendor == 0x101B ) strcpy (buffer, "Vitesse" );                               
	else if (vendor == 0x101C ) strcpy (buffer, "WD" );                                    
	else if (vendor == 0x101E ) strcpy (buffer, "AMI" );                                   
	else if (vendor == 0x101F ) strcpy (buffer, "PictureTel" );                            
	else if (vendor == 0x1020 ) strcpy (buffer, "Hitachi" );                               
	else if (vendor == 0x1021 ) strcpy (buffer, "OKI" );                                   
	else if (vendor == 0x1022 ) strcpy (buffer, "AMD" );                                   
	else if (vendor == 0x1023 ) strcpy (buffer, "Trident" );                               
	else if (vendor == 0x1024 ) strcpy (buffer, "Zenith" );                                
	else if (vendor == 0x1025 ) strcpy (buffer, "Acer" );                                  
	else if (vendor == 0x1028 ) strcpy (buffer, "Dell" );                                  
	else if (vendor == 0x1029 ) strcpy (buffer, "Siem-Nix" );                              
	else if (vendor == 0x102A ) strcpy (buffer, "LSI" );                                   
	else if (vendor == 0x102B ) strcpy (buffer, "Matrox" );                                
	else if (vendor == 0x102C ) strcpy (buffer, "C&T" );                                   
	else if (vendor == 0x102D ) strcpy (buffer, "Wyse" );                                  
	else if (vendor == 0x102E ) strcpy (buffer, "Olivetti" );                              
	else if (vendor == 0x102F ) strcpy (buffer, "Toshiba" );                               
	else if (vendor == 0x1030 ) strcpy (buffer, "TMC" );                                   
	else if (vendor == 0x1031 ) strcpy (buffer, "miro" );                                  
	else if (vendor == 0x1032 ) strcpy (buffer, "Compaq" );                                
	else if (vendor == 0x1033 ) strcpy (buffer, "NEC" );                                   
	else if (vendor == 0x1034 ) strcpy (buffer, "Burndy" );                                
	else if (vendor == 0x1035 ) strcpy (buffer, "C&CRL" );                                 
	else if (vendor == 0x1036 ) strcpy (buffer, "FDomain" );                               
	else if (vendor == 0x1037 ) strcpy (buffer, "Hitachi" );                               
	else if (vendor == 0x1038 ) strcpy (buffer, "AMP" );                                   
	else if (vendor == 0x1039 ) strcpy (buffer, "SIS" );                                   
	else if (vendor == 0x103A ) strcpy (buffer, "Seiko" );                                 
	else if (vendor == 0x103B ) strcpy (buffer, "Tatung" );                                
	else if (vendor == 0x103C ) strcpy (buffer, "HP" );                                    
	else if (vendor == 0x103E ) strcpy (buffer, "Solliday" );                              
	else if (vendor == 0x103F ) strcpy (buffer, "Logic Mod." );                            
	else if (vendor == 0x1040 ) strcpy (buffer, "Kubota" );                                
	else if (vendor == 0x1041 ) strcpy (buffer, "Computrend" );                            
	else if (vendor == 0x1042 ) strcpy (buffer, "PC Tech." );                              
	else if (vendor == 0x1043 ) strcpy (buffer, "Asustek" );                               
	else if (vendor == 0x1044 ) strcpy (buffer, "DPT" );                                   
	else if (vendor == 0x1045 ) strcpy (buffer, "OPTi" );                                  
	else if (vendor == 0x1046 ) strcpy (buffer, "IPC" );                                   
	else if (vendor == 0x1047 ) strcpy (buffer, "Genoa" );                                 
	else if (vendor == 0x1048 ) strcpy (buffer, "ELSA" );                                  
	else if (vendor == 0x1048 ) strcpy (buffer, "ELSA" );                                  
	else if (vendor == 0x1049 ) strcpy (buffer, "Fountain" );                              
	else if (vendor == 0x104A ) strcpy (buffer, "SGS" );                                   
	else if (vendor == 0x104B ) strcpy (buffer, "Buslogoc" );                              
	else if (vendor == 0x104C ) strcpy (buffer, "TI" );                                    
	else if (vendor == 0x104D ) strcpy (buffer, "Sony" );                                  
	else if (vendor == 0x104E ) strcpy (buffer, "Oak" );                                   
	else if (vendor == 0x104F ) strcpy (buffer, "Co-Time" );                               
	else if (vendor == 0x1050 ) strcpy (buffer, "Winbond" );                               
	else if (vendor == 0x1051 ) strcpy (buffer, "Anigma" );                                
	else if (vendor == 0x1052 ) strcpy (buffer, "Young" );                                 
	else if (vendor == 0x1054 ) strcpy (buffer, "Hitachi" );                               
	else if (vendor == 0x1055 ) strcpy (buffer, "EFAR" );                                  
	else if (vendor == 0x1056 ) strcpy (buffer, "ICL" );                                   
	else if (vendor == 0x1057 ) strcpy (buffer, "Motorola" );                              
	else if (vendor == 0x1058 ) strcpy (buffer, "E&TR" );                                  
	else if (vendor == 0x1059 ) strcpy (buffer, "Teknor" );                                
	else if (vendor == 0x105A ) strcpy (buffer, "Promise" );                               
	else if (vendor == 0x105B ) strcpy (buffer, "Foxconn" );                               
	else if (vendor == 0x105C ) strcpy (buffer, "Wipro" );                                 
	else if (vendor == 0x105D ) strcpy (buffer, "Number-Nine" );                           
	else if (vendor == 0x105E ) strcpy (buffer, "Vtech" );                                 
	else if (vendor == 0x105F ) strcpy (buffer, "Infotronic" );                            
	else if (vendor == 0x1060 ) strcpy (buffer, "UMC" );                                   
	else if (vendor == 0x1061 ) strcpy (buffer, "8x8" );                                   
	else if (vendor == 0x1062 ) strcpy (buffer, "Maspar" );                                
	else if (vendor == 0x1063 ) strcpy (buffer, "OOA" );                                   
	else if (vendor == 0x1064 ) strcpy (buffer, "Alcatel" );                               
	else if (vendor == 0x1065 ) strcpy (buffer, "TM" );                                    
	else if (vendor == 0x1066 ) strcpy (buffer, "Picopower" );                             
	else if (vendor == 0x1067 ) strcpy (buffer, "Mitsubishi" );                            
	else if (vendor == 0x1068 ) strcpy (buffer, "Div. Tech." );                            
	else if (vendor == 0x1069 ) strcpy (buffer, "Mylex" );                                 
	else if (vendor == 0x106A ) strcpy (buffer, "Aten" );                                  
	else if (vendor == 0x106B ) strcpy (buffer, "Apple" );                                 
	else if (vendor == 0x106C ) strcpy (buffer, "Hyundai" );                               
	else if (vendor == 0x106D ) strcpy (buffer, "Sequent" );                               
	else if (vendor == 0x106E ) strcpy (buffer, "DFI" );                                   
	else if (vendor == 0x106F ) strcpy (buffer, "CityGate" );                              
	else if (vendor == 0x1070 ) strcpy (buffer, "Daewoo" );                                
	else if (vendor == 0x1071 ) strcpy (buffer, "Mitac" );                                 
	else if (vendor == 0x1072 ) strcpy (buffer, "GIT" );                                   
	else if (vendor == 0x1073 ) strcpy (buffer, "Yamaha" );                                
	else if (vendor == 0x1074 ) strcpy (buffer, "Nexgen" );                                
	else if (vendor == 0x1075 ) strcpy (buffer, "AIR" );                                   
	else if (vendor == 0x1076 ) strcpy (buffer, "Chaintech" );                             
	else if (vendor == 0x1077 ) strcpy (buffer, "Q Logic" );                               
	else if (vendor == 0x1078 ) strcpy (buffer, "Cyrix" );                                 
	else if (vendor == 0x1079 ) strcpy (buffer, "I-Bus" );                                 
	else if (vendor == 0x107A ) strcpy (buffer, "Networth" );                              
	else if (vendor == 0x107B ) strcpy (buffer, "Gateway" );                               
	else if (vendor == 0x107C ) strcpy (buffer, "Goldstar" );                              
	else if (vendor == 0x107D ) strcpy (buffer, "Leadtek" );                               
	else if (vendor == 0x107E ) strcpy (buffer, "Interphase" );                            
	else if (vendor == 0x107F ) strcpy (buffer, "DTC" );                                   
	else if (vendor == 0x1080 ) strcpy (buffer, "Contaq" );                                
	else if (vendor == 0x1081 ) strcpy (buffer, "Supermac" );                              
	else if (vendor == 0x1082 ) strcpy (buffer, "EFA" );                                   
	else if (vendor == 0x1083 ) strcpy (buffer, "Forex" );                                 
	else if (vendor == 0x1084 ) strcpy (buffer, "Parador" );                               
	else if (vendor == 0x1085 ) strcpy (buffer, "Tulip" );                                 
	else if (vendor == 0x1086 ) strcpy (buffer, "J. Bond" );                               
	else if (vendor == 0x1087 ) strcpy (buffer, "Cache" );                                 
	else if (vendor == 0x1088 ) strcpy (buffer, "MS Son" );                                
	else if (vendor == 0x1089 ) strcpy (buffer, "DG" );                                    
	else if (vendor == 0x108A ) strcpy (buffer, "Bit3" );                                  
	else if (vendor == 0x108C ) strcpy (buffer, "Elonex" );                                
	else if (vendor == 0x108D ) strcpy (buffer, "Olicom" );                                
	else if (vendor == 0x108E ) strcpy (buffer, "Sun" );                                   
	else if (vendor == 0x108F ) strcpy (buffer, "Systemsoft" );                            
	else if (vendor == 0x1090 ) strcpy (buffer, "Encore" );                                
	else if (vendor == 0x1091 ) strcpy (buffer, "Intergraph" );                            
	else if (vendor == 0x1092 ) strcpy (buffer, "Diamond" );                               
	else if (vendor == 0x1093 ) strcpy (buffer, "Nat. Inst." );                            
	else if (vendor == 0x1094 ) strcpy (buffer, "FIC" );                                   
	else if (vendor == 0x1095 ) strcpy (buffer, "CMD" );                                   
	else if (vendor == 0x1096 ) strcpy (buffer, "Alacron" );                               
	else if (vendor == 0x1097 ) strcpy (buffer, "Appian" );                                
	else if (vendor == 0x1098 ) strcpy (buffer, "Quantum" );                               
	else if (vendor == 0x1099 ) strcpy (buffer, "Samsung" );                               
	else if (vendor == 0x109A ) strcpy (buffer, "Packard-Bell" );                          
	else if (vendor == 0x109B ) strcpy (buffer, "Gemlight" );                              
	else if (vendor == 0x109C ) strcpy (buffer, "Megachips" );                             
	else if (vendor == 0x109D ) strcpy (buffer, "Zida" );                                  
	else if (vendor == 0x109E ) strcpy (buffer, "Brooktree" );                             
	else if (vendor == 0x109F ) strcpy (buffer, "Trigem" );                                
	else if (vendor == 0x10A0 ) strcpy (buffer, "Meidensha" );                             
	else if (vendor == 0x10A1 ) strcpy (buffer, "Juko" );                                  
	else if (vendor == 0x10A2 ) strcpy (buffer, "Quantum" );                               
	else if (vendor == 0x10A3 ) strcpy (buffer, "Everex" );                                
	else if (vendor == 0x10A4 ) strcpy (buffer, "Globe" );                                 
	else if (vendor == 0x10A5 ) strcpy (buffer, "Racal" );                                 
	else if (vendor == 0x10A6 ) strcpy (buffer, "Informtech" );                            
	else if (vendor == 0x10A7 ) strcpy (buffer, "Benchmarq" );                             
	else if (vendor == 0x10A8 ) strcpy (buffer, "Sierra" );                                
	else if (vendor == 0x10A9 ) strcpy (buffer, "SG" );                                    
	else if (vendor == 0x10AA ) strcpy (buffer, "ACC" );                                   
	else if (vendor == 0x10AB ) strcpy (buffer, "Digicom" );                               
	else if (vendor == 0x10AC ) strcpy (buffer, "Honeywell" );                             
	else if (vendor == 0x10AD ) strcpy (buffer, "Symphony" );                              
	else if (vendor == 0x10AE ) strcpy (buffer, "Cornerstone" );                           
	else if (vendor == 0x10AF ) strcpy (buffer, "MCS" );                                   
	else if (vendor == 0x10B0 ) strcpy (buffer, "Cardexpert" );                            
	else if (vendor == 0x10B1 ) strcpy (buffer, "Cabletron" );                             
	else if (vendor == 0x10B2 ) strcpy (buffer, "Raytheon" );                              
	else if (vendor == 0x10B3 ) strcpy (buffer, "Databook" );                              
	else if (vendor == 0x10B4 ) strcpy (buffer, "STB" );                                   
	else if (vendor == 0x10B5 ) strcpy (buffer, "PLX" );                                   
	else if (vendor == 0x10B6 ) strcpy (buffer, "Madge" );                                 
	else if (vendor == 0x10B7 ) strcpy (buffer, "3Com" );                                  
	else if (vendor == 0x10B8 ) strcpy (buffer, "Standard" );                              
	else if (vendor == 0x10B9 ) strcpy (buffer, "ALI" );                                   
	else if (vendor == 0x10BA ) strcpy (buffer, "Mitsubishi" );                            
	else if (vendor == 0x10BB ) strcpy (buffer, "Dapha" );                                 
	else if (vendor == 0x10BC ) strcpy (buffer, "ALR" );                                   
	else if (vendor == 0x10BD ) strcpy (buffer, "Surecom" );                               
	else if (vendor == 0x10BE ) strcpy (buffer, "Tseng" );                                 
	else if (vendor == 0x10BF ) strcpy (buffer, "MOST" );                                  
	else if (vendor == 0x10C0 ) strcpy (buffer, "Boca" );                                  
	else if (vendor == 0x10C1 ) strcpy (buffer, "ICM" );                                   
	else if (vendor == 0x10C2 ) strcpy (buffer, "Auspex" );                                
	else if (vendor == 0x10C3 ) strcpy (buffer, "Samsung" );                               
	else if (vendor == 0x10C4 ) strcpy (buffer, "Award" );                                 
	else if (vendor == 0x10C5 ) strcpy (buffer, "Xerox" );                                 
	else if (vendor == 0x10C6 ) strcpy (buffer, "Rambus" );                                
	else if (vendor == 0x10C7 ) strcpy (buffer, "Media Vision" );                          
	else if (vendor == 0x10C8 ) strcpy (buffer, "Neomagic" );                              
	else if (vendor == 0x10C9 ) strcpy (buffer, "Dataexpert" );                            
	else if (vendor == 0x10CA ) strcpy (buffer, "Fujitsu" );                               
	else if (vendor == 0x10CB ) strcpy (buffer, "Omron" );                                 
	else if (vendor == 0x10CC ) strcpy (buffer, "Mentor" );                                
	else if (vendor == 0x10CD ) strcpy (buffer, "AdvanSys" );                              
	else if (vendor == 0x10CE ) strcpy (buffer, "Radius" );                                
	else if (vendor == 0x10CF ) strcpy (buffer, "TTI" );                                   
	else if (vendor == 0x10D0 ) strcpy (buffer, "Fujitsu" );                               
	else if (vendor == 0x10D1 ) strcpy (buffer, "Future+" );                               
	else if (vendor == 0x10D2 ) strcpy (buffer, "Molex" );                                 
	else if (vendor == 0x10D3 ) strcpy (buffer, "Jabil" );                                 
	else if (vendor == 0x10D4 ) strcpy (buffer, "Hualon" );                                
	else if (vendor == 0x10D5 ) strcpy (buffer, "Autologic" );                             
	else if (vendor == 0x10D6 ) strcpy (buffer, "Cetia" );                                 
	else if (vendor == 0x10D7 ) strcpy (buffer, "BCM" );                                   
	else if (vendor == 0x10D8 ) strcpy (buffer, "APL" );                                   
	else if (vendor == 0x10D9 ) strcpy (buffer, "Macronix" );                              
	else if (vendor == 0x10DA ) strcpy (buffer, "T-C" );                                   
	else if (vendor == 0x10DB ) strcpy (buffer, "Rohm" );                                  
	else if (vendor == 0x10DC ) strcpy (buffer, "CERN" );                                  
	else if (vendor == 0x10DD ) strcpy (buffer, "E&S" );                                   
	else if (vendor == 0x10DE ) strcpy (buffer, "Nvidia" );                                
	else if (vendor == 0x10DF ) strcpy (buffer, "Emulex" );                                
	else if (vendor == 0x10E0 ) strcpy (buffer, "IMS" );                                   
	else if (vendor == 0x10E1 ) strcpy (buffer, "Tekram" );                                
	else if (vendor == 0x10E2 ) strcpy (buffer, "Aptix" );                                 
	else if (vendor == 0x10E3 ) strcpy (buffer, "Tundra" );                                
	else if (vendor == 0x10E4 ) strcpy (buffer, "Tandem" );                                
	else if (vendor == 0x10E5 ) strcpy (buffer, "MIC" );                                   
	else if (vendor == 0x10E6 ) strcpy (buffer, "Gainbery" );                              
	else if (vendor == 0x10E7 ) strcpy (buffer, "Vadem" );                                 
	else if (vendor == 0x10E8 ) strcpy (buffer, "AMCC" );                                  
	else if (vendor == 0x10E9 ) strcpy (buffer, "Alps" );                                  
	else if (vendor == 0x10EA ) strcpy (buffer, "Intergraphics" );                         
	else if (vendor == 0x10EB ) strcpy (buffer, "Artist" );                                
	else if (vendor == 0x10EC ) strcpy (buffer, "Realtek" );                               
	else if (vendor == 0x10ED ) strcpy (buffer, "Ascii" );                                 
	else if (vendor == 0x10EE ) strcpy (buffer, "Xilinx" );                                
	else if (vendor == 0x10EF ) strcpy (buffer, "Racore" );                                
	else if (vendor == 0x10F0 ) strcpy (buffer, "Peritek" );                               
	else if (vendor == 0x10F1 ) strcpy (buffer, "Tyan" );                                  
	else if (vendor == 0x10F2 ) strcpy (buffer, "Achme" );                                 
	else if (vendor == 0x10F3 ) strcpy (buffer, "Alaris" );                                
	else if (vendor == 0x10F4 ) strcpy (buffer, "S-Mos" );                                 
	else if (vendor == 0x10F5 ) strcpy (buffer, "NKK" );                                   
	else if (vendor == 0x10F6 ) strcpy (buffer, "Creative" );                              
	else if (vendor == 0x10F7 ) strcpy (buffer, "Matsushita" );                            
	else if (vendor == 0x10F8 ) strcpy (buffer, "Altos" );                                 
	else if (vendor == 0x10F9 ) strcpy (buffer, "PC-Direct" );                             
	else if (vendor == 0x10FA ) strcpy (buffer, "Truevision" );                            
	else if (vendor == 0x10FB ) strcpy (buffer, "Thesys" );                                
	else if (vendor == 0x10FC ) strcpy (buffer, "I-O" );                                   
	else if (vendor == 0x10FD ) strcpy (buffer, "Soyo" );                                  
	else if (vendor == 0x10FE ) strcpy (buffer, "Fast" );                                  
	else if (vendor == 0x10FF ) strcpy (buffer, "Ncube" );                                 
	else if (vendor == 0x1100 ) strcpy (buffer, "Jazz" );                                  
	else if (vendor == 0x1101 ) strcpy (buffer, "Initio" );                                
	else if (vendor == 0x1102 ) strcpy (buffer, "Creative Labs" );                         
	else if (vendor == 0x1103 ) strcpy (buffer, "Triones" );                               
	else if (vendor == 0x1104 ) strcpy (buffer, "Rasterops" );                             
	else if (vendor == 0x1105 ) strcpy (buffer, "Sigma" );                                 
	else if (vendor == 0x1106 ) strcpy (buffer, "Via" );                                   
	else if (vendor == 0x1107 ) strcpy (buffer, "Stratus" );                               
	else if (vendor == 0x1108 ) strcpy (buffer, "Proteon" );                               
	else if (vendor == 0x1109 ) strcpy (buffer, "Cogent" );                                
	else if (vendor == 0x110A ) strcpy (buffer, "Siemens" );                               
	else if (vendor == 0x110B ) strcpy (buffer, "Chromatic" );                             
	else if (vendor == 0x110C ) strcpy (buffer, "Mini-Max" );                              
	else if (vendor == 0x110D ) strcpy (buffer, "ZNYX" );                                  
	else if (vendor == 0x110E ) strcpy (buffer, "CPU Tech." );                             
	else if (vendor == 0x110F ) strcpy (buffer, "Ross" );                                  
	else if (vendor == 0x1110 ) strcpy (buffer, "Powerhouse" );                            
	else if (vendor == 0x1111 ) strcpy (buffer, "SCO" );                                   
	else if (vendor == 0x1112 ) strcpy (buffer, "Osicom" );                                
	else if (vendor == 0x1113 ) strcpy (buffer, "Accton" );                                
	else if (vendor == 0x1114 ) strcpy (buffer, "Atmel" );                                 
	else if (vendor == 0x1115 ) strcpy (buffer, "Dupont" );                                
	else if (vendor == 0x1116 ) strcpy (buffer, "Data Trans." );                           
	else if (vendor == 0x1117 ) strcpy (buffer, "Datacube" );                              
	else if (vendor == 0x1118 ) strcpy (buffer, "Berg" );                                  
	else if (vendor == 0x1119 ) strcpy (buffer, "Vortex" );                                
	else if (vendor == 0x111A ) strcpy (buffer, "Eff. Net." );                             
	else if (vendor == 0x111B ) strcpy (buffer, "Teledyne" );                              
	else if (vendor == 0x111C ) strcpy (buffer, "Tricord" );                               
	else if (vendor == 0x111D ) strcpy (buffer, "IDT" );                                   
	else if (vendor == 0x111E ) strcpy (buffer, "Eldec" );                                 
	else if (vendor == 0x111F ) strcpy (buffer, "PDI" );                                   
	else if (vendor == 0x1120 ) strcpy (buffer, "EMC" );                                   
	else if (vendor == 0x1121 ) strcpy (buffer, "Zilog" );                                 
	else if (vendor == 0x1122 ) strcpy (buffer, "Multi-Tech" );                            
	else if (vendor == 0x1123 ) strcpy (buffer, "EDI" );                                   
	else if (vendor == 0x1124 ) strcpy (buffer, "Leutron" );                               
	else if (vendor == 0x1125 ) strcpy (buffer, "Eurocore" );                              
	else if (vendor == 0x1126 ) strcpy (buffer, "Vigra" );                                 
	else if (vendor == 0x1127 ) strcpy (buffer, "FORE" );                                  
	else if (vendor == 0x1129 ) strcpy (buffer, "Firmworks" );                             
	else if (vendor == 0x112A ) strcpy (buffer, "Hermes" );                                
	else if (vendor == 0x112B ) strcpy (buffer, "Linotype" );                              
	else if (vendor == 0x112C ) strcpy (buffer, "Zenith" );                                
	else if (vendor == 0x112D ) strcpy (buffer, "Ravicad" );                               
	else if (vendor == 0x112E ) strcpy (buffer, "Infomedia" );                             
	else if (vendor == 0x112F ) strcpy (buffer, "ImagTech" );                              
	else if (vendor == 0x1130 ) strcpy (buffer, "Computervision" );                        
	else if (vendor == 0x1131 ) strcpy (buffer, "Philips" );                               
	else if (vendor == 0x1132 ) strcpy (buffer, "Mitel" );                                 
	else if (vendor == 0x1133 ) strcpy (buffer, "EIC" );                                   
	else if (vendor == 0x1134 ) strcpy (buffer, "MCS" );                                   
	else if (vendor == 0x1135 ) strcpy (buffer, "Fuji" );                                  
	else if (vendor == 0x1136 ) strcpy (buffer, "Momentum" );                              
	else if (vendor == 0x1137 ) strcpy (buffer, "Cisco" );                                 
	else if (vendor == 0x1138 ) strcpy (buffer, "Ziatech" );                               
	else if (vendor == 0x1139 ) strcpy (buffer, "Dyn. Pict." );                            
	else if (vendor == 0x113A ) strcpy (buffer, "FWB" );                                   
	else if (vendor == 0x113B ) strcpy (buffer, "NCD" );                                   
	else if (vendor == 0x113C ) strcpy (buffer, "Cyclone" );                               
	else if (vendor == 0x113D ) strcpy (buffer, "Leading Edge" );                          
	else if (vendor == 0x113E ) strcpy (buffer, "Sanyo" );                                 
	else if (vendor == 0x113F ) strcpy (buffer, "Equinox" );                               
	else if (vendor == 0x1140 ) strcpy (buffer, "Intervoice" );                            
	else if (vendor == 0x1141 ) strcpy (buffer, "Crest" );                                 
	else if (vendor == 0x1142 ) strcpy (buffer, "Alliance" );                              
	else if (vendor == 0x1143 ) strcpy (buffer, "Netpower" );                              
	else if (vendor == 0x1144 ) strcpy (buffer, "Cinn. Mil." );                            
	else if (vendor == 0x1145 ) strcpy (buffer, "Workbit" );                               
	else if (vendor == 0x1146 ) strcpy (buffer, "Force" );                                 
	else if (vendor == 0x1147 ) strcpy (buffer, "Interface" );                             
	else if (vendor == 0x1148 ) strcpy (buffer, "S&K" );                                   
	else if (vendor == 0x1149 ) strcpy (buffer, "Win System" );                            
	else if (vendor == 0x114A ) strcpy (buffer, "VMIC" );                                  
	else if (vendor == 0x114B ) strcpy (buffer, "Canopus" );                               
	else if (vendor == 0x114C ) strcpy (buffer, "Annabooks" );                             
	else if (vendor == 0x114D ) strcpy (buffer, "IC Corp." );                              
	else if (vendor == 0x114E ) strcpy (buffer, "Nikon" );                                 
	else if (vendor == 0x114F ) strcpy (buffer, "Stargate" );                              
	else if (vendor == 0x1150 ) strcpy (buffer, "TMC" );                                   
	else if (vendor == 0x1151 ) strcpy (buffer, "JAE" );                                   
	else if (vendor == 0x1152 ) strcpy (buffer, "Megatek" );                               
	else if (vendor == 0x1153 ) strcpy (buffer, "Land Win" );                              
	else if (vendor == 0x1154 ) strcpy (buffer, "Melco" );                                 
	else if (vendor == 0x1155 ) strcpy (buffer, "Pine" );                                  
	else if (vendor == 0x1156 ) strcpy (buffer, "Periscope" );                             
	else if (vendor == 0x1157 ) strcpy (buffer, "Avsys" );                                 
	else if (vendor == 0x1158 ) strcpy (buffer, "Voarx" );                                 
	else if (vendor == 0x1159 ) strcpy (buffer, "Mutech" );                                
	else if (vendor == 0x115A ) strcpy (buffer, "Harlequin" );                             
	else if (vendor == 0x115B ) strcpy (buffer, "Parallax" );                              
	else if (vendor == 0x115C ) strcpy (buffer, "Photron" );                               
	else if (vendor == 0x115D ) strcpy (buffer, "Xircom" );                                
	else if (vendor == 0x115E ) strcpy (buffer, "Peer" );                                  
	else if (vendor == 0x115F ) strcpy (buffer, "Maxtor" );                                
	else if (vendor == 0x1160 ) strcpy (buffer, "Megasoft" );                              
	else if (vendor == 0x1161 ) strcpy (buffer, "PFU" );                                   
	else if (vendor == 0x1162 ) strcpy (buffer, "OA Lab" );                                
	else if (vendor == 0x1163 ) strcpy (buffer, "Rendition" );                             
	else if (vendor == 0x1164 ) strcpy (buffer, "APT" );                                   
	else if (vendor == 0x1165 ) strcpy (buffer, "Imagraph" );                              
	else if (vendor == 0x1166 ) strcpy (buffer, "Pequr" );                                 
	else if (vendor == 0x1167 ) strcpy (buffer, "Mutoh" );                                 
	else if (vendor == 0x1168 ) strcpy (buffer, "Thine" );                                 
	else if (vendor == 0x1169 ) strcpy (buffer, "CDAC" );                                  
	else if (vendor == 0x116A ) strcpy (buffer, "Polaris" );                               
	else if (vendor == 0x116B ) strcpy (buffer, "Connectware" );                           
	else if (vendor == 0x116C ) strcpy (buffer, "Int Res." );                              
	else if (vendor == 0x116E ) strcpy (buffer, "EFI" );                                   
	else if (vendor == 0x116F ) strcpy (buffer, "WkSta. Tech." );                          
	else if (vendor == 0x1170 ) strcpy (buffer, "Inventec" );                              
	else if (vendor == 0x1171 ) strcpy (buffer, "Lough. Sound" );                          
	else if (vendor == 0x1172 ) strcpy (buffer, "Altera" );                                
	else if (vendor == 0x1173 ) strcpy (buffer, "Adobe" );                                 
	else if (vendor == 0x1174 ) strcpy (buffer, "Bridgeport" );                            
	else if (vendor == 0x1175 ) strcpy (buffer, "Mitron" );                                
	else if (vendor == 0x1176 ) strcpy (buffer, "SBE" );                                   
	else if (vendor == 0x1177 ) strcpy (buffer, "Silicon Eng." );                          
	else if (vendor == 0x1178 ) strcpy (buffer, "Alfa" );                                  
	else if (vendor == 0x1179 ) strcpy (buffer, "Toshiba" );                               
	else if (vendor == 0x117A ) strcpy (buffer, "A-Trend" );                               
	else if (vendor == 0x117B ) strcpy (buffer, "LG Elec." );                              
	else if (vendor == 0x117C ) strcpy (buffer, "Atto" );                                  
	else if (vendor == 0x117D ) strcpy (buffer, "B&D" );                                   
	else if (vendor == 0x117E ) strcpy (buffer, "T/R" );                                   
	else if (vendor == 0x117F ) strcpy (buffer, "ICS" );                                   
	else if (vendor == 0x1180 ) strcpy (buffer, "Ricoh" );                                 
	else if (vendor == 0x1181 ) strcpy (buffer, "Telmatics" );                             
	else if (vendor == 0x1183 ) strcpy (buffer, "Fujikura" );                              
	else if (vendor == 0x1184 ) strcpy (buffer, "Forks" );                                 
	else if (vendor == 0x1185 ) strcpy (buffer, "Dataworld" );                             
	else if (vendor == 0x1186 ) strcpy (buffer, "D-Link" );                                
	else if (vendor == 0x1187 ) strcpy (buffer, "ATL" );                                   
	else if (vendor == 0x1188 ) strcpy (buffer, "Shima" );                                 
	else if (vendor == 0x1189 ) strcpy (buffer, "Matsushita" );                            
	else if (vendor == 0x118A ) strcpy (buffer, "Hilevel" );                               
	else if (vendor == 0x118B ) strcpy (buffer, "Hypertec" );                              
	else if (vendor == 0x118C ) strcpy (buffer, "Corollary" );                             
	else if (vendor == 0x118D ) strcpy (buffer, "BitFlow" );                               
	else if (vendor == 0x118E ) strcpy (buffer, "Hermstedt" );                             
	else if (vendor == 0x118F ) strcpy (buffer, "Green" );                                 
	else if (vendor == 0x1191 ) strcpy (buffer, "Artop" );                                 
	else if (vendor == 0x1192 ) strcpy (buffer, "Densan" );                                
	else if (vendor == 0x1193 ) strcpy (buffer, "Zeitnet" );                               
	else if (vendor == 0x1194 ) strcpy (buffer, "Toucan" );                                
	else if (vendor == 0x1195 ) strcpy (buffer, "Ratoc" );                                 
	else if (vendor == 0x1196 ) strcpy (buffer, "Hytec" );                                 
	else if (vendor == 0x1197 ) strcpy (buffer, "Gage" );                                  
	else if (vendor == 0x1198 ) strcpy (buffer, "Lambda" );                                
	else if (vendor == 0x1199 ) strcpy (buffer, "Attachmate" );                            
	else if (vendor == 0x119A ) strcpy (buffer, "Mind Share" );                            
	else if (vendor == 0x119B ) strcpy (buffer, "Omega" );                                 
	else if (vendor == 0x119C ) strcpy (buffer, "ITI" );                                   
	else if (vendor == 0x119D ) strcpy (buffer, "Bug" );                                   
	else if (vendor == 0x119E ) strcpy (buffer, "Fujitsu" );                               
	else if (vendor == 0x119F ) strcpy (buffer, "Bull" );                                  
	else if (vendor == 0x11A0 ) strcpy (buffer, "Convex" );                                
	else if (vendor == 0x11A1 ) strcpy (buffer, "Hamamatsu" );                             
	else if (vendor == 0x11A2 ) strcpy (buffer, "Sierra" );                                
	else if (vendor == 0x11A3 ) strcpy (buffer, "Deuretzbacher" );                         
	else if (vendor == 0x11A4 ) strcpy (buffer, "Barco" );                                 
	else if (vendor == 0x11A5 ) strcpy (buffer, "MicroUnity" );                            
	else if (vendor == 0x11A6 ) strcpy (buffer, "Pure Data" );                             
	else if (vendor == 0x11A7 ) strcpy (buffer, "Power Comp." );                           
	else if (vendor == 0x11A8 ) strcpy (buffer, "Systech" );                               
	else if (vendor == 0x11A9 ) strcpy (buffer, "InnoSys" );                               
	else if (vendor == 0x11AA ) strcpy (buffer, "Actel" );                                 
	else if (vendor == 0x11AB ) strcpy (buffer, "Galileo" );                               
	else if (vendor == 0x11AC ) strcpy (buffer, "Canon" );                                 
	else if (vendor == 0x11AD ) strcpy (buffer, "Lite-On" );                               
	else if (vendor == 0x11AE ) strcpy (buffer, "Scitex" );                                
	else if (vendor == 0x11AF ) strcpy (buffer, "Pro-Log" );                               
	else if (vendor == 0x11B0 ) strcpy (buffer, "V3" );                                    
	else if (vendor == 0x11B1 ) strcpy (buffer, "Apricot" );                               
	else if (vendor == 0x11B2 ) strcpy (buffer, "Kodak" );                                 
	else if (vendor == 0x11B3 ) strcpy (buffer, "Barr" );                                  
	else if (vendor == 0x11B4 ) strcpy (buffer, "Leitch" );                                
	else if (vendor == 0x11B5 ) strcpy (buffer, "Radstone" );                              
	else if (vendor == 0x11B6 ) strcpy (buffer, "United Video" );                          
	else if (vendor == 0x11B7 ) strcpy (buffer, "Motorola" );                              
	else if (vendor == 0x11B8 ) strcpy (buffer, "Xpoint" );                                
	else if (vendor == 0x11B9 ) strcpy (buffer, "Pathlight" );                             
	else if (vendor == 0x11BA ) strcpy (buffer, "Videotron" );                             
	else if (vendor == 0x11BB ) strcpy (buffer, "Pyramid" );                               
	else if (vendor == 0x11BC ) strcpy (buffer, "Net. Periph." );                          
	else if (vendor == 0x11BD ) strcpy (buffer, "Pinnacle" );                              
	else if (vendor == 0x11BE ) strcpy (buffer, "IMI" );                                   
	else if (vendor == 0x11BF ) strcpy (buffer, "Astrodesign" );                           
	else if (vendor == 0x11C0 ) strcpy (buffer, "H-P" );                                   
	else if (vendor == 0x11C1 ) strcpy (buffer, "AT&T" );                                  
	else if (vendor == 0x11C2 ) strcpy (buffer, "Sand" );                                  
	else if (vendor == 0x11C3 ) strcpy (buffer, "NEC" );                                   
	else if (vendor == 0x11C4 ) strcpy (buffer, "Doc. Tech." );                            
	else if (vendor == 0x11C5 ) strcpy (buffer, "Shiva" );                                 
	else if (vendor == 0x11C6 ) strcpy (buffer, "Dainippon" );                             
	else if (vendor == 0x11C7 ) strcpy (buffer, "D.C.M." );                                
	else if (vendor == 0x11C8 ) strcpy (buffer, "Dolphin" );                               
	else if (vendor == 0x11C9 ) strcpy (buffer, "MAGMA" );                                 
	else if (vendor == 0x11CA ) strcpy (buffer, "LSI Sys." );                              
	else if (vendor == 0x11CB ) strcpy (buffer, "Specialix" );                             
	else if (vendor == 0x11CC ) strcpy (buffer, "M&K" );                                   
	else if (vendor == 0x11CD ) strcpy (buffer, "HAL" );                                   
	else if (vendor == 0x11CE ) strcpy (buffer, "PRI" );                                   
	else if (vendor == 0x11CF ) strcpy (buffer, "PEC" );                                   
	else if (vendor == 0x11D0 ) strcpy (buffer, "Loral" );                                 
	else if (vendor == 0x11D1 ) strcpy (buffer, "AuraVision" );                            
	else if (vendor == 0x11D2 ) strcpy (buffer, "Intercom" );                              
	else if (vendor == 0x11D3 ) strcpy (buffer, "Trancell" );                              
	else if (vendor == 0x11D4 ) strcpy (buffer, "AD" );                                    
	else if (vendor == 0x11D5 ) strcpy (buffer, "Ikon" );                                  
	else if (vendor == 0x11D6 ) strcpy (buffer, "Tekelec" );                               
	else if (vendor == 0x11D7 ) strcpy (buffer, "Trenton" );                               
	else if (vendor == 0x11D8 ) strcpy (buffer, "ITD" );                                   
	else if (vendor == 0x11D9 ) strcpy (buffer, "Tec" );                                   
	else if (vendor == 0x11DA ) strcpy (buffer, "Novell" );                                
	else if (vendor == 0x11DB ) strcpy (buffer, "Sega" );                                  
	else if (vendor == 0x11DC ) strcpy (buffer, "Questra" );                               
	else if (vendor == 0x11DD ) strcpy (buffer, "Crosfield" );                             
	else if (vendor == 0x11DE ) strcpy (buffer, "Zoran" );                                 
	else if (vendor == 0x11DF ) strcpy (buffer, "New Wave" );                              
	else if (vendor == 0x11E0 ) strcpy (buffer, "Cray" );                                  
	else if (vendor == 0x11E1 ) strcpy (buffer, "Gec Plessey" );                           
	else if (vendor == 0x11E2 ) strcpy (buffer, "Samsung" );                               
	else if (vendor == 0x11E3 ) strcpy (buffer, "Quicklogic" );                            
	else if (vendor == 0x11E4 ) strcpy (buffer, "Second Wave" );                           
	else if (vendor == 0x11E5 ) strcpy (buffer, "IIX" );                                   
	else if (vendor == 0x11E6 ) strcpy (buffer, "Mitsui" );                                
	else if (vendor == 0x11E7 ) strcpy (buffer, "Toshiba" );                               
	else if (vendor == 0x11E8 ) strcpy (buffer, "DPSI" );                                  
	else if (vendor == 0x11E9 ) strcpy (buffer, "Highwater" );                             
	else if (vendor == 0x11EA ) strcpy (buffer, "Elsag" );                                 
	else if (vendor == 0x11EB ) strcpy (buffer, "Formation" );                             
	else if (vendor == 0x11EC ) strcpy (buffer, "Coreco" );                                
	else if (vendor == 0x11ED ) strcpy (buffer, "Mediamatics" );                           
	else if (vendor == 0x11EE ) strcpy (buffer, "Dome" );                                  
	else if (vendor == 0x11EF ) strcpy (buffer, "Nicolet" );                               
	else if (vendor == 0x11F0 ) strcpy (buffer, "Compu-Shack" );                           
	else if (vendor == 0x11F1 ) strcpy (buffer, "Symbios" );                               
	else if (vendor == 0x11F2 ) strcpy (buffer, "Pic-Tel" );                               
	else if (vendor == 0x11F3 ) strcpy (buffer, "Keithley" );                              
	else if (vendor == 0x11F4 ) strcpy (buffer, "Kinetic" );                               
	else if (vendor == 0x11F5 ) strcpy (buffer, "Comp Dev" );                              
	else if (vendor == 0x11F6 ) strcpy (buffer, "Powermatic" );                            
	else if (vendor == 0x11F7 ) strcpy (buffer, "S-A" );                                   
	else if (vendor == 0x11F8 ) strcpy (buffer, "PMC-Sierra" );                            
	else if (vendor == 0x11F9 ) strcpy (buffer, "I-Cube" );                                
	else if (vendor == 0x11FA ) strcpy (buffer, "Kasan" );                                 
	else if (vendor == 0x11FB ) strcpy (buffer, "Datel" );                                 
	else if (vendor == 0x11FC ) strcpy (buffer, "Silicon Magic" );                         
	else if (vendor == 0x11FD ) strcpy (buffer, "High Street" );                           
	else if (vendor == 0x11FE ) strcpy (buffer, "Comtrol" );                               
	else if (vendor == 0x11FF ) strcpy (buffer, "Scion" );                                 
	else if (vendor == 0x11af ) strcpy (buffer, "Avid" );                                  
	else if (vendor == 0x1200 ) strcpy (buffer, "CSS" );                                   
	else if (vendor == 0x1201 ) strcpy (buffer, "Vista" );                                 
	else if (vendor == 0x1202 ) strcpy (buffer, "Network Gen" );                           
	else if (vendor == 0x1203 ) strcpy (buffer, "Agfa" );                                  
	else if (vendor == 0x1204 ) strcpy (buffer, "Lattice" );                               
	else if (vendor == 0x1205 ) strcpy (buffer, "Array" );                                 
	else if (vendor == 0x1206 ) strcpy (buffer, "Amdahl" );                                
	else if (vendor == 0x1208 ) strcpy (buffer, "Parsytec" );                              
	else if (vendor == 0x1209 ) strcpy (buffer, "Sci Sys" );                               
	else if (vendor == 0x120A ) strcpy (buffer, "Synaptel" );                              
	else if (vendor == 0x120B ) strcpy (buffer, "Adaptive" );                              
	else if (vendor == 0x120D ) strcpy (buffer, "Comp Labs" );                             
	else if (vendor == 0x120E ) strcpy (buffer, "Cyclades" );                              
	else if (vendor == 0x120F ) strcpy (buffer, "Essential" );                             
	else if (vendor == 0x1210 ) strcpy (buffer, "Hyperparallel" );                         
	else if (vendor == 0x1211 ) strcpy (buffer, "Braintech" );                             
	else if (vendor == 0x1212 ) strcpy (buffer, "Kingston" );                              
	else if (vendor == 0x1213 ) strcpy (buffer, "AISI" );                                  
	else if (vendor == 0x1214 ) strcpy (buffer, "Perf Tech" );                             
	else if (vendor == 0x1215 ) strcpy (buffer, "Interware" );                             
	else if (vendor == 0x1216 ) strcpy (buffer, "Purup" );                                 
	else if (vendor == 0x1217 ) strcpy (buffer, "2 Micro" );                               
	else if (vendor == 0x1218 ) strcpy (buffer, "Hybricon" );                              
	else if (vendor == 0x1219 ) strcpy (buffer, "First Virtual" );                         
	else if (vendor == 0x121A ) strcpy (buffer, "3dfx" );                                  
	else if (vendor == 0x121B ) strcpy (buffer, "ATM" );                                   
	else if (vendor == 0x121C ) strcpy (buffer, "Nippon Texa" );                           
	else if (vendor == 0x121D ) strcpy (buffer, "Lippert" );                               
	else if (vendor == 0x121E ) strcpy (buffer, "CSPI" );                                  
	else if (vendor == 0x121F ) strcpy (buffer, "Arcus" );                                 
	else if (vendor == 0x1220 ) strcpy (buffer, "Ariel" );                                 
	else if (vendor == 0x1221 ) strcpy (buffer, "Contec" );                                
	else if (vendor == 0x1222 ) strcpy (buffer, "Ancor" );                                 
	else if (vendor == 0x1223 ) strcpy (buffer, "Heurikon" );                              
	else if (vendor == 0x1224 ) strcpy (buffer, "Int. Img." );                             
	else if (vendor == 0x1225 ) strcpy (buffer, "Power IO" );                              
	else if (vendor == 0x1227 ) strcpy (buffer, "Tech-Source" );                           
	else if (vendor == 0x1228 ) strcpy (buffer, "Norsk" );                                 
	else if (vendor == 0x1229 ) strcpy (buffer, "Data Kin" );                              
	else if (vendor == 0x122A ) strcpy (buffer, "Int. Telecom" );                          
	else if (vendor == 0x122B ) strcpy (buffer, "LG Ind." );                               
	else if (vendor == 0x122C ) strcpy (buffer, "Sican" );                                 
	else if (vendor == 0x122D ) strcpy (buffer, "Aztech" );                                
	else if (vendor == 0x122E ) strcpy (buffer, "Xyratex" );                               
	else if (vendor == 0x122F ) strcpy (buffer, "Andrew" );                                
	else if (vendor == 0x1230 ) strcpy (buffer, "Fishcamp" );                              
	else if (vendor == 0x1231 ) strcpy (buffer, "W McCoach" );                             
	else if (vendor == 0x1232 ) strcpy (buffer, "GPT" );                                   
	else if (vendor == 0x1233 ) strcpy (buffer, "Bus-Tech" );                              
	else if (vendor == 0x1234 ) strcpy (buffer, "Technical" );                             
	else if (vendor == 0x1235 ) strcpy (buffer, "Risq Mod" );                              
	else if (vendor == 0x1236 ) strcpy (buffer, "Sigma" );                                 
	else if (vendor == 0x1237 ) strcpy (buffer, "Alta Tech" );                             
	else if (vendor == 0x1238 ) strcpy (buffer, "Adtran" );                                
	else if (vendor == 0x1239 ) strcpy (buffer, "3DO" );                                   
	else if (vendor == 0x123A ) strcpy (buffer, "Visicom" );                               
	else if (vendor == 0x123B ) strcpy (buffer, "Seeq" );                                  
	else if (vendor == 0x123C ) strcpy (buffer, "Century Sys" );                           
	else if (vendor == 0x123D ) strcpy (buffer, "EDT" );                                   
	else if (vendor == 0x123F ) strcpy (buffer, "C-Cube" );                                
	else if (vendor == 0x1240 ) strcpy (buffer, "Marathon" );                              
	else if (vendor == 0x1241 ) strcpy (buffer, "DSC" );                                   
	else if (vendor == 0x1243 ) strcpy (buffer, "Delphax" );                               
	else if (vendor == 0x1244 ) strcpy (buffer, "AVM" );                                   
	else if (vendor == 0x1245 ) strcpy (buffer, "APD" );                                   
	else if (vendor == 0x1246 ) strcpy (buffer, "Dipix" );                                 
	else if (vendor == 0x1247 ) strcpy (buffer, "Xylon" );                                 
	else if (vendor == 0x1248 ) strcpy (buffer, "Central Data" );                          
	else if (vendor == 0x1249 ) strcpy (buffer, "Samsung" );                               
	else if (vendor == 0x124A ) strcpy (buffer, "AEG" );                                   
	else if (vendor == 0x124B ) strcpy (buffer, "GreenSpring" );                           
	else if (vendor == 0x124C ) strcpy (buffer, "Solitron" );                              
	else if (vendor == 0x124D ) strcpy (buffer, "Stallion" );                              
	else if (vendor == 0x124E ) strcpy (buffer, "Cylink" );                                
	else if (vendor == 0x124F ) strcpy (buffer, "Infortrend" );                            
	else if (vendor == 0x1250 ) strcpy (buffer, "Hitachi" );                               
	else if (vendor == 0x1251 ) strcpy (buffer, "VLSI Sol." );                             
	else if (vendor == 0x1253 ) strcpy (buffer, "Guzik" );                                 
	else if (vendor == 0x1254 ) strcpy (buffer, "Linear Systems" );                        
	else if (vendor == 0x1255 ) strcpy (buffer, "Optibase" );                              
	else if (vendor == 0x1256 ) strcpy (buffer, "Perceptive" );                            
	else if (vendor == 0x1257 ) strcpy (buffer, "Vertex" );                                
	else if (vendor == 0x1258 ) strcpy (buffer, "Gilbarco" );                              
	else if (vendor == 0x1259 ) strcpy (buffer, "Allied Tsyn" );                           
	else if (vendor == 0x125A ) strcpy (buffer, "ABB Pwr" );                               
	else if (vendor == 0x125B ) strcpy (buffer, "Asix" );                                  
	else if (vendor == 0x125C ) strcpy (buffer, "Aurora" );                                
	else if (vendor == 0x125D ) strcpy (buffer, "ESS" );                                   
	else if (vendor == 0x125E ) strcpy (buffer, "Specvideo" );                             
	else if (vendor == 0x125F ) strcpy (buffer, "Concurrent" );                            
	else if (vendor == 0x1260 ) strcpy (buffer, "Harris" );                                
	else if (vendor == 0x1261 ) strcpy (buffer, "Matsushita" );                            
	else if (vendor == 0x1262 ) strcpy (buffer, "ES Comp." );                              
	else if (vendor == 0x1263 ) strcpy (buffer, "Sonic Sol." );                            
	else if (vendor == 0x1264 ) strcpy (buffer, "Aval Nag." );                             
	else if (vendor == 0x1265 ) strcpy (buffer, "Casio" );                                 
	else if (vendor == 0x1266 ) strcpy (buffer, "Microdyne" );                             
	else if (vendor == 0x1267 ) strcpy (buffer, "SA Telecom" );                            
	else if (vendor == 0x1268 ) strcpy (buffer, "Tektronix" );                             
	else if (vendor == 0x1269 ) strcpy (buffer, "Thomson-CSF" );                           
	else if (vendor == 0x126A ) strcpy (buffer, "Lexmark" );                               
	else if (vendor == 0x126B ) strcpy (buffer, "Adax" );                                  
	else if (vendor == 0x126C ) strcpy (buffer, "NorTel" );                                
	else if (vendor == 0x126D ) strcpy (buffer, "Splash" );                                
	else if (vendor == 0x126E ) strcpy (buffer, "Sumitomo" );                              
	else if (vendor == 0x126F ) strcpy (buffer, "Sil Motion" );                            
	else if (vendor == 0x1270 ) strcpy (buffer, "Olympus" );                               
	else if (vendor == 0x1271 ) strcpy (buffer, "GW Instr." );                             
	else if (vendor == 0x1272 ) strcpy (buffer, "Telematics" );                            
	else if (vendor == 0x1273 ) strcpy (buffer, "Hughes" );                                
	else if (vendor == 0x1274 ) strcpy (buffer, "Ensoniq" );                               
	else if (vendor == 0x1275 ) strcpy (buffer, "NetApp" );                                
	else if (vendor == 0x1276 ) strcpy (buffer, "Sw Net Tech" );                           
	else if (vendor == 0x1277 ) strcpy (buffer, "Comstream" );                             
	else if (vendor == 0x1278 ) strcpy (buffer, "Transtech" );                             
	else if (vendor == 0x1279 ) strcpy (buffer, "Transmeta" );                             
	else if (vendor == 0x127A ) strcpy (buffer, "Rockwell Semi" );                         
	else if (vendor == 0x127B ) strcpy (buffer, "Pixera" );                                
	else if (vendor == 0x127C ) strcpy (buffer, "Crosspoint" );                            
	else if (vendor == 0x127D ) strcpy (buffer, "Vela Res" );                              
	else if (vendor == 0x127E ) strcpy (buffer, "Winnow" );                                
	else if (vendor == 0x127F ) strcpy (buffer, "Fujifilm" );                              
	else if (vendor == 0x1280 ) strcpy (buffer, "Photoscript" );                           
	else if (vendor == 0x1281 ) strcpy (buffer, "Yokogawa" );                              
	else if (vendor == 0x1282 ) strcpy (buffer, "Davicom" );                               
	else if (vendor == 0x1283 ) strcpy (buffer, "ITExpress" );                             
	else if (vendor == 0x1284 ) strcpy (buffer, "Sahara" );                                
	else if (vendor == 0x1285 ) strcpy (buffer, "Plat Tech" );                             
	else if (vendor == 0x1286 ) strcpy (buffer, "Mazet" );                                 
	else if (vendor == 0x1287 ) strcpy (buffer, "LuxSonor" );                              
	else if (vendor == 0x1288 ) strcpy (buffer, "Timestep" );                              
	else if (vendor == 0x1289 ) strcpy (buffer, "AVC Tech" );                              
	else if (vendor == 0x128A ) strcpy (buffer, "Asante" );                                
	else if (vendor == 0x128B ) strcpy (buffer, "Transwitch" );                            
	else if (vendor == 0x128C ) strcpy (buffer, "Retix" );                                 
	else if (vendor == 0x128D ) strcpy (buffer, "G2 Net" );                                
	else if (vendor == 0x128E ) strcpy (buffer, "Samho" );                                 
	else if (vendor == 0x128F ) strcpy (buffer, "Tateno" );                                
	else if (vendor == 0x1290 ) strcpy (buffer, "Sord" );                                  
	else if (vendor == 0x1291 ) strcpy (buffer, "NCS Comp" );                              
	else if (vendor == 0x1292 ) strcpy (buffer, "Tritech" );                               
	else if (vendor == 0x1293 ) strcpy (buffer, "M Reality" );                             
	else if (vendor == 0x1294 ) strcpy (buffer, "Rhetorex" );                              
	else if (vendor == 0x1295 ) strcpy (buffer, "Imagenation" );                           
	else if (vendor == 0x1296 ) strcpy (buffer, "Kofax" );                                 
	else if (vendor == 0x1297 ) strcpy (buffer, "Holco" );                                 
	else if (vendor == 0x1298 ) strcpy (buffer, "Spellcaster" );                           
	else if (vendor == 0x1299 ) strcpy (buffer, "Know Tech" );                             
	else if (vendor == 0x129A ) strcpy (buffer, "VMETRO" );                                
	else if (vendor == 0x129B ) strcpy (buffer, "Img Access" );                            
	else if (vendor == 0x129D ) strcpy (buffer, "CompCore" );                              
	else if (vendor == 0x129E ) strcpy (buffer, "Victor Jpn" );                            
	else if (vendor == 0x129F ) strcpy (buffer, "OEC Med" );                               
	else if (vendor == 0x12A0 ) strcpy (buffer, "A-B" );                                   
	else if (vendor == 0x12A1 ) strcpy (buffer, "Simpact" );                               
	else if (vendor == 0x12A2 ) strcpy (buffer, "NewGen" );                                
	else if (vendor == 0x12A3 ) strcpy (buffer, "Lucent" );                                
	else if (vendor == 0x12A4 ) strcpy (buffer, "NTT Elect" );                             
	else if (vendor == 0x12A5 ) strcpy (buffer, "Vision Dyn" );                            
	else if (vendor == 0x12A6 ) strcpy (buffer, "Scalable" );                              
	else if (vendor == 0x12A7 ) strcpy (buffer, "AMO" );                                   
	else if (vendor == 0x12A8 ) strcpy (buffer, "News Datacom" );                          
	else if (vendor == 0x12A9 ) strcpy (buffer, "Xiotech" );                               
	else if (vendor == 0x12AA ) strcpy (buffer, "SDL" );                                   
	else if (vendor == 0x12AB ) strcpy (buffer, "Yuan Yuan" );                             
	else if (vendor == 0x12AC ) strcpy (buffer, "MeasureX" );                              
	else if (vendor == 0x12AD ) strcpy (buffer, "Multidata" );                             
	else if (vendor == 0x12AE ) strcpy (buffer, "Alteon" );                                
	else if (vendor == 0x12AF ) strcpy (buffer, "TDK USA" );                               
	else if (vendor == 0x12B0 ) strcpy (buffer, "Jorge Sci" );                             
	else if (vendor == 0x12B1 ) strcpy (buffer, "GammaLink" );                             
	else if (vendor == 0x12B2 ) strcpy (buffer, "Gen Signal" );                            
	else if (vendor == 0x12B3 ) strcpy (buffer, "Inter-Face" );                            
	else if (vendor == 0x12B4 ) strcpy (buffer, "Future Tel" );                            
	else if (vendor == 0x12B5 ) strcpy (buffer, "Granite" );                               
	else if (vendor == 0x12B6 ) strcpy (buffer, "Nat Micro" );                             
	else if (vendor == 0x12B7 ) strcpy (buffer, "Acumen" );                                
	else if (vendor == 0x12B8 ) strcpy (buffer, "Korg" );                                  
	else if (vendor == 0x12B9 ) strcpy (buffer, "US Robotics" );                           
	else if (vendor == 0x12BA ) strcpy (buffer, "Bittware" );                              
	else if (vendor == 0x12BB ) strcpy (buffer, "Nippon Uni" );                            
	else if (vendor == 0x12BC ) strcpy (buffer, "Array Micro" );                           
	else if (vendor == 0x12BD ) strcpy (buffer, "Computerm" );                             
	else if (vendor == 0x12BE ) strcpy (buffer, "Anchor Chips" );                          
	else if (vendor == 0x12BF ) strcpy (buffer, "Fujifilm" );                              
	else if (vendor == 0x12C0 ) strcpy (buffer, "Infimed" );                               
	else if (vendor == 0x12C1 ) strcpy (buffer, "GMM Res" );                               
	else if (vendor == 0x12C2 ) strcpy (buffer, "Mentec" );                                
	else if (vendor == 0x12C3 ) strcpy (buffer, "Holtek" );                                
	else if (vendor == 0x12C4 ) strcpy (buffer, "Conn Tech" );                             
	else if (vendor == 0x12C5 ) strcpy (buffer, "PicturEl" );                              
	else if (vendor == 0x12C6 ) strcpy (buffer, "Mitani" );                                
	else if (vendor == 0x12C7 ) strcpy (buffer, "Dialogic" );                              
	else if (vendor == 0x12C8 ) strcpy (buffer, "G Force" );                               
	else if (vendor == 0x12C9 ) strcpy (buffer, "Gigi Ops" );                              
	else if (vendor == 0x12CA ) strcpy (buffer, "I C Engines" );                           
	else if (vendor == 0x12CA ) strcpy (buffer, "ICE" );                                   
	else if (vendor == 0x12CB ) strcpy (buffer, "Antex" );                                 
	else if (vendor == 0x12CC ) strcpy (buffer, "Pluto" );                                 
	else if (vendor == 0x12CD ) strcpy (buffer, "Aims Lab" );                              
	else if (vendor == 0x12CE ) strcpy (buffer, "Netspeed" );                              
	else if (vendor == 0x12CF ) strcpy (buffer, "Prophet" );                               
	else if (vendor == 0x12D0 ) strcpy (buffer, "GDE Sys" );                               
	else if (vendor == 0x12D1 ) strcpy (buffer, "PsiTech" );                               
	else if (vendor == 0x12D2 ) strcpy (buffer, "NVidia" );                                
	else if (vendor == 0x12D3 ) strcpy (buffer, "Vingmed" );                               
	else if (vendor == 0x12D4 ) strcpy (buffer, "DGM&S" );                                 
	else if (vendor == 0x12D5 ) strcpy (buffer, "Equator" );                               
	else if (vendor == 0x12D6 ) strcpy (buffer, "Analogic" );                              
	else if (vendor == 0x12D7 ) strcpy (buffer, "Biotronic" );                             
	else if (vendor == 0x12D8 ) strcpy (buffer, "Pericom" );                               
	else if (vendor == 0x12D9 ) strcpy (buffer, "Aculab" );                                
	else if (vendor == 0x12DA ) strcpy (buffer, "True Time" );                             
	else if (vendor == 0x12DB ) strcpy (buffer, "Annapolis" );                             
	else if (vendor == 0x12DC ) strcpy (buffer, "Symicron" );                              
	else if (vendor == 0x12DD ) strcpy (buffer, "MGI" );                                   
	else if (vendor == 0x12DE ) strcpy (buffer, "Rainbow" );                               
	else if (vendor == 0x12DF ) strcpy (buffer, "SBS Tech" );                              
	else if (vendor == 0x12E0 ) strcpy (buffer, "Chase" );                                 
	else if (vendor == 0x12E1 ) strcpy (buffer, "Nintendo" );                              
	else if (vendor == 0x12E2 ) strcpy (buffer, "Datum" );                                 
	else if (vendor == 0x12E3 ) strcpy (buffer, "Imation" );                               
	else if (vendor == 0x12E4 ) strcpy (buffer, "Brooktrout" );                            
	else if (vendor == 0x12E6 ) strcpy (buffer, "Cirel" );                                 
	else if (vendor == 0x12E7 ) strcpy (buffer, "Sebring" );                               
	else if (vendor == 0x12E8 ) strcpy (buffer, "CRISC" );                                 
	else if (vendor == 0x12E9 ) strcpy (buffer, "GE Spacenet" );                           
	else if (vendor == 0x12EA ) strcpy (buffer, "Zuken" );                                 
	else if (vendor == 0x12EB ) strcpy (buffer, "Aureal" );                                
	else if (vendor == 0x12EC ) strcpy (buffer, "3A Intl" );                               
	else if (vendor == 0x12ED ) strcpy (buffer, "Optivision" );                            
	else if (vendor == 0x12EE ) strcpy (buffer, "Orange Micro" );                          
	else if (vendor == 0x12EF ) strcpy (buffer, "Vienna" );                                
	else if (vendor == 0x12F0 ) strcpy (buffer, "Pentek" );                                
	else if (vendor == 0x12F1 ) strcpy (buffer, "Sorenson" );                              
	else if (vendor == 0x12F2 ) strcpy (buffer, "Gammagraphx" );                           
	else if (vendor == 0x12F4 ) strcpy (buffer, "Megatel" );                               
	else if (vendor == 0x12F5 ) strcpy (buffer, "Forks" );                                 
	else if (vendor == 0x12F6 ) strcpy (buffer, "Dawson Fr" );                             
	else if (vendor == 0x12F7 ) strcpy (buffer, "Cognex" );                                
	else if (vendor == 0x12F8 ) strcpy (buffer, "Electronic-Design" );                     
	else if (vendor == 0x12F9 ) strcpy (buffer, "FFT" );                                   
	else if (vendor == 0x12FE ) strcpy (buffer, "ESD" );                                   
	else if (vendor == 0x1331 ) strcpy (buffer, "RadiSys" );                               
	else if (vendor == 0x1335 ) strcpy (buffer, "Videomail" );                             
	else if (vendor == 0x1347 ) strcpy (buffer, "Odetics" );                               
	else if (vendor == 0x135D ) strcpy (buffer, "ABB Network Partner" );                   
	else if (vendor == 0x137E ) strcpy (buffer, "PTSC" );                                  
	else if (vendor == 0x1C1C ) strcpy (buffer, "Symphony" );                              
	else if (vendor == 0x1DE1 ) strcpy (buffer, "Tekram" );                                
	else if (vendor == 0x270F ) strcpy (buffer, "ChainTech" );                             
	else if (vendor == 0x3000 ) strcpy (buffer, "Hansol" );                                
	else if (vendor == 0x3142 ) strcpy (buffer, "PostImpressions" );                       
	else if (vendor == 0x3D3D ) strcpy (buffer, "3DLabs" );                                
	else if (vendor == 0x4005 ) strcpy (buffer, "Avance" );                                
	else if (vendor == 0x4594 ) strcpy (buffer, "" );                                      
	else if (vendor == 0x4680 ) strcpy (buffer, "UMAX Comp" );                             
	else if (vendor == 0x4B10 ) strcpy (buffer, "Buslogic" );                              
	else if (vendor == 0x5333 ) strcpy (buffer, "S3" );                                    
	else if (vendor == 0x5455 ) strcpy (buffer, "TU-Berlin" );                             
	else if (vendor == 0x5519 ) strcpy (buffer, "Cnet" );                                  
	else if (vendor == 0x5700 ) strcpy (buffer, "Netpower" );                              
	else if (vendor == 0x6374 ) strcpy (buffer, "C4T" );                                   
	else if (vendor == 0x8008 ) strcpy (buffer, "" );                                      
	else if (vendor == 0x8008 ) strcpy (buffer, "Quancom" );                               
	else if (vendor == 0x8086 ) strcpy (buffer, "Intel" );                                 
	else if (vendor == 0x8800 ) strcpy (buffer, "Trigem" );                                
	else if (vendor == 0x8888 ) strcpy (buffer, "Sil Magic" );                             
	else if (vendor == 0x8E0E ) strcpy (buffer, "Computone" );                             
	else if (vendor == 0x9004 ) strcpy (buffer, "Adaptec" );                               
	else if (vendor == 0x907F ) strcpy (buffer, "Atronics" );                              
	else if (vendor == 0xC0FE ) strcpy (buffer, "Mot Engrg" );                             
	else if (vendor == 0xD4D4 ) strcpy (buffer, "DY4" );                                   
	else if (vendor == 0xE159 ) strcpy (buffer, "Tiger Jet" );                             
	else if (vendor == 0xEDD8 ) strcpy (buffer, "ARC" );                                   
	else if (vendor == 0xFFFF ) strcpy (buffer, "BAD!" );                                                            	

   if      (vendor == 0x1022 && device == 0x1100) strcat (buffer, " K8 HyperTransport config");
   else if (vendor == 0x1022 && device == 0x1101) strcat (buffer, " K8 address map");
   else if (vendor == 0x1022 && device == 0x1102) strcat (buffer, " K8 memory controller");
   else if (vendor == 0x1022 && device == 0x1103) strcat (buffer, " K8 misc control");
   else if (vendor == 0x1022 && device == 0x7450) strcat (buffer, " 8131 PCI-X bridge");
   else if (vendor == 0x1022 && device == 0x7451) strcat (buffer, " 8131 I/O APIC");
   else if (vendor == 0x1022 && device == 0x7458) strcat (buffer, " 8132 PCI-X bridge");
   else if (vendor == 0x1022 && device == 0x7459) strcat (buffer, " 8132 I/O APIC");
   else if (vendor == 0x1022 && device == 0x7460) strcat (buffer, " 8111 PCI bridge");
   else if (vendor == 0x1022 && device == 0x7462) strcat (buffer, " ethernet");
   else if (vendor == 0x1022 && device == 0x7463) strcat (buffer, " USB enhanced host controller");
   else if (vendor == 0x1022 && device == 0x7464) strcat (buffer, " USB open host controller");
   else if (vendor == 0x1022 && device == 0x7468) strcat (buffer, " LPC bridge");
   else if (vendor == 0x1022 && device == 0x7469) strcat (buffer, " IDE");
   else if (vendor == 0x1022 && device == 0x746A) strcat (buffer, " SMBUS 2.0");
   else if (vendor == 0x1022 && device == 0x746B) strcat (buffer, " system management");
   else if (vendor == 0x1022 && device == 0x746D) strcat (buffer, " audio");
   else if (vendor == 0x1022 && device == 0x746E) strcat (buffer, " modem");
   else if (vendor == 0x8086 && device == 0x2778) strcat (buffer, " Mukilteo MCH");
   else if (vendor == 0x8086 && device == 0x2774) strcat (buffer, " Glenwood MCH");
   else if (vendor == 0x8086 && device == 0x244E) strcat (buffer, " ICHx PCI bridge");
   else if (vendor == 0x8086 && device == 0x27DE) strcat (buffer, " ICH7 AC97 audio");
   else if (vendor == 0x8086 && device == 0x27DD) strcat (buffer, " ICH7 AC97 modem");
   else if (vendor == 0x8086 && device == 0x27B0) strcat (buffer, " ICH7 LPC and misc. (engineering sample)");
   else if (vendor == 0x8086 && device == 0x27B8) strcat (buffer, " ICH7 LPC and misc.");
   else if (vendor == 0x8086 && device == 0x27DF) strcat (buffer, " ICH7 IDE");
   else if (vendor == 0x8086 && device == 0x27C0) strcat (buffer, " ICH7 SATA non-AHCI mode");
   else if (vendor == 0x8086 && device == 0x27C1) strcat (buffer, " ICH7 SATA AHCI mode");
   else if (vendor == 0x8086 && device == 0x27C2) strcat (buffer, " ICH7 SATA RAID 0/1/10");
   else if (vendor == 0x8086 && device == 0x27C3) strcat (buffer, " ICH7 SATA RAID 0/1/5/10");
   else if (vendor == 0x8086 && device == 0x27DA) strcat (buffer, " ICH7 SMBUS");
   else if (vendor == 0x8086 && device == 0x27C8) strcat (buffer, " ICH7 USB UHCI 1");
   else if (vendor == 0x8086 && device == 0x27C9) strcat (buffer, " ICH7 USB UHCI 2");
   else if (vendor == 0x8086 && device == 0x27CA) strcat (buffer, " ICH7 USB UHCI 3");
   else if (vendor == 0x8086 && device == 0x27CB) strcat (buffer, " ICH7 USB UHCI 4");
   else if (vendor == 0x8086 && device == 0x27CC) strcat (buffer, " ICH7 USB 2.0 EHCI");
   else if (vendor == 0x8086 && device == 0x27D0) strcat (buffer, " ICH7 PCI express port 1");
   else if (vendor == 0x8086 && device == 0x27D2) strcat (buffer, " ICH7 PCI express port 2");
   else if (vendor == 0x8086 && device == 0x27D4) strcat (buffer, " ICH7 PCI express port 3");
   else if (vendor == 0x8086 && device == 0x27D6) strcat (buffer, " ICH7 PCI express port 4");
   else if (vendor == 0x8086 && device == 0x27E0) strcat (buffer, " ICH7 PCI express port 5");
   else if (vendor == 0x8086 && device == 0x27E2) strcat (buffer, " ICH7 PCI express port 6");
   else if (vendor == 0x8086 && device == 0x27D8) strcat (buffer, " ICH7 HD audio");

   else if (vendor == 0x8086 && device == 0x2698) strcat (buffer, " ESB2 AC97 audio");
   else if (vendor == 0x8086 && device == 0x2699) strcat (buffer, " ESB2 AC97 modem");
   else if (vendor == 0x8086 && (device & 0xFFF0) == 0x2670) strcat (buffer, " ESB2 LPC and misc.");
   else if (vendor == 0x8086 && device == 0x269E) strcat (buffer, " ESB2 PATA");
   else if (vendor == 0x8086 && device == 0x2680) strcat (buffer, " ESB2 SATA as IDE");
   else if (vendor == 0x8086 && device == 0x2681) strcat (buffer, " ESB2 SATA as SATA");
   else if (vendor == 0x8086 && device == 0x2682) strcat (buffer, " ESB2 SATA as RAID 0/1/5");
   else if (vendor == 0x8086 && device == 0x2683) strcat (buffer, " ESB2 SATA as RAID 0/1");

   else if (vendor == 0x8086 && device == 0x269B) strcat (buffer, " ESB2 SMBUS");
   else if (vendor == 0x8086 && device == 0x2690) strcat (buffer, " ESB2 PCI express port 1a");
   else if (vendor == 0x8086 && device == 0x2691) strcat (buffer, " ESB2 PCI express port 1b");
   else if (vendor == 0x8086 && device == 0x2692) strcat (buffer, " ESB2 PCI express port 2a");
   else if (vendor == 0x8086 && device == 0x2693) strcat (buffer, " ESB2 PCI express port 2b");
   else if (vendor == 0x8086 && device == 0x2694) strcat (buffer, " ESB2 PCI express port 3a");
   else if (vendor == 0x8086 && device == 0x2695) strcat (buffer, " ESB2 PCI express port 3b");
   else if (vendor == 0x8086 && device == 0x2696) strcat (buffer, " ESB2 PCI express port 4a");
   else if (vendor == 0x8086 && device == 0x2697) strcat (buffer, " ESB2 PCI express port 4b");
   else if (vendor == 0x8086 && device == 0x2688) strcat (buffer, " ESB2 USB 1.1 port 0,1");
   else if (vendor == 0x8086 && device == 0x2689) strcat (buffer, " ESB2 USB 1.1 port 2,3");
   else if (vendor == 0x8086 && device == 0x268A) strcat (buffer, " ESB2 USB 1.1 port 4,5");
   else if (vendor == 0x8086 && device == 0x268B) strcat (buffer, " ESB2 USB 1.1 port 6,7");
   else if (vendor == 0x8086 && device == 0x268C) strcat (buffer, " ESB2 USB 2.0 host controller-0");
   else if (vendor == 0x8086 && device == 0x268D) strcat (buffer, " ESB2 USB 2.0 host controller-1");
   else if (vendor == 0x8086 && device == 0x268E) strcat (buffer, " ESB2 USB 2.0 host controller-2");
   else if (vendor == 0x8086 && device == 0x268F) strcat (buffer, " ESB2 USB 2.0 host controller-3");
   else if (vendor == 0x8086 && device == 0x269A) strcat (buffer, " ESB2 HD audio");
   else if (vendor == 0x8086 && device == 0x2698) strcat (buffer, " ESB2 AC97 audio");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x3500) strcat (buffer, " ESB2 PCIe upstream port");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x3504) strcat (buffer, " ESB2 I/OxAPIC controller");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x350C) strcat (buffer, " ESB2 PCIe to PCIX bridge");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x3510) strcat (buffer, " ESB2 PCIe downstream port 1");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x3514) strcat (buffer, " ESB2 PCIe downstream port 2");
   else if (vendor == 0x8086 && (device & 0xFFFC) == 0x3518) strcat (buffer, " ESB2 PCIe downstream port 3");

   else if (vendor == 0x8086 && device == 0x25E2) strcat (buffer, " Blackford MCH PCI express port 2");
   else if (vendor == 0x8086 && device == 0x25E3) strcat (buffer, " Blackford MCH PCI express port 3");
   else if (vendor == 0x8086 && device == 0x25E4) strcat (buffer, " Blackford MCH PCI express port 4");
   else if (vendor == 0x8086 && device == 0x25E5) strcat (buffer, " Blackford MCH PCI express port 5");
   else if (vendor == 0x8086 && device == 0x25E6) strcat (buffer, " Blackford MCH PCI express port 6");
   else if (vendor == 0x8086 && device == 0x25E7) strcat (buffer, " Blackford MCH PCI express port 7");
   else if (vendor == 0x8086 && device == 0x1A38) strcat (buffer, " Blackford MCH Crystal Beach (DMA)");
   else if (vendor == 0x8086 && device == 0x25F5) strcat (buffer, " Blackford MCH MM,EF,RAS,CC FBD branch 0");
   else if (vendor == 0x8086 && device == 0x25F6) strcat (buffer, " Blackford MCH MM,EF,RAS,CC FBD branch 1");
   else if (vendor == 0x8086 && device == 0x25F0) strcat (buffer, " Blackford MCH cpu bus, boot, int, SA");
   else if (vendor == 0x8086 && device == 0x25E8) strcat (buffer, " Blackford MCH AMB MMIO registers");
   else if (vendor == 0x8086 && device == 0x25F7) strcat (buffer, " Blackford MCH PCI express port 2-3");
   else if (vendor == 0x8086 && device == 0x25F8) strcat (buffer, " Blackford MCH PCI express port 4-5");
   else if (vendor == 0x8086 && device == 0x25F9) strcat (buffer, " Blackford MCH PCI express port 6-7");
   else if (vendor == 0x8086 && device == 0x25FA) strcat (buffer, " Blackford MCH PCI express port 4-7");

   else if (vendor == 0x1166 && device == 0x0036) strcat (buffer, " PCIX bridge");
   else if (vendor == 0x1166 && device == 0x0214) strcat (buffer, " IDE");
   else if (vendor == 0x1166 && device == 0x0234) strcat (buffer, " LPC bridge");
   else if (vendor == 0x1166 && device == 0x0238) strcat (buffer, " Watchdog timer");
   else if (vendor == 0x1166 && device == 0x0235) strcat (buffer, " XIOAPIC");
   else if (vendor == 0x1166 && device == 0x0223) strcat (buffer, " EHCI controller");
   else if (vendor == 0x1166 && device == 0x0104) strcat (buffer, " Pci to PCI bridge");
   else if (vendor == 0x1166 && device == 0x024A) strcat (buffer, " SATA");
   else if (vendor == 0x1166 && device == 0x0142) strcat (buffer, " EXB");
   else if (vendor == 0x1166 && device == 0x0144) strcat (buffer, " EXB");

   if (*buffer == '\0') return NULL;
   return buffer;
   }


#if 0
__u32 dosReadPciConfig (int bus, int device, int function, int offset, int bytes)
{
	#define	PCI_DOMAIN	0

	static struct pci_dev *p = 0;
	static struct pci_access *pacc = 0;
   	__u32 value;

	pacc = pci_alloc();
    pci_init(pacc);
	pci_scan_bus(pacc);
	p=pci_get_dev(pacc, PCI_DOMAIN,bus,device,function);
	value = pci_read_long(p,(offset& ~3));
   
	value >>= (offset & 3) * 8;
   	value &= pciConfigMask [bytes - 1];
	pci_free_dev(p);
   	return value;
}
#else
__u32 dosReadPciConfig (int bus, int device, int function, int offset, int bytes)
{
   __u32 value;
   __u32 address = inl_p (CONFADD) & 3  | // preserve reserved
                    0x80000000          | // set config enable
                    bus  << 16          | // fill in bus
                    device  << 11       | // fill in device
                    function << 8       | // fill in function
                    offset  & ~3;         // fill in register

   outl_p (address, CONFADD);
   value = inl_p (CONFDATA);
   value >>= (offset & 3) * 8;
   value &= pciConfigMask [bytes - 1];
   return value;
}
#endif

void dosWritePciConfig (int bus, int device, int function, int offset, int value, int bytes, int reserved)
{
   __u32 longval;
   int    writable= pciConfigMask [bytes - 1] & ~reserved;
   int    shift   = (offset & 3) * 8;
   __u32 address = inl_p (CONFADD) & 3  | // preserve reserved
                    0x80000000          | // set config enable
                    bus  << 16          | // fill in bus
                    device << 11        | // fill in device
                    function << 8       | // fill in function
                    offset  & ~3;         // fill in register

   outl_p (address , CONFADD);

   longval = inl_p (CONFDATA);
   longval &= ~(writable << shift);
   longval |= value << shift;
   outl_p (longval,CONFDATA);
}


//---------------------------------------------------------------------------
char *readPciConfig (int bus, int device, int function, int offset, int bytesRequested, int *bytesReturned, __u32 *data)
   {
   	
      *data = dosReadPciConfig (bus, device, function, offset, bytesRequested);
      *bytesReturned = bytesRequested;
      return NULL;
      
   }

//---------------------------------------------------------------------------

char *writePciConfig (int bus, int device, int function, int offset, int bytes, __u32 data)
   {
      dosWritePciConfig (bus, device, function, offset, data, bytes, 0);
      return NULL;
      
   }


//---------------------------------------------------------------------------

char *ioRead (int argc, char *argv [], int ioSize)
   {
   u64 port;
   char *error;
   u64 data;
   if (argc != 3) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [2], &port, 4);
   if (error) return error;

	scrollok(stdscr,TRUE);
   if(ioSize == 8)
   	{
  	  printw ("%0X\n", inb_p(port));
   	}
   else if (ioSize ==16)
   	  printw ("%02X\n", inw_p(port));
   else if(ioSize ==32)
   	  printw ("%04X\n", inl_p(port));

   scrollok(stdscr,FALSE);
   return NULL;
   }

//---------------------------------------------------------------------------

char *ioWrite (int argc, char *argv [], int ioSize)
   {
   u64 port, data;
   char *error;

   if (argc != 4) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [2], &port, 4); if (error) return error;
   error = hextoint64 (argv [3], &data, ioSize / 4); if (error) return error;

   if(ioSize == 8)
     outb_p(data,port);
   else if (ioSize ==16)
   	 outw_p(data,port);
   else if (ioSize ==32)
   	 outl_p(data,port);
   return NULL;
   }

//---------------------------------------------------------------------------

static char *dumpFunctions (int bus, int device, int dataSize, int *busFound)
   {
   int function, bytesReturned;
   char *error = NULL;

   *busFound = 1;
   for (function = 0; function < 8; function++)
      {
      __u32 data;
      char busText [4], deviceText [4], functionText [4];
      char *argv [5] = {NULL, NULL, busText, deviceText, functionText};
      char *error = readPciConfig (bus, device, function, 0, 4, &bytesReturned, &data);
      if (error) return error;
      if (bytesReturned == 0) // windows return for no bus
         {
         *busFound = 0;
         return NULL;
         }

      // skip unimplemented functions in a multi-function device
      if (bytesReturned == 2) continue; // windows return for no device
      if (data == 0xFFFFFFFF) continue; // DOS data for no function

      if (data == 0xFFFFFFFF) bytesReturned = 0;     // DOS return for no device
      if (bytesReturned < 4 && function == 0) break; // stop scanning if no function zero, no function zero means no device

      sprintf (argv [2], "%X", bus);
      sprintf (argv [3], "%X", device);
      sprintf (argv [4], "%X", function);
      error = pciDump (5, argv, dataSize);
      if (error) return error;

      // ignore remaining functions if not a multi-function device
      if (function == 0)
         {
         error = readPciConfig (bus, device, function, 0xE, 1, &bytesReturned, &data);
         if (error) return error;
         if (bytesReturned != 1) break;
         if (pullbit (data, 7) == 0) break;
         }
      }
   return error;
   }

//---------------------------------------------------------------------------

static char *dumpAllPci (int dataSize)
   {
   int bus, device, busFound;
   char *error;

   for (bus = 0; bus < 256; bus++)
      {
      for (device = 0; device < 32; device++)
         {
         error = dumpFunctions (bus, device, dataSize, &busFound);
         if (error) return error;
         if (!busFound) break;
         }
      }
   return NULL;
   }


char *pciDump (int argc, char *argv [], int dataSize)
{
	char *error;
   	u64 bus, dev, func;
   	u64 startAddress, endAddress, transferCount;

   	if (argc == 2)
    	return dumpAllPci (dataSize);
   
   	if (argc == 5) {
        startAddress = 0;
        transferCount = 256 * 8 / dataSize;
    } else if (argc == 6) {
        error = hextoint64 (argv [5], &startAddress, 3); if (error) return error;
        transferCount = 128 * 8 / dataSize;
    } else if (argc == 7) {
        error = hextoint64 (argv [5], &startAddress, 3); if (error) return error;
        error = hextoint64 (argv [6], &endAddress, 3);   if (error) return error;
      
        if (endAddress < startAddress) endAddress = endAddress + startAddress - 1;

        transferCount = (endAddress - startAddress + 1) * 8 / dataSize;
    } else return formatMessage ("invalid argument count");

    error = hextoint64 (argv [2], &bus,  2);  if (error) return error;
    error = hextoint64 (argv [3], &dev,  2);  if (error) return error;
    error = hextoint64 (argv [4], &func, 2);  if (error) return error;

    if (argc == 5) {
		char *text, *error;
        __u32 data, bytesReturned;
        error = readPciConfig (bus, dev, func, 0, 4, &bytesReturned, &data);
        if (error) return error;
	    scrollok(stdscr,TRUE);
        printw ("bus %02X, device %02X, function %02X",bus, dev, func);
	    text = pciDeviceDescription (pullbits (data, 0, 15), pullbits (data, 16, 31));
        if (text) printw (" (%s)", text);
        printw ("\n");
	    scrollok(stdscr,FALSE);
	}
    while (transferCount)
	{
    	int offset, index, addressColumns = 3, columns = 16;
        int bytesRequested = dataSize / 8, bytesReturned;
        char buffer [81];

	    scrollok(stdscr,TRUE);
        if (dataSize > 8) columns = 8;
        memset (buffer, ' ', sizeof (buffer));
	
        sprintf (buffer, "%03X-",startAddress);
        for (index = 0, offset = (addressColumns + 1) - (dataSize / 4 + 1); index < columns; index++) {
        	__u32 value;
            error = readPciConfig (bus, dev, func, startAddress, bytesRequested, &bytesReturned, &value);
            if (error) return error;
            offset += dataSize / 4 + 1;
            if (index == columns / 2) offset += 2;
            sprintf (buffer + offset, "%0*X", dataSize / 4, value);
            buffer [offset + dataSize / 4] = ' '; /* remove NULL */
            startAddress += dataSize / 8;
	//printw("offset:%d Columns:%d,dataSize:%d,startAdd:%d\n",offset,addressColumns,dataSize,startAddress);
            transferCount--;
    		if (transferCount == 0) break;
		}
      	buffer [offset + dataSize / 4] = '\0';
      	printw ("%s\n", buffer);
	  	scrollok(stdscr,FALSE);
      	if (kbhit ()) return formatMessage ("dump canceled");
	}

   	return NULL;
}

//---------------------------------------------------------------------------

char *pciEnter (int argc, char *argv [], int dataSize)
   {
   u64 data[MAX_ARGS], startAddress;
   int    index, elements;
   u64 bus, dev, func;
   char   *error;

   if (argc < 7) return formatMessage ("invalid argument count");
   error = hextoint64 (argv [5], &startAddress, 3);                        if (error) return error;
   error = buildDataList (argc - 6, &argv[6], dataSize, (u64 *)data, &elements); if (error) return error;
   
   error = hextoint64 (argv [2], &bus,  2);  if (error) return error;
   error = hextoint64 (argv [3], &dev,  2);  if (error) return error;
   error = hextoint64 (argv [4], &func, 2);  if (error) return error;

   for (index = 0; index < elements; index++)
      writePciConfig (bus, dev, func, startAddress + index * dataSize / 8, dataSize / 8, data [index]);
   return NULL;
   }

//---------------------------------------------------------------------------

__u32 ReadIoPort(__u32 _port , int  _width )
{
  __u32 index;

  if (8 == _width)
  	index = inb_p(_port);
  else if (16 == _width)
  	index = inw_p(_port);
  else if (32 == _width)
      index = inl_p(_port);

   return index;

}

void WriteIoPort (__u32 _port, int _width, __u32 data)
{
	if (8 == _width)
  		outb_p(data, _port);
  	else if (16 == _width)
  		outw_p(data, _port);
  	else if (32 == _width)
    	outl_p (data, _port);
   	return ;
 }

