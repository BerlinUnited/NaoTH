#include "Fy4000_3.h"

#include <emmintrin.h>
#include <math.h>
void Fy4000_3::cnn(float x0[16][16][1])
{
	__m128 w, x, y;
	for (int xi = 0; xi < 16; xi += 1)
	{
	x0[xi][0][0] = (x0[xi][0][0] - 0.000000f);
	x0[xi][1][0] = (x0[xi][1][0] - 0.000000f);
	x0[xi][2][0] = (x0[xi][2][0] - 0.000000f);
	x0[xi][3][0] = (x0[xi][3][0] - 0.000000f);
	x0[xi][4][0] = (x0[xi][4][0] - 0.000000f);
	x0[xi][5][0] = (x0[xi][5][0] - 0.000000f);
	x0[xi][6][0] = (x0[xi][6][0] - 0.000000f);
	x0[xi][7][0] = (x0[xi][7][0] - 0.000000f);
	x0[xi][8][0] = (x0[xi][8][0] - 0.000000f);
	x0[xi][9][0] = (x0[xi][9][0] - 0.000000f);
	x0[xi][10][0] = (x0[xi][10][0] - 0.000000f);
	x0[xi][11][0] = (x0[xi][11][0] - 0.000000f);
	x0[xi][12][0] = (x0[xi][12][0] - 0.000000f);
	x0[xi][13][0] = (x0[xi][13][0] - 0.000000f);
	x0[xi][14][0] = (x0[xi][14][0] - 0.000000f);
	x0[xi][15][0] = (x0[xi][15][0] - 0.000000f);
	}

 	// Convolution Layer
	static float x1 alignas(16) [16][16][4] = {0};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x1[i][j][0] = -0.07160348445177078f;
			x1[i][j][1] = -0.0006150354165583849f;
			x1[i][j][2] = 0.08522763848304749f;
			x1[i][j][3] = 0.01659955456852913f;
		}
	}
	for (int ix = -1; ix < 15; ix += 1)
	{
		int x_1, x_out_1;
		x_out_1 = (ix + 1) / 1;
		for (int jx = -1; jx < 15; jx += 1)
		{
			int x_2, x_out_2;
			x_out_2 = (jx + 1) / 1;
			x_1 = ix + 0;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.2667440176010132f, 0.4100657105445862f, 0.5856022238731384f, 0.336677223443985f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.0011125734308734536f, -0.6071451306343079f, 0.03805125877261162f, -0.26199498772621155f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.16027481853961945f, -0.5917308330535889f, 0.10990747064352036f, -0.224086195230484f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.20574906468391418f, 0.31702542304992676f, -0.12489445507526398f, -0.8735864758491516f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.17939677834510803f, -0.4138528108596802f, -0.45538684725761414f, 0.09528034180402756f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.2378736436367035f, 0.23510415852069855f, 0.017664095386862755f, 0.24688206613063812f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.26679617166519165f, 0.24163922667503357f, 0.14082932472229004f, 0.5002533793449402f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.6395028233528137f, 0.00944527518004179f, -0.18225769698619843f, 0.49516305327415466f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.556580126285553f, 0.3862573504447937f, -0.3034367263317108f, -0.25983309745788574f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
			}
		}
	}

 	// Leaky ReLu Layer
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{

			x = _mm_load_ps((float*)&x1[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x1[i][j][0], x);
		}
	}

 	// Convolution Layer
	static float x2 alignas(16) [16][16][8] = {0};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x2[i][j][0] = 0.0671834722161293f;
			x2[i][j][1] = 0.06121699512004852f;
			x2[i][j][2] = -0.04724065586924553f;
			x2[i][j][3] = 0.03257646784186363f;
			x2[i][j][4] = -0.3133329749107361f;
			x2[i][j][5] = -0.02087446302175522f;
			x2[i][j][6] = -0.035753730684518814f;
			x2[i][j][7] = 0.026394899934530258f;
		}
	}
	for (int ix = -1; ix < 15; ix += 1)
	{
		int x_1, x_out_1;
		x_out_1 = (ix + 1) / 1;
		for (int jx = -1; jx < 15; jx += 1)
		{
			int x_2, x_out_2;
			x_out_2 = (jx + 1) / 1;
			x_1 = ix + 0;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.17596763372421265f, 0.313097208738327f, 0.01900511048734188f, -0.10006792843341827f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1900034099817276f, 0.34909164905548096f, -0.7448179125785828f, 0.03323505073785782f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.17016471922397614f, -0.3800763189792633f, -0.14004532992839813f, 0.43245065212249756f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.48359414935112f, -0.5712310075759888f, 0.0652160570025444f, -0.06316769868135452f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6561954617500305f, -0.5107333064079285f, -0.14000500738620758f, -0.3773806691169739f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.672709584236145f, 0.19030338525772095f, -0.7883335947990417f, 0.1143096312880516f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.654394268989563f, 0.26356109976768494f, 0.26222896575927734f, 0.3897606432437897f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04161311686038971f, 0.3645418584346771f, -0.06599397957324982f, -0.19686338305473328f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.6197640299797058f, 0.01673051156103611f, 0.3107462525367737f, 0.011538592167198658f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2276974469423294f, 0.4041750729084015f, -0.07596690952777863f, -0.4191889762878418f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.055637963116168976f, -0.3699896037578583f, -0.23021985590457916f, 0.11643768846988678f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5217104554176331f, -0.7798619866371155f, 0.37971773743629456f, 0.5823628306388855f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6899532675743103f, -0.6092868447303772f, 0.1504230499267578f, -0.5445528030395508f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.46113866567611694f, 0.007022310979664326f, -0.9981273412704468f, 0.41452673077583313f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6330778002738953f, 0.6150475144386292f, 0.321220338344574f, -0.11834225803613663f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7621889114379883f, -0.13435448706150055f, 0.31625300645828247f, -0.7881188988685608f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.16674046218395233f, 0.5950223207473755f, -0.1250627040863037f, -0.10148382931947708f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.25098225474357605f, 0.1763969361782074f, -0.027267970144748688f, -0.38076311349868774f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.23243853449821472f, -0.14133571088314056f, -0.5909786820411682f, 0.5268844962120056f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.208759143948555f, 0.13208183646202087f, -0.5130669474601746f, 0.2224481701850891f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7483475804328918f, -0.3766779601573944f, -0.07568853348493576f, -0.6882567405700684f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1940121352672577f, 0.09344009310007095f, -0.5690820813179016f, 0.16575799882411957f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.27722588181495667f, -0.111441470682621f, 0.40870410203933716f, 0.22737817466259003f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9072828888893127f, -0.04330440238118172f, 0.09727923572063446f, 0.2708906829357147f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.9033680558204651f, -0.30753403902053833f, 0.004220833536237478f, -0.12407511472702026f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7012909650802612f, 0.0884649008512497f, -0.13721556961536407f, 0.05596349760890007f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04875637963414192f, -0.06682878732681274f, -0.15323153138160706f, 0.3595231771469116f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.25314533710479736f, -0.3000725209712982f, 0.20360857248306274f, 0.11093071103096008f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0096008777618408f, -0.4022608697414398f, 0.3534810543060303f, 0.23446786403656006f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.37770405411720276f, 0.1284450739622116f, -0.20869988203048706f, 0.08424650132656097f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0848932266235352f, -0.33881163597106934f, 0.27644386887550354f, -0.9210313558578491f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3441210687160492f, 0.14098906517028809f, 0.6006445288658142f, 0.1300785392522812f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.7119947671890259f, -0.42663854360580444f, 0.1242183968424797f, -0.22570312023162842f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.37018144130706787f, 0.3985995054244995f, -0.6388224959373474f, 0.06075293570756912f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.26383641362190247f, 0.34322452545166016f, -0.0784086287021637f, -0.24479322135448456f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.28427809476852417f, 0.23402440547943115f, -0.03934328630566597f, 0.477997362613678f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.114373117685318f, 0.15457741916179657f, 0.5019602179527283f, -0.05882219970226288f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09814617037773132f, -0.2659141421318054f, -0.27575477957725525f, 0.30842673778533936f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.02894202060997486f, -0.22219417989253998f, 0.222966730594635f, -0.059315603226423264f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07727928459644318f, 0.09131915122270584f, 0.1850534826517105f, -0.49044591188430786f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.12364780157804489f, 0.7993136048316956f, -0.07909968495368958f, 0.03276832401752472f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5808965563774109f, 0.09949015825986862f, -0.8020893931388855f, -0.5364657640457153f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2798762023448944f, -0.7210460901260376f, -1.1013089418411255f, 0.22264200448989868f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.409358412027359f, 0.16881786286830902f, -0.6025198698043823f, 0.27123767137527466f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2753022611141205f, 0.46912702918052673f, -0.7260338068008423f, -0.302266389131546f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5632376670837402f, -0.026087960228323936f, -1.2746243476867676f, -0.01744695194065571f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1885392814874649f, 0.7654370069503784f, -0.6444779634475708f, 0.14084306359291077f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06737932562828064f, -0.008702971041202545f, 0.4031241238117218f, -0.21255429089069366f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.39371412992477417f, -0.5036669969558716f, 0.003883365308865905f, -0.1556825339794159f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0007959081558510661f, 0.07542864978313446f, -0.15509271621704102f, -0.03307223692536354f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.13378673791885376f, -0.5291649699211121f, -0.5014726519584656f, -0.5672128796577454f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3582174479961395f, -0.17592428624629974f, 0.12108084559440613f, 0.0438196137547493f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10240817815065384f, 0.2733655571937561f, 0.10687010735273361f, 0.39405378699302673f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08358048647642136f, 0.6750089526176453f, 0.32016313076019287f, -0.12071038037538528f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.49824634194374084f, -0.5346390604972839f, -0.5629690289497375f, -0.6044914722442627f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08408714830875397f, 0.007122205104678869f, -0.002031314419582486f, 0.07445066422224045f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.3165023624897003f, -0.4546952247619629f, -0.1939680427312851f, -0.2238091230392456f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.035910796374082565f, 0.13631941378116608f, -0.6116446852684021f, -0.2530367076396942f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.22164508700370789f, -0.08412551134824753f, -0.7237638235092163f, -0.3444879353046417f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4650834798812866f, -0.07135867327451706f, -0.06519286334514618f, 0.11221817135810852f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3733874559402466f, 0.09316670149564743f, -0.3128158152103424f, 0.2419213503599167f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9292014837265015f, -0.041705794632434845f, 0.22919176518917084f, -0.23339147865772247f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09949453175067902f, -0.8869825601577759f, -0.7381510734558105f, -0.3769019544124603f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09605687856674194f, 0.4661598801612854f, -0.046071458607912064f, -0.0387939028441906f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.1225811317563057f, -0.09450529515743256f, -0.06768537312746048f, -0.0752251073718071f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0908016562461853f, 0.3058546185493469f, -0.2705666124820709f, -0.2198599874973297f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4949055314064026f, 0.6130527853965759f, -1.2495877742767334f, -0.6407939791679382f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3812049627304077f, 0.12258727848529816f, -0.5174686312675476f, 0.09060369431972504f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.33536863327026367f, -0.06346172094345093f, -1.0300519466400146f, 0.09400098025798798f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2731039524078369f, -0.00546680623665452f, -1.0389920473098755f, 0.4195901155471802f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19083596765995026f, -0.8020511269569397f, -0.901893675327301f, -0.27640625834465027f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.32361721992492676f, 0.09714113920927048f, 0.26136595010757446f, 0.14969748258590698f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][4], x);
				}
			}
		}
	}

 	// Leaky ReLu Layer
	y = _mm_set_ps1(0.30000001192092896f);
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{

			x = _mm_load_ps((float*)&x2[i][j][0]);
			x = _mm_max_ps(x, _mm_mul_ps(y, x));
			_mm_store_ps((float*)&x2[i][j][0], x);

			x = _mm_load_ps((float*)&x2[i][j][4]);
			x = _mm_max_ps(x, _mm_mul_ps(y, x));
			_mm_store_ps((float*)&x2[i][j][4], x);
		}
	}

 	// Maxpool Layer 
	static float x3[8][8][8] = {0};
	for (int ix = 0; ix < 15; ix += 2)
	{
		int x_out_1;
		x_out_1 = ix / 2;
	for (int jx = 0; jx < 15; jx += 2)
	{
		int x_out_2;
		x_out_2 = jx / 2;
		x = _mm_load_ps((float*)&x2[ix][jx][0]);
		y = _mm_load_ps((float*)&x2[ix + 0][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x2[ix + 0][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x3[x_out_1][x_out_2][0], x);
		y = _mm_load_ps((float*)&x2[ix + 1][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x2[ix + 1][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x3[x_out_1][x_out_2][0], x);
		x = _mm_load_ps((float*)&x2[ix][jx][4]);
		y = _mm_load_ps((float*)&x2[ix + 0][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x2[ix + 0][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x3[x_out_1][x_out_2][4], x);
		y = _mm_load_ps((float*)&x2[ix + 1][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x2[ix + 1][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x3[x_out_1][x_out_2][4], x);
		}
	}

 	// Convolution Layer
	static float x4 alignas(16) [8][8][12] = {0};
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{
			x4[i][j][0] = -0.3801754117012024f;
			x4[i][j][1] = -0.5734128952026367f;
			x4[i][j][2] = -0.3542691767215729f;
			x4[i][j][3] = -0.09958124905824661f;
			x4[i][j][4] = 0.2431672066450119f;
			x4[i][j][5] = -0.5655804872512817f;
			x4[i][j][6] = 0.24554775655269623f;
			x4[i][j][7] = 0.025940969586372375f;
			x4[i][j][8] = -0.3459825813770294f;
			x4[i][j][9] = -0.41427409648895264f;
			x4[i][j][10] = -0.2223549783229828f;
			x4[i][j][11] = -0.05908186361193657f;
		}
	}
	for (int ix = -1; ix < 7; ix += 1)
	{
		int x_1, x_out_1;
		x_out_1 = (ix + 1) / 1;
		for (int jx = -1; jx < 7; jx += 1)
		{
			int x_2, x_out_2;
			x_out_2 = (jx + 1) / 1;
			x_1 = ix + 0;
			if (x_1 >= 0 && x_1 < 8)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.24083395302295685f, 0.43806779384613037f, -0.5445274710655212f, 0.49696964025497437f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4732327461242676f, -0.6862719058990479f, 0.4119747281074524f, 0.27329233288764954f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3512078523635864f, 0.020845521241426468f, -0.02059810794889927f, -0.012501813471317291f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.17691388726234436f, 0.028939057141542435f, -0.22616545855998993f, -0.12370369583368301f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5333437323570251f, 0.020865794271230698f, 0.5984050631523132f, -0.18038852512836456f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.30622991919517517f, 0.39253807067871094f, 0.26288941502571106f, 0.11469930410385132f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-1.9861228466033936f, 0.047682855278253555f, 0.3518659174442291f, 0.21890230476856232f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.38850903511047363f, 0.2892104983329773f, -0.5059725046157837f, -0.057609423995018005f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09577711671590805f, -0.3762073516845703f, -0.5712858438491821f, 0.47842705249786377f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.26547756791114807f, 0.697800874710083f, 1.172672152519226f, -0.3441280424594879f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.26024991273880005f, -0.12404978275299072f, 0.26008620858192444f, -0.18159759044647217f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.020629199221730232f, 0.05882372334599495f, 0.1513042449951172f, -0.007032054476439953f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.7976546287536621f, -0.9085035920143127f, -0.4931211471557617f, 0.10786871612071991f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09094415605068207f, -0.2864587604999542f, 0.14128831028938293f, 0.3212430775165558f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3273738920688629f, -0.5408360362052917f, 0.4236941635608673f, -0.2711979150772095f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.4009903371334076f, 0.8240772485733032f, 0.11343757808208466f, 0.25166237354278564f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.19489836692810059f, 0.10704413801431656f, 0.37871548533439636f, 0.2614470422267914f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21969546377658844f, -0.9851290583610535f, 0.25968995690345764f, -0.13659556210041046f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-1.3820502758026123f, -0.05528527870774269f, -0.194245383143425f, 0.3902924358844757f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2827658951282501f, -0.5712518095970154f, -0.0013528603594750166f, 0.07061434537172318f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03345143049955368f, -0.03454413264989853f, 0.15747444331645966f, -0.2867182195186615f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.8984876275062561f, 0.3133928179740906f, -0.4265517294406891f, 0.24428659677505493f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.018472440540790558f, 0.06324316561222076f, 0.5219916105270386f, 0.01201479509472847f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.24713990092277527f, -0.2647722363471985f, -0.2922430634498596f, 0.2686174213886261f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.2812719941139221f, -0.27605128288269043f, 0.6141355633735657f, 0.3023723065853119f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08302261680364609f, -0.748256266117096f, -0.05119071528315544f, 0.03467515856027603f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3248460590839386f, 0.15124927461147308f, -0.7539427876472473f, -0.12027913331985474f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.14577753841876984f, -0.4536486864089966f, 0.03919845074415207f, -0.1172771230340004f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7519360780715942f, -0.03086145594716072f, 0.6140300631523132f, -0.6486727595329285f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06391371786594391f, -0.42381906509399414f, 0.17409057915210724f, -0.3176029324531555f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-1.1553078889846802f, 0.1835368424654007f, -0.4556010961532593f, -0.19661056995391846f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05020276829600334f, 0.5796108841896057f, -0.05191096290946007f, 0.06075441092252731f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12619176506996155f, -0.3229679763317108f, -0.20298413932323456f, -0.03771679103374481f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.07078777998685837f, -0.4674164056777954f, -0.45551788806915283f, 0.1337207853794098f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.37824729084968567f, 0.10133910179138184f, -0.4140761196613312f, -0.6202916502952576f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2510943114757538f, -0.5557945966720581f, 0.6008775234222412f, 0.4918820858001709f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.6412723064422607f, 0.00018822553101927042f, -0.18903182446956635f, -0.0846652165055275f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0020438136998564005f, 0.25877276062965393f, -0.2674504518508911f, -0.9392728805541992f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1265825778245926f, 0.08400671929121017f, 0.3686763346195221f, -0.48064011335372925f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.4188429117202759f, -0.28723251819610596f, 0.3652963936328888f, -0.10841834545135498f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18129661679267883f, -0.035397350788116455f, 0.224665567278862f, -0.5415562391281128f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.379749059677124f, 0.2571422755718231f, -0.4829973578453064f, -0.22080424427986145f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.6462740898132324f, -0.3081829249858856f, -0.11821699142456055f, -0.48570170998573303f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3072112798690796f, -0.6878482103347778f, 0.1127772256731987f, -0.8305730223655701f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.36232542991638184f, -0.19270285964012146f, -0.15925079584121704f, -0.3055815100669861f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.7767963409423828f, 0.9532845616340637f, -0.03312380984425545f, -0.35205644369125366f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11498111486434937f, 0.16841208934783936f, 0.2652629315853119f, -0.5918898582458496f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.08133866637945175f, -0.1607532650232315f, -0.2637385129928589f, -0.11869754642248154f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.33919164538383484f, -0.0083334781229496f, -0.6235449314117432f, -0.4023544192314148f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2539410889148712f, 0.036771468818187714f, 0.15394136309623718f, 0.5800958871841431f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24765968322753906f, 0.039033882319927216f, -0.21599180996418f, 0.11743202805519104f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.4497607946395874f, -0.35164350271224976f, 0.1956074833869934f, -0.534218430519104f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2917529046535492f, -0.034396544098854065f, -0.1438727080821991f, -0.5012933611869812f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11953185498714447f, -0.19408050179481506f, -0.03786057233810425f, 0.4812934398651123f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.6019572019577026f, -0.689301609992981f, -0.12705843150615692f, 0.025071339681744576f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07190308719873428f, -0.2606552541255951f, 0.04865766316652298f, -0.4424172639846802f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2980201244354248f, -0.11076446622610092f, 0.14153464138507843f, 0.1853468269109726f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3539109230041504f, -0.1624540090560913f, -0.6550677418708801f, -0.10667059570550919f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2770325243473053f, 0.05502551421523094f, -0.16839717328548431f, -0.7897548675537109f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19836361706256866f, -0.04585924372076988f, 0.32099848985671997f, 0.45162373781204224f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.20765800774097443f, -0.22577765583992004f, -0.09617625176906586f, -0.20689354836940765f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20897142589092255f, 0.36702603101730347f, -0.32880938053131104f, -0.4694531261920929f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14214259386062622f, -0.13162489235401154f, 0.32563889026641846f, -0.43401968479156494f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.09362038224935532f, -0.15765702724456787f, 0.553214430809021f, -0.9685863852500916f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2890143394470215f, -0.5129100680351257f, 0.1124742180109024f, -0.019475314766168594f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.24140571057796478f, -0.14187200367450714f, -0.36410990357398987f, -0.13319936394691467f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.508758544921875f, -0.021510165184736252f, -0.12353776395320892f, -0.15284311771392822f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.17384670674800873f, -0.2454770803451538f, -0.18663136661052704f, -0.25008416175842285f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.38434964418411255f, -0.127099871635437f, -0.2258158177137375f, 0.003174062119796872f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3819463849067688f, -0.18717990815639496f, -0.17311711609363556f, -0.6692308783531189f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.007237196434289217f, 0.026321759447455406f, -0.17625103890895844f, -0.34792444109916687f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.08471615612506866f, 0.3221624195575714f, 0.0479617565870285f, 0.5844106674194336f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 8)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.2943926751613617f, 0.48776575922966003f, 0.15994498133659363f, 0.3435438871383667f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4719104468822479f, -0.45443305373191833f, 0.31990280747413635f, 0.026716776192188263f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2994875907897949f, 0.33196601271629333f, -0.08057957142591476f, 0.5356447696685791f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.6477466821670532f, 0.21484296023845673f, -0.4800921678543091f, -0.35185033082962036f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11438275128602982f, 0.13592146337032318f, 0.6559363007545471f, -0.056253932416439056f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7085506916046143f, 0.33734333515167236f, -0.09849576652050018f, 0.27155762910842896f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.31795021891593933f, -0.2676905393600464f, 0.3114522099494934f, -0.4112224578857422f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1041739210486412f, -0.11815367639064789f, -0.037632837891578674f, 0.5658745169639587f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13540886342525482f, -0.0886630117893219f, -0.02101202867925167f, 0.5612720251083374f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.5202726125717163f, 0.14053325355052948f, -0.14410404860973358f, -0.25828152894973755f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03548271954059601f, -0.03287196531891823f, -0.15847279131412506f, -0.4273396134376526f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19943742454051971f, 0.7443671226501465f, 0.4041362702846527f, 0.45855942368507385f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.914025068283081f, -0.1892601102590561f, 0.06225059926509857f, -0.012305371463298798f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.021515166386961937f, -0.10367385298013687f, 0.5288386344909668f, -0.2222539484500885f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12225840240716934f, 0.241788849234581f, 0.06251905113458633f, -0.38329267501831055f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3668356239795685f, -0.0701138973236084f, 0.35530129075050354f, -0.1440414935350418f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2145591825246811f, 0.17696163058280945f, 0.14685387909412384f, -0.03081417642533779f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6720254421234131f, 0.015458086505532265f, 0.07159840315580368f, -0.3081827461719513f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.8101854920387268f, -0.21872085332870483f, -0.4124409556388855f, -0.26762276887893677f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.39332500100135803f, 0.6880097389221191f, -0.2970375418663025f, -0.11019710451364517f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.26430749893188477f, -0.26574215292930603f, -0.06584995239973068f, 0.26882556080818176f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5755581855773926f, 0.0884355679154396f, 0.7265874743461609f, -0.12996931374073029f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3037179410457611f, 0.2281017303466797f, -0.2577078938484192f, 0.2981201410293579f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.20934823155403137f, -0.10600105673074722f, 0.30951574444770813f, -0.15570135414600372f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.2328512817621231f, -0.27451446652412415f, 0.704505443572998f, 0.1492319256067276f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12195952236652374f, -0.3262369930744171f, -0.6500619053840637f, -0.4174894392490387f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.021200062707066536f, 0.3914617896080017f, -0.6626526713371277f, 0.4130341410636902f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.21443328261375427f, 0.2772555947303772f, 0.2529413104057312f, -0.3642987012863159f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2343626320362091f, -0.0719204768538475f, 0.3997730016708374f, -0.5333608984947205f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.52214515209198f, 0.7947424650192261f, 0.4494825601577759f, -0.16091686487197876f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3636147677898407f, -0.0684262290596962f, 0.04191524162888527f, -0.6105573773384094f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7003694176673889f, 0.4059615731239319f, -0.08334816247224808f, -0.5020490288734436f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5808159112930298f, -0.09177496284246445f, 0.12013784795999527f, 0.5755411386489868f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.38941237330436707f, 0.009570355527102947f, -0.3313926160335541f, -0.09915478527545929f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.154872328042984f, -0.32224732637405396f, 0.35422471165657043f, -0.20443633198738098f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07001005858182907f, 0.6062178611755371f, 0.15177349746227264f, -0.35532766580581665f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-1.061949372291565f, -0.2894198000431061f, -0.025236982852220535f, -0.10209138691425323f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11138994246721268f, -0.935519278049469f, -0.3954343795776367f, -0.3277069628238678f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.04796309024095535f, -0.2185257077217102f, 0.3310703933238983f, -0.3527059853076935f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.25662556290626526f, 0.5533893704414368f, -0.4980141520500183f, -0.3843615651130676f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2578183114528656f, -0.12905745208263397f, -0.856299877166748f, 0.38086071610450745f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7392334342002869f, 0.7640222907066345f, -0.24064287543296814f, -0.34416547417640686f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.7319684624671936f, -0.13634411990642548f, -0.11114083975553513f, 0.005248089320957661f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.033131666481494904f, -0.3071986734867096f, -0.20815682411193848f, -0.20401524007320404f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19394880533218384f, -0.03779066726565361f, -0.04833980277180672f, -0.11965981870889664f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.20813436806201935f, 0.05874420702457428f, 0.6551777124404907f, -0.6676369309425354f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6370450854301453f, -0.003198177320882678f, -0.3943328857421875f, 0.2757653594017029f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.243852898478508f, -0.20286040008068085f, 0.025684364140033722f, 0.0304032564163208f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.499279648065567f, -0.6553714871406555f, -0.7360981702804565f, -0.22737906873226166f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0690334215760231f, 0.10557510703802109f, 0.2248004674911499f, 0.2078542709350586f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.20742705464363098f, -0.8116433024406433f, -0.6804168820381165f, 0.1444511115550995f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.09757459163665771f, -0.23820781707763672f, 0.5214709639549255f, -0.047822803258895874f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1541718691587448f, 0.3483232855796814f, -0.7649806141853333f, -0.36868274211883545f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15696847438812256f, -0.481567919254303f, 0.2684444785118103f, 0.33191198110580444f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.1526719182729721f, -0.18733690679073334f, -0.434908926486969f, 0.027828752994537354f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.27261799573898315f, 0.40823128819465637f, -0.4697732925415039f, 0.43763864040374756f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4666798710823059f, -0.3383103311061859f, -0.0772179439663887f, 0.3056783676147461f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.07834149897098541f, -0.2953488230705261f, 0.01629706099629402f, -0.33242103457450867f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07281819730997086f, 0.37365782260894775f, -0.11896773427724838f, -0.6053880453109741f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.29596826434135437f, 0.3004465699195862f, -0.2855948507785797f, -0.0005319582996889949f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.12468906491994858f, 0.18854637444019318f, 0.3930346369743347f, -0.30321967601776123f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4378143846988678f, 0.44102931022644043f, 0.07847737520933151f, -0.39460891485214233f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1635226011276245f, 0.20164376497268677f, -0.8603360652923584f, -0.008084066212177277f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.269122838973999f, -0.1732293963432312f, -0.27114546298980713f, -0.5899711847305298f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08016318827867508f, -0.518316388130188f, 0.5175606608390808f, 0.3846602737903595f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.21907490491867065f, -0.13516244292259216f, 0.12607599794864655f, -0.038444358855485916f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.48999619483947754f, -0.0024311384186148643f, -0.0907638818025589f, -0.23568522930145264f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6442205309867859f, -0.312003493309021f, -0.07224326580762863f, -0.6394498348236084f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19171449542045593f, -0.08848489820957184f, -0.4570460915565491f, -0.08953791856765747f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.1435314416885376f, 0.4323013722896576f, -0.4458146393299103f, -0.9629963040351868f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06489552557468414f, -0.12159345299005508f, -0.37108343839645386f, 0.009616431780159473f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6592538356781006f, 0.17247195541858673f, -0.21303027868270874f, 0.040994152426719666f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 8)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.23849152028560638f, 0.22484739124774933f, 0.32428914308547974f, 0.010910660959780216f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.42075973749160767f, -0.07721032202243805f, -0.2295946627855301f, 0.2663591206073761f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.643239438533783f, -0.17769843339920044f, 0.07509502023458481f, 0.28129395842552185f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.06210755184292793f, -0.031928520649671555f, -0.6218047142028809f, 0.0737411230802536f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17382638156414032f, 0.26447317004203796f, -0.2605559229850769f, 0.039758846163749695f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.8750134110450745f, -0.18909379839897156f, -0.2197173833847046f, 0.17352178692817688f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.6884912252426147f, 0.17467725276947021f, -0.012535400688648224f, -0.48422983288764954f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04022566229104996f, -0.5745322108268738f, -0.11104933172464371f, 0.49048078060150146f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4751579761505127f, 0.2168266624212265f, -0.2194584310054779f, 0.21099796891212463f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3661627471446991f, -0.04955965280532837f, -0.2663494646549225f, 0.12920452654361725f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.057814352214336395f, 0.07773081213235855f, 0.17538020014762878f, -0.6642823815345764f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05916175618767738f, -0.23763945698738098f, 0.08508401364088058f, 0.2361549437046051f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.5441580414772034f, 0.2773546576499939f, -0.2931699752807617f, -0.06635038554668427f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.013207227922976017f, 0.4291430115699768f, -0.11683053523302078f, 0.26768848299980164f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.22164036333560944f, 0.08924418687820435f, 0.04776768386363983f, 0.5288993716239929f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.07955513894557953f, 0.2712605893611908f, -0.09256493300199509f, -0.04948435723781586f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06250941008329391f, 0.4131399095058441f, 0.41434866189956665f, -0.1632297784090042f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4462684392929077f, 0.22550339996814728f, -0.6621681451797485f, -0.09388194233179092f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.13024687767028809f, 0.3087731897830963f, -0.30204489827156067f, 0.17211805284023285f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6458457708358765f, 0.2871668338775635f, 0.11989239603281021f, -0.0547909177839756f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.47059008479118347f, -0.24995549023151398f, 0.5104843378067017f, 0.6988809704780579f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.5841384530067444f, 0.16131576895713806f, 0.6832130551338196f, -0.867954671382904f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.014970488846302032f, -0.32696232199668884f, -0.039808932691812515f, 0.6950236558914185f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09854648262262344f, 0.49578213691711426f, -0.2685644328594208f, 0.30933716893196106f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.4993513226509094f, 0.10003527998924255f, -0.2217831313610077f, -0.3377424478530884f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10528874397277832f, -0.8346781134605408f, 0.1608705371618271f, -0.11837244033813477f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09179256856441498f, 0.08294422924518585f, 0.0672026053071022f, -0.2435336410999298f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3931450843811035f, -0.22958998382091522f, 0.2280595451593399f, -0.031554143875837326f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3227747976779938f, 0.22762466967105865f, 0.2689493000507355f, -0.36872610449790955f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.21065311133861542f, 0.7614558339118958f, 0.28947046399116516f, -0.11893542110919952f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.13584613800048828f, -0.011163419112563133f, -0.12802092730998993f, -0.11290009319782257f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2862336039543152f, 0.10689876973628998f, -0.5465391874313354f, 0.02912532351911068f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02838905155658722f, -0.046453140676021576f, 0.13234470784664154f, -0.11625875532627106f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.376334547996521f, -0.019813774153590202f, -0.0029521342366933823f, -0.23379777371883392f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1841452568769455f, 0.23274558782577515f, 0.81522536277771f, -0.7202733159065247f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.043564412742853165f, -0.09513429552316666f, 0.41883647441864014f, 0.3122369945049286f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.15846870839595795f, -0.09876883774995804f, 0.25594422221183777f, 0.2825218439102173f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6039804220199585f, -0.914443850517273f, 0.14081740379333496f, -0.06898799538612366f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6916936635971069f, -0.0004432926361914724f, 0.41125231981277466f, -0.21074798703193665f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.1231168881058693f, 0.1713770478963852f, -0.21432995796203613f, -0.47920870780944824f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.16986006498336792f, 0.23359070718288422f, 0.5383260846138f, -0.17848043143749237f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0050024595111608505f, -0.033891405910253525f, 0.145419642329216f, 0.10846558213233948f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.01953057572245598f, 0.23635374009609222f, -0.06254225969314575f, 0.3614802062511444f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.428741991519928f, -0.5146733522415161f, -0.4142559766769409f, 0.21606427431106567f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6055393218994141f, -0.6663805842399597f, 0.32558324933052063f, -0.06830290704965591f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.11582554876804352f, -0.05576428025960922f, 0.017845267429947853f, -0.5232108235359192f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3098951280117035f, -0.4478859603404999f, 0.5985809564590454f, 0.29583603143692017f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18026311695575714f, -0.28846803307533264f, -0.1130075678229332f, 0.005395523738116026f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.4229571521282196f, -0.24395333230495453f, -0.24889570474624634f, 0.2933993339538574f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4007261097431183f, -0.20166414976119995f, 0.33965423703193665f, 0.24121196568012238f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02815396897494793f, -0.7496403455734253f, -0.07879231125116348f, 0.04775500297546387f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.47439754009246826f, 0.03749576956033707f, -0.12748925387859344f, -0.24271777272224426f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05047662556171417f, -0.019561022520065308f, 0.5724438428878784f, -0.056555017828941345f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2553822100162506f, 0.032348278909921646f, 0.35643088817596436f, 0.08236832171678543f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.022221431136131287f, 0.07200663536787033f, -0.7889713048934937f, -0.37637463212013245f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5508803725242615f, 0.5409446954727173f, -0.5887329578399658f, 0.3623313903808594f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18247893452644348f, -0.3275126814842224f, 0.21021123230457306f, -0.12496504187583923f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.6724485754966736f, -0.13935525715351105f, -0.22234979271888733f, 0.2414054125547409f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.44115713238716125f, -0.13200247287750244f, 0.05725405365228653f, -0.3067426383495331f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.24711184203624725f, 0.3248938322067261f, -0.1486530303955078f, 0.24330361187458038f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.06559819728136063f, -0.2194775640964508f, -0.2687816619873047f, -0.3525753915309906f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.312876433134079f, 1.3369543012231588e-05f, 0.17274558544158936f, -0.28107377886772156f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6245012879371643f, 0.30018752813339233f, -0.1893022507429123f, 0.044795673340559006f);
					x = _mm_load_ps1(&x3[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.30119243264198303f, -0.021948494017124176f, -0.586502730846405f, 0.037482306361198425f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4087464213371277f, 0.07600506395101547f, 0.8687641024589539f, 0.7575304508209229f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4169246256351471f, -0.3410809636116028f, -0.17141084372997284f, 0.16408848762512207f);
					x = _mm_load_ps1(&x3[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.0037081590853631496f, 0.08605296164751053f, -0.1150682270526886f, 0.5197429060935974f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3879738450050354f, -0.4147425591945648f, 0.005999790038913488f, -0.06732665747404099f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1598721742630005f, -0.7786436676979065f, -0.01827504299581051f, 0.12546071410179138f);
					x = _mm_load_ps1(&x3[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.11397237330675125f, -0.4100170135498047f, -0.720267117023468f, -0.2710488438606262f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11127623915672302f, -0.4802251160144806f, 0.6255661845207214f, 0.42607349157333374f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12247038632631302f, 0.0627957284450531f, 0.3753453493118286f, 0.06423015892505646f);
					x = _mm_load_ps1(&x3[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][8], x);
				}
			}
		}
	}

 	// Leaky ReLu Layer
	y = _mm_set_ps1(0.30000001192092896f);
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{

			x = _mm_load_ps((float*)&x4[i][j][0]);
			x = _mm_max_ps(x, _mm_mul_ps(y, x));
			_mm_store_ps((float*)&x4[i][j][0], x);

			x = _mm_load_ps((float*)&x4[i][j][4]);
			x = _mm_max_ps(x, _mm_mul_ps(y, x));
			_mm_store_ps((float*)&x4[i][j][4], x);

			x = _mm_load_ps((float*)&x4[i][j][8]);
			x = _mm_max_ps(x, _mm_mul_ps(y, x));
			_mm_store_ps((float*)&x4[i][j][8], x);
		}
	}

 	// Maxpool Layer 
	static float x5[4][4][12] = {0};
	for (int ix = 0; ix < 7; ix += 2)
	{
		int x_out_1;
		x_out_1 = ix / 2;
	for (int jx = 0; jx < 7; jx += 2)
	{
		int x_out_2;
		x_out_2 = jx / 2;
		x = _mm_load_ps((float*)&x4[ix][jx][0]);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][0], x);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][0], x);
		x = _mm_load_ps((float*)&x4[ix][jx][4]);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][4], x);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][4], x);
		x = _mm_load_ps((float*)&x4[ix][jx][8]);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 0][8]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 0][jx + 1][8]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][8], x);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 0][8]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x4[ix + 1][jx + 1][8]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x5[x_out_1][x_out_2][8], x);
		}
	}

 	// Convolution Layer
	static float x6 alignas(16) [4][4][16] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x6[i][j][0] = 0.7237785458564758f;
			x6[i][j][1] = -1.0003058910369873f;
			x6[i][j][2] = 0.38476860523223877f;
			x6[i][j][3] = 0.17183814942836761f;
			x6[i][j][4] = -0.4389638602733612f;
			x6[i][j][5] = -0.40320059657096863f;
			x6[i][j][6] = 0.3227500319480896f;
			x6[i][j][7] = -0.8473401069641113f;
			x6[i][j][8] = -0.7270433902740479f;
			x6[i][j][9] = 0.27902668714523315f;
			x6[i][j][10] = -0.36557793617248535f;
			x6[i][j][11] = -1.3862062692642212f;
			x6[i][j][12] = 0.18592776358127594f;
			x6[i][j][13] = -0.41966867446899414f;
			x6[i][j][14] = -0.5876384973526001f;
			x6[i][j][15] = -0.5425975322723389f;
		}
	}
	for (int ix = -1; ix < 3; ix += 1)
	{
		int x_1, x_out_1;
		x_out_1 = (ix + 1) / 1;
		for (int jx = -1; jx < 3; jx += 1)
		{
			int x_2, x_out_2;
			x_out_2 = (jx + 1) / 1;
			x_1 = ix + 0;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.535276472568512f, 0.07299161702394485f, -0.22156144678592682f, -0.4601023495197296f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.03210628777742386f, 0.05864528566598892f, 0.30514487624168396f, -0.4429571032524109f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.16041770577430725f, -0.6904374957084656f, 0.1704360395669937f, -0.30858609080314636f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.25089991092681885f, 0.5827366709709167f, -0.404401570558548f, 0.2838596701622009f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.3452443480491638f, 0.08940690755844116f, -0.45000672340393066f, -0.12312902510166168f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.25874432921409607f, 0.09650370478630066f, 0.21801821887493134f, 0.14988559484481812f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11608697474002838f, -0.26173195242881775f, -0.030171506106853485f, 0.3201344609260559f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2667385935783386f, -0.194195955991745f, -0.2789275348186493f, -0.20139265060424805f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.04076360538601875f, -0.02177744172513485f, -0.7330309152603149f, 0.2117764949798584f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9183027744293213f, 0.06383951753377914f, 0.06327327340841293f, 0.15938544273376465f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05446842312812805f, -0.20690171420574188f, -0.03670504689216614f, -0.935481607913971f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.0874352678656578f, -0.2530480623245239f, 0.6626562476158142f, -0.3281922936439514f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.08390451222658157f, 0.17362195253372192f, 0.1926720291376114f, -0.6015980839729309f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.21356697380542755f, -0.2248229682445526f, 0.5202717781066895f, -0.3370974659919739f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.062402013689279556f, 1.0272619724273682f, -0.5078545212745667f, 0.8832592964172363f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.13166461884975433f, 0.2947766184806824f, -0.5434463620185852f, 0.12906433641910553f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.33651939034461975f, -0.12192691117525101f, 0.12450960278511047f, 0.31917861104011536f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4832311272621155f, -0.2961832582950592f, 0.07660248875617981f, 0.7047585844993591f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.822756826877594f, -0.03629609942436218f, -1.266801357269287f, -0.11041673272848129f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.17249353229999542f, 0.22497951984405518f, 0.8967576622962952f, -0.0592070035636425f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.017428556457161903f, -0.1478632092475891f, 0.0319773368537426f, -0.0741877630352974f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3577333688735962f, 0.022192899137735367f, -0.3726370334625244f, -0.3396311402320862f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.34949633479118347f, -0.1311459094285965f, -0.06108745560050011f, -0.05350220948457718f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.09946682304143906f, 0.034470416605472565f, -0.6724622845649719f, 0.08226051926612854f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.10103410482406616f, 0.14806614816188812f, 0.37615278363227844f, -0.0909976065158844f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.399074524641037f, -0.09586545079946518f, -1.0837898254394531f, 0.16392730176448822f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06618694216012955f, -0.41073811054229736f, -0.5559412837028503f, -0.24736641347408295f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.11928452551364899f, -0.8748543858528137f, -0.6208484768867493f, -0.12474804371595383f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.29338690638542175f, 0.0788022130727768f, -0.44846606254577637f, 0.02264200523495674f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5536165237426758f, 0.1060163751244545f, -0.11031985282897949f, -0.050389330834150314f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.9174336194992065f, -0.30515480041503906f, 0.27517619729042053f, 0.386957585811615f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.40501320362091064f, -0.36444514989852905f, 0.231696218252182f, -0.09759117662906647f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.14682240784168243f, -0.18216419219970703f, 0.07313627004623413f, 0.08395475894212723f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7184692621231079f, -0.057865142822265625f, 0.1485447734594345f, -0.4628857374191284f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07024233043193817f, -0.26817867159843445f, 0.013752848841249943f, -1.2459653615951538f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.18019260466098785f, -0.31638726592063904f, -0.08314916491508484f, -0.2290835976600647f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.11750301718711853f, -0.24247601628303528f, 0.1346144825220108f, -0.022570986300706863f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.36665987968444824f, -0.04166093096137047f, -0.46394363045692444f, -0.47714975476264954f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03206180781126022f, -0.6605899930000305f, -0.01966494508087635f, -0.959624707698822f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3592171370983124f, -1.0888615846633911f, -0.29674020409584045f, -0.431904137134552f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.13251131772994995f, -0.011907783336937428f, -0.047105398029088974f, -0.48315590620040894f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.41496559977531433f, -0.0075280689634382725f, -0.6630868911743164f, -0.11962924897670746f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15139462053775787f, -0.08702024072408676f, -0.03876617178320885f, -0.6508035063743591f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.46724799275398254f, -0.028632670640945435f, -0.6940580010414124f, 0.021558718755841255f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.2978643774986267f, -0.1973404586315155f, -0.8985399603843689f, -0.2822519838809967f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1407003551721573f, 0.006022826302796602f, -0.13456061482429504f, -0.530317485332489f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6849668025970459f, -0.3285456597805023f, -0.04864852875471115f, -0.5556925535202026f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.014613243751227856f, 0.0527217797935009f, 0.28148239850997925f, -0.20075789093971252f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.06172932684421539f, -0.587864339351654f, 0.06133313477039337f, 0.40659934282302856f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.297829806804657f, -0.03433122858405113f, -0.07338282465934753f, 0.6224638819694519f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.004217588808387518f, -0.9562809467315674f, 0.09063514322042465f, -0.848866879940033f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.19931477308273315f, -0.04384392127394676f, -0.43681132793426514f, -0.3319765627384186f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.6305017471313477f, 0.04838434234261513f, -0.04628324136137962f, -0.43290314078330994f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.46287354826927185f, -0.07291899621486664f, -0.372916042804718f, -0.06614337116479874f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21399107575416565f, -0.1873588263988495f, 0.022433286532759666f, 0.18585935235023499f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5641589760780334f, -0.280178427696228f, -0.2441924512386322f, 0.11028284579515457f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.014808794483542442f, -0.025360215455293655f, -0.7212812304496765f, -0.5391528010368347f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.862641453742981f, -0.07355108857154846f, -1.0911091566085815f, -0.11161711812019348f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6674261689186096f, -0.08405919373035431f, -0.23212215304374695f, -0.6705090403556824f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5999467968940735f, -0.20221440494060516f, 0.27540117502212524f, 0.02416713908314705f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.09066154807806015f, 0.10758408904075623f, 0.6182509660720825f, 0.4037839472293854f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19687585532665253f, 0.0739808976650238f, 0.08727582544088364f, -0.050660498440265656f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15031221508979797f, -1.1940507888793945f, -0.24952492117881775f, -0.11405039578676224f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.9628451466560364f, 0.21589604020118713f, 0.12082003057003021f, 0.1750592291355133f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.16443194448947906f, 0.01901751570403576f, -0.5715548396110535f, -0.045536793768405914f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.29902422428131104f, -0.12479881942272186f, -0.04483385384082794f, 0.057513147592544556f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6363019347190857f, 0.388388067483902f, 0.19242069125175476f, 0.6241953372955322f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.606325089931488f, -0.40917012095451355f, 0.1765335649251938f, 0.029282977804541588f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.01901816576719284f, -0.0938221737742424f, -0.5568390488624573f, -0.14675921201705933f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.22223588824272156f, -0.0959685817360878f, -0.0956609696149826f, -0.7261563539505005f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07298915833234787f, -0.017898112535476685f, -0.040326423943042755f, -0.3295651376247406f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.058626893907785416f, -0.026267752051353455f, 0.05210893228650093f, -0.1872623860836029f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.25213098526000977f, 0.1926000714302063f, 0.40705016255378723f, -0.25819915533065796f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.571520209312439f, -0.4245567321777344f, -0.47625184059143066f, 0.0231485515832901f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4499531686306f, 0.3127477169036865f, -0.3783884048461914f, 0.15475179255008698f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2545028626918793f, 0.16157200932502747f, 0.44339847564697266f, -0.156862273812294f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.15960748493671417f, -0.015912024304270744f, -0.14896458387374878f, -0.6236928105354309f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.27798110246658325f, -0.2056325525045395f, 0.44915133714675903f, -0.44952908158302307f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.31585314869880676f, 0.08470842987298965f, -0.3310701251029968f, -0.05440840497612953f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.19147568941116333f, 0.3850488066673279f, -0.17705892026424408f, -0.045650970190763474f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.18719492852687836f, -0.0025551090948283672f, -0.1898937076330185f, -0.23210161924362183f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.023489587008953094f, -0.0557066947221756f, 0.13875670731067657f, -0.1279042512178421f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.18461768329143524f, 0.5918542742729187f, -0.12371137738227844f, -0.4875933825969696f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.30480849742889404f, -0.18090292811393738f, -0.2206316739320755f, -0.07446041703224182f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.004851583391427994f, -0.12807127833366394f, -0.37611472606658936f, 0.2589849531650543f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.41046997904777527f, -0.0843905583024025f, 0.0129780862480402f, -0.396537184715271f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03087913990020752f, -0.7142977714538574f, -0.29272976517677307f, -0.45224905014038086f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.097531758248806f, -0.6938222646713257f, 0.22493378818035126f, -0.028009746223688126f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1872982233762741f, 0.01052191760390997f, -0.129159614443779f, -0.4151235818862915f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.24005737900733948f, -0.02798216976225376f, -0.41867122054100037f, -0.4195355772972107f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1769283562898636f, -0.36549124121665955f, -0.21436427533626556f, -0.1892441362142563f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5132771730422974f, -0.10473228245973587f, -0.6927418112754822f, -0.0809158906340599f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.3284693956375122f, 0.04043655842542648f, -0.18487469851970673f, -0.28149548172950745f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1634630262851715f, 0.14551232755184174f, -0.3249741494655609f, 0.2679454982280731f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.11648058891296387f, -0.0669197365641594f, -0.42585206031799316f, -0.1465921550989151f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2853730618953705f, -0.5011815428733826f, -0.0681672915816307f, 0.03561950474977493f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.3456936776638031f, -0.2656833231449127f, 0.0059957606717944145f, -0.6727607846260071f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12207111716270447f, -0.024821585044264793f, -0.3821990489959717f, -0.18364295363426208f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10068349540233612f, -0.03673900291323662f, 0.16722485423088074f, 0.2333221137523651f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5305949449539185f, -0.2523682713508606f, -0.471394419670105f, -0.28604668378829956f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1795411854982376f, 0.11974678188562393f, -0.16748180985450745f, -0.30976536870002747f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3066342771053314f, -0.025905419141054153f, -0.1187528595328331f, 0.08895710110664368f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21320292353630066f, -0.5845272541046143f, 0.09261307120323181f, -0.5067208409309387f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2931937575340271f, -0.03303183242678642f, -0.5743114948272705f, -0.23834194242954254f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.16370168328285217f, 0.035934362560510635f, -0.8848102688789368f, -0.1984570175409317f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04298153147101402f, -0.10402490943670273f, -0.2371465265750885f, -0.0747496485710144f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4490179419517517f, 0.06806188821792603f, 0.07413879036903381f, -0.3972771465778351f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.03042026050388813f, -0.1311177760362625f, 0.007517811842262745f, -0.42710068821907043f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.2491590976715088f, -0.03071439452469349f, 0.6378651857376099f, 0.019158517941832542f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4684808552265167f, -0.05114326253533363f, 0.06364841014146805f, -0.09754438698291779f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.050028245896101f, 0.14139586687088013f, -0.1665189266204834f, 0.6335973143577576f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.5567398071289062f, 0.02051752619445324f, -0.05367641896009445f, 0.4216216504573822f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.2791365683078766f, -0.29876694083213806f, -0.41427645087242126f, -0.441071093082428f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09330957382917404f, 0.1478336602449417f, -0.1819286346435547f, 0.3177931606769562f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0442124605178833f, -0.43070539832115173f, 0.15881557762622833f, -0.29418349266052246f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.13822737336158752f, -0.29724088311195374f, -0.1056516095995903f, -0.4779742360115051f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.24239608645439148f, 0.01902472972869873f, -0.6715130805969238f, 0.2394731193780899f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3442474603652954f, -0.09616931527853012f, -0.3014724850654602f, -0.624140202999115f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18831011652946472f, -0.6208498477935791f, -0.16494464874267578f, -0.49639755487442017f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3608764708042145f, -0.30469658970832825f, -0.44546955823898315f, -0.039226893335580826f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.2584793269634247f, 0.17822858691215515f, 0.4326924979686737f, 0.15222860872745514f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.020206168293952942f, -0.11814766377210617f, -0.040951356291770935f, -0.28345075249671936f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.15950113534927368f, 0.4491014778614044f, -0.013642753474414349f, -0.8870423436164856f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.03446003049612045f, 0.43858516216278076f, -0.3360324203968048f, -0.49629679322242737f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.17960746586322784f, -0.02337913028895855f, 0.439784973859787f, -0.2522306442260742f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17271676659584045f, 0.014471791684627533f, 0.19532732665538788f, -0.011546589434146881f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6176075339317322f, 0.027821123600006104f, 0.3590906858444214f, -0.6470829844474792f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3087240159511566f, 0.22463828325271606f, -0.08114691823720932f, 0.12967275083065033f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.10199949145317078f, -0.04103270173072815f, 0.039337191730737686f, 0.1545412242412567f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6732388138771057f, -0.08444926887750626f, 0.2074316442012787f, -0.8962298631668091f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5321703553199768f, 0.30809450149536133f, -0.33866068720817566f, 0.05354302003979683f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.22695226967334747f, -0.2839603126049042f, 0.3870607912540436f, -0.43601852655410767f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.32547664642333984f, -0.23593175411224365f, 0.05222000926733017f, -0.11290975660085678f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3141990900039673f, -0.04296553507447243f, 0.40397772192955017f, 0.06126798689365387f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.481611043214798f, 0.32459890842437744f, 0.1849726140499115f, -0.3632677495479584f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.4380442798137665f, -0.7621551156044006f, 0.05683271214365959f, -1.3515304327011108f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.09480292350053787f, 0.04849500581622124f, 0.3842579126358032f, -0.00131073291413486f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2119712084531784f, -0.07997581362724304f, -0.5453699827194214f, -0.8357753157615662f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.11524207144975662f, -0.6350045204162598f, -0.10251275449991226f, 0.07274653762578964f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2763637900352478f, -0.18241740763187408f, -0.5809943675994873f, -0.0018219870980829f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.02144186571240425f, 0.026919282972812653f, -0.3413466811180115f, 0.09013751149177551f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5547404885292053f, 0.07029787451028824f, -0.2832023501396179f, -0.04505566135048866f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.17088115215301514f, -0.07728799432516098f, -0.35954272747039795f, 0.2003278136253357f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.11466965079307556f, 0.0975058525800705f, 0.05824635177850723f, 0.06029798090457916f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.2616176903247833f, -0.29052987694740295f, -0.37930211424827576f, -0.1581808477640152f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.153259739279747f, 0.06143234297633171f, -0.013505587354302406f, -0.2329336404800415f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3586775064468384f, -0.26813364028930664f, 0.08315247297286987f, -0.6870081424713135f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.33100205659866333f, -0.6034889817237854f, -0.08381596207618713f, -0.4217648208141327f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.09847182780504227f, 0.05806264653801918f, -0.3302398920059204f, -0.015315382741391659f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.012728290632367134f, 0.07003194838762283f, 0.05662727728486061f, 0.22789406776428223f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24928298592567444f, -0.6048449873924255f, 0.11356963962316513f, 0.6866715550422668f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.26880210638046265f, -0.054565198719501495f, -0.2682479918003082f, 0.027206970378756523f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.14882421493530273f, 0.10750920325517654f, 0.27998438477516174f, -0.022129088640213013f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18221013247966766f, -0.1032160222530365f, 0.45166724920272827f, 0.3907441794872284f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1236066073179245f, -0.21738025546073914f, -0.2126932591199875f, -0.2626948356628418f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.23440469801425934f, -0.2779945433139801f, -0.3262939155101776f, -0.2174815684556961f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.0981321930885315f, -0.24270851910114288f, -0.5480981469154358f, 0.5914121866226196f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2649158537387848f, -0.0187146607786417f, 0.6239756345748901f, 0.4848490357398987f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0772905945777893f, -0.7490223050117493f, 1.3916982412338257f, -0.35758674144744873f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.019304733723402023f, 0.017477339133620262f, -0.013287841342389584f, 0.027839643880724907f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.31626567244529724f, -0.11157798022031784f, -0.9498052597045898f, 0.18643930554389954f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3020581305027008f, -0.09466765820980072f, 0.12097106128931046f, 0.17161811888217926f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25649088621139526f, 0.3917306661605835f, 0.7034508585929871f, -0.04089624434709549f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.17476068437099457f, -0.36870908737182617f, -0.22004589438438416f, -0.43844643235206604f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.13328105211257935f, 0.022386593744158745f, -0.24407543241977692f, 0.09877738356590271f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.005593865644186735f, 0.010174626484513283f, 0.04859426990151405f, -0.18692219257354736f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3518059551715851f, -0.28386321663856506f, 0.036577753722667694f, -0.36568763852119446f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.26566216349601746f, -0.22513701021671295f, -0.4522128403186798f, 0.265725702047348f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.40464648604393005f, 0.1907399296760559f, 0.6215327382087708f, 0.494876503944397f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.34369298815727234f, 0.1863015592098236f, -0.8473568558692932f, -0.10654020309448242f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.019786370918154716f, -0.29109373688697815f, -0.3012631833553314f, 0.05391073599457741f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.025806035846471786f, 0.5933928489685059f, -0.3371090292930603f, 0.5605087876319885f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.0734008401632309f, 0.0919523760676384f, -0.7547067999839783f, -0.24915899336338043f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3549569249153137f, 0.13157257437705994f, 0.20336100459098816f, -0.39907899498939514f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.238680899143219f, -0.4791157841682434f, -0.3714432418346405f, 0.19250893592834473f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.35040953755378723f, -0.694178581237793f, -0.5310273170471191f, 0.114865243434906f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.04951171949505806f, -0.06842619180679321f, -0.31931695342063904f, -0.047524016350507736f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.35905930399894714f, 0.009945779107511044f, 0.1461525559425354f, -0.18487316370010376f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1675833761692047f, -0.3135823607444763f, -0.25883978605270386f, -0.022949257865548134f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.6605438590049744f, 0.24284665286540985f, -0.17817404866218567f, -0.6243636012077332f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.06373844295740128f, 0.05645439401268959f, 0.3258027732372284f, -0.24437886476516724f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.47224509716033936f, -0.05038953945040703f, 0.359274685382843f, -0.10173387825489044f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.15666663646697998f, -0.34872764348983765f, -0.10789564251899719f, -0.05080730840563774f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.24306407570838928f, -0.3019932508468628f, -0.18354353308677673f, 0.31070858240127563f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.14922955632209778f, -0.024627558887004852f, -0.05523451417684555f, -0.013702630065381527f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03402445837855339f, 0.009724468924105167f, -0.3859938085079193f, -0.0906456708908081f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.39072373509407043f, 0.10919148474931717f, -0.13176485896110535f, 0.4071090817451477f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3294825851917267f, -0.09106819331645966f, 0.03797660768032074f, 0.13983939588069916f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.30686676502227783f, 0.0784907117486f, -0.7862734794616699f, 0.00845569558441639f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.026318170130252838f, -0.07478606700897217f, 0.01708209328353405f, 0.08044975996017456f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5217393040657043f, 0.09281986951828003f, -0.11669626086950302f, 0.006974596530199051f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.41376402974128723f, -0.86345374584198f, -0.09966111928224564f, -0.037186168134212494f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.08773181587457657f, -0.09135366231203079f, -0.2700524628162384f, -0.20219837129116058f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3121437728404999f, -0.28450697660446167f, 0.33195406198501587f, 0.05120118707418442f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03324336186051369f, -0.5823737382888794f, 0.36915338039398193f, 0.0573669858276844f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.28072765469551086f, -0.09921979159116745f, -0.12946124374866486f, -0.5288215279579163f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.03819398209452629f, 0.11152829229831696f, 0.32953307032585144f, -0.5257638692855835f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08502589166164398f, 0.007977086119353771f, -0.6930481791496277f, -0.3992619514465332f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5385283827781677f, -0.02145589515566826f, -0.10494295507669449f, 0.23738156259059906f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.46224039793014526f, -0.037887707352638245f, -0.4178978502750397f, -0.22697141766548157f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.12194901704788208f, 0.0758337453007698f, 0.2784591317176819f, -0.11122304201126099f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9612894654273987f, 0.0677468404173851f, -0.432759165763855f, 0.012799382209777832f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14680494368076324f, -0.0525222085416317f, -0.9998912811279297f, 0.41898611187934875f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.023402245715260506f, -0.3365493714809418f, 0.09667261689901352f, 0.15530981123447418f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.3181828260421753f, -0.18019141256809235f, -0.6456221342086792f, 0.28632333874702454f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23349952697753906f, -0.11188318580389023f, 0.22804094851016998f, -0.14065243303775787f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02552318386733532f, -0.10538754612207413f, 0.5887371897697449f, -0.7481870651245117f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.09117446094751358f, 0.08615238219499588f, -0.09256907552480698f, -0.5180731415748596f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.008673572912812233f, -0.063080333173275f, 0.5898791551589966f, -0.1782035380601883f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8462703824043274f, -0.24307575821876526f, 0.07433278858661652f, 0.038626689463853836f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18592745065689087f, 0.1329939365386963f, 0.6681243777275085f, -0.364426851272583f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.7257593274116516f, -0.642350971698761f, -0.5119629502296448f, 0.12412341684103012f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.008346009999513626f, 0.018027065321803093f, -0.2580065131187439f, -0.06916020810604095f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14932651817798615f, 0.03931877389550209f, 0.11795657128095627f, -0.8444218039512634f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09447131305932999f, -0.19198600947856903f, -0.06441015750169754f, -0.4529542326927185f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.1032252386212349f, 0.0957658588886261f, -0.4079103171825409f, -0.18029780685901642f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.08682739734649658f, 0.03005346655845642f, -0.07365173101425171f, 0.08901289105415344f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5609607696533203f, 0.1311282068490982f, -0.521889328956604f, -0.24891310930252075f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19065959751605988f, 0.481213241815567f, -0.22793972492218018f, 0.22034703195095062f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.38051638007164f, 0.6676343679428101f, 0.043391041457653046f, 0.28912249207496643f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.025891177356243134f, 0.08260679244995117f, -0.528643012046814f, -0.16739779710769653f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04841991886496544f, 0.02270808257162571f, 0.4879094064235687f, 0.05620688945055008f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06633733212947845f, -0.13837629556655884f, -0.8082304000854492f, 0.30333125591278076f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.18228895962238312f, 0.12034188956022263f, -0.16241568326950073f, 0.1856759786605835f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.17777466773986816f, 0.06536310911178589f, -0.29619452357292175f, -0.5820060968399048f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.02987539768218994f, 0.10252834856510162f, -0.26620760560035706f, -0.8194718956947327f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5147186517715454f, 0.09275715798139572f, -0.39484846591949463f, 0.009306215681135654f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.14910228550434113f, -0.475153386592865f, 0.270565390586853f, -0.15708914399147034f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.3031683564186096f, 0.03393208235502243f, 0.08322024345397949f, -0.3692801892757416f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3598269820213318f, -0.14957013726234436f, -0.3057488203048706f, -0.26560190320014954f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.16499091684818268f, -0.16106031835079193f, -0.08763781934976578f, -0.36464327573776245f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.26960042119026184f, -0.12263339757919312f, 0.3658009171485901f, 0.10181722790002823f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.17527829110622406f, -0.005442533642053604f, 0.031602755188941956f, 0.12297879904508591f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.17916268110275269f, 0.0631098821759224f, -0.2389420121908188f, -0.005943884141743183f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07745514810085297f, -0.3611951768398285f, -0.8897659182548523f, -0.35563021898269653f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2685992121696472f, -0.12775151431560516f, -0.6207064986228943f, 0.0741957500576973f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.21474424004554749f, 0.0042769210413098335f, -0.10914576798677444f, -0.19932803511619568f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.17717094719409943f, 0.0237381961196661f, 0.24736037850379944f, 0.038370661437511444f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.16349159181118011f, -0.0951368510723114f, 0.15221476554870605f, -0.07812264561653137f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.01893807202577591f, -0.19079925119876862f, 0.0852329358458519f, -0.18639761209487915f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.05170593038201332f, -0.018887294456362724f, -0.045186541974544525f, 0.1485709547996521f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04890509322285652f, -0.019636500626802444f, 0.05825452506542206f, -0.573119044303894f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2810657024383545f, -0.3517954349517822f, -0.09866992384195328f, -0.18176855146884918f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.24851581454277039f, -0.5430111885070801f, -0.08496446162462234f, -0.6058451533317566f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.039954774081707f, 0.11905413866043091f, 0.26928427815437317f, 0.2607111930847168f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9814372062683105f, -0.0016090016579255462f, -0.7060885429382324f, -0.180353581905365f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.31961527466773987f, -0.3987630605697632f, -0.3524853587150574f, 0.05258146673440933f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.27185937762260437f, -0.05512363836169243f, -0.8096258044242859f, -6.116533768363297e-06f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.058027494698762894f, 0.06286831945180893f, 0.12428133934736252f, -0.04986940696835518f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.888037919998169f, -0.01470701489597559f, -0.03184780851006508f, 0.10246045887470245f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13497282564640045f, -0.24273808300495148f, -0.2971906363964081f, -0.515424370765686f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3035948872566223f, 0.03917880728840828f, 0.007012266665697098f, -0.13595892488956451f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.15007148683071136f, 0.07434109598398209f, 0.07308024168014526f, -0.10417983680963516f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3009698688983917f, -0.1546563357114792f, 0.56558758020401f, 0.11172559857368469f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.026935674250125885f, 0.46607840061187744f, 0.7017078995704651f, 0.4120970070362091f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.1899392008781433f, -0.5812105536460876f, 0.4694801867008209f, 0.17445002496242523f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.010920071043074131f, -0.3340900242328644f, 0.3063432276248932f, 0.03494013100862503f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12006135284900665f, 0.034152232110500336f, 0.31442099809646606f, -0.4203527569770813f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7052530646324158f, -0.2921006977558136f, -0.18088948726654053f, 0.26557043194770813f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.24028010666370392f, -0.047632768750190735f, 0.17916272580623627f, 0.09310987591743469f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.07428624480962753f, -0.028405720368027687f, -0.3083864152431488f, 0.026067040860652924f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.27684512734413147f, -0.007922480814158916f, -0.2165045589208603f, -0.49488309025764465f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1397215574979782f, -0.6489741802215576f, -0.1469569057226181f, -0.3410564959049225f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.021682702004909515f, 0.0713534951210022f, -0.06477244943380356f, -0.17321263253688812f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.02251339517533779f, 0.21481762826442719f, -0.7739362120628357f, 0.07974051684141159f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16641902923583984f, -0.04702223464846611f, -0.19726744294166565f, 0.2920706868171692f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5421085953712463f, 0.13043741881847382f, -0.24904385209083557f, -0.3951195776462555f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3076334595680237f, 0.13175448775291443f, -0.4504379630088806f, 0.43415823578834534f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.013147608377039433f, 0.14619611203670502f, 0.3734551966190338f, -0.06891816854476929f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.27067041397094727f, -0.10104739665985107f, 0.02009979635477066f, 0.1897105723619461f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4136867821216583f, 0.07047945261001587f, -0.4755399227142334f, 0.08599875122308731f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3876517713069916f, 0.672195553779602f, -0.48316875100135803f, -0.16365990042686462f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.10021288692951202f, 0.10590212792158127f, 0.05769629031419754f, -0.2242746502161026f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07289767265319824f, -0.06738036870956421f, 0.17908522486686707f, 0.6077287197113037f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.15809902548789978f, -0.01510611642152071f, -0.25813522934913635f, 0.5481286644935608f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.07406977564096451f, 0.13789869844913483f, -0.6759117841720581f, -0.3678787052631378f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.16310937702655792f, -0.06685572117567062f, 0.025781329721212387f, 0.053569164127111435f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.201792374253273f, -0.15843123197555542f, -0.075200155377388f, 0.10885771363973618f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.08688516914844513f, 0.14584769308567047f, -0.07005152106285095f, 0.3022420108318329f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2511817216873169f, -0.07201545685529709f, -0.6304064989089966f, 0.0676710456609726f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.056667376309633255f, 0.0058761476539075375f, -0.06724907457828522f, -0.03185738995671272f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2466416358947754f, 0.07234425097703934f, -0.04735364019870758f, -0.48232442140579224f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.17889399826526642f, -0.5170738697052002f, -0.6969519257545471f, -0.33033809065818787f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.25203412771224976f, 0.13416728377342224f, -0.5290964841842651f, 0.3155360221862793f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.01942925527691841f, -0.05078971013426781f, -0.06352586299180984f, -0.25411683320999146f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3664446771144867f, 0.07299511134624481f, 0.024836190044879913f, 0.527407705783844f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18644468486309052f, -0.05886450037360191f, -0.03226267173886299f, 0.23701216280460358f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.2611461877822876f, -0.01361213717609644f, 0.1715620905160904f, -0.07003699243068695f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.3115498423576355f, 0.25311279296875f, -0.9245365858078003f, 0.013397219590842724f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16962802410125732f, -0.19409961998462677f, -0.11011151969432831f, -0.6041406989097595f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5126888751983643f, 0.443503201007843f, -0.2058255523443222f, -0.8600834012031555f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.06192993000149727f, -1.1902447938919067f, -0.2672514021396637f, 0.3518851697444916f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.1642398089170456f, -0.23757041990756989f, -0.2393425554037094f, -0.09552646428346634f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2848502993583679f, 0.07515034824609756f, -0.07564450800418854f, -0.12042634934186935f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.38591647148132324f, -0.5218241810798645f, 0.29431721568107605f, -0.35286977887153625f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2781018912792206f, -0.38762709498405457f, 0.32250747084617615f, -0.33053290843963623f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.043193794786930084f, -0.0013729864731431007f, 0.20648178458213806f, -0.49760517477989197f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15970715880393982f, -0.007747118826955557f, 0.05345345288515091f, 0.11435584723949432f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2392938882112503f, -0.4775587320327759f, -0.005791224539279938f, 0.448983371257782f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.5162863731384277f, -0.3598189651966095f, 0.34071478247642517f, -0.44662946462631226f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.12107391655445099f, -0.09029394388198853f, 0.8077271580696106f, -0.1381063461303711f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.16224472224712372f, -0.006658978294581175f, -0.48068293929100037f, -0.8697905540466309f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8295291066169739f, 0.6349614262580872f, 0.8300522565841675f, 0.483279824256897f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(1.0573769807815552f, 1.8660895824432373f, -0.10188239812850952f, 0.0925055667757988f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.11934347450733185f, 0.4367518723011017f, 0.6518720984458923f, -0.06417704373598099f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7625374794006348f, 0.09191720932722092f, 0.2756422460079193f, 0.04326695203781128f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.805992066860199f, -0.3468455672264099f, -0.5468319058418274f, 0.39976316690444946f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.07793784886598587f, 0.32603874802589417f, -0.7061629891395569f, 0.2629776895046234f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.07600945979356766f, -0.2587796449661255f, -0.9643576145172119f, 0.057136595249176025f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9987847208976746f, -0.035489700734615326f, -0.1424994170665741f, -0.6987243294715881f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.05733451247215271f, -0.7105033993721008f, -0.08633946627378464f, -1.063939094543457f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.33978816866874695f, -0.26773300766944885f, -0.33851975202560425f, -0.30102530121803284f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.32210394740104675f, -0.3113836348056793f, 0.5083877444267273f, 0.35066232085227966f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2955268919467926f, 0.007426346652209759f, -0.46666672825813293f, 0.6739743947982788f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.12284353375434875f, 0.1007217988371849f, -0.06970325857400894f, -0.09150916337966919f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3012506067752838f, 0.5832492709159851f, 0.5870798230171204f, 0.3531637489795685f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.2513144910335541f, -0.22360798716545105f, -0.02785378322005272f, -0.2833777666091919f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7210028767585754f, 0.19155065715312958f, 0.023479115217924118f, -1.167000651359558f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0845501497387886f, -0.19184456765651703f, -0.2327272742986679f, 0.08383885771036148f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.29886341094970703f, -0.5265319347381592f, 0.1775490939617157f, -0.22964215278625488f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.11313892155885696f, -0.21635258197784424f, -0.24539823830127716f, 0.3398914337158203f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12003515660762787f, -0.11792804300785065f, 0.01884751208126545f, 0.3626290559768677f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.33427080512046814f, -0.6173397302627563f, 0.07452046126127243f, -0.004254186060279608f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.3631173372268677f, -0.520716667175293f, 0.1012108102440834f, -0.32873937487602234f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.002211008919402957f, 0.00985468365252018f, 0.17076607048511505f, -0.14539982378482819f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.011818593367934227f, -0.016827059909701347f, 0.015503055416047573f, -0.14805911481380463f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24486027657985687f, -0.006390402093529701f, -0.1095045804977417f, 0.14953935146331787f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.03353625163435936f, -0.2987041175365448f, 0.24542415142059326f, 0.5602733492851257f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.021163079887628555f, -0.2005782127380371f, -0.5118440389633179f, -0.12061896175146103f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.47601744532585144f, -0.0390193909406662f, -0.03162465617060661f, -0.014503688551485538f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06413935124874115f, -0.3483959138393402f, -0.244791641831398f, -0.5561141967773438f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.18137241899967194f, 0.003583827055990696f, 0.36929771304130554f, -0.03511669486761093f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.09287673979997635f, -0.13827680051326752f, -0.7902947068214417f, 0.0066602774895727634f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9345495104789734f, -0.12343862652778625f, 0.05196176469326019f, 0.04174554720520973f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04003060981631279f, -0.2999218702316284f, -0.16051368415355682f, -1.0400012731552124f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.22662995755672455f, -0.5991834998130798f, -0.01489940658211708f, -0.35765179991722107f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.4444921612739563f, 0.12688547372817993f, -0.10080482810735703f, -0.20230187475681305f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3443980813026428f, -0.25512510538101196f, -0.2445124089717865f, -0.30698275566101074f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08956052362918854f, -0.13802818953990936f, -0.3801608979701996f, -0.48597824573516846f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.16534554958343506f, 0.3881378173828125f, -0.5119370222091675f, -0.03343392536044121f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.1478985846042633f, -0.08665990084409714f, -0.3263630270957947f, -0.48688217997550964f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6963279843330383f, 0.030137045308947563f, -0.9963594675064087f, -0.3812248706817627f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6941856741905212f, -0.125852569937706f, 0.14537978172302246f, -0.07449955493211746f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.4726664423942566f, -0.05367129668593407f, 0.0710461363196373f, -0.325671911239624f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.05160718783736229f, 0.05058136582374573f, 0.1309281885623932f, -0.2965872585773468f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12428833544254303f, 0.08153370767831802f, -0.1608051061630249f, 0.028292251750826836f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5277004241943359f, -0.3590107262134552f, -0.7151719331741333f, 0.18359804153442383f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.0420965850353241f, -0.267212450504303f, -0.006186389829963446f, -0.21907755732536316f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.22329995036125183f, -0.06430532783269882f, 0.17631381750106812f, 0.4096227288246155f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.21207913756370544f, 0.0081788944080472f, 0.37686944007873535f, 0.48675671219825745f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.7734858393669128f, 0.028568251058459282f, -0.07072032243013382f, -0.06779592484235764f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.6021891832351685f, 0.1775641143321991f, 0.10699255019426346f, 0.4185470938682556f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.16305552423000336f, 0.027317028492689133f, 0.6280088424682617f, -0.019779790192842484f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3507991433143616f, -0.08342547714710236f, 0.20676468312740326f, -0.02893582172691822f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15688878297805786f, -0.050676651298999786f, 0.1446932703256607f, 0.8446587324142456f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.20053750276565552f, 0.36954236030578613f, 0.32788214087486267f, 0.14655432105064392f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.24820604920387268f, -0.07342085987329483f, -0.2615714967250824f, -0.18018867075443268f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9659533500671387f, 0.008283614180982113f, -0.8379557728767395f, -0.9178813099861145f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7427949905395508f, -0.16141091287136078f, -0.20804761350154877f, -0.22772683203220367f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3062721788883209f, -0.3847574293613434f, -0.4810410737991333f, -0.13488620519638062f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.16422544419765472f, -0.25489237904548645f, 0.5833603739738464f, -0.1400790959596634f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.14729218184947968f, 0.030068377032876015f, -0.43326836824417114f, 0.10477990657091141f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3549654483795166f, 0.19323045015335083f, -0.2326027899980545f, 0.22717119753360748f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.19765356183052063f, -0.36688509583473206f, 0.18247035145759583f, -0.03888355940580368f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.20774246752262115f, 0.0030329751316457987f, -0.866528332233429f, -0.37167611718177795f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2348516583442688f, 0.03647555783390999f, 0.22194179892539978f, -0.16711656749248505f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2221805304288864f, -0.4844137132167816f, -0.054559264332056046f, -0.46226224303245544f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.15770265460014343f, -1.254003882408142f, -0.490621417760849f, -0.2195492684841156f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1487424522638321f, 0.17169725894927979f, -0.2650962769985199f, -0.09570272266864777f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04594700038433075f, 0.03352804109454155f, -0.4201999604701996f, -0.0779200866818428f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.39585208892822266f, 0.4485001862049103f, -0.25169235467910767f, 0.18135030567646027f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.24094758927822113f, -0.0979398638010025f, -1.0874391794204712f, -0.21894749999046326f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.053840361535549164f, -0.044372644275426865f, -0.03142696991562843f, 0.07824156433343887f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2914268672466278f, 0.06656879931688309f, -0.21771566569805145f, 0.01605723425745964f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2565188407897949f, 0.1723630577325821f, 0.32439902424812317f, 0.3188416063785553f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.11352860182523727f, -0.5157119035720825f, -0.3677060604095459f, 0.42385390400886536f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.2088475376367569f, -0.19588391482830048f, -0.591138482093811f, 0.050890546292066574f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.34606534242630005f, 0.05640208721160889f, -0.12561866641044617f, -0.30156829953193665f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.34072282910346985f, -0.12556426227092743f, -0.13589948415756226f, -0.8087167739868164f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.03998088836669922f, -0.27231764793395996f, 0.4152093231678009f, -0.03230809047818184f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.15631988644599915f, -0.09876128286123276f, -0.1994314342737198f, -0.25114279985427856f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07858365029096603f, 0.020791610702872276f, 0.4340840280056f, 0.3104812502861023f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04355959594249725f, -0.5428842902183533f, 0.6034690737724304f, -0.324400931596756f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.24176177382469177f, -0.053477589040994644f, 0.23805634677410126f, -0.035092808306217194f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.29649218916893005f, 0.13258036971092224f, 0.22518274188041687f, -0.22254575788974762f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0719284862279892f, 0.10145611315965652f, 0.25383299589157104f, 0.0708184614777565f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.36168909072875977f, 0.36879757046699524f, -0.29812783002853394f, 0.22916753590106964f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.13193002343177795f, 0.41455763578414917f, 0.3800143897533417f, 0.24532708525657654f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.02992353029549122f, -0.021976681426167488f, -0.16908606886863708f, 0.022767482325434685f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6498480439186096f, -0.0354464091360569f, -1.1816803216934204f, -0.2418510764837265f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7905831933021545f, -0.25298964977264404f, 0.1884005069732666f, -0.16085903346538544f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.2827940881252289f, -0.4424899220466614f, 0.2400144636631012f, 0.09001973271369934f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.1962072104215622f, -0.005196545738726854f, 0.1638936847448349f, -0.2732633054256439f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0021051005460321903f, 0.0017330285627394915f, -0.07714937627315521f, 0.19780221581459045f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4628094732761383f, -0.08814553171396255f, 0.08875206112861633f, -0.02819853276014328f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.16154995560646057f, -0.09158051759004593f, -0.17082548141479492f, 0.002783258678391576f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.09007047861814499f, 0.23465894162654877f, -0.4658844470977783f, 0.21330812573432922f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.8101361989974976f, 0.19535574316978455f, 0.5062299966812134f, 0.32319357991218567f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4743511974811554f, 0.3076760470867157f, -0.05093669146299362f, 0.8865552544593811f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3000199794769287f, 0.1443277895450592f, 0.1924012154340744f, 0.7053855061531067f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1837674379348755f, 0.007042667828500271f, -0.12621521949768066f, -0.32830560207366943f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.44513267278671265f, -0.2959537208080292f, 0.2376992553472519f, -0.4692932963371277f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.38307425379753113f, 0.37248873710632324f, 0.033888641744852066f, 0.15009666979312897f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.34911203384399414f, 0.2524794042110443f, -0.03840165212750435f, -0.17417855560779572f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.12184762209653854f, -0.05764125660061836f, -0.2972954511642456f, 0.1548186093568802f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3599441349506378f, -0.07432294636964798f, 0.12515953183174133f, -0.8129859566688538f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3666805028915405f, -0.7228356003761292f, -0.6458382606506348f, -0.4133889973163605f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.00633725943043828f, -0.36890602111816406f, -0.04492630064487457f, -0.30337288975715637f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.22189144790172577f, -0.1897186040878296f, -0.37679752707481384f, -0.16482894122600555f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5076165795326233f, -0.11142115294933319f, -0.5771175026893616f, -0.1313973069190979f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.060855086892843246f, -0.19773103296756744f, 0.10044671595096588f, -0.22159922122955322f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.7419093251228333f, -0.5424380898475647f, 0.853117823600769f, 0.07737082242965698f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.11513962596654892f, -0.059824712574481964f, -0.009545126929879189f, -0.221866175532341f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09874138981103897f, -0.027767188847064972f, -0.14077679812908173f, 0.45134755969047546f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13876672089099884f, 0.5491201281547546f, 0.24569734930992126f, 0.2810036540031433f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.3328883945941925f, -0.44733715057373047f, -0.04103585332632065f, 0.04020750895142555f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1406012773513794f, -0.1125219464302063f, 0.16391324996948242f, 0.05819522589445114f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.977746307849884f, -0.11257314682006836f, -0.00516075175255537f, -0.07781311124563217f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.42390719056129456f, 0.07859744876623154f, -0.2380615770816803f, 0.39334049820899963f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.06998493522405624f, -0.2165336310863495f, 0.30756211280822754f, 0.06810407340526581f);
					x = _mm_load_ps1(&x5[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.1452159285545349f, 0.06837530434131622f, -0.5675054788589478f, -0.058689605444669724f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5657535791397095f, 0.11397146433591843f, 0.027851179242134094f, 0.08015567064285278f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3334823548793793f, 0.3466959595680237f, 0.0642574355006218f, 0.6024985313415527f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.5240156650543213f, 0.5847660899162292f, 0.15995679795742035f, 0.6471492052078247f);
					x = _mm_load_ps1(&x5[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.05495285242795944f, -0.05321935936808586f, -0.329301655292511f, 0.023950763046741486f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2486618161201477f, -0.013431238941848278f, 0.24190686643123627f, -0.07499246299266815f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.004210083745419979f, -0.6476871371269226f, 0.29539817571640015f, -0.42087388038635254f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.08472468703985214f, -0.17787715792655945f, -0.09814652055501938f, -0.32457685470581055f);
					x = _mm_load_ps1(&x5[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.06727426499128342f, -0.08970202505588531f, -0.353171169757843f, 0.052172958850860596f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23119090497493744f, -0.008141670376062393f, 0.5864337682723999f, 0.22137098014354706f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3924926817417145f, 0.12668386101722717f, 0.2744649052619934f, -0.2774985730648041f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.14574795961380005f, -0.9123021960258484f, 0.7531700134277344f, -0.31853559613227844f);
					x = _mm_load_ps1(&x5[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][12], x);
				}
			}
		}
	}

 	// Leaky ReLu Layer
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{

			x = _mm_load_ps((float*)&x6[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x6[i][j][0], x);

			x = _mm_load_ps((float*)&x6[i][j][4]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x6[i][j][4], x);

			x = _mm_load_ps((float*)&x6[i][j][8]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x6[i][j][8], x);

			x = _mm_load_ps((float*)&x6[i][j][12]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x6[i][j][12], x);
		}
	}

 	// Convolution Layer
	static float x7 alignas(16) [4][4][16] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x7[i][j][0] = -0.024787476286292076f;
			x7[i][j][1] = 0.04106058552861214f;
			x7[i][j][2] = 0.008744983933866024f;
			x7[i][j][3] = 0.012997046113014221f;
			x7[i][j][4] = -0.00512483436614275f;
			x7[i][j][5] = 0.006927697453647852f;
			x7[i][j][6] = 0.004542346578091383f;
			x7[i][j][7] = 0.0018496820703148842f;
			x7[i][j][8] = 0.001420044107362628f;
			x7[i][j][9] = -0.007753792218863964f;
			x7[i][j][10] = 0.007245745975524187f;
			x7[i][j][11] = -0.028609471395611763f;
			x7[i][j][12] = -0.007483311463147402f;
			x7[i][j][13] = -0.014519674703478813f;
			x7[i][j][14] = -0.10852902382612228f;
			x7[i][j][15] = 0.024526068940758705f;
		}
	}
	for (int ix = -0; ix < 4; ix += 1)
	{
		int x_1, x_out_1;
		x_out_1 = (ix + 0) / 1;
		for (int jx = -0; jx < 4; jx += 1)
		{
			int x_2, x_out_2;
			x_out_2 = (jx + 0) / 1;
			x_1 = ix + 0;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.011959575116634369f, -0.09636310487985611f, -0.019497202709317207f, 0.05292070657014847f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.010214264504611492f, 0.00134528917260468f, 0.026297686621546745f, 0.03318197280168533f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0010573220206424594f, 0.019998779520392418f, -0.00020270953245926648f, -0.01077241636812687f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.06923780590295792f, -0.05633874982595444f, -0.08734875917434692f, -0.09498074650764465f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.021095920354127884f, 0.052115101367235184f, 0.3382876515388489f, -0.036228861659765244f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10759782046079636f, -0.0103936567902565f, -0.14120611548423767f, 0.5464736223220825f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1212681457400322f, 0.06687340885400772f, -0.0049284049309790134f, 0.016041269525885582f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.16043587028980255f, 0.40300723910331726f, -0.6810236573219299f, 0.1924823522567749f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.06463407725095749f, 0.03778674080967903f, -0.0391031950712204f, 0.017880920320749283f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.020786968991160393f, 0.003980920650064945f, -0.00975409522652626f, 0.12666188180446625f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0336029976606369f, -0.00827960204333067f, -0.0033393455669283867f, 0.013170245103538036f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.020780077204108238f, 0.060749925673007965f, 0.03610888868570328f, -0.07135992497205734f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.050217870622873306f, 0.023435167968273163f, -0.051825158298015594f, 0.016261862590909004f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.021634472534060478f, 0.00441726902499795f, -0.06065024808049202f, -0.04287528991699219f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02549995854496956f, -0.019507838413119316f, 0.00013432653213385493f, 0.00973938312381506f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.016140863299369812f, -0.0906897634267807f, 0.06255611777305603f, -0.056523486971855164f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.001312731415964663f, -0.01909431256353855f, -0.10586608201265335f, 0.038839004933834076f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.017282553017139435f, 0.02161889709532261f, 0.0038979812525212765f, -0.042506974190473557f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06329536437988281f, 0.03437664732336998f, -0.003154862904921174f, -0.01716066710650921f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.004585517104715109f, 0.2971765995025635f, 0.08012472838163376f, 0.16598086059093475f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.011626604944467545f, 0.013055344112217426f, -0.07637812942266464f, -0.00431745545938611f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.001955109415575862f, 0.012048556469380856f, -0.005531452130526304f, -0.05991891026496887f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.008394922129809856f, 0.024739056825637817f, 0.0030626379884779453f, 0.04092269390821457f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.042489975690841675f, 0.2288818508386612f, 0.040484827011823654f, 0.08419982343912125f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.04682251065969467f, -0.032343462109565735f, -0.03578681871294975f, 0.04047868400812149f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.027835315093398094f, -0.002943433355540037f, 0.007440053392201662f, -0.019199516624212265f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.059165626764297485f, 0.0027983486652374268f, -0.0024341067764908075f, -0.003447113325819373f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.12292249500751495f, -0.013131793588399887f, -0.025506149977445602f, 0.07029297947883606f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.05011020228266716f, -0.050438523292541504f, 0.09776417911052704f, -0.0946011170744896f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3342726230621338f, -0.009386242367327213f, -0.009913841262459755f, -0.49122488498687744f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.14478914439678192f, -0.011492578312754631f, -0.0017667514039203525f, 0.0185672789812088f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.01190225500613451f, 0.13084205985069275f, -0.2179276943206787f, -0.2020948976278305f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.018932752311229706f, -0.03494133800268173f, -0.12570936977863312f, -0.05331391468644142f);
					x = _mm_load_ps1(&x6[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06551303714513779f, 0.004040763713419437f, 0.029391519725322723f, 0.23551853001117706f);
					x = _mm_load_ps1(&x6[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2877637445926666f, -0.09242063015699387f, 0.006815925240516663f, 0.024931173771619797f);
					x = _mm_load_ps1(&x6[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.38699987530708313f, 0.1674993485212326f, 0.059931494295597076f, -0.090153768658638f);
					x = _mm_load_ps1(&x6[x_1][x_2][8]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(-0.05309603363275528f, -0.07780415564775467f, -0.017307450994849205f, -0.0017928675515577197f);
					x = _mm_load_ps1(&x6[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0004483883094508201f, -0.001075386069715023f, -0.0457814559340477f, -0.06690777093172073f);
					x = _mm_load_ps1(&x6[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03078031726181507f, -0.026631051674485207f, -0.005682766437530518f, -0.0052316817454993725f);
					x = _mm_load_ps1(&x6[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.022934498265385628f, 0.20083720982074738f, 0.032134827226400375f, 0.11663199961185455f);
					x = _mm_load_ps1(&x6[x_1][x_2][9]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.04445098713040352f, -0.035853177309036255f, 0.23672737181186676f, 0.026958094909787178f);
					x = _mm_load_ps1(&x6[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07587805390357971f, 0.008052978664636612f, -0.014856160618364811f, -0.16021820902824402f);
					x = _mm_load_ps1(&x6[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.010884691961109638f, 0.03809216618537903f, 0.0026834162417799234f, -0.028768865391612053f);
					x = _mm_load_ps1(&x6[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.12660369277000427f, 0.12520113587379456f, 0.08060327917337418f, -0.21162289381027222f);
					x = _mm_load_ps1(&x6[x_1][x_2][10]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.0017852454911917448f, 0.056110769510269165f, 0.4701107144355774f, 0.07755071669816971f);
					x = _mm_load_ps1(&x6[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08001942187547684f, -0.003309413557872176f, 0.005638668779283762f, 0.04637496918439865f);
					x = _mm_load_ps1(&x6[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06430163979530334f, -0.0878603458404541f, -0.002238328568637371f, -0.004344630055129528f);
					x = _mm_load_ps1(&x6[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.07131743431091309f, 0.2809644937515259f, -0.34182605147361755f, 0.07842276245355606f);
					x = _mm_load_ps1(&x6[x_1][x_2][11]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.02970866486430168f, -0.05488219112157822f, 0.015495962463319302f, -0.06533809006214142f);
					x = _mm_load_ps1(&x6[x_1][x_2][12]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08872728049755096f, 0.003175513120368123f, 0.03647501394152641f, 0.1688123196363449f);
					x = _mm_load_ps1(&x6[x_1][x_2][12]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.13783054053783417f, -0.021968651562929153f, -0.008169733919203281f, 0.019082721322774887f);
					x = _mm_load_ps1(&x6[x_1][x_2][12]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.03406347334384918f, 0.025790544226765633f, -0.008938143961131573f, 0.026739487424492836f);
					x = _mm_load_ps1(&x6[x_1][x_2][12]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.005534734111279249f, 0.02689754217863083f, -0.04800627380609512f, 0.019582798704504967f);
					x = _mm_load_ps1(&x6[x_1][x_2][13]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.015917634591460228f, -0.06980206072330475f, -0.0041450997814536095f, -0.0684012770652771f);
					x = _mm_load_ps1(&x6[x_1][x_2][13]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.016073737293481827f, -0.021081386134028435f, 0.005360403098165989f, 0.014049358665943146f);
					x = _mm_load_ps1(&x6[x_1][x_2][13]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.023901619017124176f, 0.29106372594833374f, 0.01976567879319191f, 0.09938770532608032f);
					x = _mm_load_ps1(&x6[x_1][x_2][13]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.0893646851181984f, -0.0668681189417839f, -0.14166827499866486f, 0.03075251169502735f);
					x = _mm_load_ps1(&x6[x_1][x_2][14]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3729197382926941f, -0.030101967975497246f, -0.023501131683588028f, 0.6258032321929932f);
					x = _mm_load_ps1(&x6[x_1][x_2][14]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07203620672225952f, 0.03322453051805496f, -0.009293504990637302f, -0.007579449098557234f);
					x = _mm_load_ps1(&x6[x_1][x_2][14]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(-0.17010214924812317f, 0.161991685628891f, 0.19117595255374908f, 0.17498177289962769f);
					x = _mm_load_ps1(&x6[x_1][x_2][14]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);

					w = _mm_set_ps(0.00303328363224864f, 0.016276951879262924f, -0.04836525022983551f, -0.005861056502908468f);
					x = _mm_load_ps1(&x6[x_1][x_2][15]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.013629225082695484f, 0.010435969568789005f, 0.0037750594783574343f, -0.06081560626626015f);
					x = _mm_load_ps1(&x6[x_1][x_2][15]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.02829556167125702f, -0.002003468805924058f, -0.0030907250475138426f, -0.029929883778095245f);
					x = _mm_load_ps1(&x6[x_1][x_2][15]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][8]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][8], x);

					w = _mm_set_ps(0.007358240429311991f, 0.20741018652915955f, 0.007333409506827593f, 0.06358809024095535f);
					x = _mm_load_ps1(&x6[x_1][x_2][15]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][12]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][12], x);
				}
			}
		}
	}

 	// Dense Layer
	scores[0] = 0.264054f
	 + 0.127285f * x7[0][0][0] + 0.027337f * x7[0][0][1] - 0.004056f * x7[0][0][2] + 0.105375f * x7[0][0][3]
	 - 0.006320f * x7[0][0][4] - 0.135199f * x7[0][0][5] - 0.072177f * x7[0][0][6] - 0.012985f * x7[0][0][7]
	 - 0.003300f * x7[0][0][8] - 0.004497f * x7[0][0][9] + 0.002147f * x7[0][0][10] + 0.054980f * x7[0][0][11]
	 + 0.083650f * x7[0][0][12] + 0.057644f * x7[0][0][13] - 0.027916f * x7[0][0][14] - 0.095367f * x7[0][0][15]
	 + 0.008713f * x7[0][1][0] + 0.095273f * x7[0][1][1] + 0.020769f * x7[0][1][2] + 0.010895f * x7[0][1][3]
	 - 0.327427f * x7[0][1][4] + 0.054647f * x7[0][1][5] - 0.022840f * x7[0][1][6] - 0.011193f * x7[0][1][7]
	 + 0.069375f * x7[0][1][8] - 0.001776f * x7[0][1][9] + 0.024867f * x7[0][1][10] + 0.239363f * x7[0][1][11]
	 + 0.107167f * x7[0][1][12] - 0.023552f * x7[0][1][13] - 0.127948f * x7[0][1][14] + 0.073690f * x7[0][1][15]
	 + 0.129104f * x7[0][2][0] + 0.010649f * x7[0][2][1] - 0.023352f * x7[0][2][2] + 0.108995f * x7[0][2][3]
	 - 0.291975f * x7[0][2][4] + 0.035393f * x7[0][2][5] + 0.019887f * x7[0][2][6] - 0.054217f * x7[0][2][7]
	 + 0.009325f * x7[0][2][8] - 0.006447f * x7[0][2][9] - 0.006578f * x7[0][2][10] - 0.132944f * x7[0][2][11]
	 + 0.007461f * x7[0][2][12] - 0.053661f * x7[0][2][13] - 0.042959f * x7[0][2][14] - 0.134915f * x7[0][2][15]
	 - 0.030307f * x7[0][3][0] + 0.004942f * x7[0][3][1] - 0.012258f * x7[0][3][2] + 0.023052f * x7[0][3][3]
	 - 0.021679f * x7[0][3][4] - 0.053699f * x7[0][3][5] - 0.003134f * x7[0][3][6] + 0.014031f * x7[0][3][7]
	 - 0.021255f * x7[0][3][8] - 0.006567f * x7[0][3][9] - 0.003130f * x7[0][3][10] + 0.009528f * x7[0][3][11]
	 + 0.033784f * x7[0][3][12] - 0.009852f * x7[0][3][13] - 0.004977f * x7[0][3][14] - 0.027342f * x7[0][3][15]
	 - 0.002900f * x7[1][0][0] - 0.064757f * x7[1][0][1] + 0.046155f * x7[1][0][2] - 0.002703f * x7[1][0][3]
	 - 0.006699f * x7[1][0][4] + 0.020886f * x7[1][0][5] - 0.015264f * x7[1][0][6] + 0.054257f * x7[1][0][7]
	 - 0.050900f * x7[1][0][8] - 0.015612f * x7[1][0][9] - 0.065814f * x7[1][0][10] + 0.152890f * x7[1][0][11]
	 + 0.146807f * x7[1][0][12] - 0.101463f * x7[1][0][13] - 0.062207f * x7[1][0][14] + 0.130090f * x7[1][0][15]
	 - 0.058570f * x7[1][1][0] - 0.294341f * x7[1][1][1] + 0.043479f * x7[1][1][2] - 0.044106f * x7[1][1][3]
	 + 0.075906f * x7[1][1][4] + 0.013341f * x7[1][1][5] - 0.024943f * x7[1][1][6] - 0.112319f * x7[1][1][7]
	 - 0.008318f * x7[1][1][8] - 0.002805f * x7[1][1][9] + 0.009739f * x7[1][1][10] - 0.074453f * x7[1][1][11]
	 + 0.137497f * x7[1][1][12] + 0.063262f * x7[1][1][13] - 0.104001f * x7[1][1][14] + 0.070354f * x7[1][1][15]
	 - 0.023921f * x7[1][2][0] - 0.043318f * x7[1][2][1] - 0.044823f * x7[1][2][2] - 0.048688f * x7[1][2][3]
	 - 0.048161f * x7[1][2][4] + 0.089644f * x7[1][2][5] - 0.006110f * x7[1][2][6] + 0.006411f * x7[1][2][7]
	 - 0.024507f * x7[1][2][8] - 0.000592f * x7[1][2][9] - 0.008298f * x7[1][2][10] + 0.048137f * x7[1][2][11]
	 - 0.024981f * x7[1][2][12] + 0.201584f * x7[1][2][13] - 0.037115f * x7[1][2][14] - 0.027211f * x7[1][2][15]
	 + 0.082406f * x7[1][3][0] - 0.006709f * x7[1][3][1] - 0.007494f * x7[1][3][2] - 0.041728f * x7[1][3][3]
	 - 0.016325f * x7[1][3][4] + 0.029891f * x7[1][3][5] + 0.017502f * x7[1][3][6] - 0.035246f * x7[1][3][7]
	 + 0.017316f * x7[1][3][8] - 0.014644f * x7[1][3][9] + 0.024662f * x7[1][3][10] + 0.032484f * x7[1][3][11]
	 + 0.044697f * x7[1][3][12] - 0.020584f * x7[1][3][13] - 0.010617f * x7[1][3][14] - 0.053878f * x7[1][3][15]
	 - 0.017809f * x7[2][0][0] - 0.025136f * x7[2][0][1] + 0.035599f * x7[2][0][2] + 0.000269f * x7[2][0][3]
	 + 0.012689f * x7[2][0][4] - 0.026102f * x7[2][0][5] + 0.080514f * x7[2][0][6] - 0.022737f * x7[2][0][7]
	 - 0.022495f * x7[2][0][8] - 0.010452f * x7[2][0][9] - 0.000781f * x7[2][0][10] + 0.019303f * x7[2][0][11]
	 - 0.003644f * x7[2][0][12] - 0.008004f * x7[2][0][13] + 0.009952f * x7[2][0][14] + 0.022522f * x7[2][0][15]
	 - 0.112672f * x7[2][1][0] + 0.384803f * x7[2][1][1] + 0.103484f * x7[2][1][2] - 0.115651f * x7[2][1][3]
	 + 0.295668f * x7[2][1][4] + 0.019694f * x7[2][1][5] - 0.035278f * x7[2][1][6] + 0.011928f * x7[2][1][7]
	 + 0.032247f * x7[2][1][8] - 0.013171f * x7[2][1][9] + 0.121868f * x7[2][1][10] - 0.034023f * x7[2][1][11]
	 - 0.282372f * x7[2][1][12] - 0.114073f * x7[2][1][13] - 0.792317f * x7[2][1][14] - 0.164720f * x7[2][1][15]
	 - 0.021034f * x7[2][2][0] - 0.150400f * x7[2][2][1] - 0.013873f * x7[2][2][2] + 0.031145f * x7[2][2][3]
	 - 0.030629f * x7[2][2][4] - 0.039558f * x7[2][2][5] + 0.012216f * x7[2][2][6] - 0.013176f * x7[2][2][7]
	 + 0.001110f * x7[2][2][8] + 0.016781f * x7[2][2][9] + 0.021354f * x7[2][2][10] - 0.071913f * x7[2][2][11]
	 - 0.056431f * x7[2][2][12] + 0.107761f * x7[2][2][13] - 0.009326f * x7[2][2][14] + 0.004080f * x7[2][2][15]
	 - 0.024811f * x7[2][3][0] + 0.030668f * x7[2][3][1] - 0.018977f * x7[2][3][2] + 0.043635f * x7[2][3][3]
	 + 0.022119f * x7[2][3][4] - 0.007431f * x7[2][3][5] + 0.066710f * x7[2][3][6] + 0.035654f * x7[2][3][7]
	 - 0.012152f * x7[2][3][8] + 0.001921f * x7[2][3][9] + 0.040505f * x7[2][3][10] + 0.013681f * x7[2][3][11]
	 + 0.011096f * x7[2][3][12] + 0.011144f * x7[2][3][13] + 0.004699f * x7[2][3][14] + 0.019052f * x7[2][3][15]
	 - 0.102470f * x7[3][0][0] + 0.012077f * x7[3][0][1] + 0.011125f * x7[3][0][2] - 0.012731f * x7[3][0][3]
	 + 0.038093f * x7[3][0][4] - 0.047890f * x7[3][0][5] + 0.003968f * x7[3][0][6] + 0.015567f * x7[3][0][7]
	 + 0.031700f * x7[3][0][8] + 0.003765f * x7[3][0][9] - 0.033329f * x7[3][0][10] + 0.012261f * x7[3][0][11]
	 - 0.018543f * x7[3][0][12] + 0.027635f * x7[3][0][13] + 0.031108f * x7[3][0][14] + 0.052313f * x7[3][0][15]
	 + 0.028000f * x7[3][1][0] + 0.056138f * x7[3][1][1] + 0.113174f * x7[3][1][2] + 0.050843f * x7[3][1][3]
	 + 0.127156f * x7[3][1][4] + 0.033219f * x7[3][1][5] + 0.021993f * x7[3][1][6] - 0.190516f * x7[3][1][7]
	 + 0.013622f * x7[3][1][8] - 0.005677f * x7[3][1][9] + 0.020683f * x7[3][1][10] - 0.031445f * x7[3][1][11]
	 + 0.037999f * x7[3][1][12] + 0.100828f * x7[3][1][13] - 0.010499f * x7[3][1][14] + 0.023335f * x7[3][1][15]
	 + 0.018072f * x7[3][2][0] - 0.073815f * x7[3][2][1] + 0.146998f * x7[3][2][2] - 0.144218f * x7[3][2][3]
	 + 0.091421f * x7[3][2][4] + 0.031778f * x7[3][2][5] + 0.027705f * x7[3][2][6] + 0.097687f * x7[3][2][7]
	 - 0.044576f * x7[3][2][8] + 0.002198f * x7[3][2][9] - 0.075381f * x7[3][2][10] - 0.069246f * x7[3][2][11]
	 - 0.078443f * x7[3][2][12] - 0.001617f * x7[3][2][13] + 0.023887f * x7[3][2][14] + 0.010512f * x7[3][2][15]
	 + 0.006703f * x7[3][3][0] + 0.016064f * x7[3][3][1] - 0.047434f * x7[3][3][2] - 0.024916f * x7[3][3][3]
	 + 0.041619f * x7[3][3][4] - 0.071557f * x7[3][3][5] - 0.031314f * x7[3][3][6] + 0.031158f * x7[3][3][7]
	 + 0.040014f * x7[3][3][8] - 0.000968f * x7[3][3][9] - 0.029188f * x7[3][3][10] - 0.055038f * x7[3][3][11]
	 + 0.021152f * x7[3][3][12] + 0.054288f * x7[3][3][13] + 0.021031f * x7[3][3][14] - 0.013745f * x7[3][3][15];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.217162f
	 + 0.012418f * x7[0][0][0] + 0.039676f * x7[0][0][1] - 0.096277f * x7[0][0][2] + 0.001670f * x7[0][0][3]
	 + 0.103870f * x7[0][0][4] + 0.074725f * x7[0][0][5] + 0.012548f * x7[0][0][6] - 0.003885f * x7[0][0][7]
	 - 0.050912f * x7[0][0][8] - 0.013018f * x7[0][0][9] + 0.075500f * x7[0][0][10] - 0.076762f * x7[0][0][11]
	 - 0.103406f * x7[0][0][12] + 0.044410f * x7[0][0][13] + 0.069204f * x7[0][0][14] + 0.175117f * x7[0][0][15]
	 - 0.030966f * x7[0][1][0] + 0.017592f * x7[0][1][1] - 0.029022f * x7[0][1][2] - 0.048290f * x7[0][1][3]
	 - 0.283628f * x7[0][1][4] + 0.033196f * x7[0][1][5] + 0.004974f * x7[0][1][6] - 0.024674f * x7[0][1][7]
	 + 0.016229f * x7[0][1][8] - 0.002352f * x7[0][1][9] + 0.058574f * x7[0][1][10] + 0.215221f * x7[0][1][11]
	 - 0.082172f * x7[0][1][12] - 0.112138f * x7[0][1][13] - 0.052669f * x7[0][1][14] + 0.094181f * x7[0][1][15]
	 - 0.007069f * x7[0][2][0] + 0.013704f * x7[0][2][1] + 0.010284f * x7[0][2][2] - 0.054511f * x7[0][2][3]
	 - 0.270868f * x7[0][2][4] - 0.068237f * x7[0][2][5] + 0.027652f * x7[0][2][6] - 0.062383f * x7[0][2][7]
	 - 0.060764f * x7[0][2][8] + 0.003399f * x7[0][2][9] - 0.068581f * x7[0][2][10] - 0.093215f * x7[0][2][11]
	 + 0.055220f * x7[0][2][12] - 0.021665f * x7[0][2][13] - 0.074128f * x7[0][2][14] - 0.158365f * x7[0][2][15]
	 - 0.159363f * x7[0][3][0] + 0.113215f * x7[0][3][1] - 0.002456f * x7[0][3][2] + 0.105842f * x7[0][3][3]
	 - 0.088800f * x7[0][3][4] - 0.003973f * x7[0][3][5] + 0.061131f * x7[0][3][6] + 0.062980f * x7[0][3][7]
	 + 0.038330f * x7[0][3][8] - 0.001765f * x7[0][3][9] + 0.023736f * x7[0][3][10] - 0.076336f * x7[0][3][11]
	 + 0.144302f * x7[0][3][12] + 0.064522f * x7[0][3][13] - 0.041582f * x7[0][3][14] - 0.082911f * x7[0][3][15]
	 - 0.010777f * x7[1][0][0] + 0.026129f * x7[1][0][1] + 0.043816f * x7[1][0][2] - 0.091684f * x7[1][0][3]
	 + 0.025295f * x7[1][0][4] + 0.001590f * x7[1][0][5] + 0.008747f * x7[1][0][6] + 0.137199f * x7[1][0][7]
	 + 0.104023f * x7[1][0][8] - 0.007147f * x7[1][0][9] + 0.023524f * x7[1][0][10] - 0.107268f * x7[1][0][11]
	 + 0.126804f * x7[1][0][12] - 0.039625f * x7[1][0][13] - 0.042551f * x7[1][0][14] + 0.266365f * x7[1][0][15]
	 + 0.022664f * x7[1][1][0] - 0.281986f * x7[1][1][1] - 0.054573f * x7[1][1][2] + 0.053139f * x7[1][1][3]
	 + 0.106630f * x7[1][1][4] + 0.021323f * x7[1][1][5] + 0.010150f * x7[1][1][6] - 0.099193f * x7[1][1][7]
	 - 0.051038f * x7[1][1][8] - 0.014835f * x7[1][1][9] - 0.008277f * x7[1][1][10] + 0.036079f * x7[1][1][11]
	 + 0.041870f * x7[1][1][12] - 0.020649f * x7[1][1][13] - 0.050259f * x7[1][1][14] + 0.080654f * x7[1][1][15]
	 - 0.057011f * x7[1][2][0] - 0.015590f * x7[1][2][1] + 0.031755f * x7[1][2][2] - 0.068013f * x7[1][2][3]
	 - 0.017641f * x7[1][2][4] + 0.067403f * x7[1][2][5] + 0.013452f * x7[1][2][6] + 0.008757f * x7[1][2][7]
	 - 0.013924f * x7[1][2][8] - 0.008930f * x7[1][2][9] + 0.018468f * x7[1][2][10] + 0.056494f * x7[1][2][11]
	 + 0.002507f * x7[1][2][12] + 0.198477f * x7[1][2][13] - 0.037056f * x7[1][2][14] - 0.010789f * x7[1][2][15]
	 + 0.034694f * x7[1][3][0] + 0.024787f * x7[1][3][1] - 0.037680f * x7[1][3][2] - 0.044655f * x7[1][3][3]
	 - 0.057404f * x7[1][3][4] + 0.000781f * x7[1][3][5] - 0.028900f * x7[1][3][6] - 0.068508f * x7[1][3][7]
	 - 0.052084f * x7[1][3][8] - 0.017397f * x7[1][3][9] + 0.032406f * x7[1][3][10] + 0.045152f * x7[1][3][11]
	 + 0.143626f * x7[1][3][12] - 0.019855f * x7[1][3][13] - 0.067758f * x7[1][3][14] - 0.127761f * x7[1][3][15]
	 + 0.071753f * x7[2][0][0] + 0.052176f * x7[2][0][1] - 0.224698f * x7[2][0][2] - 0.034868f * x7[2][0][3]
	 - 0.037483f * x7[2][0][4] - 0.012369f * x7[2][0][5] - 0.044460f * x7[2][0][6] + 0.001039f * x7[2][0][7]
	 + 0.017541f * x7[2][0][8] - 0.005693f * x7[2][0][9] - 0.002878f * x7[2][0][10] - 0.083282f * x7[2][0][11]
	 + 0.128113f * x7[2][0][12] + 0.009543f * x7[2][0][13] - 0.031332f * x7[2][0][14] - 0.005114f * x7[2][0][15]
	 + 0.057141f * x7[2][1][0] + 0.167608f * x7[2][1][1] - 0.064477f * x7[2][1][2] + 0.004148f * x7[2][1][3]
	 + 0.290081f * x7[2][1][4] + 0.091621f * x7[2][1][5] + 0.084278f * x7[2][1][6] - 0.009256f * x7[2][1][7]
	 + 0.041793f * x7[2][1][8] - 0.001092f * x7[2][1][9] - 0.025605f * x7[2][1][10] + 0.080678f * x7[2][1][11]
	 - 0.571075f * x7[2][1][12] - 0.332925f * x7[2][1][13] - 0.650685f * x7[2][1][14] - 0.031351f * x7[2][1][15]
	 - 0.040585f * x7[2][2][0] - 0.139566f * x7[2][2][1] + 0.075421f * x7[2][2][2] - 0.116302f * x7[2][2][3]
	 - 0.023727f * x7[2][2][4] - 0.077133f * x7[2][2][5] - 0.072475f * x7[2][2][6] + 0.055853f * x7[2][2][7]
	 - 0.018269f * x7[2][2][8] - 0.004931f * x7[2][2][9] + 0.042281f * x7[2][2][10] - 0.006949f * x7[2][2][11]
	 - 0.119747f * x7[2][2][12] + 0.168522f * x7[2][2][13] - 0.040961f * x7[2][2][14] - 0.013312f * x7[2][2][15]
	 + 0.070131f * x7[2][3][0] + 0.051222f * x7[2][3][1] + 0.050228f * x7[2][3][2] + 0.035647f * x7[2][3][3]
	 + 0.011035f * x7[2][3][4] - 0.011817f * x7[2][3][5] - 0.083052f * x7[2][3][6] + 0.070293f * x7[2][3][7]
	 + 0.039633f * x7[2][3][8] - 0.004498f * x7[2][3][9] - 0.052294f * x7[2][3][10] - 0.002285f * x7[2][3][11]
	 + 0.036214f * x7[2][3][12] + 0.046421f * x7[2][3][13] - 0.026143f * x7[2][3][14] + 0.022596f * x7[2][3][15]
	 + 0.026729f * x7[3][0][0] - 0.056856f * x7[3][0][1] - 0.131092f * x7[3][0][2] + 0.089981f * x7[3][0][3]
	 - 0.020858f * x7[3][0][4] + 0.013948f * x7[3][0][5] + 0.000591f * x7[3][0][6] - 0.099104f * x7[3][0][7]
	 - 0.009382f * x7[3][0][8] + 0.001167f * x7[3][0][9] - 0.070043f * x7[3][0][10] + 0.128936f * x7[3][0][11]
	 + 0.015910f * x7[3][0][12] - 0.064780f * x7[3][0][13] - 0.011075f * x7[3][0][14] - 0.051120f * x7[3][0][15]
	 - 0.011004f * x7[3][1][0] + 0.079738f * x7[3][1][1] + 0.109234f * x7[3][1][2] - 0.040372f * x7[3][1][3]
	 + 0.141259f * x7[3][1][4] - 0.097858f * x7[3][1][5] - 0.042250f * x7[3][1][6] - 0.130841f * x7[3][1][7]
	 - 0.065871f * x7[3][1][8] - 0.004063f * x7[3][1][9] + 0.006050f * x7[3][1][10] - 0.134520f * x7[3][1][11]
	 + 0.000895f * x7[3][1][12] + 0.112579f * x7[3][1][13] + 0.040462f * x7[3][1][14] + 0.149441f * x7[3][1][15]
	 + 0.107474f * x7[3][2][0] - 0.084844f * x7[3][2][1] - 0.011077f * x7[3][2][2] - 0.064738f * x7[3][2][3]
	 + 0.233688f * x7[3][2][4] - 0.080084f * x7[3][2][5] + 0.045516f * x7[3][2][6] + 0.239608f * x7[3][2][7]
	 + 0.047866f * x7[3][2][8] + 0.002003f * x7[3][2][9] + 0.043908f * x7[3][2][10] + 0.090468f * x7[3][2][11]
	 - 0.071450f * x7[3][2][12] + 0.034065f * x7[3][2][13] + 0.066245f * x7[3][2][14] - 0.108883f * x7[3][2][15]
	 - 0.079600f * x7[3][3][0] - 0.017223f * x7[3][3][1] + 0.080460f * x7[3][3][2] + 0.101499f * x7[3][3][3]
	 + 0.002619f * x7[3][3][4] + 0.035318f * x7[3][3][5] - 0.022203f * x7[3][3][6] + 0.021690f * x7[3][3][7]
	 + 0.017319f * x7[3][3][8] - 0.016799f * x7[3][3][9] - 0.086389f * x7[3][3][10] - 0.112742f * x7[3][3][11]
	 + 0.051102f * x7[3][3][12] - 0.047228f * x7[3][3][13] - 0.026379f * x7[3][3][14] - 0.090876f * x7[3][3][15];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.271101f
	 - 0.024621f * x7[0][0][0] + 0.058879f * x7[0][0][1] - 0.054225f * x7[0][0][2] + 0.134187f * x7[0][0][3]
	 + 0.056073f * x7[0][0][4] - 0.021045f * x7[0][0][5] - 0.016889f * x7[0][0][6] + 0.021707f * x7[0][0][7]
	 - 0.054902f * x7[0][0][8] + 0.001623f * x7[0][0][9] + 0.011618f * x7[0][0][10] - 0.055132f * x7[0][0][11]
	 - 0.064075f * x7[0][0][12] + 0.086371f * x7[0][0][13] + 0.042708f * x7[0][0][14] + 0.107525f * x7[0][0][15]
	 - 0.048848f * x7[0][1][0] + 0.111616f * x7[0][1][1] + 0.048224f * x7[0][1][2] + 0.012164f * x7[0][1][3]
	 - 0.337618f * x7[0][1][4] - 0.026984f * x7[0][1][5] + 0.002931f * x7[0][1][6] + 0.048273f * x7[0][1][7]
	 - 0.017990f * x7[0][1][8] + 0.001662f * x7[0][1][9] - 0.049497f * x7[0][1][10] + 0.168278f * x7[0][1][11]
	 - 0.096593f * x7[0][1][12] - 0.038180f * x7[0][1][13] - 0.049477f * x7[0][1][14] + 0.330778f * x7[0][1][15]
	 + 0.017570f * x7[0][2][0] + 0.056131f * x7[0][2][1] + 0.063010f * x7[0][2][2] - 0.010017f * x7[0][2][3]
	 - 0.281677f * x7[0][2][4] + 0.019167f * x7[0][2][5] - 0.001488f * x7[0][2][6] + 0.006705f * x7[0][2][7]
	 - 0.025053f * x7[0][2][8] + 0.002976f * x7[0][2][9] + 0.019393f * x7[0][2][10] - 0.287645f * x7[0][2][11]
	 - 0.113695f * x7[0][2][12] - 0.022552f * x7[0][2][13] - 0.004761f * x7[0][2][14] + 0.035680f * x7[0][2][15]
	 - 0.058140f * x7[0][3][0] + 0.009798f * x7[0][3][1] - 0.096955f * x7[0][3][2] - 0.038135f * x7[0][3][3]
	 + 0.000075f * x7[0][3][4] + 0.029273f * x7[0][3][5] - 0.015014f * x7[0][3][6] - 0.004013f * x7[0][3][7]
	 + 0.067891f * x7[0][3][8] - 0.000708f * x7[0][3][9] - 0.001741f * x7[0][3][10] + 0.060030f * x7[0][3][11]
	 - 0.017024f * x7[0][3][12] - 0.019307f * x7[0][3][13] + 0.021987f * x7[0][3][14] + 0.022215f * x7[0][3][15]
	 + 0.078732f * x7[1][0][0] - 0.058292f * x7[1][0][1] - 0.005417f * x7[1][0][2] + 0.055079f * x7[1][0][3]
	 - 0.073863f * x7[1][0][4] - 0.059586f * x7[1][0][5] + 0.007680f * x7[1][0][6] + 0.076489f * x7[1][0][7]
	 - 0.068801f * x7[1][0][8] + 0.002308f * x7[1][0][9] - 0.006811f * x7[1][0][10] + 0.110571f * x7[1][0][11]
	 + 0.273078f * x7[1][0][12] - 0.097478f * x7[1][0][13] - 0.099565f * x7[1][0][14] + 0.097260f * x7[1][0][15]
	 - 0.024485f * x7[1][1][0] - 0.311605f * x7[1][1][1] - 0.042027f * x7[1][1][2] + 0.026149f * x7[1][1][3]
	 + 0.036603f * x7[1][1][4] - 0.032067f * x7[1][1][5] - 0.027867f * x7[1][1][6] - 0.088786f * x7[1][1][7]
	 + 0.083086f * x7[1][1][8] + 0.011191f * x7[1][1][9] + 0.043685f * x7[1][1][10] - 0.161792f * x7[1][1][11]
	 + 0.246505f * x7[1][1][12] + 0.075807f * x7[1][1][13] - 0.125049f * x7[1][1][14] + 0.067081f * x7[1][1][15]
	 + 0.028544f * x7[1][2][0] - 0.063958f * x7[1][2][1] - 0.064877f * x7[1][2][2] + 0.009739f * x7[1][2][3]
	 - 0.040683f * x7[1][2][4] + 0.032432f * x7[1][2][5] + 0.018833f * x7[1][2][6] + 0.059768f * x7[1][2][7]
	 + 0.009562f * x7[1][2][8] + 0.001259f * x7[1][2][9] - 0.008311f * x7[1][2][10] + 0.041103f * x7[1][2][11]
	 - 0.033708f * x7[1][2][12] + 0.236319f * x7[1][2][13] - 0.029687f * x7[1][2][14] - 0.027870f * x7[1][2][15]
	 + 0.088766f * x7[1][3][0] - 0.022979f * x7[1][3][1] - 0.025386f * x7[1][3][2] + 0.014694f * x7[1][3][3]
	 - 0.011154f * x7[1][3][4] + 0.011143f * x7[1][3][5] + 0.008475f * x7[1][3][6] - 0.028175f * x7[1][3][7]
	 - 0.015928f * x7[1][3][8] + 0.009651f * x7[1][3][9] - 0.000721f * x7[1][3][10] + 0.004439f * x7[1][3][11]
	 + 0.030206f * x7[1][3][12] - 0.039765f * x7[1][3][13] - 0.004865f * x7[1][3][14] - 0.065140f * x7[1][3][15]
	 - 0.085318f * x7[2][0][0] + 0.023116f * x7[2][0][1] - 0.059713f * x7[2][0][2] - 0.034089f * x7[2][0][3]
	 - 0.000098f * x7[2][0][4] - 0.000958f * x7[2][0][5] - 0.012877f * x7[2][0][6] - 0.023195f * x7[2][0][7]
	 + 0.008590f * x7[2][0][8] + 0.013946f * x7[2][0][9] + 0.004697f * x7[2][0][10] - 0.027903f * x7[2][0][11]
	 + 0.038430f * x7[2][0][12] + 0.014825f * x7[2][0][13] - 0.007782f * x7[2][0][14] - 0.004653f * x7[2][0][15]
	 - 0.001606f * x7[2][1][0] + 0.377806f * x7[2][1][1] + 0.031263f * x7[2][1][2] + 0.058950f * x7[2][1][3]
	 + 0.310730f * x7[2][1][4] - 0.078042f * x7[2][1][5] - 0.038914f * x7[2][1][6] + 0.014988f * x7[2][1][7]
	 - 0.034764f * x7[2][1][8] + 0.009203f * x7[2][1][9] - 0.035530f * x7[2][1][10] + 0.004617f * x7[2][1][11]
	 - 0.417248f * x7[2][1][12] - 0.272981f * x7[2][1][13] - 0.988937f * x7[2][1][14] - 0.226616f * x7[2][1][15]
	 + 0.062231f * x7[2][2][0] - 0.196049f * x7[2][2][1] - 0.034241f * x7[2][2][2] + 0.110650f * x7[2][2][3]
	 - 0.123398f * x7[2][2][4] - 0.197886f * x7[2][2][5] + 0.040285f * x7[2][2][6] - 0.017385f * x7[2][2][7]
	 + 0.085385f * x7[2][2][8] - 0.006085f * x7[2][2][9] - 0.116279f * x7[2][2][10] - 0.037524f * x7[2][2][11]
	 - 0.061382f * x7[2][2][12] + 0.197938f * x7[2][2][13] - 0.061408f * x7[2][2][14] - 0.041352f * x7[2][2][15]
	 + 0.018631f * x7[2][3][0] + 0.005333f * x7[2][3][1] - 0.016372f * x7[2][3][2] + 0.020989f * x7[2][3][3]
	 + 0.005561f * x7[2][3][4] + 0.002946f * x7[2][3][5] + 0.032819f * x7[2][3][6] + 0.033435f * x7[2][3][7]
	 - 0.019682f * x7[2][3][8] + 0.022475f * x7[2][3][9] + 0.069829f * x7[2][3][10] + 0.020096f * x7[2][3][11]
	 - 0.004795f * x7[2][3][12] + 0.003511f * x7[2][3][13] + 0.007473f * x7[2][3][14] - 0.022843f * x7[2][3][15]
	 + 0.004745f * x7[3][0][0] - 0.009838f * x7[3][0][1] + 0.038310f * x7[3][0][2] + 0.024658f * x7[3][0][3]
	 + 0.028832f * x7[3][0][4] + 0.065714f * x7[3][0][5] - 0.007118f * x7[3][0][6] + 0.008069f * x7[3][0][7]
	 - 0.003532f * x7[3][0][8] - 0.004145f * x7[3][0][9] + 0.007235f * x7[3][0][10] - 0.017626f * x7[3][0][11]
	 - 0.022937f * x7[3][0][12] - 0.016697f * x7[3][0][13] - 0.002291f * x7[3][0][14] - 0.001620f * x7[3][0][15]
	 + 0.035343f * x7[3][1][0] + 0.044383f * x7[3][1][1] - 0.011633f * x7[3][1][2] - 0.002903f * x7[3][1][3]
	 + 0.136236f * x7[3][1][4] + 0.034122f * x7[3][1][5] + 0.024281f * x7[3][1][6] - 0.263852f * x7[3][1][7]
	 + 0.042547f * x7[3][1][8] - 0.009004f * x7[3][1][9] - 0.001629f * x7[3][1][10] - 0.034605f * x7[3][1][11]
	 + 0.069033f * x7[3][1][12] + 0.077014f * x7[3][1][13] - 0.022454f * x7[3][1][14] + 0.002929f * x7[3][1][15]
	 - 0.006698f * x7[3][2][0] - 0.054092f * x7[3][2][1] + 0.147933f * x7[3][2][2] + 0.045879f * x7[3][2][3]
	 + 0.170267f * x7[3][2][4] + 0.104582f * x7[3][2][5] - 0.047455f * x7[3][2][6] + 0.231869f * x7[3][2][7]
	 - 0.016948f * x7[3][2][8] + 0.018155f * x7[3][2][9] + 0.098886f * x7[3][2][10] + 0.133153f * x7[3][2][11]
	 + 0.022021f * x7[3][2][12] + 0.022819f * x7[3][2][13] + 0.003636f * x7[3][2][14] - 0.025311f * x7[3][2][15]
	 - 0.052366f * x7[3][3][0] - 0.052797f * x7[3][3][1] + 0.089536f * x7[3][3][2] - 0.049472f * x7[3][3][3]
	 + 0.031116f * x7[3][3][4] + 0.025677f * x7[3][3][5] - 0.025008f * x7[3][3][6] - 0.069321f * x7[3][3][7]
	 - 0.035589f * x7[3][3][8] - 0.007352f * x7[3][3][9] - 0.060448f * x7[3][3][10] - 0.046532f * x7[3][3][11]
	 - 0.044604f * x7[3][3][12] - 0.050013f * x7[3][3][13] + 0.018837f * x7[3][3][14] - 0.015804f * x7[3][3][15];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

}



void Fy4000_3::find(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
{
	ASSERT(patch.size() == 16);

	for(size_t x=0; x < patch.size(); x++) {
		for(size_t y=0; y < patch.size(); y++) {
			// TODO: check
			float value = (static_cast<float>((patch.data[patch.size() * x + y].pixel.y)) / 255.0f) - static_cast<float>(meanBrightness);
			in_step[y][x][0] = value;
		}
	}

	cnn(in_step);
	//std::cout << "scores[0]=" << scores[0] << " scores[1]=" << scores[1] << " scores[2]=" << scores[2] << std::endl;
}
double Fy4000_3::getRadius() {return scores[0];}
Vector2d Fy4000_3::getCenter() {return Vector2d(scores[1], scores[2]);}
