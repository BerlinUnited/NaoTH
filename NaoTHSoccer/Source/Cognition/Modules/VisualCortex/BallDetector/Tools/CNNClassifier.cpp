#include "CNNClassifier.h"

bool CNNClassifier::classify(const BallCandidates::Patch& p){
// copy patch into intial data array out_step0
ASSERT(p.data.size() == 256);

out_step0[ 0][ 0] = p.data[ 0];
out_step0[ 0][ 1] = p.data[16];
out_step0[ 0][ 2] = p.data[32];
out_step0[ 0][ 3] = p.data[48];
out_step0[ 0][ 4] = p.data[64];
out_step0[ 0][ 5] = p.data[80];
out_step0[ 0][ 6] = p.data[96];
out_step0[ 0][ 7] = p.data[112];
out_step0[ 0][ 8] = p.data[128];
out_step0[ 0][ 9] = p.data[144];
out_step0[ 0][10] = p.data[160];
out_step0[ 0][11] = p.data[176];
out_step0[ 0][12] = p.data[192];
out_step0[ 0][13] = p.data[208];
out_step0[ 0][14] = p.data[224];
out_step0[ 0][15] = p.data[240];
out_step0[ 1][ 0] = p.data[ 1];
out_step0[ 1][ 1] = p.data[17];
out_step0[ 1][ 2] = p.data[33];
out_step0[ 1][ 3] = p.data[49];
out_step0[ 1][ 4] = p.data[65];
out_step0[ 1][ 5] = p.data[81];
out_step0[ 1][ 6] = p.data[97];
out_step0[ 1][ 7] = p.data[113];
out_step0[ 1][ 8] = p.data[129];
out_step0[ 1][ 9] = p.data[145];
out_step0[ 1][10] = p.data[161];
out_step0[ 1][11] = p.data[177];
out_step0[ 1][12] = p.data[193];
out_step0[ 1][13] = p.data[209];
out_step0[ 1][14] = p.data[225];
out_step0[ 1][15] = p.data[241];
out_step0[ 2][ 0] = p.data[ 2];
out_step0[ 2][ 1] = p.data[18];
out_step0[ 2][ 2] = p.data[34];
out_step0[ 2][ 3] = p.data[50];
out_step0[ 2][ 4] = p.data[66];
out_step0[ 2][ 5] = p.data[82];
out_step0[ 2][ 6] = p.data[98];
out_step0[ 2][ 7] = p.data[114];
out_step0[ 2][ 8] = p.data[130];
out_step0[ 2][ 9] = p.data[146];
out_step0[ 2][10] = p.data[162];
out_step0[ 2][11] = p.data[178];
out_step0[ 2][12] = p.data[194];
out_step0[ 2][13] = p.data[210];
out_step0[ 2][14] = p.data[226];
out_step0[ 2][15] = p.data[242];
out_step0[ 3][ 0] = p.data[ 3];
out_step0[ 3][ 1] = p.data[19];
out_step0[ 3][ 2] = p.data[35];
out_step0[ 3][ 3] = p.data[51];
out_step0[ 3][ 4] = p.data[67];
out_step0[ 3][ 5] = p.data[83];
out_step0[ 3][ 6] = p.data[99];
out_step0[ 3][ 7] = p.data[115];
out_step0[ 3][ 8] = p.data[131];
out_step0[ 3][ 9] = p.data[147];
out_step0[ 3][10] = p.data[163];
out_step0[ 3][11] = p.data[179];
out_step0[ 3][12] = p.data[195];
out_step0[ 3][13] = p.data[211];
out_step0[ 3][14] = p.data[227];
out_step0[ 3][15] = p.data[243];
out_step0[ 4][ 0] = p.data[ 4];
out_step0[ 4][ 1] = p.data[20];
out_step0[ 4][ 2] = p.data[36];
out_step0[ 4][ 3] = p.data[52];
out_step0[ 4][ 4] = p.data[68];
out_step0[ 4][ 5] = p.data[84];
out_step0[ 4][ 6] = p.data[100];
out_step0[ 4][ 7] = p.data[116];
out_step0[ 4][ 8] = p.data[132];
out_step0[ 4][ 9] = p.data[148];
out_step0[ 4][10] = p.data[164];
out_step0[ 4][11] = p.data[180];
out_step0[ 4][12] = p.data[196];
out_step0[ 4][13] = p.data[212];
out_step0[ 4][14] = p.data[228];
out_step0[ 4][15] = p.data[244];
out_step0[ 5][ 0] = p.data[ 5];
out_step0[ 5][ 1] = p.data[21];
out_step0[ 5][ 2] = p.data[37];
out_step0[ 5][ 3] = p.data[53];
out_step0[ 5][ 4] = p.data[69];
out_step0[ 5][ 5] = p.data[85];
out_step0[ 5][ 6] = p.data[101];
out_step0[ 5][ 7] = p.data[117];
out_step0[ 5][ 8] = p.data[133];
out_step0[ 5][ 9] = p.data[149];
out_step0[ 5][10] = p.data[165];
out_step0[ 5][11] = p.data[181];
out_step0[ 5][12] = p.data[197];
out_step0[ 5][13] = p.data[213];
out_step0[ 5][14] = p.data[229];
out_step0[ 5][15] = p.data[245];
out_step0[ 6][ 0] = p.data[ 6];
out_step0[ 6][ 1] = p.data[22];
out_step0[ 6][ 2] = p.data[38];
out_step0[ 6][ 3] = p.data[54];
out_step0[ 6][ 4] = p.data[70];
out_step0[ 6][ 5] = p.data[86];
out_step0[ 6][ 6] = p.data[102];
out_step0[ 6][ 7] = p.data[118];
out_step0[ 6][ 8] = p.data[134];
out_step0[ 6][ 9] = p.data[150];
out_step0[ 6][10] = p.data[166];
out_step0[ 6][11] = p.data[182];
out_step0[ 6][12] = p.data[198];
out_step0[ 6][13] = p.data[214];
out_step0[ 6][14] = p.data[230];
out_step0[ 6][15] = p.data[246];
out_step0[ 7][ 0] = p.data[ 7];
out_step0[ 7][ 1] = p.data[23];
out_step0[ 7][ 2] = p.data[39];
out_step0[ 7][ 3] = p.data[55];
out_step0[ 7][ 4] = p.data[71];
out_step0[ 7][ 5] = p.data[87];
out_step0[ 7][ 6] = p.data[103];
out_step0[ 7][ 7] = p.data[119];
out_step0[ 7][ 8] = p.data[135];
out_step0[ 7][ 9] = p.data[151];
out_step0[ 7][10] = p.data[167];
out_step0[ 7][11] = p.data[183];
out_step0[ 7][12] = p.data[199];
out_step0[ 7][13] = p.data[215];
out_step0[ 7][14] = p.data[231];
out_step0[ 7][15] = p.data[247];
out_step0[ 8][ 0] = p.data[ 8];
out_step0[ 8][ 1] = p.data[24];
out_step0[ 8][ 2] = p.data[40];
out_step0[ 8][ 3] = p.data[56];
out_step0[ 8][ 4] = p.data[72];
out_step0[ 8][ 5] = p.data[88];
out_step0[ 8][ 6] = p.data[104];
out_step0[ 8][ 7] = p.data[120];
out_step0[ 8][ 8] = p.data[136];
out_step0[ 8][ 9] = p.data[152];
out_step0[ 8][10] = p.data[168];
out_step0[ 8][11] = p.data[184];
out_step0[ 8][12] = p.data[200];
out_step0[ 8][13] = p.data[216];
out_step0[ 8][14] = p.data[232];
out_step0[ 8][15] = p.data[248];
out_step0[ 9][ 0] = p.data[ 9];
out_step0[ 9][ 1] = p.data[25];
out_step0[ 9][ 2] = p.data[41];
out_step0[ 9][ 3] = p.data[57];
out_step0[ 9][ 4] = p.data[73];
out_step0[ 9][ 5] = p.data[89];
out_step0[ 9][ 6] = p.data[105];
out_step0[ 9][ 7] = p.data[121];
out_step0[ 9][ 8] = p.data[137];
out_step0[ 9][ 9] = p.data[153];
out_step0[ 9][10] = p.data[169];
out_step0[ 9][11] = p.data[185];
out_step0[ 9][12] = p.data[201];
out_step0[ 9][13] = p.data[217];
out_step0[ 9][14] = p.data[233];
out_step0[ 9][15] = p.data[249];
out_step0[10][ 0] = p.data[10];
out_step0[10][ 1] = p.data[26];
out_step0[10][ 2] = p.data[42];
out_step0[10][ 3] = p.data[58];
out_step0[10][ 4] = p.data[74];
out_step0[10][ 5] = p.data[90];
out_step0[10][ 6] = p.data[106];
out_step0[10][ 7] = p.data[122];
out_step0[10][ 8] = p.data[138];
out_step0[10][ 9] = p.data[154];
out_step0[10][10] = p.data[170];
out_step0[10][11] = p.data[186];
out_step0[10][12] = p.data[202];
out_step0[10][13] = p.data[218];
out_step0[10][14] = p.data[234];
out_step0[10][15] = p.data[250];
out_step0[11][ 0] = p.data[11];
out_step0[11][ 1] = p.data[27];
out_step0[11][ 2] = p.data[43];
out_step0[11][ 3] = p.data[59];
out_step0[11][ 4] = p.data[75];
out_step0[11][ 5] = p.data[91];
out_step0[11][ 6] = p.data[107];
out_step0[11][ 7] = p.data[123];
out_step0[11][ 8] = p.data[139];
out_step0[11][ 9] = p.data[155];
out_step0[11][10] = p.data[171];
out_step0[11][11] = p.data[187];
out_step0[11][12] = p.data[203];
out_step0[11][13] = p.data[219];
out_step0[11][14] = p.data[235];
out_step0[11][15] = p.data[251];
out_step0[12][ 0] = p.data[12];
out_step0[12][ 1] = p.data[28];
out_step0[12][ 2] = p.data[44];
out_step0[12][ 3] = p.data[60];
out_step0[12][ 4] = p.data[76];
out_step0[12][ 5] = p.data[92];
out_step0[12][ 6] = p.data[108];
out_step0[12][ 7] = p.data[124];
out_step0[12][ 8] = p.data[140];
out_step0[12][ 9] = p.data[156];
out_step0[12][10] = p.data[172];
out_step0[12][11] = p.data[188];
out_step0[12][12] = p.data[204];
out_step0[12][13] = p.data[220];
out_step0[12][14] = p.data[236];
out_step0[12][15] = p.data[252];
out_step0[13][ 0] = p.data[13];
out_step0[13][ 1] = p.data[29];
out_step0[13][ 2] = p.data[45];
out_step0[13][ 3] = p.data[61];
out_step0[13][ 4] = p.data[77];
out_step0[13][ 5] = p.data[93];
out_step0[13][ 6] = p.data[109];
out_step0[13][ 7] = p.data[125];
out_step0[13][ 8] = p.data[141];
out_step0[13][ 9] = p.data[157];
out_step0[13][10] = p.data[173];
out_step0[13][11] = p.data[189];
out_step0[13][12] = p.data[205];
out_step0[13][13] = p.data[221];
out_step0[13][14] = p.data[237];
out_step0[13][15] = p.data[253];
out_step0[14][ 0] = p.data[14];
out_step0[14][ 1] = p.data[30];
out_step0[14][ 2] = p.data[46];
out_step0[14][ 3] = p.data[62];
out_step0[14][ 4] = p.data[78];
out_step0[14][ 5] = p.data[94];
out_step0[14][ 6] = p.data[110];
out_step0[14][ 7] = p.data[126];
out_step0[14][ 8] = p.data[142];
out_step0[14][ 9] = p.data[158];
out_step0[14][10] = p.data[174];
out_step0[14][11] = p.data[190];
out_step0[14][12] = p.data[206];
out_step0[14][13] = p.data[222];
out_step0[14][14] = p.data[238];
out_step0[14][15] = p.data[254];
out_step0[15][ 0] = p.data[15];
out_step0[15][ 1] = p.data[31];
out_step0[15][ 2] = p.data[47];
out_step0[15][ 3] = p.data[63];
out_step0[15][ 4] = p.data[79];
out_step0[15][ 5] = p.data[95];
out_step0[15][ 6] = p.data[111];
out_step0[15][ 7] = p.data[127];
out_step0[15][ 8] = p.data[143];
out_step0[15][ 9] = p.data[159];
out_step0[15][10] = p.data[175];
out_step0[15][11] = p.data[191];
out_step0[15][12] = p.data[207];
out_step0[15][13] = p.data[223];
out_step0[15][14] = p.data[239];
out_step0[15][15] = p.data[255];

// performe zero mean normalization
out_step1[ 0][ 0] = out_step0[ 0][ 0] - 157.97685242;
out_step1[ 0][ 1] = out_step0[ 0][ 1] - 159.02984619;
out_step1[ 0][ 2] = out_step0[ 0][ 2] - 159.81364441;
out_step1[ 0][ 3] = out_step0[ 0][ 3] - 160.60108948;
out_step1[ 0][ 4] = out_step0[ 0][ 4] - 160.35444641;
out_step1[ 0][ 5] = out_step0[ 0][ 5] - 158.68635559;
out_step1[ 0][ 6] = out_step0[ 0][ 6] - 158.72290039;
out_step1[ 0][ 7] = out_step0[ 0][ 7] - 158.18574524;
out_step1[ 0][ 8] = out_step0[ 0][ 8] - 158.27406311;
out_step1[ 0][ 9] = out_step0[ 0][ 9] - 156.85078430;
out_step1[ 0][10] = out_step0[ 0][10] - 154.57003784;
out_step1[ 0][11] = out_step0[ 0][11] - 154.15286255;
out_step1[ 0][12] = out_step0[ 0][12] - 154.44519043;
out_step1[ 0][13] = out_step0[ 0][13] - 155.27648926;
out_step1[ 0][14] = out_step0[ 0][14] - 155.64738464;
out_step1[ 0][15] = out_step0[ 0][15] - 155.40194702;
out_step1[ 1][ 0] = out_step0[ 1][ 0] - 157.17539978;
out_step1[ 1][ 1] = out_step0[ 1][ 1] - 158.96284485;
out_step1[ 1][ 2] = out_step0[ 1][ 2] - 160.04080200;
out_step1[ 1][ 3] = out_step0[ 1][ 3] - 161.62059021;
out_step1[ 1][ 4] = out_step0[ 1][ 4] - 163.47076416;
out_step1[ 1][ 5] = out_step0[ 1][ 5] - 165.79171753;
out_step1[ 1][ 6] = out_step0[ 1][ 6] - 166.79841614;
out_step1[ 1][ 7] = out_step0[ 1][ 7] - 166.82398987;
out_step1[ 1][ 8] = out_step0[ 1][ 8] - 165.75883484;
out_step1[ 1][ 9] = out_step0[ 1][ 9] - 163.52984619;
out_step1[ 1][10] = out_step0[ 1][10] - 159.91961670;
out_step1[ 1][11] = out_step0[ 1][11] - 157.40499878;
out_step1[ 1][12] = out_step0[ 1][12] - 154.91047668;
out_step1[ 1][13] = out_step0[ 1][13] - 155.46833801;
out_step1[ 1][14] = out_step0[ 1][14] - 155.90011597;
out_step1[ 1][15] = out_step0[ 1][15] - 155.04566956;
out_step1[ 2][ 0] = out_step0[ 2][ 0] - 157.32643127;
out_step1[ 2][ 1] = out_step0[ 2][ 1] - 160.08099365;
out_step1[ 2][ 2] = out_step0[ 2][ 2] - 163.14981079;
out_step1[ 2][ 3] = out_step0[ 2][ 3] - 166.63032532;
out_step1[ 2][ 4] = out_step0[ 2][ 4] - 175.96954346;
out_step1[ 2][ 5] = out_step0[ 2][ 5] - 186.06028748;
out_step1[ 2][ 6] = out_step0[ 2][ 6] - 190.18574524;
out_step1[ 2][ 7] = out_step0[ 2][ 7] - 191.10231018;
out_step1[ 2][ 8] = out_step0[ 2][ 8] - 190.28684998;
out_step1[ 2][ 9] = out_step0[ 2][ 9] - 188.30146790;
out_step1[ 2][10] = out_step0[ 2][10] - 177.91839600;
out_step1[ 2][11] = out_step0[ 2][11] - 167.66992188;
out_step1[ 2][12] = out_step0[ 2][12] - 161.95858765;
out_step1[ 2][13] = out_step0[ 2][13] - 157.37515259;
out_step1[ 2][14] = out_step0[ 2][14] - 155.65469360;
out_step1[ 2][15] = out_step0[ 2][15] - 155.48721313;
out_step1[ 3][ 0] = out_step0[ 3][ 0] - 156.92448425;
out_step1[ 3][ 1] = out_step0[ 3][ 1] - 159.83312988;
out_step1[ 3][ 2] = out_step0[ 3][ 2] - 165.77832031;
out_step1[ 3][ 3] = out_step0[ 3][ 3] - 178.93240356;
out_step1[ 3][ 4] = out_step0[ 3][ 4] - 197.59683228;
out_step1[ 3][ 5] = out_step0[ 3][ 5] - 201.11753845;
out_step1[ 3][ 6] = out_step0[ 3][ 6] - 203.36906433;
out_step1[ 3][ 7] = out_step0[ 3][ 7] - 200.02923584;
out_step1[ 3][ 8] = out_step0[ 3][ 8] - 199.66503906;
out_step1[ 3][ 9] = out_step0[ 3][ 9] - 199.86784363;
out_step1[ 3][10] = out_step0[ 3][10] - 197.12789917;
out_step1[ 3][11] = out_step0[ 3][11] - 189.82704163;
out_step1[ 3][12] = out_step0[ 3][12] - 174.21133423;
out_step1[ 3][13] = out_step0[ 3][13] - 163.02192688;
out_step1[ 3][14] = out_step0[ 3][14] - 156.53166199;
out_step1[ 3][15] = out_step0[ 3][15] - 155.02070618;
out_step1[ 4][ 0] = out_step0[ 4][ 0] - 155.68087769;
out_step1[ 4][ 1] = out_step0[ 4][ 1] - 158.35322571;
out_step1[ 4][ 2] = out_step0[ 4][ 2] - 171.60719299;
out_step1[ 4][ 3] = out_step0[ 4][ 3] - 195.16625977;
out_step1[ 4][ 4] = out_step0[ 4][ 4] - 205.47198486;
out_step1[ 4][ 5] = out_step0[ 4][ 5] - 207.05906677;
out_step1[ 4][ 6] = out_step0[ 4][ 6] - 205.55725098;
out_step1[ 4][ 7] = out_step0[ 4][ 7] - 204.24847412;
out_step1[ 4][ 8] = out_step0[ 4][ 8] - 203.86967468;
out_step1[ 4][ 9] = out_step0[ 4][ 9] - 204.22351074;
out_step1[ 4][10] = out_step0[ 4][10] - 200.68087769;
out_step1[ 4][11] = out_step0[ 4][11] - 192.58343506;
out_step1[ 4][12] = out_step0[ 4][12] - 187.48173523;
out_step1[ 4][13] = out_step0[ 4][13] - 170.92265320;
out_step1[ 4][14] = out_step0[ 4][14] - 157.45675659;
out_step1[ 4][15] = out_step0[ 4][15] - 153.74421692;
out_step1[ 5][ 0] = out_step0[ 5][ 0] - 153.92874146;
out_step1[ 5][ 1] = out_step0[ 5][ 1] - 159.37028503;
out_step1[ 5][ 2] = out_step0[ 5][ 2] - 180.50914001;
out_step1[ 5][ 3] = out_step0[ 5][ 3] - 197.88977051;
out_step1[ 5][ 4] = out_step0[ 5][ 4] - 205.29536438;
out_step1[ 5][ 5] = out_step0[ 5][ 5] - 206.35931396;
out_step1[ 5][ 6] = out_step0[ 5][ 6] - 205.05116272;
out_step1[ 5][ 7] = out_step0[ 5][ 7] - 205.66320801;
out_step1[ 5][ 8] = out_step0[ 5][ 8] - 207.67417908;
out_step1[ 5][ 9] = out_step0[ 5][ 9] - 208.09866333;
out_step1[ 5][10] = out_step0[ 5][10] - 202.36906433;
out_step1[ 5][11] = out_step0[ 5][11] - 194.22412109;
out_step1[ 5][12] = out_step0[ 5][12] - 186.32521057;
out_step1[ 5][13] = out_step0[ 5][13] - 178.74604797;
out_step1[ 5][14] = out_step0[ 5][14] - 158.80511475;
out_step1[ 5][15] = out_step0[ 5][15] - 154.02253723;
out_step1[ 6][ 0] = out_step0[ 6][ 0] - 153.57551575;
out_step1[ 6][ 1] = out_step0[ 6][ 1] - 159.87271118;
out_step1[ 6][ 2] = out_step0[ 6][ 2] - 183.11083984;
out_step1[ 6][ 3] = out_step0[ 6][ 3] - 191.76126099;
out_step1[ 6][ 4] = out_step0[ 6][ 4] - 201.44458008;
out_step1[ 6][ 5] = out_step0[ 6][ 5] - 203.14981079;
out_step1[ 6][ 6] = out_step0[ 6][ 6] - 202.02496338;
out_step1[ 6][ 7] = out_step0[ 6][ 7] - 199.99877930;
out_step1[ 6][ 8] = out_step0[ 6][ 8] - 204.58952332;
out_step1[ 6][ 9] = out_step0[ 6][ 9] - 207.84226990;
out_step1[ 6][10] = out_step0[ 6][10] - 203.19671631;
out_step1[ 6][11] = out_step0[ 6][11] - 194.03106689;
out_step1[ 6][12] = out_step0[ 6][12] - 180.55236816;
out_step1[ 6][13] = out_step0[ 6][13] - 180.31851196;
out_step1[ 6][14] = out_step0[ 6][14] - 158.95736694;
out_step1[ 6][15] = out_step0[ 6][15] - 152.31120300;
out_step1[ 7][ 0] = out_step0[ 7][ 0] - 151.20036316;
out_step1[ 7][ 1] = out_step0[ 7][ 1] - 156.87820435;
out_step1[ 7][ 2] = out_step0[ 7][ 2] - 175.05419922;
out_step1[ 7][ 3] = out_step0[ 7][ 3] - 185.85809326;
out_step1[ 7][ 4] = out_step0[ 7][ 4] - 195.53654480;
out_step1[ 7][ 5] = out_step0[ 7][ 5] - 198.32034302;
out_step1[ 7][ 6] = out_step0[ 7][ 6] - 194.40438843;
out_step1[ 7][ 7] = out_step0[ 7][ 7] - 194.29963684;
out_step1[ 7][ 8] = out_step0[ 7][ 8] - 197.66748047;
out_step1[ 7][ 9] = out_step0[ 7][ 9] - 205.31303406;
out_step1[ 7][10] = out_step0[ 7][10] - 201.16200256;
out_step1[ 7][11] = out_step0[ 7][11] - 191.23263550;
out_step1[ 7][12] = out_step0[ 7][12] - 180.23751831;
out_step1[ 7][13] = out_step0[ 7][13] - 182.07368469;
out_step1[ 7][14] = out_step0[ 7][14] - 157.94396973;
out_step1[ 7][15] = out_step0[ 7][15] - 149.84408569;
out_step1[ 8][ 0] = out_step0[ 8][ 0] - 150.27162170;
out_step1[ 8][ 1] = out_step0[ 8][ 1] - 154.07795715;
out_step1[ 8][ 2] = out_step0[ 8][ 2] - 171.63824463;
out_step1[ 8][ 3] = out_step0[ 8][ 3] - 183.99024963;
out_step1[ 8][ 4] = out_step0[ 8][ 4] - 188.77954102;
out_step1[ 8][ 5] = out_step0[ 8][ 5] - 188.97929382;
out_step1[ 8][ 6] = out_step0[ 8][ 6] - 189.07368469;
out_step1[ 8][ 7] = out_step0[ 8][ 7] - 188.51887512;
out_step1[ 8][ 8] = out_step0[ 8][ 8] - 194.83312988;
out_step1[ 8][ 9] = out_step0[ 8][ 9] - 200.86967468;
out_step1[ 8][10] = out_step0[ 8][10] - 197.93179321;
out_step1[ 8][11] = out_step0[ 8][11] - 187.15530396;
out_step1[ 8][12] = out_step0[ 8][12] - 176.45614624;
out_step1[ 8][13] = out_step0[ 8][13] - 180.90560913;
out_step1[ 8][14] = out_step0[ 8][14] - 156.56517029;
out_step1[ 8][15] = out_step0[ 8][15] - 148.48112488;
out_step1[ 9][ 0] = out_step0[ 9][ 0] - 149.96893311;
out_step1[ 9][ 1] = out_step0[ 9][ 1] - 152.91961670;
out_step1[ 9][ 2] = out_step0[ 9][ 2] - 170.08221436;
out_step1[ 9][ 3] = out_step0[ 9][ 3] - 182.26553345;
out_step1[ 9][ 4] = out_step0[ 9][ 4] - 184.79232788;
out_step1[ 9][ 5] = out_step0[ 9][ 5] - 184.67051697;
out_step1[ 9][ 6] = out_step0[ 9][ 6] - 181.62910461;
out_step1[ 9][ 7] = out_step0[ 9][ 7] - 183.55297852;
out_step1[ 9][ 8] = out_step0[ 9][ 8] - 190.92935181;
out_step1[ 9][ 9] = out_step0[ 9][ 9] - 196.57125854;
out_step1[ 9][10] = out_step0[ 9][10] - 193.38429260;
out_step1[ 9][11] = out_step0[ 9][11] - 183.86967468;
out_step1[ 9][12] = out_step0[ 9][12] - 175.76856995;
out_step1[ 9][13] = out_step0[ 9][13] - 178.33496094;
out_step1[ 9][14] = out_step0[ 9][14] - 153.75639343;
out_step1[ 9][15] = out_step0[ 9][15] - 147.59744263;
out_step1[10][ 0] = out_step0[10][ 0] - 148.65164185;
out_step1[10][ 1] = out_step0[10][ 1] - 150.75822449;
out_step1[10][ 2] = out_step0[10][ 2] - 165.13276672;
out_step1[10][ 3] = out_step0[10][ 3] - 175.04811096;
out_step1[10][ 4] = out_step0[10][ 4] - 178.75700378;
out_step1[10][ 5] = out_step0[10][ 5] - 177.87515259;
out_step1[10][ 6] = out_step0[10][ 6] - 174.30633545;
out_step1[10][ 7] = out_step0[10][ 7] - 178.71864319;
out_step1[10][ 8] = out_step0[10][ 8] - 184.32337952;
out_step1[10][ 9] = out_step0[10][ 9] - 189.29780579;
out_step1[10][10] = out_step0[10][10] - 187.42692566;
out_step1[10][11] = out_step0[10][11] - 181.16809082;
out_step1[10][12] = out_step0[10][12] - 174.48416138;
out_step1[10][13] = out_step0[10][13] - 167.65408325;
out_step1[10][14] = out_step0[10][14] - 152.61328125;
out_step1[10][15] = out_step0[10][15] - 148.15773010;
out_step1[11][ 0] = out_step0[11][ 0] - 144.77465820;
out_step1[11][ 1] = out_step0[11][ 1] - 145.12910461;
out_step1[11][ 2] = out_step0[11][ 2] - 152.10292053;
out_step1[11][ 3] = out_step0[11][ 3] - 164.40255737;
out_step1[11][ 4] = out_step0[11][ 4] - 167.45249939;
out_step1[11][ 5] = out_step0[11][ 5] - 165.40438843;
out_step1[11][ 6] = out_step0[11][ 6] - 163.42996216;
out_step1[11][ 7] = out_step0[11][ 7] - 167.80937195;
out_step1[11][ 8] = out_step0[11][ 8] - 172.86236572;
out_step1[11][ 9] = out_step0[11][ 9] - 177.35078430;
out_step1[11][10] = out_step0[11][10] - 175.98963928;
out_step1[11][11] = out_step0[11][11] - 171.32521057;
out_step1[11][12] = out_step0[11][12] - 169.08587646;
out_step1[11][13] = out_step0[11][13] - 157.27770996;
out_step1[11][14] = out_step0[11][14] - 149.39038086;
out_step1[11][15] = out_step0[11][15] - 147.66078186;
out_step1[12][ 0] = out_step0[12][ 0] - 140.40681458;
out_step1[12][ 1] = out_step0[12][ 1] - 138.69244385;
out_step1[12][ 2] = out_step0[12][ 2] - 138.96041870;
out_step1[12][ 3] = out_step0[12][ 3] - 145.63824463;
out_step1[12][ 4] = out_step0[12][ 4] - 153.43057251;
out_step1[12][ 5] = out_step0[12][ 5] - 151.33921814;
out_step1[12][ 6] = out_step0[12][ 6] - 151.18635559;
out_step1[12][ 7] = out_step0[12][ 7] - 153.25456238;
out_step1[12][ 8] = out_step0[12][ 8] - 157.40499878;
out_step1[12][ 9] = out_step0[12][ 9] - 161.22229004;
out_step1[12][10] = out_step0[12][10] - 159.11692810;
out_step1[12][11] = out_step0[12][11] - 153.89950562;
out_step1[12][12] = out_step0[12][12] - 148.83190918;
out_step1[12][13] = out_step0[12][13] - 145.01095581;
out_step1[12][14] = out_step0[12][14] - 142.39221191;
out_step1[12][15] = out_step0[12][15] - 143.51583862;
out_step1[13][ 0] = out_step0[13][ 0] - 133.83374023;
out_step1[13][ 1] = out_step0[13][ 1] - 131.38368225;
out_step1[13][ 2] = out_step0[13][ 2] - 128.46833801;
out_step1[13][ 3] = out_step0[13][ 3] - 127.44031525;
out_step1[13][ 4] = out_step0[13][ 4] - 128.50061035;
out_step1[13][ 5] = out_step0[13][ 5] - 132.14372253;
out_step1[13][ 6] = out_step0[13][ 6] - 134.40072632;
out_step1[13][ 7] = out_step0[13][ 7] - 137.93727112;
out_step1[13][ 8] = out_step0[13][ 8] - 142.10108948;
out_step1[13][ 9] = out_step0[13][ 9] - 141.17782593;
out_step1[13][10] = out_step0[13][10] - 135.40803528;
out_step1[13][11] = out_step0[13][11] - 129.36540222;
out_step1[13][12] = out_step0[13][12] - 131.31303406;
out_step1[13][13] = out_step0[13][13] - 135.21498108;
out_step1[13][14] = out_step0[13][14] - 137.09744263;
out_step1[13][15] = out_step0[13][15] - 139.33738708;
out_step1[14][ 0] = out_step0[14][ 0] - 129.04566956;
out_step1[14][ 1] = out_step0[14][ 1] - 125.92691803;
out_step1[14][ 2] = out_step0[14][ 2] - 121.63397980;
out_step1[14][ 3] = out_step0[14][ 3] - 119.65712738;
out_step1[14][ 4] = out_step0[14][ 4] - 115.89463806;
out_step1[14][ 5] = out_step0[14][ 5] - 114.25091553;
out_step1[14][ 6] = out_step0[14][ 6] - 114.55603027;
out_step1[14][ 7] = out_step0[14][ 7] - 115.85505676;
out_step1[14][ 8] = out_step0[14][ 8] - 116.48964691;
out_step1[14][ 9] = out_step0[14][ 9] - 115.63824463;
out_step1[14][10] = out_step0[14][10] - 114.69427490;
out_step1[14][11] = out_step0[14][11] - 116.59683228;
out_step1[14][12] = out_step0[14][12] - 122.39829254;
out_step1[14][13] = out_step0[14][13] - 129.22959900;
out_step1[14][14] = out_step0[14][14] - 133.68513489;
out_step1[14][15] = out_step0[14][15] - 135.21192932;
out_step1[15][ 0] = out_step0[15][ 0] - 125.96467590;
out_step1[15][ 1] = out_step0[15][ 1] - 122.31973267;
out_step1[15][ 2] = out_step0[15][ 2] - 118.66503906;
out_step1[15][ 3] = out_step0[15][ 3] - 115.37637329;
out_step1[15][ 4] = out_step0[15][ 4] - 111.38854980;
out_step1[15][ 5] = out_step0[15][ 5] - 109.04567719;
out_step1[15][ 6] = out_step0[15][ 6] - 107.58891296;
out_step1[15][ 7] = out_step0[15][ 7] - 107.41474152;
out_step1[15][ 8] = out_step0[15][ 8] - 107.91595459;
out_step1[15][ 9] = out_step0[15][ 9] - 108.03044891;
out_step1[15][10] = out_step0[15][10] - 109.96955109;
out_step1[15][11] = out_step0[15][11] - 113.00547791;
out_step1[15][12] = out_step0[15][12] - 120.03289032;
out_step1[15][13] = out_step0[15][13] - 126.56089783;
out_step1[15][14] = out_step0[15][14] - 130.56211853;
out_step1[15][15] = out_step0[15][15] - 132.43544006;

// determine output for this convolution step
out_step2[0][0] = 
   - 0.10991814 * out_step1[ 0][ 0] - 0.07047570 * out_step1[ 0][ 1] + 0.32846937 * out_step1[ 0][ 2]
   + 0.05873375 * out_step1[ 1][ 0] + 0.08658601 * out_step1[ 1][ 1] + 0.03624630 * out_step1[ 1][ 2]
   + 0.01479519 * out_step1[ 2][ 0] + 0.36403248 * out_step1[ 2][ 1] - 0.13253643 * out_step1[ 2][ 2]
   + 0.00906407;

out_step2[0][1] = 
   - 0.10991814 * out_step1[ 0][ 1] - 0.07047570 * out_step1[ 0][ 2] + 0.32846937 * out_step1[ 0][ 3]
   + 0.05873375 * out_step1[ 1][ 1] + 0.08658601 * out_step1[ 1][ 2] + 0.03624630 * out_step1[ 1][ 3]
   + 0.01479519 * out_step1[ 2][ 1] + 0.36403248 * out_step1[ 2][ 2] - 0.13253643 * out_step1[ 2][ 3]
   + 0.00906407;

out_step2[0][2] = 
   - 0.10991814 * out_step1[ 0][ 2] - 0.07047570 * out_step1[ 0][ 3] + 0.32846937 * out_step1[ 0][ 4]
   + 0.05873375 * out_step1[ 1][ 2] + 0.08658601 * out_step1[ 1][ 3] + 0.03624630 * out_step1[ 1][ 4]
   + 0.01479519 * out_step1[ 2][ 2] + 0.36403248 * out_step1[ 2][ 3] - 0.13253643 * out_step1[ 2][ 4]
   + 0.00906407;

out_step2[0][3] = 
   - 0.10991814 * out_step1[ 0][ 3] - 0.07047570 * out_step1[ 0][ 4] + 0.32846937 * out_step1[ 0][ 5]
   + 0.05873375 * out_step1[ 1][ 3] + 0.08658601 * out_step1[ 1][ 4] + 0.03624630 * out_step1[ 1][ 5]
   + 0.01479519 * out_step1[ 2][ 3] + 0.36403248 * out_step1[ 2][ 4] - 0.13253643 * out_step1[ 2][ 5]
   + 0.00906407;

out_step2[0][4] = 
   - 0.10991814 * out_step1[ 0][ 4] - 0.07047570 * out_step1[ 0][ 5] + 0.32846937 * out_step1[ 0][ 6]
   + 0.05873375 * out_step1[ 1][ 4] + 0.08658601 * out_step1[ 1][ 5] + 0.03624630 * out_step1[ 1][ 6]
   + 0.01479519 * out_step1[ 2][ 4] + 0.36403248 * out_step1[ 2][ 5] - 0.13253643 * out_step1[ 2][ 6]
   + 0.00906407;

out_step2[0][5] = 
   - 0.10991814 * out_step1[ 0][ 5] - 0.07047570 * out_step1[ 0][ 6] + 0.32846937 * out_step1[ 0][ 7]
   + 0.05873375 * out_step1[ 1][ 5] + 0.08658601 * out_step1[ 1][ 6] + 0.03624630 * out_step1[ 1][ 7]
   + 0.01479519 * out_step1[ 2][ 5] + 0.36403248 * out_step1[ 2][ 6] - 0.13253643 * out_step1[ 2][ 7]
   + 0.00906407;

out_step2[0][6] = 
   - 0.10991814 * out_step1[ 0][ 6] - 0.07047570 * out_step1[ 0][ 7] + 0.32846937 * out_step1[ 0][ 8]
   + 0.05873375 * out_step1[ 1][ 6] + 0.08658601 * out_step1[ 1][ 7] + 0.03624630 * out_step1[ 1][ 8]
   + 0.01479519 * out_step1[ 2][ 6] + 0.36403248 * out_step1[ 2][ 7] - 0.13253643 * out_step1[ 2][ 8]
   + 0.00906407;

out_step2[0][7] = 
   - 0.10991814 * out_step1[ 0][ 7] - 0.07047570 * out_step1[ 0][ 8] + 0.32846937 * out_step1[ 0][ 9]
   + 0.05873375 * out_step1[ 1][ 7] + 0.08658601 * out_step1[ 1][ 8] + 0.03624630 * out_step1[ 1][ 9]
   + 0.01479519 * out_step1[ 2][ 7] + 0.36403248 * out_step1[ 2][ 8] - 0.13253643 * out_step1[ 2][ 9]
   + 0.00906407;

out_step2[0][8] = 
   - 0.10991814 * out_step1[ 0][ 8] - 0.07047570 * out_step1[ 0][ 9] + 0.32846937 * out_step1[ 0][10]
   + 0.05873375 * out_step1[ 1][ 8] + 0.08658601 * out_step1[ 1][ 9] + 0.03624630 * out_step1[ 1][10]
   + 0.01479519 * out_step1[ 2][ 8] + 0.36403248 * out_step1[ 2][ 9] - 0.13253643 * out_step1[ 2][10]
   + 0.00906407;

out_step2[0][9] = 
   - 0.10991814 * out_step1[ 0][ 9] - 0.07047570 * out_step1[ 0][10] + 0.32846937 * out_step1[ 0][11]
   + 0.05873375 * out_step1[ 1][ 9] + 0.08658601 * out_step1[ 1][10] + 0.03624630 * out_step1[ 1][11]
   + 0.01479519 * out_step1[ 2][ 9] + 0.36403248 * out_step1[ 2][10] - 0.13253643 * out_step1[ 2][11]
   + 0.00906407;

out_step2[0][10] = 
   - 0.10991814 * out_step1[ 0][10] - 0.07047570 * out_step1[ 0][11] + 0.32846937 * out_step1[ 0][12]
   + 0.05873375 * out_step1[ 1][10] + 0.08658601 * out_step1[ 1][11] + 0.03624630 * out_step1[ 1][12]
   + 0.01479519 * out_step1[ 2][10] + 0.36403248 * out_step1[ 2][11] - 0.13253643 * out_step1[ 2][12]
   + 0.00906407;

out_step2[0][11] = 
   - 0.10991814 * out_step1[ 0][11] - 0.07047570 * out_step1[ 0][12] + 0.32846937 * out_step1[ 0][13]
   + 0.05873375 * out_step1[ 1][11] + 0.08658601 * out_step1[ 1][12] + 0.03624630 * out_step1[ 1][13]
   + 0.01479519 * out_step1[ 2][11] + 0.36403248 * out_step1[ 2][12] - 0.13253643 * out_step1[ 2][13]
   + 0.00906407;

out_step2[0][12] = 
   - 0.10991814 * out_step1[ 0][12] - 0.07047570 * out_step1[ 0][13] + 0.32846937 * out_step1[ 0][14]
   + 0.05873375 * out_step1[ 1][12] + 0.08658601 * out_step1[ 1][13] + 0.03624630 * out_step1[ 1][14]
   + 0.01479519 * out_step1[ 2][12] + 0.36403248 * out_step1[ 2][13] - 0.13253643 * out_step1[ 2][14]
   + 0.00906407;

out_step2[0][13] = 
   - 0.10991814 * out_step1[ 0][13] - 0.07047570 * out_step1[ 0][14] + 0.32846937 * out_step1[ 0][15]
   + 0.05873375 * out_step1[ 1][13] + 0.08658601 * out_step1[ 1][14] + 0.03624630 * out_step1[ 1][15]
   + 0.01479519 * out_step1[ 2][13] + 0.36403248 * out_step1[ 2][14] - 0.13253643 * out_step1[ 2][15]
   + 0.00906407;

out_step2[1][0] = 
   - 0.10991814 * out_step1[ 1][ 0] - 0.07047570 * out_step1[ 1][ 1] + 0.32846937 * out_step1[ 1][ 2]
   + 0.05873375 * out_step1[ 2][ 0] + 0.08658601 * out_step1[ 2][ 1] + 0.03624630 * out_step1[ 2][ 2]
   + 0.01479519 * out_step1[ 3][ 0] + 0.36403248 * out_step1[ 3][ 1] - 0.13253643 * out_step1[ 3][ 2]
   + 0.00906407;

out_step2[1][1] = 
   - 0.10991814 * out_step1[ 1][ 1] - 0.07047570 * out_step1[ 1][ 2] + 0.32846937 * out_step1[ 1][ 3]
   + 0.05873375 * out_step1[ 2][ 1] + 0.08658601 * out_step1[ 2][ 2] + 0.03624630 * out_step1[ 2][ 3]
   + 0.01479519 * out_step1[ 3][ 1] + 0.36403248 * out_step1[ 3][ 2] - 0.13253643 * out_step1[ 3][ 3]
   + 0.00906407;

out_step2[1][2] = 
   - 0.10991814 * out_step1[ 1][ 2] - 0.07047570 * out_step1[ 1][ 3] + 0.32846937 * out_step1[ 1][ 4]
   + 0.05873375 * out_step1[ 2][ 2] + 0.08658601 * out_step1[ 2][ 3] + 0.03624630 * out_step1[ 2][ 4]
   + 0.01479519 * out_step1[ 3][ 2] + 0.36403248 * out_step1[ 3][ 3] - 0.13253643 * out_step1[ 3][ 4]
   + 0.00906407;

out_step2[1][3] = 
   - 0.10991814 * out_step1[ 1][ 3] - 0.07047570 * out_step1[ 1][ 4] + 0.32846937 * out_step1[ 1][ 5]
   + 0.05873375 * out_step1[ 2][ 3] + 0.08658601 * out_step1[ 2][ 4] + 0.03624630 * out_step1[ 2][ 5]
   + 0.01479519 * out_step1[ 3][ 3] + 0.36403248 * out_step1[ 3][ 4] - 0.13253643 * out_step1[ 3][ 5]
   + 0.00906407;

out_step2[1][4] = 
   - 0.10991814 * out_step1[ 1][ 4] - 0.07047570 * out_step1[ 1][ 5] + 0.32846937 * out_step1[ 1][ 6]
   + 0.05873375 * out_step1[ 2][ 4] + 0.08658601 * out_step1[ 2][ 5] + 0.03624630 * out_step1[ 2][ 6]
   + 0.01479519 * out_step1[ 3][ 4] + 0.36403248 * out_step1[ 3][ 5] - 0.13253643 * out_step1[ 3][ 6]
   + 0.00906407;

out_step2[1][5] = 
   - 0.10991814 * out_step1[ 1][ 5] - 0.07047570 * out_step1[ 1][ 6] + 0.32846937 * out_step1[ 1][ 7]
   + 0.05873375 * out_step1[ 2][ 5] + 0.08658601 * out_step1[ 2][ 6] + 0.03624630 * out_step1[ 2][ 7]
   + 0.01479519 * out_step1[ 3][ 5] + 0.36403248 * out_step1[ 3][ 6] - 0.13253643 * out_step1[ 3][ 7]
   + 0.00906407;

out_step2[1][6] = 
   - 0.10991814 * out_step1[ 1][ 6] - 0.07047570 * out_step1[ 1][ 7] + 0.32846937 * out_step1[ 1][ 8]
   + 0.05873375 * out_step1[ 2][ 6] + 0.08658601 * out_step1[ 2][ 7] + 0.03624630 * out_step1[ 2][ 8]
   + 0.01479519 * out_step1[ 3][ 6] + 0.36403248 * out_step1[ 3][ 7] - 0.13253643 * out_step1[ 3][ 8]
   + 0.00906407;

out_step2[1][7] = 
   - 0.10991814 * out_step1[ 1][ 7] - 0.07047570 * out_step1[ 1][ 8] + 0.32846937 * out_step1[ 1][ 9]
   + 0.05873375 * out_step1[ 2][ 7] + 0.08658601 * out_step1[ 2][ 8] + 0.03624630 * out_step1[ 2][ 9]
   + 0.01479519 * out_step1[ 3][ 7] + 0.36403248 * out_step1[ 3][ 8] - 0.13253643 * out_step1[ 3][ 9]
   + 0.00906407;

out_step2[1][8] = 
   - 0.10991814 * out_step1[ 1][ 8] - 0.07047570 * out_step1[ 1][ 9] + 0.32846937 * out_step1[ 1][10]
   + 0.05873375 * out_step1[ 2][ 8] + 0.08658601 * out_step1[ 2][ 9] + 0.03624630 * out_step1[ 2][10]
   + 0.01479519 * out_step1[ 3][ 8] + 0.36403248 * out_step1[ 3][ 9] - 0.13253643 * out_step1[ 3][10]
   + 0.00906407;

out_step2[1][9] = 
   - 0.10991814 * out_step1[ 1][ 9] - 0.07047570 * out_step1[ 1][10] + 0.32846937 * out_step1[ 1][11]
   + 0.05873375 * out_step1[ 2][ 9] + 0.08658601 * out_step1[ 2][10] + 0.03624630 * out_step1[ 2][11]
   + 0.01479519 * out_step1[ 3][ 9] + 0.36403248 * out_step1[ 3][10] - 0.13253643 * out_step1[ 3][11]
   + 0.00906407;

out_step2[1][10] = 
   - 0.10991814 * out_step1[ 1][10] - 0.07047570 * out_step1[ 1][11] + 0.32846937 * out_step1[ 1][12]
   + 0.05873375 * out_step1[ 2][10] + 0.08658601 * out_step1[ 2][11] + 0.03624630 * out_step1[ 2][12]
   + 0.01479519 * out_step1[ 3][10] + 0.36403248 * out_step1[ 3][11] - 0.13253643 * out_step1[ 3][12]
   + 0.00906407;

out_step2[1][11] = 
   - 0.10991814 * out_step1[ 1][11] - 0.07047570 * out_step1[ 1][12] + 0.32846937 * out_step1[ 1][13]
   + 0.05873375 * out_step1[ 2][11] + 0.08658601 * out_step1[ 2][12] + 0.03624630 * out_step1[ 2][13]
   + 0.01479519 * out_step1[ 3][11] + 0.36403248 * out_step1[ 3][12] - 0.13253643 * out_step1[ 3][13]
   + 0.00906407;

out_step2[1][12] = 
   - 0.10991814 * out_step1[ 1][12] - 0.07047570 * out_step1[ 1][13] + 0.32846937 * out_step1[ 1][14]
   + 0.05873375 * out_step1[ 2][12] + 0.08658601 * out_step1[ 2][13] + 0.03624630 * out_step1[ 2][14]
   + 0.01479519 * out_step1[ 3][12] + 0.36403248 * out_step1[ 3][13] - 0.13253643 * out_step1[ 3][14]
   + 0.00906407;

out_step2[1][13] = 
   - 0.10991814 * out_step1[ 1][13] - 0.07047570 * out_step1[ 1][14] + 0.32846937 * out_step1[ 1][15]
   + 0.05873375 * out_step1[ 2][13] + 0.08658601 * out_step1[ 2][14] + 0.03624630 * out_step1[ 2][15]
   + 0.01479519 * out_step1[ 3][13] + 0.36403248 * out_step1[ 3][14] - 0.13253643 * out_step1[ 3][15]
   + 0.00906407;

out_step2[2][0] = 
   - 0.10991814 * out_step1[ 2][ 0] - 0.07047570 * out_step1[ 2][ 1] + 0.32846937 * out_step1[ 2][ 2]
   + 0.05873375 * out_step1[ 3][ 0] + 0.08658601 * out_step1[ 3][ 1] + 0.03624630 * out_step1[ 3][ 2]
   + 0.01479519 * out_step1[ 4][ 0] + 0.36403248 * out_step1[ 4][ 1] - 0.13253643 * out_step1[ 4][ 2]
   + 0.00906407;

out_step2[2][1] = 
   - 0.10991814 * out_step1[ 2][ 1] - 0.07047570 * out_step1[ 2][ 2] + 0.32846937 * out_step1[ 2][ 3]
   + 0.05873375 * out_step1[ 3][ 1] + 0.08658601 * out_step1[ 3][ 2] + 0.03624630 * out_step1[ 3][ 3]
   + 0.01479519 * out_step1[ 4][ 1] + 0.36403248 * out_step1[ 4][ 2] - 0.13253643 * out_step1[ 4][ 3]
   + 0.00906407;

out_step2[2][2] = 
   - 0.10991814 * out_step1[ 2][ 2] - 0.07047570 * out_step1[ 2][ 3] + 0.32846937 * out_step1[ 2][ 4]
   + 0.05873375 * out_step1[ 3][ 2] + 0.08658601 * out_step1[ 3][ 3] + 0.03624630 * out_step1[ 3][ 4]
   + 0.01479519 * out_step1[ 4][ 2] + 0.36403248 * out_step1[ 4][ 3] - 0.13253643 * out_step1[ 4][ 4]
   + 0.00906407;

out_step2[2][3] = 
   - 0.10991814 * out_step1[ 2][ 3] - 0.07047570 * out_step1[ 2][ 4] + 0.32846937 * out_step1[ 2][ 5]
   + 0.05873375 * out_step1[ 3][ 3] + 0.08658601 * out_step1[ 3][ 4] + 0.03624630 * out_step1[ 3][ 5]
   + 0.01479519 * out_step1[ 4][ 3] + 0.36403248 * out_step1[ 4][ 4] - 0.13253643 * out_step1[ 4][ 5]
   + 0.00906407;

out_step2[2][4] = 
   - 0.10991814 * out_step1[ 2][ 4] - 0.07047570 * out_step1[ 2][ 5] + 0.32846937 * out_step1[ 2][ 6]
   + 0.05873375 * out_step1[ 3][ 4] + 0.08658601 * out_step1[ 3][ 5] + 0.03624630 * out_step1[ 3][ 6]
   + 0.01479519 * out_step1[ 4][ 4] + 0.36403248 * out_step1[ 4][ 5] - 0.13253643 * out_step1[ 4][ 6]
   + 0.00906407;

out_step2[2][5] = 
   - 0.10991814 * out_step1[ 2][ 5] - 0.07047570 * out_step1[ 2][ 6] + 0.32846937 * out_step1[ 2][ 7]
   + 0.05873375 * out_step1[ 3][ 5] + 0.08658601 * out_step1[ 3][ 6] + 0.03624630 * out_step1[ 3][ 7]
   + 0.01479519 * out_step1[ 4][ 5] + 0.36403248 * out_step1[ 4][ 6] - 0.13253643 * out_step1[ 4][ 7]
   + 0.00906407;

out_step2[2][6] = 
   - 0.10991814 * out_step1[ 2][ 6] - 0.07047570 * out_step1[ 2][ 7] + 0.32846937 * out_step1[ 2][ 8]
   + 0.05873375 * out_step1[ 3][ 6] + 0.08658601 * out_step1[ 3][ 7] + 0.03624630 * out_step1[ 3][ 8]
   + 0.01479519 * out_step1[ 4][ 6] + 0.36403248 * out_step1[ 4][ 7] - 0.13253643 * out_step1[ 4][ 8]
   + 0.00906407;

out_step2[2][7] = 
   - 0.10991814 * out_step1[ 2][ 7] - 0.07047570 * out_step1[ 2][ 8] + 0.32846937 * out_step1[ 2][ 9]
   + 0.05873375 * out_step1[ 3][ 7] + 0.08658601 * out_step1[ 3][ 8] + 0.03624630 * out_step1[ 3][ 9]
   + 0.01479519 * out_step1[ 4][ 7] + 0.36403248 * out_step1[ 4][ 8] - 0.13253643 * out_step1[ 4][ 9]
   + 0.00906407;

out_step2[2][8] = 
   - 0.10991814 * out_step1[ 2][ 8] - 0.07047570 * out_step1[ 2][ 9] + 0.32846937 * out_step1[ 2][10]
   + 0.05873375 * out_step1[ 3][ 8] + 0.08658601 * out_step1[ 3][ 9] + 0.03624630 * out_step1[ 3][10]
   + 0.01479519 * out_step1[ 4][ 8] + 0.36403248 * out_step1[ 4][ 9] - 0.13253643 * out_step1[ 4][10]
   + 0.00906407;

out_step2[2][9] = 
   - 0.10991814 * out_step1[ 2][ 9] - 0.07047570 * out_step1[ 2][10] + 0.32846937 * out_step1[ 2][11]
   + 0.05873375 * out_step1[ 3][ 9] + 0.08658601 * out_step1[ 3][10] + 0.03624630 * out_step1[ 3][11]
   + 0.01479519 * out_step1[ 4][ 9] + 0.36403248 * out_step1[ 4][10] - 0.13253643 * out_step1[ 4][11]
   + 0.00906407;

out_step2[2][10] = 
   - 0.10991814 * out_step1[ 2][10] - 0.07047570 * out_step1[ 2][11] + 0.32846937 * out_step1[ 2][12]
   + 0.05873375 * out_step1[ 3][10] + 0.08658601 * out_step1[ 3][11] + 0.03624630 * out_step1[ 3][12]
   + 0.01479519 * out_step1[ 4][10] + 0.36403248 * out_step1[ 4][11] - 0.13253643 * out_step1[ 4][12]
   + 0.00906407;

out_step2[2][11] = 
   - 0.10991814 * out_step1[ 2][11] - 0.07047570 * out_step1[ 2][12] + 0.32846937 * out_step1[ 2][13]
   + 0.05873375 * out_step1[ 3][11] + 0.08658601 * out_step1[ 3][12] + 0.03624630 * out_step1[ 3][13]
   + 0.01479519 * out_step1[ 4][11] + 0.36403248 * out_step1[ 4][12] - 0.13253643 * out_step1[ 4][13]
   + 0.00906407;

out_step2[2][12] = 
   - 0.10991814 * out_step1[ 2][12] - 0.07047570 * out_step1[ 2][13] + 0.32846937 * out_step1[ 2][14]
   + 0.05873375 * out_step1[ 3][12] + 0.08658601 * out_step1[ 3][13] + 0.03624630 * out_step1[ 3][14]
   + 0.01479519 * out_step1[ 4][12] + 0.36403248 * out_step1[ 4][13] - 0.13253643 * out_step1[ 4][14]
   + 0.00906407;

out_step2[2][13] = 
   - 0.10991814 * out_step1[ 2][13] - 0.07047570 * out_step1[ 2][14] + 0.32846937 * out_step1[ 2][15]
   + 0.05873375 * out_step1[ 3][13] + 0.08658601 * out_step1[ 3][14] + 0.03624630 * out_step1[ 3][15]
   + 0.01479519 * out_step1[ 4][13] + 0.36403248 * out_step1[ 4][14] - 0.13253643 * out_step1[ 4][15]
   + 0.00906407;

out_step2[3][0] = 
   - 0.10991814 * out_step1[ 3][ 0] - 0.07047570 * out_step1[ 3][ 1] + 0.32846937 * out_step1[ 3][ 2]
   + 0.05873375 * out_step1[ 4][ 0] + 0.08658601 * out_step1[ 4][ 1] + 0.03624630 * out_step1[ 4][ 2]
   + 0.01479519 * out_step1[ 5][ 0] + 0.36403248 * out_step1[ 5][ 1] - 0.13253643 * out_step1[ 5][ 2]
   + 0.00906407;

out_step2[3][1] = 
   - 0.10991814 * out_step1[ 3][ 1] - 0.07047570 * out_step1[ 3][ 2] + 0.32846937 * out_step1[ 3][ 3]
   + 0.05873375 * out_step1[ 4][ 1] + 0.08658601 * out_step1[ 4][ 2] + 0.03624630 * out_step1[ 4][ 3]
   + 0.01479519 * out_step1[ 5][ 1] + 0.36403248 * out_step1[ 5][ 2] - 0.13253643 * out_step1[ 5][ 3]
   + 0.00906407;

out_step2[3][2] = 
   - 0.10991814 * out_step1[ 3][ 2] - 0.07047570 * out_step1[ 3][ 3] + 0.32846937 * out_step1[ 3][ 4]
   + 0.05873375 * out_step1[ 4][ 2] + 0.08658601 * out_step1[ 4][ 3] + 0.03624630 * out_step1[ 4][ 4]
   + 0.01479519 * out_step1[ 5][ 2] + 0.36403248 * out_step1[ 5][ 3] - 0.13253643 * out_step1[ 5][ 4]
   + 0.00906407;

out_step2[3][3] = 
   - 0.10991814 * out_step1[ 3][ 3] - 0.07047570 * out_step1[ 3][ 4] + 0.32846937 * out_step1[ 3][ 5]
   + 0.05873375 * out_step1[ 4][ 3] + 0.08658601 * out_step1[ 4][ 4] + 0.03624630 * out_step1[ 4][ 5]
   + 0.01479519 * out_step1[ 5][ 3] + 0.36403248 * out_step1[ 5][ 4] - 0.13253643 * out_step1[ 5][ 5]
   + 0.00906407;

out_step2[3][4] = 
   - 0.10991814 * out_step1[ 3][ 4] - 0.07047570 * out_step1[ 3][ 5] + 0.32846937 * out_step1[ 3][ 6]
   + 0.05873375 * out_step1[ 4][ 4] + 0.08658601 * out_step1[ 4][ 5] + 0.03624630 * out_step1[ 4][ 6]
   + 0.01479519 * out_step1[ 5][ 4] + 0.36403248 * out_step1[ 5][ 5] - 0.13253643 * out_step1[ 5][ 6]
   + 0.00906407;

out_step2[3][5] = 
   - 0.10991814 * out_step1[ 3][ 5] - 0.07047570 * out_step1[ 3][ 6] + 0.32846937 * out_step1[ 3][ 7]
   + 0.05873375 * out_step1[ 4][ 5] + 0.08658601 * out_step1[ 4][ 6] + 0.03624630 * out_step1[ 4][ 7]
   + 0.01479519 * out_step1[ 5][ 5] + 0.36403248 * out_step1[ 5][ 6] - 0.13253643 * out_step1[ 5][ 7]
   + 0.00906407;

out_step2[3][6] = 
   - 0.10991814 * out_step1[ 3][ 6] - 0.07047570 * out_step1[ 3][ 7] + 0.32846937 * out_step1[ 3][ 8]
   + 0.05873375 * out_step1[ 4][ 6] + 0.08658601 * out_step1[ 4][ 7] + 0.03624630 * out_step1[ 4][ 8]
   + 0.01479519 * out_step1[ 5][ 6] + 0.36403248 * out_step1[ 5][ 7] - 0.13253643 * out_step1[ 5][ 8]
   + 0.00906407;

out_step2[3][7] = 
   - 0.10991814 * out_step1[ 3][ 7] - 0.07047570 * out_step1[ 3][ 8] + 0.32846937 * out_step1[ 3][ 9]
   + 0.05873375 * out_step1[ 4][ 7] + 0.08658601 * out_step1[ 4][ 8] + 0.03624630 * out_step1[ 4][ 9]
   + 0.01479519 * out_step1[ 5][ 7] + 0.36403248 * out_step1[ 5][ 8] - 0.13253643 * out_step1[ 5][ 9]
   + 0.00906407;

out_step2[3][8] = 
   - 0.10991814 * out_step1[ 3][ 8] - 0.07047570 * out_step1[ 3][ 9] + 0.32846937 * out_step1[ 3][10]
   + 0.05873375 * out_step1[ 4][ 8] + 0.08658601 * out_step1[ 4][ 9] + 0.03624630 * out_step1[ 4][10]
   + 0.01479519 * out_step1[ 5][ 8] + 0.36403248 * out_step1[ 5][ 9] - 0.13253643 * out_step1[ 5][10]
   + 0.00906407;

out_step2[3][9] = 
   - 0.10991814 * out_step1[ 3][ 9] - 0.07047570 * out_step1[ 3][10] + 0.32846937 * out_step1[ 3][11]
   + 0.05873375 * out_step1[ 4][ 9] + 0.08658601 * out_step1[ 4][10] + 0.03624630 * out_step1[ 4][11]
   + 0.01479519 * out_step1[ 5][ 9] + 0.36403248 * out_step1[ 5][10] - 0.13253643 * out_step1[ 5][11]
   + 0.00906407;

out_step2[3][10] = 
   - 0.10991814 * out_step1[ 3][10] - 0.07047570 * out_step1[ 3][11] + 0.32846937 * out_step1[ 3][12]
   + 0.05873375 * out_step1[ 4][10] + 0.08658601 * out_step1[ 4][11] + 0.03624630 * out_step1[ 4][12]
   + 0.01479519 * out_step1[ 5][10] + 0.36403248 * out_step1[ 5][11] - 0.13253643 * out_step1[ 5][12]
   + 0.00906407;

out_step2[3][11] = 
   - 0.10991814 * out_step1[ 3][11] - 0.07047570 * out_step1[ 3][12] + 0.32846937 * out_step1[ 3][13]
   + 0.05873375 * out_step1[ 4][11] + 0.08658601 * out_step1[ 4][12] + 0.03624630 * out_step1[ 4][13]
   + 0.01479519 * out_step1[ 5][11] + 0.36403248 * out_step1[ 5][12] - 0.13253643 * out_step1[ 5][13]
   + 0.00906407;

out_step2[3][12] = 
   - 0.10991814 * out_step1[ 3][12] - 0.07047570 * out_step1[ 3][13] + 0.32846937 * out_step1[ 3][14]
   + 0.05873375 * out_step1[ 4][12] + 0.08658601 * out_step1[ 4][13] + 0.03624630 * out_step1[ 4][14]
   + 0.01479519 * out_step1[ 5][12] + 0.36403248 * out_step1[ 5][13] - 0.13253643 * out_step1[ 5][14]
   + 0.00906407;

out_step2[3][13] = 
   - 0.10991814 * out_step1[ 3][13] - 0.07047570 * out_step1[ 3][14] + 0.32846937 * out_step1[ 3][15]
   + 0.05873375 * out_step1[ 4][13] + 0.08658601 * out_step1[ 4][14] + 0.03624630 * out_step1[ 4][15]
   + 0.01479519 * out_step1[ 5][13] + 0.36403248 * out_step1[ 5][14] - 0.13253643 * out_step1[ 5][15]
   + 0.00906407;

out_step2[4][0] = 
   - 0.10991814 * out_step1[ 4][ 0] - 0.07047570 * out_step1[ 4][ 1] + 0.32846937 * out_step1[ 4][ 2]
   + 0.05873375 * out_step1[ 5][ 0] + 0.08658601 * out_step1[ 5][ 1] + 0.03624630 * out_step1[ 5][ 2]
   + 0.01479519 * out_step1[ 6][ 0] + 0.36403248 * out_step1[ 6][ 1] - 0.13253643 * out_step1[ 6][ 2]
   + 0.00906407;

out_step2[4][1] = 
   - 0.10991814 * out_step1[ 4][ 1] - 0.07047570 * out_step1[ 4][ 2] + 0.32846937 * out_step1[ 4][ 3]
   + 0.05873375 * out_step1[ 5][ 1] + 0.08658601 * out_step1[ 5][ 2] + 0.03624630 * out_step1[ 5][ 3]
   + 0.01479519 * out_step1[ 6][ 1] + 0.36403248 * out_step1[ 6][ 2] - 0.13253643 * out_step1[ 6][ 3]
   + 0.00906407;

out_step2[4][2] = 
   - 0.10991814 * out_step1[ 4][ 2] - 0.07047570 * out_step1[ 4][ 3] + 0.32846937 * out_step1[ 4][ 4]
   + 0.05873375 * out_step1[ 5][ 2] + 0.08658601 * out_step1[ 5][ 3] + 0.03624630 * out_step1[ 5][ 4]
   + 0.01479519 * out_step1[ 6][ 2] + 0.36403248 * out_step1[ 6][ 3] - 0.13253643 * out_step1[ 6][ 4]
   + 0.00906407;

out_step2[4][3] = 
   - 0.10991814 * out_step1[ 4][ 3] - 0.07047570 * out_step1[ 4][ 4] + 0.32846937 * out_step1[ 4][ 5]
   + 0.05873375 * out_step1[ 5][ 3] + 0.08658601 * out_step1[ 5][ 4] + 0.03624630 * out_step1[ 5][ 5]
   + 0.01479519 * out_step1[ 6][ 3] + 0.36403248 * out_step1[ 6][ 4] - 0.13253643 * out_step1[ 6][ 5]
   + 0.00906407;

out_step2[4][4] = 
   - 0.10991814 * out_step1[ 4][ 4] - 0.07047570 * out_step1[ 4][ 5] + 0.32846937 * out_step1[ 4][ 6]
   + 0.05873375 * out_step1[ 5][ 4] + 0.08658601 * out_step1[ 5][ 5] + 0.03624630 * out_step1[ 5][ 6]
   + 0.01479519 * out_step1[ 6][ 4] + 0.36403248 * out_step1[ 6][ 5] - 0.13253643 * out_step1[ 6][ 6]
   + 0.00906407;

out_step2[4][5] = 
   - 0.10991814 * out_step1[ 4][ 5] - 0.07047570 * out_step1[ 4][ 6] + 0.32846937 * out_step1[ 4][ 7]
   + 0.05873375 * out_step1[ 5][ 5] + 0.08658601 * out_step1[ 5][ 6] + 0.03624630 * out_step1[ 5][ 7]
   + 0.01479519 * out_step1[ 6][ 5] + 0.36403248 * out_step1[ 6][ 6] - 0.13253643 * out_step1[ 6][ 7]
   + 0.00906407;

out_step2[4][6] = 
   - 0.10991814 * out_step1[ 4][ 6] - 0.07047570 * out_step1[ 4][ 7] + 0.32846937 * out_step1[ 4][ 8]
   + 0.05873375 * out_step1[ 5][ 6] + 0.08658601 * out_step1[ 5][ 7] + 0.03624630 * out_step1[ 5][ 8]
   + 0.01479519 * out_step1[ 6][ 6] + 0.36403248 * out_step1[ 6][ 7] - 0.13253643 * out_step1[ 6][ 8]
   + 0.00906407;

out_step2[4][7] = 
   - 0.10991814 * out_step1[ 4][ 7] - 0.07047570 * out_step1[ 4][ 8] + 0.32846937 * out_step1[ 4][ 9]
   + 0.05873375 * out_step1[ 5][ 7] + 0.08658601 * out_step1[ 5][ 8] + 0.03624630 * out_step1[ 5][ 9]
   + 0.01479519 * out_step1[ 6][ 7] + 0.36403248 * out_step1[ 6][ 8] - 0.13253643 * out_step1[ 6][ 9]
   + 0.00906407;

out_step2[4][8] = 
   - 0.10991814 * out_step1[ 4][ 8] - 0.07047570 * out_step1[ 4][ 9] + 0.32846937 * out_step1[ 4][10]
   + 0.05873375 * out_step1[ 5][ 8] + 0.08658601 * out_step1[ 5][ 9] + 0.03624630 * out_step1[ 5][10]
   + 0.01479519 * out_step1[ 6][ 8] + 0.36403248 * out_step1[ 6][ 9] - 0.13253643 * out_step1[ 6][10]
   + 0.00906407;

out_step2[4][9] = 
   - 0.10991814 * out_step1[ 4][ 9] - 0.07047570 * out_step1[ 4][10] + 0.32846937 * out_step1[ 4][11]
   + 0.05873375 * out_step1[ 5][ 9] + 0.08658601 * out_step1[ 5][10] + 0.03624630 * out_step1[ 5][11]
   + 0.01479519 * out_step1[ 6][ 9] + 0.36403248 * out_step1[ 6][10] - 0.13253643 * out_step1[ 6][11]
   + 0.00906407;

out_step2[4][10] = 
   - 0.10991814 * out_step1[ 4][10] - 0.07047570 * out_step1[ 4][11] + 0.32846937 * out_step1[ 4][12]
   + 0.05873375 * out_step1[ 5][10] + 0.08658601 * out_step1[ 5][11] + 0.03624630 * out_step1[ 5][12]
   + 0.01479519 * out_step1[ 6][10] + 0.36403248 * out_step1[ 6][11] - 0.13253643 * out_step1[ 6][12]
   + 0.00906407;

out_step2[4][11] = 
   - 0.10991814 * out_step1[ 4][11] - 0.07047570 * out_step1[ 4][12] + 0.32846937 * out_step1[ 4][13]
   + 0.05873375 * out_step1[ 5][11] + 0.08658601 * out_step1[ 5][12] + 0.03624630 * out_step1[ 5][13]
   + 0.01479519 * out_step1[ 6][11] + 0.36403248 * out_step1[ 6][12] - 0.13253643 * out_step1[ 6][13]
   + 0.00906407;

out_step2[4][12] = 
   - 0.10991814 * out_step1[ 4][12] - 0.07047570 * out_step1[ 4][13] + 0.32846937 * out_step1[ 4][14]
   + 0.05873375 * out_step1[ 5][12] + 0.08658601 * out_step1[ 5][13] + 0.03624630 * out_step1[ 5][14]
   + 0.01479519 * out_step1[ 6][12] + 0.36403248 * out_step1[ 6][13] - 0.13253643 * out_step1[ 6][14]
   + 0.00906407;

out_step2[4][13] = 
   - 0.10991814 * out_step1[ 4][13] - 0.07047570 * out_step1[ 4][14] + 0.32846937 * out_step1[ 4][15]
   + 0.05873375 * out_step1[ 5][13] + 0.08658601 * out_step1[ 5][14] + 0.03624630 * out_step1[ 5][15]
   + 0.01479519 * out_step1[ 6][13] + 0.36403248 * out_step1[ 6][14] - 0.13253643 * out_step1[ 6][15]
   + 0.00906407;

out_step2[5][0] = 
   - 0.10991814 * out_step1[ 5][ 0] - 0.07047570 * out_step1[ 5][ 1] + 0.32846937 * out_step1[ 5][ 2]
   + 0.05873375 * out_step1[ 6][ 0] + 0.08658601 * out_step1[ 6][ 1] + 0.03624630 * out_step1[ 6][ 2]
   + 0.01479519 * out_step1[ 7][ 0] + 0.36403248 * out_step1[ 7][ 1] - 0.13253643 * out_step1[ 7][ 2]
   + 0.00906407;

out_step2[5][1] = 
   - 0.10991814 * out_step1[ 5][ 1] - 0.07047570 * out_step1[ 5][ 2] + 0.32846937 * out_step1[ 5][ 3]
   + 0.05873375 * out_step1[ 6][ 1] + 0.08658601 * out_step1[ 6][ 2] + 0.03624630 * out_step1[ 6][ 3]
   + 0.01479519 * out_step1[ 7][ 1] + 0.36403248 * out_step1[ 7][ 2] - 0.13253643 * out_step1[ 7][ 3]
   + 0.00906407;

out_step2[5][2] = 
   - 0.10991814 * out_step1[ 5][ 2] - 0.07047570 * out_step1[ 5][ 3] + 0.32846937 * out_step1[ 5][ 4]
   + 0.05873375 * out_step1[ 6][ 2] + 0.08658601 * out_step1[ 6][ 3] + 0.03624630 * out_step1[ 6][ 4]
   + 0.01479519 * out_step1[ 7][ 2] + 0.36403248 * out_step1[ 7][ 3] - 0.13253643 * out_step1[ 7][ 4]
   + 0.00906407;

out_step2[5][3] = 
   - 0.10991814 * out_step1[ 5][ 3] - 0.07047570 * out_step1[ 5][ 4] + 0.32846937 * out_step1[ 5][ 5]
   + 0.05873375 * out_step1[ 6][ 3] + 0.08658601 * out_step1[ 6][ 4] + 0.03624630 * out_step1[ 6][ 5]
   + 0.01479519 * out_step1[ 7][ 3] + 0.36403248 * out_step1[ 7][ 4] - 0.13253643 * out_step1[ 7][ 5]
   + 0.00906407;

out_step2[5][4] = 
   - 0.10991814 * out_step1[ 5][ 4] - 0.07047570 * out_step1[ 5][ 5] + 0.32846937 * out_step1[ 5][ 6]
   + 0.05873375 * out_step1[ 6][ 4] + 0.08658601 * out_step1[ 6][ 5] + 0.03624630 * out_step1[ 6][ 6]
   + 0.01479519 * out_step1[ 7][ 4] + 0.36403248 * out_step1[ 7][ 5] - 0.13253643 * out_step1[ 7][ 6]
   + 0.00906407;

out_step2[5][5] = 
   - 0.10991814 * out_step1[ 5][ 5] - 0.07047570 * out_step1[ 5][ 6] + 0.32846937 * out_step1[ 5][ 7]
   + 0.05873375 * out_step1[ 6][ 5] + 0.08658601 * out_step1[ 6][ 6] + 0.03624630 * out_step1[ 6][ 7]
   + 0.01479519 * out_step1[ 7][ 5] + 0.36403248 * out_step1[ 7][ 6] - 0.13253643 * out_step1[ 7][ 7]
   + 0.00906407;

out_step2[5][6] = 
   - 0.10991814 * out_step1[ 5][ 6] - 0.07047570 * out_step1[ 5][ 7] + 0.32846937 * out_step1[ 5][ 8]
   + 0.05873375 * out_step1[ 6][ 6] + 0.08658601 * out_step1[ 6][ 7] + 0.03624630 * out_step1[ 6][ 8]
   + 0.01479519 * out_step1[ 7][ 6] + 0.36403248 * out_step1[ 7][ 7] - 0.13253643 * out_step1[ 7][ 8]
   + 0.00906407;

out_step2[5][7] = 
   - 0.10991814 * out_step1[ 5][ 7] - 0.07047570 * out_step1[ 5][ 8] + 0.32846937 * out_step1[ 5][ 9]
   + 0.05873375 * out_step1[ 6][ 7] + 0.08658601 * out_step1[ 6][ 8] + 0.03624630 * out_step1[ 6][ 9]
   + 0.01479519 * out_step1[ 7][ 7] + 0.36403248 * out_step1[ 7][ 8] - 0.13253643 * out_step1[ 7][ 9]
   + 0.00906407;

out_step2[5][8] = 
   - 0.10991814 * out_step1[ 5][ 8] - 0.07047570 * out_step1[ 5][ 9] + 0.32846937 * out_step1[ 5][10]
   + 0.05873375 * out_step1[ 6][ 8] + 0.08658601 * out_step1[ 6][ 9] + 0.03624630 * out_step1[ 6][10]
   + 0.01479519 * out_step1[ 7][ 8] + 0.36403248 * out_step1[ 7][ 9] - 0.13253643 * out_step1[ 7][10]
   + 0.00906407;

out_step2[5][9] = 
   - 0.10991814 * out_step1[ 5][ 9] - 0.07047570 * out_step1[ 5][10] + 0.32846937 * out_step1[ 5][11]
   + 0.05873375 * out_step1[ 6][ 9] + 0.08658601 * out_step1[ 6][10] + 0.03624630 * out_step1[ 6][11]
   + 0.01479519 * out_step1[ 7][ 9] + 0.36403248 * out_step1[ 7][10] - 0.13253643 * out_step1[ 7][11]
   + 0.00906407;

out_step2[5][10] = 
   - 0.10991814 * out_step1[ 5][10] - 0.07047570 * out_step1[ 5][11] + 0.32846937 * out_step1[ 5][12]
   + 0.05873375 * out_step1[ 6][10] + 0.08658601 * out_step1[ 6][11] + 0.03624630 * out_step1[ 6][12]
   + 0.01479519 * out_step1[ 7][10] + 0.36403248 * out_step1[ 7][11] - 0.13253643 * out_step1[ 7][12]
   + 0.00906407;

out_step2[5][11] = 
   - 0.10991814 * out_step1[ 5][11] - 0.07047570 * out_step1[ 5][12] + 0.32846937 * out_step1[ 5][13]
   + 0.05873375 * out_step1[ 6][11] + 0.08658601 * out_step1[ 6][12] + 0.03624630 * out_step1[ 6][13]
   + 0.01479519 * out_step1[ 7][11] + 0.36403248 * out_step1[ 7][12] - 0.13253643 * out_step1[ 7][13]
   + 0.00906407;

out_step2[5][12] = 
   - 0.10991814 * out_step1[ 5][12] - 0.07047570 * out_step1[ 5][13] + 0.32846937 * out_step1[ 5][14]
   + 0.05873375 * out_step1[ 6][12] + 0.08658601 * out_step1[ 6][13] + 0.03624630 * out_step1[ 6][14]
   + 0.01479519 * out_step1[ 7][12] + 0.36403248 * out_step1[ 7][13] - 0.13253643 * out_step1[ 7][14]
   + 0.00906407;

out_step2[5][13] = 
   - 0.10991814 * out_step1[ 5][13] - 0.07047570 * out_step1[ 5][14] + 0.32846937 * out_step1[ 5][15]
   + 0.05873375 * out_step1[ 6][13] + 0.08658601 * out_step1[ 6][14] + 0.03624630 * out_step1[ 6][15]
   + 0.01479519 * out_step1[ 7][13] + 0.36403248 * out_step1[ 7][14] - 0.13253643 * out_step1[ 7][15]
   + 0.00906407;

out_step2[6][0] = 
   - 0.10991814 * out_step1[ 6][ 0] - 0.07047570 * out_step1[ 6][ 1] + 0.32846937 * out_step1[ 6][ 2]
   + 0.05873375 * out_step1[ 7][ 0] + 0.08658601 * out_step1[ 7][ 1] + 0.03624630 * out_step1[ 7][ 2]
   + 0.01479519 * out_step1[ 8][ 0] + 0.36403248 * out_step1[ 8][ 1] - 0.13253643 * out_step1[ 8][ 2]
   + 0.00906407;

out_step2[6][1] = 
   - 0.10991814 * out_step1[ 6][ 1] - 0.07047570 * out_step1[ 6][ 2] + 0.32846937 * out_step1[ 6][ 3]
   + 0.05873375 * out_step1[ 7][ 1] + 0.08658601 * out_step1[ 7][ 2] + 0.03624630 * out_step1[ 7][ 3]
   + 0.01479519 * out_step1[ 8][ 1] + 0.36403248 * out_step1[ 8][ 2] - 0.13253643 * out_step1[ 8][ 3]
   + 0.00906407;

out_step2[6][2] = 
   - 0.10991814 * out_step1[ 6][ 2] - 0.07047570 * out_step1[ 6][ 3] + 0.32846937 * out_step1[ 6][ 4]
   + 0.05873375 * out_step1[ 7][ 2] + 0.08658601 * out_step1[ 7][ 3] + 0.03624630 * out_step1[ 7][ 4]
   + 0.01479519 * out_step1[ 8][ 2] + 0.36403248 * out_step1[ 8][ 3] - 0.13253643 * out_step1[ 8][ 4]
   + 0.00906407;

out_step2[6][3] = 
   - 0.10991814 * out_step1[ 6][ 3] - 0.07047570 * out_step1[ 6][ 4] + 0.32846937 * out_step1[ 6][ 5]
   + 0.05873375 * out_step1[ 7][ 3] + 0.08658601 * out_step1[ 7][ 4] + 0.03624630 * out_step1[ 7][ 5]
   + 0.01479519 * out_step1[ 8][ 3] + 0.36403248 * out_step1[ 8][ 4] - 0.13253643 * out_step1[ 8][ 5]
   + 0.00906407;

out_step2[6][4] = 
   - 0.10991814 * out_step1[ 6][ 4] - 0.07047570 * out_step1[ 6][ 5] + 0.32846937 * out_step1[ 6][ 6]
   + 0.05873375 * out_step1[ 7][ 4] + 0.08658601 * out_step1[ 7][ 5] + 0.03624630 * out_step1[ 7][ 6]
   + 0.01479519 * out_step1[ 8][ 4] + 0.36403248 * out_step1[ 8][ 5] - 0.13253643 * out_step1[ 8][ 6]
   + 0.00906407;

out_step2[6][5] = 
   - 0.10991814 * out_step1[ 6][ 5] - 0.07047570 * out_step1[ 6][ 6] + 0.32846937 * out_step1[ 6][ 7]
   + 0.05873375 * out_step1[ 7][ 5] + 0.08658601 * out_step1[ 7][ 6] + 0.03624630 * out_step1[ 7][ 7]
   + 0.01479519 * out_step1[ 8][ 5] + 0.36403248 * out_step1[ 8][ 6] - 0.13253643 * out_step1[ 8][ 7]
   + 0.00906407;

out_step2[6][6] = 
   - 0.10991814 * out_step1[ 6][ 6] - 0.07047570 * out_step1[ 6][ 7] + 0.32846937 * out_step1[ 6][ 8]
   + 0.05873375 * out_step1[ 7][ 6] + 0.08658601 * out_step1[ 7][ 7] + 0.03624630 * out_step1[ 7][ 8]
   + 0.01479519 * out_step1[ 8][ 6] + 0.36403248 * out_step1[ 8][ 7] - 0.13253643 * out_step1[ 8][ 8]
   + 0.00906407;

out_step2[6][7] = 
   - 0.10991814 * out_step1[ 6][ 7] - 0.07047570 * out_step1[ 6][ 8] + 0.32846937 * out_step1[ 6][ 9]
   + 0.05873375 * out_step1[ 7][ 7] + 0.08658601 * out_step1[ 7][ 8] + 0.03624630 * out_step1[ 7][ 9]
   + 0.01479519 * out_step1[ 8][ 7] + 0.36403248 * out_step1[ 8][ 8] - 0.13253643 * out_step1[ 8][ 9]
   + 0.00906407;

out_step2[6][8] = 
   - 0.10991814 * out_step1[ 6][ 8] - 0.07047570 * out_step1[ 6][ 9] + 0.32846937 * out_step1[ 6][10]
   + 0.05873375 * out_step1[ 7][ 8] + 0.08658601 * out_step1[ 7][ 9] + 0.03624630 * out_step1[ 7][10]
   + 0.01479519 * out_step1[ 8][ 8] + 0.36403248 * out_step1[ 8][ 9] - 0.13253643 * out_step1[ 8][10]
   + 0.00906407;

out_step2[6][9] = 
   - 0.10991814 * out_step1[ 6][ 9] - 0.07047570 * out_step1[ 6][10] + 0.32846937 * out_step1[ 6][11]
   + 0.05873375 * out_step1[ 7][ 9] + 0.08658601 * out_step1[ 7][10] + 0.03624630 * out_step1[ 7][11]
   + 0.01479519 * out_step1[ 8][ 9] + 0.36403248 * out_step1[ 8][10] - 0.13253643 * out_step1[ 8][11]
   + 0.00906407;

out_step2[6][10] = 
   - 0.10991814 * out_step1[ 6][10] - 0.07047570 * out_step1[ 6][11] + 0.32846937 * out_step1[ 6][12]
   + 0.05873375 * out_step1[ 7][10] + 0.08658601 * out_step1[ 7][11] + 0.03624630 * out_step1[ 7][12]
   + 0.01479519 * out_step1[ 8][10] + 0.36403248 * out_step1[ 8][11] - 0.13253643 * out_step1[ 8][12]
   + 0.00906407;

out_step2[6][11] = 
   - 0.10991814 * out_step1[ 6][11] - 0.07047570 * out_step1[ 6][12] + 0.32846937 * out_step1[ 6][13]
   + 0.05873375 * out_step1[ 7][11] + 0.08658601 * out_step1[ 7][12] + 0.03624630 * out_step1[ 7][13]
   + 0.01479519 * out_step1[ 8][11] + 0.36403248 * out_step1[ 8][12] - 0.13253643 * out_step1[ 8][13]
   + 0.00906407;

out_step2[6][12] = 
   - 0.10991814 * out_step1[ 6][12] - 0.07047570 * out_step1[ 6][13] + 0.32846937 * out_step1[ 6][14]
   + 0.05873375 * out_step1[ 7][12] + 0.08658601 * out_step1[ 7][13] + 0.03624630 * out_step1[ 7][14]
   + 0.01479519 * out_step1[ 8][12] + 0.36403248 * out_step1[ 8][13] - 0.13253643 * out_step1[ 8][14]
   + 0.00906407;

out_step2[6][13] = 
   - 0.10991814 * out_step1[ 6][13] - 0.07047570 * out_step1[ 6][14] + 0.32846937 * out_step1[ 6][15]
   + 0.05873375 * out_step1[ 7][13] + 0.08658601 * out_step1[ 7][14] + 0.03624630 * out_step1[ 7][15]
   + 0.01479519 * out_step1[ 8][13] + 0.36403248 * out_step1[ 8][14] - 0.13253643 * out_step1[ 8][15]
   + 0.00906407;

out_step2[7][0] = 
   - 0.10991814 * out_step1[ 7][ 0] - 0.07047570 * out_step1[ 7][ 1] + 0.32846937 * out_step1[ 7][ 2]
   + 0.05873375 * out_step1[ 8][ 0] + 0.08658601 * out_step1[ 8][ 1] + 0.03624630 * out_step1[ 8][ 2]
   + 0.01479519 * out_step1[ 9][ 0] + 0.36403248 * out_step1[ 9][ 1] - 0.13253643 * out_step1[ 9][ 2]
   + 0.00906407;

out_step2[7][1] = 
   - 0.10991814 * out_step1[ 7][ 1] - 0.07047570 * out_step1[ 7][ 2] + 0.32846937 * out_step1[ 7][ 3]
   + 0.05873375 * out_step1[ 8][ 1] + 0.08658601 * out_step1[ 8][ 2] + 0.03624630 * out_step1[ 8][ 3]
   + 0.01479519 * out_step1[ 9][ 1] + 0.36403248 * out_step1[ 9][ 2] - 0.13253643 * out_step1[ 9][ 3]
   + 0.00906407;

out_step2[7][2] = 
   - 0.10991814 * out_step1[ 7][ 2] - 0.07047570 * out_step1[ 7][ 3] + 0.32846937 * out_step1[ 7][ 4]
   + 0.05873375 * out_step1[ 8][ 2] + 0.08658601 * out_step1[ 8][ 3] + 0.03624630 * out_step1[ 8][ 4]
   + 0.01479519 * out_step1[ 9][ 2] + 0.36403248 * out_step1[ 9][ 3] - 0.13253643 * out_step1[ 9][ 4]
   + 0.00906407;

out_step2[7][3] = 
   - 0.10991814 * out_step1[ 7][ 3] - 0.07047570 * out_step1[ 7][ 4] + 0.32846937 * out_step1[ 7][ 5]
   + 0.05873375 * out_step1[ 8][ 3] + 0.08658601 * out_step1[ 8][ 4] + 0.03624630 * out_step1[ 8][ 5]
   + 0.01479519 * out_step1[ 9][ 3] + 0.36403248 * out_step1[ 9][ 4] - 0.13253643 * out_step1[ 9][ 5]
   + 0.00906407;

out_step2[7][4] = 
   - 0.10991814 * out_step1[ 7][ 4] - 0.07047570 * out_step1[ 7][ 5] + 0.32846937 * out_step1[ 7][ 6]
   + 0.05873375 * out_step1[ 8][ 4] + 0.08658601 * out_step1[ 8][ 5] + 0.03624630 * out_step1[ 8][ 6]
   + 0.01479519 * out_step1[ 9][ 4] + 0.36403248 * out_step1[ 9][ 5] - 0.13253643 * out_step1[ 9][ 6]
   + 0.00906407;

out_step2[7][5] = 
   - 0.10991814 * out_step1[ 7][ 5] - 0.07047570 * out_step1[ 7][ 6] + 0.32846937 * out_step1[ 7][ 7]
   + 0.05873375 * out_step1[ 8][ 5] + 0.08658601 * out_step1[ 8][ 6] + 0.03624630 * out_step1[ 8][ 7]
   + 0.01479519 * out_step1[ 9][ 5] + 0.36403248 * out_step1[ 9][ 6] - 0.13253643 * out_step1[ 9][ 7]
   + 0.00906407;

out_step2[7][6] = 
   - 0.10991814 * out_step1[ 7][ 6] - 0.07047570 * out_step1[ 7][ 7] + 0.32846937 * out_step1[ 7][ 8]
   + 0.05873375 * out_step1[ 8][ 6] + 0.08658601 * out_step1[ 8][ 7] + 0.03624630 * out_step1[ 8][ 8]
   + 0.01479519 * out_step1[ 9][ 6] + 0.36403248 * out_step1[ 9][ 7] - 0.13253643 * out_step1[ 9][ 8]
   + 0.00906407;

out_step2[7][7] = 
   - 0.10991814 * out_step1[ 7][ 7] - 0.07047570 * out_step1[ 7][ 8] + 0.32846937 * out_step1[ 7][ 9]
   + 0.05873375 * out_step1[ 8][ 7] + 0.08658601 * out_step1[ 8][ 8] + 0.03624630 * out_step1[ 8][ 9]
   + 0.01479519 * out_step1[ 9][ 7] + 0.36403248 * out_step1[ 9][ 8] - 0.13253643 * out_step1[ 9][ 9]
   + 0.00906407;

out_step2[7][8] = 
   - 0.10991814 * out_step1[ 7][ 8] - 0.07047570 * out_step1[ 7][ 9] + 0.32846937 * out_step1[ 7][10]
   + 0.05873375 * out_step1[ 8][ 8] + 0.08658601 * out_step1[ 8][ 9] + 0.03624630 * out_step1[ 8][10]
   + 0.01479519 * out_step1[ 9][ 8] + 0.36403248 * out_step1[ 9][ 9] - 0.13253643 * out_step1[ 9][10]
   + 0.00906407;

out_step2[7][9] = 
   - 0.10991814 * out_step1[ 7][ 9] - 0.07047570 * out_step1[ 7][10] + 0.32846937 * out_step1[ 7][11]
   + 0.05873375 * out_step1[ 8][ 9] + 0.08658601 * out_step1[ 8][10] + 0.03624630 * out_step1[ 8][11]
   + 0.01479519 * out_step1[ 9][ 9] + 0.36403248 * out_step1[ 9][10] - 0.13253643 * out_step1[ 9][11]
   + 0.00906407;

out_step2[7][10] = 
   - 0.10991814 * out_step1[ 7][10] - 0.07047570 * out_step1[ 7][11] + 0.32846937 * out_step1[ 7][12]
   + 0.05873375 * out_step1[ 8][10] + 0.08658601 * out_step1[ 8][11] + 0.03624630 * out_step1[ 8][12]
   + 0.01479519 * out_step1[ 9][10] + 0.36403248 * out_step1[ 9][11] - 0.13253643 * out_step1[ 9][12]
   + 0.00906407;

out_step2[7][11] = 
   - 0.10991814 * out_step1[ 7][11] - 0.07047570 * out_step1[ 7][12] + 0.32846937 * out_step1[ 7][13]
   + 0.05873375 * out_step1[ 8][11] + 0.08658601 * out_step1[ 8][12] + 0.03624630 * out_step1[ 8][13]
   + 0.01479519 * out_step1[ 9][11] + 0.36403248 * out_step1[ 9][12] - 0.13253643 * out_step1[ 9][13]
   + 0.00906407;

out_step2[7][12] = 
   - 0.10991814 * out_step1[ 7][12] - 0.07047570 * out_step1[ 7][13] + 0.32846937 * out_step1[ 7][14]
   + 0.05873375 * out_step1[ 8][12] + 0.08658601 * out_step1[ 8][13] + 0.03624630 * out_step1[ 8][14]
   + 0.01479519 * out_step1[ 9][12] + 0.36403248 * out_step1[ 9][13] - 0.13253643 * out_step1[ 9][14]
   + 0.00906407;

out_step2[7][13] = 
   - 0.10991814 * out_step1[ 7][13] - 0.07047570 * out_step1[ 7][14] + 0.32846937 * out_step1[ 7][15]
   + 0.05873375 * out_step1[ 8][13] + 0.08658601 * out_step1[ 8][14] + 0.03624630 * out_step1[ 8][15]
   + 0.01479519 * out_step1[ 9][13] + 0.36403248 * out_step1[ 9][14] - 0.13253643 * out_step1[ 9][15]
   + 0.00906407;

out_step2[8][0] = 
   - 0.10991814 * out_step1[ 8][ 0] - 0.07047570 * out_step1[ 8][ 1] + 0.32846937 * out_step1[ 8][ 2]
   + 0.05873375 * out_step1[ 9][ 0] + 0.08658601 * out_step1[ 9][ 1] + 0.03624630 * out_step1[ 9][ 2]
   + 0.01479519 * out_step1[10][ 0] + 0.36403248 * out_step1[10][ 1] - 0.13253643 * out_step1[10][ 2]
   + 0.00906407;

out_step2[8][1] = 
   - 0.10991814 * out_step1[ 8][ 1] - 0.07047570 * out_step1[ 8][ 2] + 0.32846937 * out_step1[ 8][ 3]
   + 0.05873375 * out_step1[ 9][ 1] + 0.08658601 * out_step1[ 9][ 2] + 0.03624630 * out_step1[ 9][ 3]
   + 0.01479519 * out_step1[10][ 1] + 0.36403248 * out_step1[10][ 2] - 0.13253643 * out_step1[10][ 3]
   + 0.00906407;

out_step2[8][2] = 
   - 0.10991814 * out_step1[ 8][ 2] - 0.07047570 * out_step1[ 8][ 3] + 0.32846937 * out_step1[ 8][ 4]
   + 0.05873375 * out_step1[ 9][ 2] + 0.08658601 * out_step1[ 9][ 3] + 0.03624630 * out_step1[ 9][ 4]
   + 0.01479519 * out_step1[10][ 2] + 0.36403248 * out_step1[10][ 3] - 0.13253643 * out_step1[10][ 4]
   + 0.00906407;

out_step2[8][3] = 
   - 0.10991814 * out_step1[ 8][ 3] - 0.07047570 * out_step1[ 8][ 4] + 0.32846937 * out_step1[ 8][ 5]
   + 0.05873375 * out_step1[ 9][ 3] + 0.08658601 * out_step1[ 9][ 4] + 0.03624630 * out_step1[ 9][ 5]
   + 0.01479519 * out_step1[10][ 3] + 0.36403248 * out_step1[10][ 4] - 0.13253643 * out_step1[10][ 5]
   + 0.00906407;

out_step2[8][4] = 
   - 0.10991814 * out_step1[ 8][ 4] - 0.07047570 * out_step1[ 8][ 5] + 0.32846937 * out_step1[ 8][ 6]
   + 0.05873375 * out_step1[ 9][ 4] + 0.08658601 * out_step1[ 9][ 5] + 0.03624630 * out_step1[ 9][ 6]
   + 0.01479519 * out_step1[10][ 4] + 0.36403248 * out_step1[10][ 5] - 0.13253643 * out_step1[10][ 6]
   + 0.00906407;

out_step2[8][5] = 
   - 0.10991814 * out_step1[ 8][ 5] - 0.07047570 * out_step1[ 8][ 6] + 0.32846937 * out_step1[ 8][ 7]
   + 0.05873375 * out_step1[ 9][ 5] + 0.08658601 * out_step1[ 9][ 6] + 0.03624630 * out_step1[ 9][ 7]
   + 0.01479519 * out_step1[10][ 5] + 0.36403248 * out_step1[10][ 6] - 0.13253643 * out_step1[10][ 7]
   + 0.00906407;

out_step2[8][6] = 
   - 0.10991814 * out_step1[ 8][ 6] - 0.07047570 * out_step1[ 8][ 7] + 0.32846937 * out_step1[ 8][ 8]
   + 0.05873375 * out_step1[ 9][ 6] + 0.08658601 * out_step1[ 9][ 7] + 0.03624630 * out_step1[ 9][ 8]
   + 0.01479519 * out_step1[10][ 6] + 0.36403248 * out_step1[10][ 7] - 0.13253643 * out_step1[10][ 8]
   + 0.00906407;

out_step2[8][7] = 
   - 0.10991814 * out_step1[ 8][ 7] - 0.07047570 * out_step1[ 8][ 8] + 0.32846937 * out_step1[ 8][ 9]
   + 0.05873375 * out_step1[ 9][ 7] + 0.08658601 * out_step1[ 9][ 8] + 0.03624630 * out_step1[ 9][ 9]
   + 0.01479519 * out_step1[10][ 7] + 0.36403248 * out_step1[10][ 8] - 0.13253643 * out_step1[10][ 9]
   + 0.00906407;

out_step2[8][8] = 
   - 0.10991814 * out_step1[ 8][ 8] - 0.07047570 * out_step1[ 8][ 9] + 0.32846937 * out_step1[ 8][10]
   + 0.05873375 * out_step1[ 9][ 8] + 0.08658601 * out_step1[ 9][ 9] + 0.03624630 * out_step1[ 9][10]
   + 0.01479519 * out_step1[10][ 8] + 0.36403248 * out_step1[10][ 9] - 0.13253643 * out_step1[10][10]
   + 0.00906407;

out_step2[8][9] = 
   - 0.10991814 * out_step1[ 8][ 9] - 0.07047570 * out_step1[ 8][10] + 0.32846937 * out_step1[ 8][11]
   + 0.05873375 * out_step1[ 9][ 9] + 0.08658601 * out_step1[ 9][10] + 0.03624630 * out_step1[ 9][11]
   + 0.01479519 * out_step1[10][ 9] + 0.36403248 * out_step1[10][10] - 0.13253643 * out_step1[10][11]
   + 0.00906407;

out_step2[8][10] = 
   - 0.10991814 * out_step1[ 8][10] - 0.07047570 * out_step1[ 8][11] + 0.32846937 * out_step1[ 8][12]
   + 0.05873375 * out_step1[ 9][10] + 0.08658601 * out_step1[ 9][11] + 0.03624630 * out_step1[ 9][12]
   + 0.01479519 * out_step1[10][10] + 0.36403248 * out_step1[10][11] - 0.13253643 * out_step1[10][12]
   + 0.00906407;

out_step2[8][11] = 
   - 0.10991814 * out_step1[ 8][11] - 0.07047570 * out_step1[ 8][12] + 0.32846937 * out_step1[ 8][13]
   + 0.05873375 * out_step1[ 9][11] + 0.08658601 * out_step1[ 9][12] + 0.03624630 * out_step1[ 9][13]
   + 0.01479519 * out_step1[10][11] + 0.36403248 * out_step1[10][12] - 0.13253643 * out_step1[10][13]
   + 0.00906407;

out_step2[8][12] = 
   - 0.10991814 * out_step1[ 8][12] - 0.07047570 * out_step1[ 8][13] + 0.32846937 * out_step1[ 8][14]
   + 0.05873375 * out_step1[ 9][12] + 0.08658601 * out_step1[ 9][13] + 0.03624630 * out_step1[ 9][14]
   + 0.01479519 * out_step1[10][12] + 0.36403248 * out_step1[10][13] - 0.13253643 * out_step1[10][14]
   + 0.00906407;

out_step2[8][13] = 
   - 0.10991814 * out_step1[ 8][13] - 0.07047570 * out_step1[ 8][14] + 0.32846937 * out_step1[ 8][15]
   + 0.05873375 * out_step1[ 9][13] + 0.08658601 * out_step1[ 9][14] + 0.03624630 * out_step1[ 9][15]
   + 0.01479519 * out_step1[10][13] + 0.36403248 * out_step1[10][14] - 0.13253643 * out_step1[10][15]
   + 0.00906407;

out_step2[9][0] = 
   - 0.10991814 * out_step1[ 9][ 0] - 0.07047570 * out_step1[ 9][ 1] + 0.32846937 * out_step1[ 9][ 2]
   + 0.05873375 * out_step1[10][ 0] + 0.08658601 * out_step1[10][ 1] + 0.03624630 * out_step1[10][ 2]
   + 0.01479519 * out_step1[11][ 0] + 0.36403248 * out_step1[11][ 1] - 0.13253643 * out_step1[11][ 2]
   + 0.00906407;

out_step2[9][1] = 
   - 0.10991814 * out_step1[ 9][ 1] - 0.07047570 * out_step1[ 9][ 2] + 0.32846937 * out_step1[ 9][ 3]
   + 0.05873375 * out_step1[10][ 1] + 0.08658601 * out_step1[10][ 2] + 0.03624630 * out_step1[10][ 3]
   + 0.01479519 * out_step1[11][ 1] + 0.36403248 * out_step1[11][ 2] - 0.13253643 * out_step1[11][ 3]
   + 0.00906407;

out_step2[9][2] = 
   - 0.10991814 * out_step1[ 9][ 2] - 0.07047570 * out_step1[ 9][ 3] + 0.32846937 * out_step1[ 9][ 4]
   + 0.05873375 * out_step1[10][ 2] + 0.08658601 * out_step1[10][ 3] + 0.03624630 * out_step1[10][ 4]
   + 0.01479519 * out_step1[11][ 2] + 0.36403248 * out_step1[11][ 3] - 0.13253643 * out_step1[11][ 4]
   + 0.00906407;

out_step2[9][3] = 
   - 0.10991814 * out_step1[ 9][ 3] - 0.07047570 * out_step1[ 9][ 4] + 0.32846937 * out_step1[ 9][ 5]
   + 0.05873375 * out_step1[10][ 3] + 0.08658601 * out_step1[10][ 4] + 0.03624630 * out_step1[10][ 5]
   + 0.01479519 * out_step1[11][ 3] + 0.36403248 * out_step1[11][ 4] - 0.13253643 * out_step1[11][ 5]
   + 0.00906407;

out_step2[9][4] = 
   - 0.10991814 * out_step1[ 9][ 4] - 0.07047570 * out_step1[ 9][ 5] + 0.32846937 * out_step1[ 9][ 6]
   + 0.05873375 * out_step1[10][ 4] + 0.08658601 * out_step1[10][ 5] + 0.03624630 * out_step1[10][ 6]
   + 0.01479519 * out_step1[11][ 4] + 0.36403248 * out_step1[11][ 5] - 0.13253643 * out_step1[11][ 6]
   + 0.00906407;

out_step2[9][5] = 
   - 0.10991814 * out_step1[ 9][ 5] - 0.07047570 * out_step1[ 9][ 6] + 0.32846937 * out_step1[ 9][ 7]
   + 0.05873375 * out_step1[10][ 5] + 0.08658601 * out_step1[10][ 6] + 0.03624630 * out_step1[10][ 7]
   + 0.01479519 * out_step1[11][ 5] + 0.36403248 * out_step1[11][ 6] - 0.13253643 * out_step1[11][ 7]
   + 0.00906407;

out_step2[9][6] = 
   - 0.10991814 * out_step1[ 9][ 6] - 0.07047570 * out_step1[ 9][ 7] + 0.32846937 * out_step1[ 9][ 8]
   + 0.05873375 * out_step1[10][ 6] + 0.08658601 * out_step1[10][ 7] + 0.03624630 * out_step1[10][ 8]
   + 0.01479519 * out_step1[11][ 6] + 0.36403248 * out_step1[11][ 7] - 0.13253643 * out_step1[11][ 8]
   + 0.00906407;

out_step2[9][7] = 
   - 0.10991814 * out_step1[ 9][ 7] - 0.07047570 * out_step1[ 9][ 8] + 0.32846937 * out_step1[ 9][ 9]
   + 0.05873375 * out_step1[10][ 7] + 0.08658601 * out_step1[10][ 8] + 0.03624630 * out_step1[10][ 9]
   + 0.01479519 * out_step1[11][ 7] + 0.36403248 * out_step1[11][ 8] - 0.13253643 * out_step1[11][ 9]
   + 0.00906407;

out_step2[9][8] = 
   - 0.10991814 * out_step1[ 9][ 8] - 0.07047570 * out_step1[ 9][ 9] + 0.32846937 * out_step1[ 9][10]
   + 0.05873375 * out_step1[10][ 8] + 0.08658601 * out_step1[10][ 9] + 0.03624630 * out_step1[10][10]
   + 0.01479519 * out_step1[11][ 8] + 0.36403248 * out_step1[11][ 9] - 0.13253643 * out_step1[11][10]
   + 0.00906407;

out_step2[9][9] = 
   - 0.10991814 * out_step1[ 9][ 9] - 0.07047570 * out_step1[ 9][10] + 0.32846937 * out_step1[ 9][11]
   + 0.05873375 * out_step1[10][ 9] + 0.08658601 * out_step1[10][10] + 0.03624630 * out_step1[10][11]
   + 0.01479519 * out_step1[11][ 9] + 0.36403248 * out_step1[11][10] - 0.13253643 * out_step1[11][11]
   + 0.00906407;

out_step2[9][10] = 
   - 0.10991814 * out_step1[ 9][10] - 0.07047570 * out_step1[ 9][11] + 0.32846937 * out_step1[ 9][12]
   + 0.05873375 * out_step1[10][10] + 0.08658601 * out_step1[10][11] + 0.03624630 * out_step1[10][12]
   + 0.01479519 * out_step1[11][10] + 0.36403248 * out_step1[11][11] - 0.13253643 * out_step1[11][12]
   + 0.00906407;

out_step2[9][11] = 
   - 0.10991814 * out_step1[ 9][11] - 0.07047570 * out_step1[ 9][12] + 0.32846937 * out_step1[ 9][13]
   + 0.05873375 * out_step1[10][11] + 0.08658601 * out_step1[10][12] + 0.03624630 * out_step1[10][13]
   + 0.01479519 * out_step1[11][11] + 0.36403248 * out_step1[11][12] - 0.13253643 * out_step1[11][13]
   + 0.00906407;

out_step2[9][12] = 
   - 0.10991814 * out_step1[ 9][12] - 0.07047570 * out_step1[ 9][13] + 0.32846937 * out_step1[ 9][14]
   + 0.05873375 * out_step1[10][12] + 0.08658601 * out_step1[10][13] + 0.03624630 * out_step1[10][14]
   + 0.01479519 * out_step1[11][12] + 0.36403248 * out_step1[11][13] - 0.13253643 * out_step1[11][14]
   + 0.00906407;

out_step2[9][13] = 
   - 0.10991814 * out_step1[ 9][13] - 0.07047570 * out_step1[ 9][14] + 0.32846937 * out_step1[ 9][15]
   + 0.05873375 * out_step1[10][13] + 0.08658601 * out_step1[10][14] + 0.03624630 * out_step1[10][15]
   + 0.01479519 * out_step1[11][13] + 0.36403248 * out_step1[11][14] - 0.13253643 * out_step1[11][15]
   + 0.00906407;

out_step2[10][0] = 
   - 0.10991814 * out_step1[10][ 0] - 0.07047570 * out_step1[10][ 1] + 0.32846937 * out_step1[10][ 2]
   + 0.05873375 * out_step1[11][ 0] + 0.08658601 * out_step1[11][ 1] + 0.03624630 * out_step1[11][ 2]
   + 0.01479519 * out_step1[12][ 0] + 0.36403248 * out_step1[12][ 1] - 0.13253643 * out_step1[12][ 2]
   + 0.00906407;

out_step2[10][1] = 
   - 0.10991814 * out_step1[10][ 1] - 0.07047570 * out_step1[10][ 2] + 0.32846937 * out_step1[10][ 3]
   + 0.05873375 * out_step1[11][ 1] + 0.08658601 * out_step1[11][ 2] + 0.03624630 * out_step1[11][ 3]
   + 0.01479519 * out_step1[12][ 1] + 0.36403248 * out_step1[12][ 2] - 0.13253643 * out_step1[12][ 3]
   + 0.00906407;

out_step2[10][2] = 
   - 0.10991814 * out_step1[10][ 2] - 0.07047570 * out_step1[10][ 3] + 0.32846937 * out_step1[10][ 4]
   + 0.05873375 * out_step1[11][ 2] + 0.08658601 * out_step1[11][ 3] + 0.03624630 * out_step1[11][ 4]
   + 0.01479519 * out_step1[12][ 2] + 0.36403248 * out_step1[12][ 3] - 0.13253643 * out_step1[12][ 4]
   + 0.00906407;

out_step2[10][3] = 
   - 0.10991814 * out_step1[10][ 3] - 0.07047570 * out_step1[10][ 4] + 0.32846937 * out_step1[10][ 5]
   + 0.05873375 * out_step1[11][ 3] + 0.08658601 * out_step1[11][ 4] + 0.03624630 * out_step1[11][ 5]
   + 0.01479519 * out_step1[12][ 3] + 0.36403248 * out_step1[12][ 4] - 0.13253643 * out_step1[12][ 5]
   + 0.00906407;

out_step2[10][4] = 
   - 0.10991814 * out_step1[10][ 4] - 0.07047570 * out_step1[10][ 5] + 0.32846937 * out_step1[10][ 6]
   + 0.05873375 * out_step1[11][ 4] + 0.08658601 * out_step1[11][ 5] + 0.03624630 * out_step1[11][ 6]
   + 0.01479519 * out_step1[12][ 4] + 0.36403248 * out_step1[12][ 5] - 0.13253643 * out_step1[12][ 6]
   + 0.00906407;

out_step2[10][5] = 
   - 0.10991814 * out_step1[10][ 5] - 0.07047570 * out_step1[10][ 6] + 0.32846937 * out_step1[10][ 7]
   + 0.05873375 * out_step1[11][ 5] + 0.08658601 * out_step1[11][ 6] + 0.03624630 * out_step1[11][ 7]
   + 0.01479519 * out_step1[12][ 5] + 0.36403248 * out_step1[12][ 6] - 0.13253643 * out_step1[12][ 7]
   + 0.00906407;

out_step2[10][6] = 
   - 0.10991814 * out_step1[10][ 6] - 0.07047570 * out_step1[10][ 7] + 0.32846937 * out_step1[10][ 8]
   + 0.05873375 * out_step1[11][ 6] + 0.08658601 * out_step1[11][ 7] + 0.03624630 * out_step1[11][ 8]
   + 0.01479519 * out_step1[12][ 6] + 0.36403248 * out_step1[12][ 7] - 0.13253643 * out_step1[12][ 8]
   + 0.00906407;

out_step2[10][7] = 
   - 0.10991814 * out_step1[10][ 7] - 0.07047570 * out_step1[10][ 8] + 0.32846937 * out_step1[10][ 9]
   + 0.05873375 * out_step1[11][ 7] + 0.08658601 * out_step1[11][ 8] + 0.03624630 * out_step1[11][ 9]
   + 0.01479519 * out_step1[12][ 7] + 0.36403248 * out_step1[12][ 8] - 0.13253643 * out_step1[12][ 9]
   + 0.00906407;

out_step2[10][8] = 
   - 0.10991814 * out_step1[10][ 8] - 0.07047570 * out_step1[10][ 9] + 0.32846937 * out_step1[10][10]
   + 0.05873375 * out_step1[11][ 8] + 0.08658601 * out_step1[11][ 9] + 0.03624630 * out_step1[11][10]
   + 0.01479519 * out_step1[12][ 8] + 0.36403248 * out_step1[12][ 9] - 0.13253643 * out_step1[12][10]
   + 0.00906407;

out_step2[10][9] = 
   - 0.10991814 * out_step1[10][ 9] - 0.07047570 * out_step1[10][10] + 0.32846937 * out_step1[10][11]
   + 0.05873375 * out_step1[11][ 9] + 0.08658601 * out_step1[11][10] + 0.03624630 * out_step1[11][11]
   + 0.01479519 * out_step1[12][ 9] + 0.36403248 * out_step1[12][10] - 0.13253643 * out_step1[12][11]
   + 0.00906407;

out_step2[10][10] = 
   - 0.10991814 * out_step1[10][10] - 0.07047570 * out_step1[10][11] + 0.32846937 * out_step1[10][12]
   + 0.05873375 * out_step1[11][10] + 0.08658601 * out_step1[11][11] + 0.03624630 * out_step1[11][12]
   + 0.01479519 * out_step1[12][10] + 0.36403248 * out_step1[12][11] - 0.13253643 * out_step1[12][12]
   + 0.00906407;

out_step2[10][11] = 
   - 0.10991814 * out_step1[10][11] - 0.07047570 * out_step1[10][12] + 0.32846937 * out_step1[10][13]
   + 0.05873375 * out_step1[11][11] + 0.08658601 * out_step1[11][12] + 0.03624630 * out_step1[11][13]
   + 0.01479519 * out_step1[12][11] + 0.36403248 * out_step1[12][12] - 0.13253643 * out_step1[12][13]
   + 0.00906407;

out_step2[10][12] = 
   - 0.10991814 * out_step1[10][12] - 0.07047570 * out_step1[10][13] + 0.32846937 * out_step1[10][14]
   + 0.05873375 * out_step1[11][12] + 0.08658601 * out_step1[11][13] + 0.03624630 * out_step1[11][14]
   + 0.01479519 * out_step1[12][12] + 0.36403248 * out_step1[12][13] - 0.13253643 * out_step1[12][14]
   + 0.00906407;

out_step2[10][13] = 
   - 0.10991814 * out_step1[10][13] - 0.07047570 * out_step1[10][14] + 0.32846937 * out_step1[10][15]
   + 0.05873375 * out_step1[11][13] + 0.08658601 * out_step1[11][14] + 0.03624630 * out_step1[11][15]
   + 0.01479519 * out_step1[12][13] + 0.36403248 * out_step1[12][14] - 0.13253643 * out_step1[12][15]
   + 0.00906407;

out_step2[11][0] = 
   - 0.10991814 * out_step1[11][ 0] - 0.07047570 * out_step1[11][ 1] + 0.32846937 * out_step1[11][ 2]
   + 0.05873375 * out_step1[12][ 0] + 0.08658601 * out_step1[12][ 1] + 0.03624630 * out_step1[12][ 2]
   + 0.01479519 * out_step1[13][ 0] + 0.36403248 * out_step1[13][ 1] - 0.13253643 * out_step1[13][ 2]
   + 0.00906407;

out_step2[11][1] = 
   - 0.10991814 * out_step1[11][ 1] - 0.07047570 * out_step1[11][ 2] + 0.32846937 * out_step1[11][ 3]
   + 0.05873375 * out_step1[12][ 1] + 0.08658601 * out_step1[12][ 2] + 0.03624630 * out_step1[12][ 3]
   + 0.01479519 * out_step1[13][ 1] + 0.36403248 * out_step1[13][ 2] - 0.13253643 * out_step1[13][ 3]
   + 0.00906407;

out_step2[11][2] = 
   - 0.10991814 * out_step1[11][ 2] - 0.07047570 * out_step1[11][ 3] + 0.32846937 * out_step1[11][ 4]
   + 0.05873375 * out_step1[12][ 2] + 0.08658601 * out_step1[12][ 3] + 0.03624630 * out_step1[12][ 4]
   + 0.01479519 * out_step1[13][ 2] + 0.36403248 * out_step1[13][ 3] - 0.13253643 * out_step1[13][ 4]
   + 0.00906407;

out_step2[11][3] = 
   - 0.10991814 * out_step1[11][ 3] - 0.07047570 * out_step1[11][ 4] + 0.32846937 * out_step1[11][ 5]
   + 0.05873375 * out_step1[12][ 3] + 0.08658601 * out_step1[12][ 4] + 0.03624630 * out_step1[12][ 5]
   + 0.01479519 * out_step1[13][ 3] + 0.36403248 * out_step1[13][ 4] - 0.13253643 * out_step1[13][ 5]
   + 0.00906407;

out_step2[11][4] = 
   - 0.10991814 * out_step1[11][ 4] - 0.07047570 * out_step1[11][ 5] + 0.32846937 * out_step1[11][ 6]
   + 0.05873375 * out_step1[12][ 4] + 0.08658601 * out_step1[12][ 5] + 0.03624630 * out_step1[12][ 6]
   + 0.01479519 * out_step1[13][ 4] + 0.36403248 * out_step1[13][ 5] - 0.13253643 * out_step1[13][ 6]
   + 0.00906407;

out_step2[11][5] = 
   - 0.10991814 * out_step1[11][ 5] - 0.07047570 * out_step1[11][ 6] + 0.32846937 * out_step1[11][ 7]
   + 0.05873375 * out_step1[12][ 5] + 0.08658601 * out_step1[12][ 6] + 0.03624630 * out_step1[12][ 7]
   + 0.01479519 * out_step1[13][ 5] + 0.36403248 * out_step1[13][ 6] - 0.13253643 * out_step1[13][ 7]
   + 0.00906407;

out_step2[11][6] = 
   - 0.10991814 * out_step1[11][ 6] - 0.07047570 * out_step1[11][ 7] + 0.32846937 * out_step1[11][ 8]
   + 0.05873375 * out_step1[12][ 6] + 0.08658601 * out_step1[12][ 7] + 0.03624630 * out_step1[12][ 8]
   + 0.01479519 * out_step1[13][ 6] + 0.36403248 * out_step1[13][ 7] - 0.13253643 * out_step1[13][ 8]
   + 0.00906407;

out_step2[11][7] = 
   - 0.10991814 * out_step1[11][ 7] - 0.07047570 * out_step1[11][ 8] + 0.32846937 * out_step1[11][ 9]
   + 0.05873375 * out_step1[12][ 7] + 0.08658601 * out_step1[12][ 8] + 0.03624630 * out_step1[12][ 9]
   + 0.01479519 * out_step1[13][ 7] + 0.36403248 * out_step1[13][ 8] - 0.13253643 * out_step1[13][ 9]
   + 0.00906407;

out_step2[11][8] = 
   - 0.10991814 * out_step1[11][ 8] - 0.07047570 * out_step1[11][ 9] + 0.32846937 * out_step1[11][10]
   + 0.05873375 * out_step1[12][ 8] + 0.08658601 * out_step1[12][ 9] + 0.03624630 * out_step1[12][10]
   + 0.01479519 * out_step1[13][ 8] + 0.36403248 * out_step1[13][ 9] - 0.13253643 * out_step1[13][10]
   + 0.00906407;

out_step2[11][9] = 
   - 0.10991814 * out_step1[11][ 9] - 0.07047570 * out_step1[11][10] + 0.32846937 * out_step1[11][11]
   + 0.05873375 * out_step1[12][ 9] + 0.08658601 * out_step1[12][10] + 0.03624630 * out_step1[12][11]
   + 0.01479519 * out_step1[13][ 9] + 0.36403248 * out_step1[13][10] - 0.13253643 * out_step1[13][11]
   + 0.00906407;

out_step2[11][10] = 
   - 0.10991814 * out_step1[11][10] - 0.07047570 * out_step1[11][11] + 0.32846937 * out_step1[11][12]
   + 0.05873375 * out_step1[12][10] + 0.08658601 * out_step1[12][11] + 0.03624630 * out_step1[12][12]
   + 0.01479519 * out_step1[13][10] + 0.36403248 * out_step1[13][11] - 0.13253643 * out_step1[13][12]
   + 0.00906407;

out_step2[11][11] = 
   - 0.10991814 * out_step1[11][11] - 0.07047570 * out_step1[11][12] + 0.32846937 * out_step1[11][13]
   + 0.05873375 * out_step1[12][11] + 0.08658601 * out_step1[12][12] + 0.03624630 * out_step1[12][13]
   + 0.01479519 * out_step1[13][11] + 0.36403248 * out_step1[13][12] - 0.13253643 * out_step1[13][13]
   + 0.00906407;

out_step2[11][12] = 
   - 0.10991814 * out_step1[11][12] - 0.07047570 * out_step1[11][13] + 0.32846937 * out_step1[11][14]
   + 0.05873375 * out_step1[12][12] + 0.08658601 * out_step1[12][13] + 0.03624630 * out_step1[12][14]
   + 0.01479519 * out_step1[13][12] + 0.36403248 * out_step1[13][13] - 0.13253643 * out_step1[13][14]
   + 0.00906407;

out_step2[11][13] = 
   - 0.10991814 * out_step1[11][13] - 0.07047570 * out_step1[11][14] + 0.32846937 * out_step1[11][15]
   + 0.05873375 * out_step1[12][13] + 0.08658601 * out_step1[12][14] + 0.03624630 * out_step1[12][15]
   + 0.01479519 * out_step1[13][13] + 0.36403248 * out_step1[13][14] - 0.13253643 * out_step1[13][15]
   + 0.00906407;

out_step2[12][0] = 
   - 0.10991814 * out_step1[12][ 0] - 0.07047570 * out_step1[12][ 1] + 0.32846937 * out_step1[12][ 2]
   + 0.05873375 * out_step1[13][ 0] + 0.08658601 * out_step1[13][ 1] + 0.03624630 * out_step1[13][ 2]
   + 0.01479519 * out_step1[14][ 0] + 0.36403248 * out_step1[14][ 1] - 0.13253643 * out_step1[14][ 2]
   + 0.00906407;

out_step2[12][1] = 
   - 0.10991814 * out_step1[12][ 1] - 0.07047570 * out_step1[12][ 2] + 0.32846937 * out_step1[12][ 3]
   + 0.05873375 * out_step1[13][ 1] + 0.08658601 * out_step1[13][ 2] + 0.03624630 * out_step1[13][ 3]
   + 0.01479519 * out_step1[14][ 1] + 0.36403248 * out_step1[14][ 2] - 0.13253643 * out_step1[14][ 3]
   + 0.00906407;

out_step2[12][2] = 
   - 0.10991814 * out_step1[12][ 2] - 0.07047570 * out_step1[12][ 3] + 0.32846937 * out_step1[12][ 4]
   + 0.05873375 * out_step1[13][ 2] + 0.08658601 * out_step1[13][ 3] + 0.03624630 * out_step1[13][ 4]
   + 0.01479519 * out_step1[14][ 2] + 0.36403248 * out_step1[14][ 3] - 0.13253643 * out_step1[14][ 4]
   + 0.00906407;

out_step2[12][3] = 
   - 0.10991814 * out_step1[12][ 3] - 0.07047570 * out_step1[12][ 4] + 0.32846937 * out_step1[12][ 5]
   + 0.05873375 * out_step1[13][ 3] + 0.08658601 * out_step1[13][ 4] + 0.03624630 * out_step1[13][ 5]
   + 0.01479519 * out_step1[14][ 3] + 0.36403248 * out_step1[14][ 4] - 0.13253643 * out_step1[14][ 5]
   + 0.00906407;

out_step2[12][4] = 
   - 0.10991814 * out_step1[12][ 4] - 0.07047570 * out_step1[12][ 5] + 0.32846937 * out_step1[12][ 6]
   + 0.05873375 * out_step1[13][ 4] + 0.08658601 * out_step1[13][ 5] + 0.03624630 * out_step1[13][ 6]
   + 0.01479519 * out_step1[14][ 4] + 0.36403248 * out_step1[14][ 5] - 0.13253643 * out_step1[14][ 6]
   + 0.00906407;

out_step2[12][5] = 
   - 0.10991814 * out_step1[12][ 5] - 0.07047570 * out_step1[12][ 6] + 0.32846937 * out_step1[12][ 7]
   + 0.05873375 * out_step1[13][ 5] + 0.08658601 * out_step1[13][ 6] + 0.03624630 * out_step1[13][ 7]
   + 0.01479519 * out_step1[14][ 5] + 0.36403248 * out_step1[14][ 6] - 0.13253643 * out_step1[14][ 7]
   + 0.00906407;

out_step2[12][6] = 
   - 0.10991814 * out_step1[12][ 6] - 0.07047570 * out_step1[12][ 7] + 0.32846937 * out_step1[12][ 8]
   + 0.05873375 * out_step1[13][ 6] + 0.08658601 * out_step1[13][ 7] + 0.03624630 * out_step1[13][ 8]
   + 0.01479519 * out_step1[14][ 6] + 0.36403248 * out_step1[14][ 7] - 0.13253643 * out_step1[14][ 8]
   + 0.00906407;

out_step2[12][7] = 
   - 0.10991814 * out_step1[12][ 7] - 0.07047570 * out_step1[12][ 8] + 0.32846937 * out_step1[12][ 9]
   + 0.05873375 * out_step1[13][ 7] + 0.08658601 * out_step1[13][ 8] + 0.03624630 * out_step1[13][ 9]
   + 0.01479519 * out_step1[14][ 7] + 0.36403248 * out_step1[14][ 8] - 0.13253643 * out_step1[14][ 9]
   + 0.00906407;

out_step2[12][8] = 
   - 0.10991814 * out_step1[12][ 8] - 0.07047570 * out_step1[12][ 9] + 0.32846937 * out_step1[12][10]
   + 0.05873375 * out_step1[13][ 8] + 0.08658601 * out_step1[13][ 9] + 0.03624630 * out_step1[13][10]
   + 0.01479519 * out_step1[14][ 8] + 0.36403248 * out_step1[14][ 9] - 0.13253643 * out_step1[14][10]
   + 0.00906407;

out_step2[12][9] = 
   - 0.10991814 * out_step1[12][ 9] - 0.07047570 * out_step1[12][10] + 0.32846937 * out_step1[12][11]
   + 0.05873375 * out_step1[13][ 9] + 0.08658601 * out_step1[13][10] + 0.03624630 * out_step1[13][11]
   + 0.01479519 * out_step1[14][ 9] + 0.36403248 * out_step1[14][10] - 0.13253643 * out_step1[14][11]
   + 0.00906407;

out_step2[12][10] = 
   - 0.10991814 * out_step1[12][10] - 0.07047570 * out_step1[12][11] + 0.32846937 * out_step1[12][12]
   + 0.05873375 * out_step1[13][10] + 0.08658601 * out_step1[13][11] + 0.03624630 * out_step1[13][12]
   + 0.01479519 * out_step1[14][10] + 0.36403248 * out_step1[14][11] - 0.13253643 * out_step1[14][12]
   + 0.00906407;

out_step2[12][11] = 
   - 0.10991814 * out_step1[12][11] - 0.07047570 * out_step1[12][12] + 0.32846937 * out_step1[12][13]
   + 0.05873375 * out_step1[13][11] + 0.08658601 * out_step1[13][12] + 0.03624630 * out_step1[13][13]
   + 0.01479519 * out_step1[14][11] + 0.36403248 * out_step1[14][12] - 0.13253643 * out_step1[14][13]
   + 0.00906407;

out_step2[12][12] = 
   - 0.10991814 * out_step1[12][12] - 0.07047570 * out_step1[12][13] + 0.32846937 * out_step1[12][14]
   + 0.05873375 * out_step1[13][12] + 0.08658601 * out_step1[13][13] + 0.03624630 * out_step1[13][14]
   + 0.01479519 * out_step1[14][12] + 0.36403248 * out_step1[14][13] - 0.13253643 * out_step1[14][14]
   + 0.00906407;

out_step2[12][13] = 
   - 0.10991814 * out_step1[12][13] - 0.07047570 * out_step1[12][14] + 0.32846937 * out_step1[12][15]
   + 0.05873375 * out_step1[13][13] + 0.08658601 * out_step1[13][14] + 0.03624630 * out_step1[13][15]
   + 0.01479519 * out_step1[14][13] + 0.36403248 * out_step1[14][14] - 0.13253643 * out_step1[14][15]
   + 0.00906407;

out_step2[13][0] = 
   - 0.10991814 * out_step1[13][ 0] - 0.07047570 * out_step1[13][ 1] + 0.32846937 * out_step1[13][ 2]
   + 0.05873375 * out_step1[14][ 0] + 0.08658601 * out_step1[14][ 1] + 0.03624630 * out_step1[14][ 2]
   + 0.01479519 * out_step1[15][ 0] + 0.36403248 * out_step1[15][ 1] - 0.13253643 * out_step1[15][ 2]
   + 0.00906407;

out_step2[13][1] = 
   - 0.10991814 * out_step1[13][ 1] - 0.07047570 * out_step1[13][ 2] + 0.32846937 * out_step1[13][ 3]
   + 0.05873375 * out_step1[14][ 1] + 0.08658601 * out_step1[14][ 2] + 0.03624630 * out_step1[14][ 3]
   + 0.01479519 * out_step1[15][ 1] + 0.36403248 * out_step1[15][ 2] - 0.13253643 * out_step1[15][ 3]
   + 0.00906407;

out_step2[13][2] = 
   - 0.10991814 * out_step1[13][ 2] - 0.07047570 * out_step1[13][ 3] + 0.32846937 * out_step1[13][ 4]
   + 0.05873375 * out_step1[14][ 2] + 0.08658601 * out_step1[14][ 3] + 0.03624630 * out_step1[14][ 4]
   + 0.01479519 * out_step1[15][ 2] + 0.36403248 * out_step1[15][ 3] - 0.13253643 * out_step1[15][ 4]
   + 0.00906407;

out_step2[13][3] = 
   - 0.10991814 * out_step1[13][ 3] - 0.07047570 * out_step1[13][ 4] + 0.32846937 * out_step1[13][ 5]
   + 0.05873375 * out_step1[14][ 3] + 0.08658601 * out_step1[14][ 4] + 0.03624630 * out_step1[14][ 5]
   + 0.01479519 * out_step1[15][ 3] + 0.36403248 * out_step1[15][ 4] - 0.13253643 * out_step1[15][ 5]
   + 0.00906407;

out_step2[13][4] = 
   - 0.10991814 * out_step1[13][ 4] - 0.07047570 * out_step1[13][ 5] + 0.32846937 * out_step1[13][ 6]
   + 0.05873375 * out_step1[14][ 4] + 0.08658601 * out_step1[14][ 5] + 0.03624630 * out_step1[14][ 6]
   + 0.01479519 * out_step1[15][ 4] + 0.36403248 * out_step1[15][ 5] - 0.13253643 * out_step1[15][ 6]
   + 0.00906407;

out_step2[13][5] = 
   - 0.10991814 * out_step1[13][ 5] - 0.07047570 * out_step1[13][ 6] + 0.32846937 * out_step1[13][ 7]
   + 0.05873375 * out_step1[14][ 5] + 0.08658601 * out_step1[14][ 6] + 0.03624630 * out_step1[14][ 7]
   + 0.01479519 * out_step1[15][ 5] + 0.36403248 * out_step1[15][ 6] - 0.13253643 * out_step1[15][ 7]
   + 0.00906407;

out_step2[13][6] = 
   - 0.10991814 * out_step1[13][ 6] - 0.07047570 * out_step1[13][ 7] + 0.32846937 * out_step1[13][ 8]
   + 0.05873375 * out_step1[14][ 6] + 0.08658601 * out_step1[14][ 7] + 0.03624630 * out_step1[14][ 8]
   + 0.01479519 * out_step1[15][ 6] + 0.36403248 * out_step1[15][ 7] - 0.13253643 * out_step1[15][ 8]
   + 0.00906407;

out_step2[13][7] = 
   - 0.10991814 * out_step1[13][ 7] - 0.07047570 * out_step1[13][ 8] + 0.32846937 * out_step1[13][ 9]
   + 0.05873375 * out_step1[14][ 7] + 0.08658601 * out_step1[14][ 8] + 0.03624630 * out_step1[14][ 9]
   + 0.01479519 * out_step1[15][ 7] + 0.36403248 * out_step1[15][ 8] - 0.13253643 * out_step1[15][ 9]
   + 0.00906407;

out_step2[13][8] = 
   - 0.10991814 * out_step1[13][ 8] - 0.07047570 * out_step1[13][ 9] + 0.32846937 * out_step1[13][10]
   + 0.05873375 * out_step1[14][ 8] + 0.08658601 * out_step1[14][ 9] + 0.03624630 * out_step1[14][10]
   + 0.01479519 * out_step1[15][ 8] + 0.36403248 * out_step1[15][ 9] - 0.13253643 * out_step1[15][10]
   + 0.00906407;

out_step2[13][9] = 
   - 0.10991814 * out_step1[13][ 9] - 0.07047570 * out_step1[13][10] + 0.32846937 * out_step1[13][11]
   + 0.05873375 * out_step1[14][ 9] + 0.08658601 * out_step1[14][10] + 0.03624630 * out_step1[14][11]
   + 0.01479519 * out_step1[15][ 9] + 0.36403248 * out_step1[15][10] - 0.13253643 * out_step1[15][11]
   + 0.00906407;

out_step2[13][10] = 
   - 0.10991814 * out_step1[13][10] - 0.07047570 * out_step1[13][11] + 0.32846937 * out_step1[13][12]
   + 0.05873375 * out_step1[14][10] + 0.08658601 * out_step1[14][11] + 0.03624630 * out_step1[14][12]
   + 0.01479519 * out_step1[15][10] + 0.36403248 * out_step1[15][11] - 0.13253643 * out_step1[15][12]
   + 0.00906407;

out_step2[13][11] = 
   - 0.10991814 * out_step1[13][11] - 0.07047570 * out_step1[13][12] + 0.32846937 * out_step1[13][13]
   + 0.05873375 * out_step1[14][11] + 0.08658601 * out_step1[14][12] + 0.03624630 * out_step1[14][13]
   + 0.01479519 * out_step1[15][11] + 0.36403248 * out_step1[15][12] - 0.13253643 * out_step1[15][13]
   + 0.00906407;

out_step2[13][12] = 
   - 0.10991814 * out_step1[13][12] - 0.07047570 * out_step1[13][13] + 0.32846937 * out_step1[13][14]
   + 0.05873375 * out_step1[14][12] + 0.08658601 * out_step1[14][13] + 0.03624630 * out_step1[14][14]
   + 0.01479519 * out_step1[15][12] + 0.36403248 * out_step1[15][13] - 0.13253643 * out_step1[15][14]
   + 0.00906407;

out_step2[13][13] = 
   - 0.10991814 * out_step1[13][13] - 0.07047570 * out_step1[13][14] + 0.32846937 * out_step1[13][15]
   + 0.05873375 * out_step1[14][13] + 0.08658601 * out_step1[14][14] + 0.03624630 * out_step1[14][15]
   + 0.01479519 * out_step1[15][13] + 0.36403248 * out_step1[15][14] - 0.13253643 * out_step1[15][15]
   + 0.00906407;


// determine value of activation function Rectified-Linear-Unit
out_step3[ 0][ 0] = (out_step2[ 0][ 0] > 0) ? out_step2[ 0][ 0] : 0;
out_step3[ 0][ 1] = (out_step2[ 0][ 1] > 0) ? out_step2[ 0][ 1] : 0;
out_step3[ 0][ 2] = (out_step2[ 0][ 2] > 0) ? out_step2[ 0][ 2] : 0;
out_step3[ 0][ 3] = (out_step2[ 0][ 3] > 0) ? out_step2[ 0][ 3] : 0;
out_step3[ 0][ 4] = (out_step2[ 0][ 4] > 0) ? out_step2[ 0][ 4] : 0;
out_step3[ 0][ 5] = (out_step2[ 0][ 5] > 0) ? out_step2[ 0][ 5] : 0;
out_step3[ 0][ 6] = (out_step2[ 0][ 6] > 0) ? out_step2[ 0][ 6] : 0;
out_step3[ 0][ 7] = (out_step2[ 0][ 7] > 0) ? out_step2[ 0][ 7] : 0;
out_step3[ 0][ 8] = (out_step2[ 0][ 8] > 0) ? out_step2[ 0][ 8] : 0;
out_step3[ 0][ 9] = (out_step2[ 0][ 9] > 0) ? out_step2[ 0][ 9] : 0;
out_step3[ 0][10] = (out_step2[ 0][10] > 0) ? out_step2[ 0][10] : 0;
out_step3[ 0][11] = (out_step2[ 0][11] > 0) ? out_step2[ 0][11] : 0;
out_step3[ 0][12] = (out_step2[ 0][12] > 0) ? out_step2[ 0][12] : 0;
out_step3[ 0][13] = (out_step2[ 0][13] > 0) ? out_step2[ 0][13] : 0;
out_step3[ 1][ 0] = (out_step2[ 1][ 0] > 0) ? out_step2[ 1][ 0] : 0;
out_step3[ 1][ 1] = (out_step2[ 1][ 1] > 0) ? out_step2[ 1][ 1] : 0;
out_step3[ 1][ 2] = (out_step2[ 1][ 2] > 0) ? out_step2[ 1][ 2] : 0;
out_step3[ 1][ 3] = (out_step2[ 1][ 3] > 0) ? out_step2[ 1][ 3] : 0;
out_step3[ 1][ 4] = (out_step2[ 1][ 4] > 0) ? out_step2[ 1][ 4] : 0;
out_step3[ 1][ 5] = (out_step2[ 1][ 5] > 0) ? out_step2[ 1][ 5] : 0;
out_step3[ 1][ 6] = (out_step2[ 1][ 6] > 0) ? out_step2[ 1][ 6] : 0;
out_step3[ 1][ 7] = (out_step2[ 1][ 7] > 0) ? out_step2[ 1][ 7] : 0;
out_step3[ 1][ 8] = (out_step2[ 1][ 8] > 0) ? out_step2[ 1][ 8] : 0;
out_step3[ 1][ 9] = (out_step2[ 1][ 9] > 0) ? out_step2[ 1][ 9] : 0;
out_step3[ 1][10] = (out_step2[ 1][10] > 0) ? out_step2[ 1][10] : 0;
out_step3[ 1][11] = (out_step2[ 1][11] > 0) ? out_step2[ 1][11] : 0;
out_step3[ 1][12] = (out_step2[ 1][12] > 0) ? out_step2[ 1][12] : 0;
out_step3[ 1][13] = (out_step2[ 1][13] > 0) ? out_step2[ 1][13] : 0;
out_step3[ 2][ 0] = (out_step2[ 2][ 0] > 0) ? out_step2[ 2][ 0] : 0;
out_step3[ 2][ 1] = (out_step2[ 2][ 1] > 0) ? out_step2[ 2][ 1] : 0;
out_step3[ 2][ 2] = (out_step2[ 2][ 2] > 0) ? out_step2[ 2][ 2] : 0;
out_step3[ 2][ 3] = (out_step2[ 2][ 3] > 0) ? out_step2[ 2][ 3] : 0;
out_step3[ 2][ 4] = (out_step2[ 2][ 4] > 0) ? out_step2[ 2][ 4] : 0;
out_step3[ 2][ 5] = (out_step2[ 2][ 5] > 0) ? out_step2[ 2][ 5] : 0;
out_step3[ 2][ 6] = (out_step2[ 2][ 6] > 0) ? out_step2[ 2][ 6] : 0;
out_step3[ 2][ 7] = (out_step2[ 2][ 7] > 0) ? out_step2[ 2][ 7] : 0;
out_step3[ 2][ 8] = (out_step2[ 2][ 8] > 0) ? out_step2[ 2][ 8] : 0;
out_step3[ 2][ 9] = (out_step2[ 2][ 9] > 0) ? out_step2[ 2][ 9] : 0;
out_step3[ 2][10] = (out_step2[ 2][10] > 0) ? out_step2[ 2][10] : 0;
out_step3[ 2][11] = (out_step2[ 2][11] > 0) ? out_step2[ 2][11] : 0;
out_step3[ 2][12] = (out_step2[ 2][12] > 0) ? out_step2[ 2][12] : 0;
out_step3[ 2][13] = (out_step2[ 2][13] > 0) ? out_step2[ 2][13] : 0;
out_step3[ 3][ 0] = (out_step2[ 3][ 0] > 0) ? out_step2[ 3][ 0] : 0;
out_step3[ 3][ 1] = (out_step2[ 3][ 1] > 0) ? out_step2[ 3][ 1] : 0;
out_step3[ 3][ 2] = (out_step2[ 3][ 2] > 0) ? out_step2[ 3][ 2] : 0;
out_step3[ 3][ 3] = (out_step2[ 3][ 3] > 0) ? out_step2[ 3][ 3] : 0;
out_step3[ 3][ 4] = (out_step2[ 3][ 4] > 0) ? out_step2[ 3][ 4] : 0;
out_step3[ 3][ 5] = (out_step2[ 3][ 5] > 0) ? out_step2[ 3][ 5] : 0;
out_step3[ 3][ 6] = (out_step2[ 3][ 6] > 0) ? out_step2[ 3][ 6] : 0;
out_step3[ 3][ 7] = (out_step2[ 3][ 7] > 0) ? out_step2[ 3][ 7] : 0;
out_step3[ 3][ 8] = (out_step2[ 3][ 8] > 0) ? out_step2[ 3][ 8] : 0;
out_step3[ 3][ 9] = (out_step2[ 3][ 9] > 0) ? out_step2[ 3][ 9] : 0;
out_step3[ 3][10] = (out_step2[ 3][10] > 0) ? out_step2[ 3][10] : 0;
out_step3[ 3][11] = (out_step2[ 3][11] > 0) ? out_step2[ 3][11] : 0;
out_step3[ 3][12] = (out_step2[ 3][12] > 0) ? out_step2[ 3][12] : 0;
out_step3[ 3][13] = (out_step2[ 3][13] > 0) ? out_step2[ 3][13] : 0;
out_step3[ 4][ 0] = (out_step2[ 4][ 0] > 0) ? out_step2[ 4][ 0] : 0;
out_step3[ 4][ 1] = (out_step2[ 4][ 1] > 0) ? out_step2[ 4][ 1] : 0;
out_step3[ 4][ 2] = (out_step2[ 4][ 2] > 0) ? out_step2[ 4][ 2] : 0;
out_step3[ 4][ 3] = (out_step2[ 4][ 3] > 0) ? out_step2[ 4][ 3] : 0;
out_step3[ 4][ 4] = (out_step2[ 4][ 4] > 0) ? out_step2[ 4][ 4] : 0;
out_step3[ 4][ 5] = (out_step2[ 4][ 5] > 0) ? out_step2[ 4][ 5] : 0;
out_step3[ 4][ 6] = (out_step2[ 4][ 6] > 0) ? out_step2[ 4][ 6] : 0;
out_step3[ 4][ 7] = (out_step2[ 4][ 7] > 0) ? out_step2[ 4][ 7] : 0;
out_step3[ 4][ 8] = (out_step2[ 4][ 8] > 0) ? out_step2[ 4][ 8] : 0;
out_step3[ 4][ 9] = (out_step2[ 4][ 9] > 0) ? out_step2[ 4][ 9] : 0;
out_step3[ 4][10] = (out_step2[ 4][10] > 0) ? out_step2[ 4][10] : 0;
out_step3[ 4][11] = (out_step2[ 4][11] > 0) ? out_step2[ 4][11] : 0;
out_step3[ 4][12] = (out_step2[ 4][12] > 0) ? out_step2[ 4][12] : 0;
out_step3[ 4][13] = (out_step2[ 4][13] > 0) ? out_step2[ 4][13] : 0;
out_step3[ 5][ 0] = (out_step2[ 5][ 0] > 0) ? out_step2[ 5][ 0] : 0;
out_step3[ 5][ 1] = (out_step2[ 5][ 1] > 0) ? out_step2[ 5][ 1] : 0;
out_step3[ 5][ 2] = (out_step2[ 5][ 2] > 0) ? out_step2[ 5][ 2] : 0;
out_step3[ 5][ 3] = (out_step2[ 5][ 3] > 0) ? out_step2[ 5][ 3] : 0;
out_step3[ 5][ 4] = (out_step2[ 5][ 4] > 0) ? out_step2[ 5][ 4] : 0;
out_step3[ 5][ 5] = (out_step2[ 5][ 5] > 0) ? out_step2[ 5][ 5] : 0;
out_step3[ 5][ 6] = (out_step2[ 5][ 6] > 0) ? out_step2[ 5][ 6] : 0;
out_step3[ 5][ 7] = (out_step2[ 5][ 7] > 0) ? out_step2[ 5][ 7] : 0;
out_step3[ 5][ 8] = (out_step2[ 5][ 8] > 0) ? out_step2[ 5][ 8] : 0;
out_step3[ 5][ 9] = (out_step2[ 5][ 9] > 0) ? out_step2[ 5][ 9] : 0;
out_step3[ 5][10] = (out_step2[ 5][10] > 0) ? out_step2[ 5][10] : 0;
out_step3[ 5][11] = (out_step2[ 5][11] > 0) ? out_step2[ 5][11] : 0;
out_step3[ 5][12] = (out_step2[ 5][12] > 0) ? out_step2[ 5][12] : 0;
out_step3[ 5][13] = (out_step2[ 5][13] > 0) ? out_step2[ 5][13] : 0;
out_step3[ 6][ 0] = (out_step2[ 6][ 0] > 0) ? out_step2[ 6][ 0] : 0;
out_step3[ 6][ 1] = (out_step2[ 6][ 1] > 0) ? out_step2[ 6][ 1] : 0;
out_step3[ 6][ 2] = (out_step2[ 6][ 2] > 0) ? out_step2[ 6][ 2] : 0;
out_step3[ 6][ 3] = (out_step2[ 6][ 3] > 0) ? out_step2[ 6][ 3] : 0;
out_step3[ 6][ 4] = (out_step2[ 6][ 4] > 0) ? out_step2[ 6][ 4] : 0;
out_step3[ 6][ 5] = (out_step2[ 6][ 5] > 0) ? out_step2[ 6][ 5] : 0;
out_step3[ 6][ 6] = (out_step2[ 6][ 6] > 0) ? out_step2[ 6][ 6] : 0;
out_step3[ 6][ 7] = (out_step2[ 6][ 7] > 0) ? out_step2[ 6][ 7] : 0;
out_step3[ 6][ 8] = (out_step2[ 6][ 8] > 0) ? out_step2[ 6][ 8] : 0;
out_step3[ 6][ 9] = (out_step2[ 6][ 9] > 0) ? out_step2[ 6][ 9] : 0;
out_step3[ 6][10] = (out_step2[ 6][10] > 0) ? out_step2[ 6][10] : 0;
out_step3[ 6][11] = (out_step2[ 6][11] > 0) ? out_step2[ 6][11] : 0;
out_step3[ 6][12] = (out_step2[ 6][12] > 0) ? out_step2[ 6][12] : 0;
out_step3[ 6][13] = (out_step2[ 6][13] > 0) ? out_step2[ 6][13] : 0;
out_step3[ 7][ 0] = (out_step2[ 7][ 0] > 0) ? out_step2[ 7][ 0] : 0;
out_step3[ 7][ 1] = (out_step2[ 7][ 1] > 0) ? out_step2[ 7][ 1] : 0;
out_step3[ 7][ 2] = (out_step2[ 7][ 2] > 0) ? out_step2[ 7][ 2] : 0;
out_step3[ 7][ 3] = (out_step2[ 7][ 3] > 0) ? out_step2[ 7][ 3] : 0;
out_step3[ 7][ 4] = (out_step2[ 7][ 4] > 0) ? out_step2[ 7][ 4] : 0;
out_step3[ 7][ 5] = (out_step2[ 7][ 5] > 0) ? out_step2[ 7][ 5] : 0;
out_step3[ 7][ 6] = (out_step2[ 7][ 6] > 0) ? out_step2[ 7][ 6] : 0;
out_step3[ 7][ 7] = (out_step2[ 7][ 7] > 0) ? out_step2[ 7][ 7] : 0;
out_step3[ 7][ 8] = (out_step2[ 7][ 8] > 0) ? out_step2[ 7][ 8] : 0;
out_step3[ 7][ 9] = (out_step2[ 7][ 9] > 0) ? out_step2[ 7][ 9] : 0;
out_step3[ 7][10] = (out_step2[ 7][10] > 0) ? out_step2[ 7][10] : 0;
out_step3[ 7][11] = (out_step2[ 7][11] > 0) ? out_step2[ 7][11] : 0;
out_step3[ 7][12] = (out_step2[ 7][12] > 0) ? out_step2[ 7][12] : 0;
out_step3[ 7][13] = (out_step2[ 7][13] > 0) ? out_step2[ 7][13] : 0;
out_step3[ 8][ 0] = (out_step2[ 8][ 0] > 0) ? out_step2[ 8][ 0] : 0;
out_step3[ 8][ 1] = (out_step2[ 8][ 1] > 0) ? out_step2[ 8][ 1] : 0;
out_step3[ 8][ 2] = (out_step2[ 8][ 2] > 0) ? out_step2[ 8][ 2] : 0;
out_step3[ 8][ 3] = (out_step2[ 8][ 3] > 0) ? out_step2[ 8][ 3] : 0;
out_step3[ 8][ 4] = (out_step2[ 8][ 4] > 0) ? out_step2[ 8][ 4] : 0;
out_step3[ 8][ 5] = (out_step2[ 8][ 5] > 0) ? out_step2[ 8][ 5] : 0;
out_step3[ 8][ 6] = (out_step2[ 8][ 6] > 0) ? out_step2[ 8][ 6] : 0;
out_step3[ 8][ 7] = (out_step2[ 8][ 7] > 0) ? out_step2[ 8][ 7] : 0;
out_step3[ 8][ 8] = (out_step2[ 8][ 8] > 0) ? out_step2[ 8][ 8] : 0;
out_step3[ 8][ 9] = (out_step2[ 8][ 9] > 0) ? out_step2[ 8][ 9] : 0;
out_step3[ 8][10] = (out_step2[ 8][10] > 0) ? out_step2[ 8][10] : 0;
out_step3[ 8][11] = (out_step2[ 8][11] > 0) ? out_step2[ 8][11] : 0;
out_step3[ 8][12] = (out_step2[ 8][12] > 0) ? out_step2[ 8][12] : 0;
out_step3[ 8][13] = (out_step2[ 8][13] > 0) ? out_step2[ 8][13] : 0;
out_step3[ 9][ 0] = (out_step2[ 9][ 0] > 0) ? out_step2[ 9][ 0] : 0;
out_step3[ 9][ 1] = (out_step2[ 9][ 1] > 0) ? out_step2[ 9][ 1] : 0;
out_step3[ 9][ 2] = (out_step2[ 9][ 2] > 0) ? out_step2[ 9][ 2] : 0;
out_step3[ 9][ 3] = (out_step2[ 9][ 3] > 0) ? out_step2[ 9][ 3] : 0;
out_step3[ 9][ 4] = (out_step2[ 9][ 4] > 0) ? out_step2[ 9][ 4] : 0;
out_step3[ 9][ 5] = (out_step2[ 9][ 5] > 0) ? out_step2[ 9][ 5] : 0;
out_step3[ 9][ 6] = (out_step2[ 9][ 6] > 0) ? out_step2[ 9][ 6] : 0;
out_step3[ 9][ 7] = (out_step2[ 9][ 7] > 0) ? out_step2[ 9][ 7] : 0;
out_step3[ 9][ 8] = (out_step2[ 9][ 8] > 0) ? out_step2[ 9][ 8] : 0;
out_step3[ 9][ 9] = (out_step2[ 9][ 9] > 0) ? out_step2[ 9][ 9] : 0;
out_step3[ 9][10] = (out_step2[ 9][10] > 0) ? out_step2[ 9][10] : 0;
out_step3[ 9][11] = (out_step2[ 9][11] > 0) ? out_step2[ 9][11] : 0;
out_step3[ 9][12] = (out_step2[ 9][12] > 0) ? out_step2[ 9][12] : 0;
out_step3[ 9][13] = (out_step2[ 9][13] > 0) ? out_step2[ 9][13] : 0;
out_step3[10][ 0] = (out_step2[10][ 0] > 0) ? out_step2[10][ 0] : 0;
out_step3[10][ 1] = (out_step2[10][ 1] > 0) ? out_step2[10][ 1] : 0;
out_step3[10][ 2] = (out_step2[10][ 2] > 0) ? out_step2[10][ 2] : 0;
out_step3[10][ 3] = (out_step2[10][ 3] > 0) ? out_step2[10][ 3] : 0;
out_step3[10][ 4] = (out_step2[10][ 4] > 0) ? out_step2[10][ 4] : 0;
out_step3[10][ 5] = (out_step2[10][ 5] > 0) ? out_step2[10][ 5] : 0;
out_step3[10][ 6] = (out_step2[10][ 6] > 0) ? out_step2[10][ 6] : 0;
out_step3[10][ 7] = (out_step2[10][ 7] > 0) ? out_step2[10][ 7] : 0;
out_step3[10][ 8] = (out_step2[10][ 8] > 0) ? out_step2[10][ 8] : 0;
out_step3[10][ 9] = (out_step2[10][ 9] > 0) ? out_step2[10][ 9] : 0;
out_step3[10][10] = (out_step2[10][10] > 0) ? out_step2[10][10] : 0;
out_step3[10][11] = (out_step2[10][11] > 0) ? out_step2[10][11] : 0;
out_step3[10][12] = (out_step2[10][12] > 0) ? out_step2[10][12] : 0;
out_step3[10][13] = (out_step2[10][13] > 0) ? out_step2[10][13] : 0;
out_step3[11][ 0] = (out_step2[11][ 0] > 0) ? out_step2[11][ 0] : 0;
out_step3[11][ 1] = (out_step2[11][ 1] > 0) ? out_step2[11][ 1] : 0;
out_step3[11][ 2] = (out_step2[11][ 2] > 0) ? out_step2[11][ 2] : 0;
out_step3[11][ 3] = (out_step2[11][ 3] > 0) ? out_step2[11][ 3] : 0;
out_step3[11][ 4] = (out_step2[11][ 4] > 0) ? out_step2[11][ 4] : 0;
out_step3[11][ 5] = (out_step2[11][ 5] > 0) ? out_step2[11][ 5] : 0;
out_step3[11][ 6] = (out_step2[11][ 6] > 0) ? out_step2[11][ 6] : 0;
out_step3[11][ 7] = (out_step2[11][ 7] > 0) ? out_step2[11][ 7] : 0;
out_step3[11][ 8] = (out_step2[11][ 8] > 0) ? out_step2[11][ 8] : 0;
out_step3[11][ 9] = (out_step2[11][ 9] > 0) ? out_step2[11][ 9] : 0;
out_step3[11][10] = (out_step2[11][10] > 0) ? out_step2[11][10] : 0;
out_step3[11][11] = (out_step2[11][11] > 0) ? out_step2[11][11] : 0;
out_step3[11][12] = (out_step2[11][12] > 0) ? out_step2[11][12] : 0;
out_step3[11][13] = (out_step2[11][13] > 0) ? out_step2[11][13] : 0;
out_step3[12][ 0] = (out_step2[12][ 0] > 0) ? out_step2[12][ 0] : 0;
out_step3[12][ 1] = (out_step2[12][ 1] > 0) ? out_step2[12][ 1] : 0;
out_step3[12][ 2] = (out_step2[12][ 2] > 0) ? out_step2[12][ 2] : 0;
out_step3[12][ 3] = (out_step2[12][ 3] > 0) ? out_step2[12][ 3] : 0;
out_step3[12][ 4] = (out_step2[12][ 4] > 0) ? out_step2[12][ 4] : 0;
out_step3[12][ 5] = (out_step2[12][ 5] > 0) ? out_step2[12][ 5] : 0;
out_step3[12][ 6] = (out_step2[12][ 6] > 0) ? out_step2[12][ 6] : 0;
out_step3[12][ 7] = (out_step2[12][ 7] > 0) ? out_step2[12][ 7] : 0;
out_step3[12][ 8] = (out_step2[12][ 8] > 0) ? out_step2[12][ 8] : 0;
out_step3[12][ 9] = (out_step2[12][ 9] > 0) ? out_step2[12][ 9] : 0;
out_step3[12][10] = (out_step2[12][10] > 0) ? out_step2[12][10] : 0;
out_step3[12][11] = (out_step2[12][11] > 0) ? out_step2[12][11] : 0;
out_step3[12][12] = (out_step2[12][12] > 0) ? out_step2[12][12] : 0;
out_step3[12][13] = (out_step2[12][13] > 0) ? out_step2[12][13] : 0;
out_step3[13][ 0] = (out_step2[13][ 0] > 0) ? out_step2[13][ 0] : 0;
out_step3[13][ 1] = (out_step2[13][ 1] > 0) ? out_step2[13][ 1] : 0;
out_step3[13][ 2] = (out_step2[13][ 2] > 0) ? out_step2[13][ 2] : 0;
out_step3[13][ 3] = (out_step2[13][ 3] > 0) ? out_step2[13][ 3] : 0;
out_step3[13][ 4] = (out_step2[13][ 4] > 0) ? out_step2[13][ 4] : 0;
out_step3[13][ 5] = (out_step2[13][ 5] > 0) ? out_step2[13][ 5] : 0;
out_step3[13][ 6] = (out_step2[13][ 6] > 0) ? out_step2[13][ 6] : 0;
out_step3[13][ 7] = (out_step2[13][ 7] > 0) ? out_step2[13][ 7] : 0;
out_step3[13][ 8] = (out_step2[13][ 8] > 0) ? out_step2[13][ 8] : 0;
out_step3[13][ 9] = (out_step2[13][ 9] > 0) ? out_step2[13][ 9] : 0;
out_step3[13][10] = (out_step2[13][10] > 0) ? out_step2[13][10] : 0;
out_step3[13][11] = (out_step2[13][11] > 0) ? out_step2[13][11] : 0;
out_step3[13][12] = (out_step2[13][12] > 0) ? out_step2[13][12] : 0;
out_step3[13][13] = (out_step2[13][13] > 0) ? out_step2[13][13] : 0;

// determine output for this fully connected step
out_step4[0][0] = 
   - 0.03336591 * out_step3[ 0][ 0] - 0.02325516 * out_step3[ 1][ 0] - 0.12105709 * out_step3[ 2][ 0] - 0.04534440 * out_step3[ 3][ 0]
   - 0.03547922 * out_step3[ 4][ 0] + 0.00866207 * out_step3[ 5][ 0] - 0.06342828 * out_step3[ 6][ 0] + 0.01803839 * out_step3[ 7][ 0]
   - 0.01498788 * out_step3[ 8][ 0] + 0.00611906 * out_step3[ 9][ 0] + 0.02133747 * out_step3[10][ 0] - 0.07483032 * out_step3[11][ 0]
   - 0.01877831 * out_step3[12][ 0] + 0.01198448 * out_step3[13][ 0] - 0.01618516 * out_step3[ 0][ 1] - 0.02896284 * out_step3[ 1][ 1]
   + 0.05064066 * out_step3[ 2][ 1] - 0.03787059 * out_step3[ 3][ 1] + 0.10141790 * out_step3[ 4][ 1] + 0.10300613 * out_step3[ 5][ 1]
   + 0.03796652 * out_step3[ 6][ 1] + 0.01526272 * out_step3[ 7][ 1] + 0.00743996 * out_step3[ 8][ 1] + 0.00490154 * out_step3[ 9][ 1]
   - 0.02582215 * out_step3[10][ 1] + 0.02945370 * out_step3[11][ 1] + 0.02757339 * out_step3[12][ 1] + 0.01714399 * out_step3[13][ 1]
   + 0.06783661 * out_step3[ 0][ 2] - 0.06478661 * out_step3[ 1][ 2] + 0.05835465 * out_step3[ 2][ 2] + 0.07610069 * out_step3[ 3][ 2]
   + 0.01655093 * out_step3[ 4][ 2] - 0.00526783 * out_step3[ 5][ 2] - 0.00461301 * out_step3[ 6][ 2] + 0.06376541 * out_step3[ 7][ 2]
   + 0.03655368 * out_step3[ 8][ 2] + 0.06145429 * out_step3[ 9][ 2] - 0.01446019 * out_step3[10][ 2] + 0.01744191 * out_step3[11][ 2]
   + 0.05604537 * out_step3[12][ 2] - 0.01885433 * out_step3[13][ 2] - 0.03169328 * out_step3[ 0][ 3] + 0.03278098 * out_step3[ 1][ 3]
   + 0.07828612 * out_step3[ 2][ 3] + 0.07614708 * out_step3[ 3][ 3] + 0.01780646 * out_step3[ 4][ 3] - 0.00462223 * out_step3[ 5][ 3]
   + 0.06531325 * out_step3[ 6][ 3] + 0.05618798 * out_step3[ 7][ 3] + 0.00852917 * out_step3[ 8][ 3] + 0.03515044 * out_step3[ 9][ 3]
   - 0.06406964 * out_step3[10][ 3] - 0.01028153 * out_step3[11][ 3] + 0.01025952 * out_step3[12][ 3] - 0.02059528 * out_step3[13][ 3]
   + 0.05800336 * out_step3[ 0][ 4] + 0.04613731 * out_step3[ 1][ 4] + 0.03665877 * out_step3[ 2][ 4] - 0.00524032 * out_step3[ 3][ 4]
   + 0.03239070 * out_step3[ 4][ 4] + 0.01028684 * out_step3[ 5][ 4] + 0.04217515 * out_step3[ 6][ 4] - 0.05242280 * out_step3[ 7][ 4]
   + 0.04197036 * out_step3[ 8][ 4] + 0.01642611 * out_step3[ 9][ 4] + 0.00199332 * out_step3[10][ 4] + 0.02741295 * out_step3[11][ 4]
   + 0.01159924 * out_step3[12][ 4] - 0.07735870 * out_step3[13][ 4] - 0.00441207 * out_step3[ 0][ 5] - 0.02351503 * out_step3[ 1][ 5]
   + 0.02344317 * out_step3[ 2][ 5] - 0.00562011 * out_step3[ 3][ 5] + 0.03967815 * out_step3[ 4][ 5] - 0.07049017 * out_step3[ 5][ 5]
   + 0.03042419 * out_step3[ 6][ 5] - 0.06983824 * out_step3[ 7][ 5] - 0.02553785 * out_step3[ 8][ 5] + 0.00019677 * out_step3[ 9][ 5]
   + 0.00506797 * out_step3[10][ 5] - 0.04062985 * out_step3[11][ 5] + 0.00306727 * out_step3[12][ 5] - 0.02751856 * out_step3[13][ 5]
   + 0.07413442 * out_step3[ 0][ 6] - 0.02026912 * out_step3[ 1][ 6] + 0.06586147 * out_step3[ 2][ 6] + 0.05969341 * out_step3[ 3][ 6]
   + 0.10746769 * out_step3[ 4][ 6] - 0.04590751 * out_step3[ 5][ 6] + 0.01294855 * out_step3[ 6][ 6] - 0.02760603 * out_step3[ 7][ 6]
   - 0.07795448 * out_step3[ 8][ 6] + 0.02212426 * out_step3[ 9][ 6] - 0.07102582 * out_step3[10][ 6] - 0.09073550 * out_step3[11][ 6]
   + 0.05245681 * out_step3[12][ 6] - 0.07888416 * out_step3[13][ 6] - 0.06807664 * out_step3[ 0][ 7] - 0.01234203 * out_step3[ 1][ 7]
   + 0.07325999 * out_step3[ 2][ 7] + 0.07712540 * out_step3[ 3][ 7] + 0.02168201 * out_step3[ 4][ 7] + 0.01285801 * out_step3[ 5][ 7]
   - 0.07548288 * out_step3[ 6][ 7] + 0.01632245 * out_step3[ 7][ 7] - 0.09596946 * out_step3[ 8][ 7] - 0.04708050 * out_step3[ 9][ 7]
   + 0.04102194 * out_step3[10][ 7] - 0.01925552 * out_step3[11][ 7] - 0.03598686 * out_step3[12][ 7] - 0.04461047 * out_step3[13][ 7]
   - 0.01866007 * out_step3[ 0][ 8] + 0.10944767 * out_step3[ 1][ 8] - 0.02296875 * out_step3[ 2][ 8] + 0.05083752 * out_step3[ 3][ 8]
   + 0.04543770 * out_step3[ 4][ 8] - 0.03447407 * out_step3[ 5][ 8] + 0.02472430 * out_step3[ 6][ 8] - 0.04503316 * out_step3[ 7][ 8]
   + 0.01296419 * out_step3[ 8][ 8] + 0.01466487 * out_step3[ 9][ 8] + 0.00198217 * out_step3[10][ 8] + 0.04380584 * out_step3[11][ 8]
   + 0.05644375 * out_step3[12][ 8] + 0.05470167 * out_step3[13][ 8] - 0.04862870 * out_step3[ 0][ 9] + 0.04155042 * out_step3[ 1][ 9]
   - 0.01207306 * out_step3[ 2][ 9] + 0.04620437 * out_step3[ 3][ 9] + 0.08142418 * out_step3[ 4][ 9] + 0.04694096 * out_step3[ 5][ 9]
   + 0.02021865 * out_step3[ 6][ 9] + 0.03130104 * out_step3[ 7][ 9] + 0.01284675 * out_step3[ 8][ 9] - 0.03644331 * out_step3[ 9][ 9]
   + 0.01361564 * out_step3[10][ 9] + 0.00713713 * out_step3[11][ 9] + 0.02453433 * out_step3[12][ 9] - 0.06476215 * out_step3[13][ 9]
   - 0.03864865 * out_step3[ 0][10] + 0.01167559 * out_step3[ 1][10] + 0.01469047 * out_step3[ 2][10] + 0.01850416 * out_step3[ 3][10]
   + 0.11022507 * out_step3[ 4][10] - 0.00403061 * out_step3[ 5][10] - 0.04177484 * out_step3[ 6][10] - 0.06191868 * out_step3[ 7][10]
   - 0.00672799 * out_step3[ 8][10] + 0.03913474 * out_step3[ 9][10] - 0.05905705 * out_step3[10][10] + 0.06105304 * out_step3[11][10]
   - 0.04654726 * out_step3[12][10] - 0.04690268 * out_step3[13][10] - 0.05419086 * out_step3[ 0][11] - 0.11213516 * out_step3[ 1][11]
   + 0.03486596 * out_step3[ 2][11] - 0.02372439 * out_step3[ 3][11] + 0.07924570 * out_step3[ 4][11] + 0.04058645 * out_step3[ 5][11]
   + 0.04823261 * out_step3[ 6][11] + 0.07052492 * out_step3[ 7][11] + 0.04422250 * out_step3[ 8][11] - 0.00384214 * out_step3[ 9][11]
   - 0.01742649 * out_step3[10][11] - 0.00780718 * out_step3[11][11] - 0.04819882 * out_step3[12][11] - 0.08775666 * out_step3[13][11]
   - 0.05632878 * out_step3[ 0][12] - 0.00223449 * out_step3[ 1][12] + 0.02468692 * out_step3[ 2][12] - 0.06180303 * out_step3[ 3][12]
   - 0.03389818 * out_step3[ 4][12] - 0.03972032 * out_step3[ 5][12] - 0.03132495 * out_step3[ 6][12] + 0.04544247 * out_step3[ 7][12]
   + 0.01519446 * out_step3[ 8][12] + 0.03819647 * out_step3[ 9][12] + 0.02444332 * out_step3[10][12] + 0.02148645 * out_step3[11][12]
   - 0.00229122 * out_step3[12][12] - 0.01803391 * out_step3[13][12] - 0.05053189 * out_step3[ 0][13] - 0.00812195 * out_step3[ 1][13]
   - 0.11317353 * out_step3[ 2][13] - 0.10135447 * out_step3[ 3][13] - 0.06175981 * out_step3[ 4][13] + 0.00387141 * out_step3[ 5][13]
   - 0.09559490 * out_step3[ 6][13] - 0.00034119 * out_step3[ 7][13] + 0.02512987 * out_step3[ 8][13] - 0.08169007 * out_step3[ 9][13]
   - 0.02195470 * out_step3[10][13] - 0.00315784 * out_step3[11][13] + 0.02057242 * out_step3[12][13] - 0.07564373 * out_step3[13][13]
   - 0.25387558;

out_step4[1][0] = 
   + 0.02975832 * out_step3[ 0][ 0] + 0.03589328 * out_step3[ 1][ 0] + 0.10776548 * out_step3[ 2][ 0] + 0.04239635 * out_step3[ 3][ 0]
   + 0.05239161 * out_step3[ 4][ 0] - 0.01503415 * out_step3[ 5][ 0] + 0.03756517 * out_step3[ 6][ 0] - 0.01781407 * out_step3[ 7][ 0]
   + 0.03449496 * out_step3[ 8][ 0] - 0.01519396 * out_step3[ 9][ 0] - 0.00457017 * out_step3[10][ 0] + 0.04549729 * out_step3[11][ 0]
   + 0.01360906 * out_step3[12][ 0] - 0.02634277 * out_step3[13][ 0] + 0.00416781 * out_step3[ 0][ 1] + 0.02480890 * out_step3[ 1][ 1]
   - 0.05906149 * out_step3[ 2][ 1] + 0.03734747 * out_step3[ 3][ 1] - 0.09806990 * out_step3[ 4][ 1] - 0.09318617 * out_step3[ 5][ 1]
   - 0.02143970 * out_step3[ 6][ 1] - 0.03484919 * out_step3[ 7][ 1] + 0.02077156 * out_step3[ 8][ 1] + 0.01186369 * out_step3[ 9][ 1]
   + 0.02680589 * out_step3[10][ 1] - 0.02663168 * out_step3[11][ 1] - 0.03652284 * out_step3[12][ 1] - 0.00296591 * out_step3[13][ 1]
   - 0.07384368 * out_step3[ 0][ 2] + 0.08414046 * out_step3[ 1][ 2] - 0.05473005 * out_step3[ 2][ 2] - 0.08477516 * out_step3[ 3][ 2]
   - 0.01302181 * out_step3[ 4][ 2] + 0.02079068 * out_step3[ 5][ 2] - 0.01648733 * out_step3[ 6][ 2] - 0.07630613 * out_step3[ 7][ 2]
   - 0.03532451 * out_step3[ 8][ 2] - 0.04499120 * out_step3[ 9][ 2] + 0.01729709 * out_step3[10][ 2] - 0.02025643 * out_step3[11][ 2]
   - 0.07253364 * out_step3[12][ 2] + 0.02004046 * out_step3[13][ 2] + 0.04010371 * out_step3[ 0][ 3] - 0.04321144 * out_step3[ 1][ 3]
   - 0.08695387 * out_step3[ 2][ 3] - 0.07032674 * out_step3[ 3][ 3] - 0.00154767 * out_step3[ 4][ 3] - 0.00212968 * out_step3[ 5][ 3]
   - 0.05576752 * out_step3[ 6][ 3] - 0.06471778 * out_step3[ 7][ 3] - 0.00087649 * out_step3[ 8][ 3] - 0.01119206 * out_step3[ 9][ 3]
   + 0.07458796 * out_step3[10][ 3] - 0.01789855 * out_step3[11][ 3] - 0.00982520 * out_step3[12][ 3] + 0.02173833 * out_step3[13][ 3]
   - 0.04445776 * out_step3[ 0][ 4] - 0.02572627 * out_step3[ 1][ 4] - 0.01660992 * out_step3[ 2][ 4] + 0.01961609 * out_step3[ 3][ 4]
   - 0.03834381 * out_step3[ 4][ 4] - 0.01489826 * out_step3[ 5][ 4] - 0.04266727 * out_step3[ 6][ 4] + 0.05722321 * out_step3[ 7][ 4]
   - 0.04033323 * out_step3[ 8][ 4] + 0.00012421 * out_step3[ 9][ 4] - 0.02023107 * out_step3[10][ 4] - 0.00429306 * out_step3[11][ 4]
   - 0.01182545 * out_step3[12][ 4] + 0.06805842 * out_step3[13][ 4] + 0.01761909 * out_step3[ 0][ 5] + 0.02287347 * out_step3[ 1][ 5]
   - 0.01530656 * out_step3[ 2][ 5] + 0.00092267 * out_step3[ 3][ 5] - 0.05640272 * out_step3[ 4][ 5] + 0.06667810 * out_step3[ 5][ 5]
   - 0.04135128 * out_step3[ 6][ 5] + 0.07050373 * out_step3[ 7][ 5] + 0.00640085 * out_step3[ 8][ 5] - 0.00831426 * out_step3[ 9][ 5]
   - 0.00185266 * out_step3[10][ 5] + 0.05362444 * out_step3[11][ 5] + 0.00711274 * out_step3[12][ 5] + 0.03090644 * out_step3[13][ 5]
   - 0.05457315 * out_step3[ 0][ 6] - 0.00220147 * out_step3[ 1][ 6] - 0.05778651 * out_step3[ 2][ 6] - 0.08668954 * out_step3[ 3][ 6]
   - 0.08997142 * out_step3[ 4][ 6] + 0.05598419 * out_step3[ 5][ 6] - 0.01208994 * out_step3[ 6][ 6] + 0.02004997 * out_step3[ 7][ 6]
   + 0.06927241 * out_step3[ 8][ 6] - 0.03507007 * out_step3[ 9][ 6] + 0.09549789 * out_step3[10][ 6] + 0.07376146 * out_step3[11][ 6]
   - 0.03329099 * out_step3[12][ 6] + 0.06787375 * out_step3[13][ 6] + 0.07799705 * out_step3[ 0][ 7] + 0.00242088 * out_step3[ 1][ 7]
   - 0.07720615 * out_step3[ 2][ 7] - 0.07524730 * out_step3[ 3][ 7] - 0.03744585 * out_step3[ 4][ 7] - 0.02355212 * out_step3[ 5][ 7]
   + 0.05979412 * out_step3[ 6][ 7] - 0.00687716 * out_step3[ 7][ 7] + 0.10056350 * out_step3[ 8][ 7] + 0.05514842 * out_step3[ 9][ 7]
   - 0.05571279 * out_step3[10][ 7] + 0.01430163 * out_step3[11][ 7] + 0.04185321 * out_step3[12][ 7] + 0.05491559 * out_step3[13][ 7]
   + 0.01036141 * out_step3[ 0][ 8] - 0.09527688 * out_step3[ 1][ 8] + 0.00504399 * out_step3[ 2][ 8] - 0.02057003 * out_step3[ 3][ 8]
   - 0.03217699 * out_step3[ 4][ 8] + 0.04060709 * out_step3[ 5][ 8] + 0.01087005 * out_step3[ 6][ 8] + 0.03848815 * out_step3[ 7][ 8]
   - 0.00974287 * out_step3[ 8][ 8] - 0.01382185 * out_step3[ 9][ 8] + 0.00605501 * out_step3[10][ 8] - 0.06384061 * out_step3[11][ 8]
   - 0.06176941 * out_step3[12][ 8] - 0.05072620 * out_step3[13][ 8] + 0.03818783 * out_step3[ 0][ 9] - 0.04382310 * out_step3[ 1][ 9]
   + 0.00239683 * out_step3[ 2][ 9] - 0.02067773 * out_step3[ 3][ 9] - 0.06202119 * out_step3[ 4][ 9] - 0.05287086 * out_step3[ 5][ 9]
   - 0.03706156 * out_step3[ 6][ 9] - 0.02269542 * out_step3[ 7][ 9] - 0.00912425 * out_step3[ 8][ 9] + 0.00885934 * out_step3[ 9][ 9]
   - 0.02489990 * out_step3[10][ 9] + 0.02077889 * out_step3[11][ 9] - 0.01787323 * out_step3[12][ 9] + 0.04602303 * out_step3[13][ 9]
   + 0.04450347 * out_step3[ 0][10] - 0.01250134 * out_step3[ 1][10] - 0.01451723 * out_step3[ 2][10] - 0.01769516 * out_step3[ 3][10]
   - 0.10653573 * out_step3[ 4][10] + 0.02475655 * out_step3[ 5][10] + 0.04377123 * out_step3[ 6][10] + 0.06083157 * out_step3[ 7][10]
   - 0.00757726 * out_step3[ 8][10] - 0.05881472 * out_step3[ 9][10] + 0.06471855 * out_step3[10][10] - 0.05681210 * out_step3[11][10]
   + 0.04530618 * out_step3[12][10] + 0.04731765 * out_step3[13][10] + 0.04459693 * out_step3[ 0][11] + 0.10853466 * out_step3[ 1][11]
   - 0.01313989 * out_step3[ 2][11] + 0.04209728 * out_step3[ 3][11] - 0.07266121 * out_step3[ 4][11] - 0.05094980 * out_step3[ 5][11]
   - 0.04229941 * out_step3[ 6][11] - 0.08199473 * out_step3[ 7][11] - 0.03620228 * out_step3[ 8][11] - 0.02022579 * out_step3[ 9][11]
   + 0.02142653 * out_step3[10][11] + 0.00476125 * out_step3[11][11] + 0.03206272 * out_step3[12][11] + 0.08239815 * out_step3[13][11]
   + 0.04887506 * out_step3[ 0][12] + 0.03884654 * out_step3[ 1][12] - 0.00741941 * out_step3[ 2][12] + 0.05611546 * out_step3[ 3][12]
   + 0.03177745 * out_step3[ 4][12] + 0.03349658 * out_step3[ 5][12] + 0.02319003 * out_step3[ 6][12] - 0.06975812 * out_step3[ 7][12]
   - 0.00310838 * out_step3[ 8][12] - 0.00310859 * out_step3[ 9][12] - 0.04383368 * out_step3[10][12] - 0.02024104 * out_step3[11][12]
   + 0.01500400 * out_step3[12][12] + 0.01739977 * out_step3[13][12] + 0.04575537 * out_step3[ 0][13] + 0.01738844 * out_step3[ 1][13]
   + 0.10018238 * out_step3[ 2][13] + 0.10947301 * out_step3[ 3][13] + 0.07463653 * out_step3[ 4][13] - 0.03470362 * out_step3[ 5][13]
   + 0.11066878 * out_step3[ 6][13] + 0.00853612 * out_step3[ 7][13] - 0.01686093 * out_step3[ 8][13] + 0.08714316 * out_step3[ 9][13]
   + 0.04184404 * out_step3[10][13] + 0.02491191 * out_step3[11][13] - 0.00024464 * out_step3[12][13] + 0.06216197 * out_step3[13][13]
   + 0.25387448;


// determine maximum
double x_max = std::numeric_limits<double>::min();
for ( int i = 0; i < 2; i++){
	for ( int j = 0; j < 1; j++){
		if (out_step4[i][j] > x_max){
			x_max = out_step4[i][j];
		}
	}
}

// subtract max, use as exponent and sum up
double sum = 0;
for ( int i = 0; i < 2; i++){
	for ( int j = 0; j < 1; j++){
		out_step5[i][j] = exp(out_step4[i][j] - x_max);
		sum += out_step5[i][j]; 
	}
}

// normalize
for ( int i = 0; i < 2; i++){
	for ( int j = 0; j < 1; j++){
		out_step5[i][j] /= sum;
	}
}

// return classification
return out_step5[0][0] > out_step5[1][0];
}

