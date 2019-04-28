#include "Fy1500_2.h"

#include <emmintrin.h>
#include <math.h>
void Fy1500_2::cnn(float x0[16][16][1])
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
			x1[i][j][0] = 0.14149290323257446f;
			x1[i][j][1] = 0.11254548281431198f;
			x1[i][j][2] = 0.08359338343143463f;
			x1[i][j][3] = -0.3711223602294922f;
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

					w = _mm_set_ps(-0.29951557517051697f, -0.042512811720371246f, -0.6158457398414612f, -0.03534487634897232f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.19097115099430084f, -0.8305932879447937f, -0.012155035510659218f, 0.5010180473327637f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.004641752690076828f, -0.46164390444755554f, -0.019096458330750465f, 0.28490954637527466f);
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

					w = _mm_set_ps(-0.634843111038208f, 0.019706690683960915f, -0.7707207202911377f, 0.11230916529893875f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.08079458773136139f, -0.14551886916160583f, 0.08277289569377899f, -0.19182038307189941f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.12080492079257965f, -0.22106465697288513f, 0.5135176181793213f, -0.9976816773414612f);
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

					w = _mm_set_ps(-0.24635732173919678f, 0.5950180888175964f, 0.5673109292984009f, 0.5143070816993713f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.17089255154132843f, 0.9082926511764526f, 0.44692325592041016f, 0.580011785030365f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.2707919776439667f, 0.15404367446899414f, -0.1815149486064911f, -0.6470063328742981f);
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
	static float x2 alignas(16) [16][16][4] = {0};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x2[i][j][0] = -0.017952801659703255f;
			x2[i][j][1] = 0.1476670205593109f;
			x2[i][j][2] = 0.09788747131824493f;
			x2[i][j][3] = -0.14887136220932007f;
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

					w = _mm_set_ps(0.5179833769798279f, 0.14305007457733154f, -0.15076488256454468f, 0.39913681149482727f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.011148687452077866f, -0.28528836369514465f, -0.6402507424354553f, 0.3096420168876648f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13555864989757538f, 0.32271724939346313f, -0.7447779178619385f, -0.5049409866333008f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.48364192247390747f, 0.024290522560477257f, -0.027774741873145103f, -0.5328748822212219f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.18939748406410217f, -0.43685901165008545f, 0.21273799240589142f, -1.0953011512756348f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3361549973487854f, 0.2371158003807068f, -0.9445690512657166f, -0.14455047249794006f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.43931809067726135f, 0.09190241992473602f, -0.2315307855606079f, -0.642038881778717f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0626014694571495f, 0.12867432832717896f, -0.07453013211488724f, 0.05515095964074135f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.5330191254615784f, 0.29261696338653564f, 0.1336371898651123f, -0.2710295021533966f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07998738437891006f, -0.13016274571418762f, -0.9750950336456299f, 0.8876855969429016f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.051645807921886444f, -1.4013508558273315f, -0.40365132689476013f, 0.269064337015152f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2257986068725586f, 0.5509620904922485f, -0.22663633525371552f, 0.7920385003089905f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.4938294589519501f, -0.9027146100997925f, 0.22352749109268188f, -0.7189694046974182f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5293824672698975f, 0.19709141552448273f, -0.6654446721076965f, 0.22015666961669922f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7359017729759216f, 0.4757489860057831f, 0.1829897165298462f, -0.6770718693733215f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20638185739517212f, 0.169114351272583f, 0.3818497657775879f, 0.03067873604595661f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.0464140959084034f, -0.013566042296588421f, 0.24889026582241058f, -0.24892449378967285f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20502890646457672f, 0.1998348832130432f, -0.5266227722167969f, 0.10609129071235657f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15179435908794403f, -0.19243791699409485f, -0.14354687929153442f, 0.6826489567756653f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18776920437812805f, 0.36053845286369324f, 0.2267162948846817f, 0.1956857442855835f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.7136876583099365f, 0.08686323463916779f, -0.1806437075138092f, -0.2071204036474228f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16547881066799164f, 0.07402597367763519f, 0.2983320951461792f, 0.030337942764163017f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23000045120716095f, -0.8017011880874634f, -0.35177409648895264f, 0.3738345503807068f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1338084489107132f, 0.25070860981941223f, 0.23630274832248688f, -0.08368150889873505f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 16)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.49358004331588745f, -0.9049330949783325f, -0.20041169226169586f, 0.22888489067554474f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.290824055671692f, 0.040531668812036514f, -0.2036646008491516f, -0.12248127162456512f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1824222207069397f, 0.2688882350921631f, -0.029790755361318588f, -0.06598120927810669f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1979358196258545f, -0.38965362310409546f, -0.4654191732406616f, 0.04728219658136368f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.3528844118118286f, -0.25585004687309265f, -0.9296444058418274f, 0.1911260038614273f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10782427340745926f, 0.523862898349762f, 0.5965712666511536f, 0.39309582114219666f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5763721466064453f, -0.4644998610019684f, 0.6528669595718384f, -0.2894647717475891f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1887763887643814f, -0.2928315997123718f, 0.0734744444489479f, -0.016889743506908417f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.4005924165248871f, 0.14356327056884766f, -0.3835940957069397f, 0.22852502763271332f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.21896176040172577f, -0.06465116143226624f, 0.4204936623573303f, -0.18393298983573914f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2363341748714447f, -0.9736528992652893f, -0.42415836453437805f, -0.3841037154197693f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.8237643241882324f, -0.5063252449035645f, 0.3563235104084015f, -0.6342613101005554f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
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
		}
	}

 	// Maxpool Layer 
	static float x3[8][8][4] = {0};
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
		}
	}

 	// Convolution Layer
	static float x4 alignas(16) [8][8][8] = {0};
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{
			x4[i][j][0] = -0.06321907788515091f;
			x4[i][j][1] = -0.20904633402824402f;
			x4[i][j][2] = -0.22771120071411133f;
			x4[i][j][3] = -0.17742587625980377f;
			x4[i][j][4] = -0.3083110749721527f;
			x4[i][j][5] = -0.23906466364860535f;
			x4[i][j][6] = -0.4099181294441223f;
			x4[i][j][7] = -0.14310301840305328f;
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

					w = _mm_set_ps(0.1558258831501007f, 0.31625959277153015f, 0.1927758753299713f, 0.7865510582923889f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20646555721759796f, 0.2242601364850998f, -0.4779425263404846f, 0.19561918079853058f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19518627226352692f, -0.21177397668361664f, -0.0887654721736908f, -0.6371820569038391f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5171574950218201f, -0.12086554616689682f, 0.6828232407569885f, 0.04803707078099251f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4462184011936188f, -0.3387019634246826f, -0.13097350299358368f, -0.031065678223967552f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.016321830451488495f, -0.07008830457925797f, 0.8699040412902832f, -0.03998477756977081f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0520881712436676f, 0.20622196793556213f, -0.5858598351478577f, 0.8055903315544128f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24563100934028625f, -0.03973187878727913f, 0.318176805973053f, -0.20778435468673706f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.4482085406780243f, 0.18074791133403778f, 0.059783369302749634f, -0.253799706697464f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24516668915748596f, -0.2181350588798523f, -0.3400377333164215f, 0.3299926519393921f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.05474449694156647f, 0.2100837081670761f, 0.5244954228401184f, 0.03787774592638016f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.004987346474081278f, -0.07323004305362701f, -0.010209789499640465f, -0.22434215247631073f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3691641688346863f, 0.19270330667495728f, -0.35534974932670593f, 0.055390775203704834f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23510609567165375f, -0.4743228256702423f, -0.4823654592037201f, -0.24206559360027313f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10227984935045242f, -0.029147010296583176f, -0.8411033153533936f, 0.04026975482702255f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24185332655906677f, 0.003700579982250929f, -0.6360729336738586f, 0.1595982164144516f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.31846290826797485f, 0.5080881714820862f, -0.27773329615592957f, 0.09758962690830231f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5239970088005066f, -0.8454416990280151f, 0.0611078180372715f, -0.5377366542816162f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07771111279726028f, 0.350225031375885f, -0.38660070300102234f, 0.21158626675605774f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23511065542697906f, -0.10672449320554733f, -0.4499768018722534f, 0.4171382188796997f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1108640804886818f, -0.05996684730052948f, -0.42373740673065186f, 0.241924449801445f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.21641264855861664f, -0.06461489945650101f, 0.14534182846546173f, -0.26882073283195496f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10685212910175323f, -0.18450124561786652f, 0.2962723672389984f, -0.044375523924827576f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4033542275428772f, -0.25980526208877563f, 0.12068948149681091f, 0.783207893371582f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 8)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.20225736498832703f, 0.1740586757659912f, -0.36842259764671326f, -0.32728999853134155f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5545554161071777f, 0.006806107237935066f, -0.3822641968727112f, -0.38420483469963074f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11879082024097443f, 0.20326951146125793f, 0.2894936203956604f, -0.7744701504707336f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6498332023620605f, -0.2599472999572754f, 0.6058487296104431f, -0.19798095524311066f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8282896876335144f, 0.011399705894291401f, 0.12048213183879852f, 0.36759448051452637f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11299636214971542f, -0.2898801267147064f, 0.6498626470565796f, -0.04272690415382385f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.33295631408691406f, -0.5446929931640625f, -0.0656445100903511f, 0.5055597424507141f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.393330454826355f, 0.36200010776519775f, 0.4248844385147095f, 0.036410797387361526f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.3208591341972351f, 0.23728296160697937f, -1.1004151105880737f, 0.2122150957584381f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6010763645172119f, -0.5501278638839722f, -0.49611324071884155f, -0.18027155101299286f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.11183646321296692f, 0.21003670990467072f, 0.37163427472114563f, -0.3465065360069275f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6506394743919373f, 0.9423313736915588f, 0.020481513813138008f, -0.5268182158470154f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11725303530693054f, 0.5365093946456909f, 0.37937018275260925f, -0.37146908044815063f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4871695041656494f, 0.0738820880651474f, 0.2721002995967865f, -0.3881637752056122f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06367114186286926f, -0.4652878940105438f, -0.9562394022941589f, -0.07291491329669952f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18661265075206757f, 0.6628214716911316f, 0.13404153287410736f, -0.43119916319847107f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.03965827822685242f, 0.017325207591056824f, -0.047498349100351334f, -0.5793511271476746f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10060171037912369f, -0.7655443549156189f, 0.01195390336215496f, 0.2482006549835205f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.43615588545799255f, -0.264069139957428f, 0.372408002614975f, 0.16949908435344696f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08491472154855728f, -0.17381148040294647f, -0.17591342329978943f, 0.5074724555015564f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.14217032492160797f, -0.08555058389902115f, 0.7098881006240845f, -0.3854120969772339f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09956956654787064f, 0.6649160981178284f, -0.24623218178749084f, 0.2428247034549713f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11763790994882584f, -0.033962421119213104f, -0.49118730425834656f, -0.41483601927757263f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2987655699253082f, -0.0031991172581911087f, 0.11214647442102432f, -1.3766121864318848f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 8)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.2594161629676819f, -0.026453662663698196f, -0.6570960879325867f, -0.983505129814148f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4779469668865204f, -0.10544423013925552f, -0.08859788626432419f, -0.35219231247901917f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7030404210090637f, -0.007043845020234585f, 0.1554902344942093f, -0.47338712215423584f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4056153893470764f, -0.1255430281162262f, 0.324863463640213f, -0.0040487851947546005f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4214355945587158f, -0.13328254222869873f, -0.2712188959121704f, 0.591242253780365f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.43592700362205505f, 0.22700637578964233f, 0.3338007628917694f, 0.14726725220680237f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0766914039850235f, -0.27174392342567444f, -0.7017195820808411f, 0.22831974923610687f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12902480363845825f, -0.06877078115940094f, 0.19945921003818512f, -0.06938932090997696f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.17856010794639587f, 0.12431558221578598f, -0.24143008887767792f, 0.09102339297533035f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1809156984090805f, -0.10538513213396072f, -0.22595307230949402f, -0.09515516459941864f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.37124374508857727f, -0.04844874516129494f, -0.05728984996676445f, -0.6053875088691711f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0925007089972496f, 0.016344981268048286f, 0.529509961605072f, -0.0976702868938446f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07987578213214874f, 0.6092477440834045f, -0.08983366191387177f, -0.2153359204530716f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1510985940694809f, 0.3328632116317749f, 0.5327436327934265f, 0.05722297728061676f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.9683732390403748f, 0.2577310800552368f, -0.39633089303970337f, -0.3891235291957855f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.36931419372558594f, -0.7514039278030396f, 0.05266774818301201f, -0.04203016683459282f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.2956756055355072f, -0.4678823947906494f, 0.012707691639661789f, -0.060200706124305725f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5678749084472656f, -0.38735640048980713f, 0.07767391949892044f, -0.35005566477775574f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.01852560229599476f, -0.05985603854060173f, 0.7809085249900818f, -0.30492278933525085f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18927469849586487f, 0.10606420785188675f, 0.15588489174842834f, -0.19933529198169708f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.30773410201072693f, 0.05304335057735443f, 0.20467884838581085f, 0.07086722552776337f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04793456196784973f, 0.3062141239643097f, 0.04112090915441513f, -0.27085089683532715f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22347404062747955f, 0.18734276294708252f, -0.3488759994506836f, -0.07707249373197556f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32525742053985596f, -0.14819349348545074f, 0.16694512963294983f, 0.1285305619239807f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
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
		}
	}

 	// Maxpool Layer 
	static float x5[4][4][8] = {0};
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
		}
	}

 	// Convolution Layer
	static float x6 alignas(16) [4][4][8] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x6[i][j][0] = 0.04245574027299881f;
			x6[i][j][1] = 0.1278551071882248f;
			x6[i][j][2] = -0.4631645679473877f;
			x6[i][j][3] = 0.7337232828140259f;
			x6[i][j][4] = -0.5277491807937622f;
			x6[i][j][5] = -0.17725269496440887f;
			x6[i][j][6] = -0.11543360352516174f;
			x6[i][j][7] = 0.3106488287448883f;
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

					w = _mm_set_ps(0.07438274472951889f, 0.5122897624969482f, -0.5101463794708252f, 0.7207766771316528f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3452832102775574f, -0.09724368155002594f, -0.31232038140296936f, 0.37094131112098694f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15223200619220734f, 0.5750941038131714f, -0.5596245527267456f, 0.11823802441358566f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23300132155418396f, -0.08540601283311844f, 0.43232792615890503f, -0.5649205446243286f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07724645733833313f, 0.09480277448892593f, 0.1669377237558365f, -0.2560656666755676f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07267773896455765f, 0.12314162403345108f, 0.1474756896495819f, 0.027303418144583702f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1666187196969986f, 0.3827887177467346f, 0.6697883605957031f, 0.026884647086262703f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17976118624210358f, -0.02113783359527588f, -0.26680803298950195f, -0.26026779413223267f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10365532338619232f, 0.18648944795131683f, -1.0583757162094116f, -0.4367213547229767f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04162788763642311f, -0.09729904681444168f, -0.07097935676574707f, -0.45246368646621704f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0792670026421547f, -0.18302607536315918f, -1.9176301956176758f, 0.12343551218509674f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05443183705210686f, 0.027729956433176994f, -0.02573450841009617f, -0.2284879982471466f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.008658912032842636f, -0.29851946234703064f, -0.09722674638032913f, -0.7745174169540405f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.22603346407413483f, 0.030050218105316162f, 0.06704263389110565f, -0.03835495561361313f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0377466045320034f, -0.01124569308012724f, 0.1454562544822693f, 0.07873167097568512f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1047796681523323f, -0.0833883062005043f, -0.15567706525325775f, 0.037869203835725784f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.2231963574886322f, 0.298044353723526f, 0.20567849278450012f, 0.7345849275588989f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.018410688266158104f, -0.14054788649082184f, 0.13048098981380463f, 0.18549610674381256f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0761711522936821f, 0.4073951840400696f, 0.08159585297107697f, 0.052592698484659195f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1624089777469635f, -0.006725248880684376f, 0.4053262174129486f, 0.25109437108039856f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.007504125591367483f, -0.2711482644081116f, -0.3426981568336487f, -0.2528754472732544f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0035782281775027514f, -0.014832367189228535f, -0.1732068657875061f, -0.700351893901825f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1736185997724533f, 0.1558057963848114f, -0.6653266549110413f, -0.2920331358909607f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05409708246588707f, 0.1864473521709442f, -0.06599577516317368f, -0.7497085928916931f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1523582637310028f, 0.29145634174346924f, -0.3872385621070862f, -0.38980168104171753f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05845783278346062f, -0.011352279223501682f, 0.1437249630689621f, 0.19895894825458527f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15572334825992584f, -0.40365564823150635f, -0.09502583742141724f, -0.37407705187797546f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07351050525903702f, -0.012697300873696804f, -0.42004984617233276f, -0.24952766299247742f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22058117389678955f, -0.050463445484638214f, -0.10842498391866684f, -0.6757423877716064f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10924611985683441f, -0.20169420540332794f, -0.1554146260023117f, 0.09852706640958786f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.23610460758209229f, 0.469143807888031f, -0.05719413608312607f, 0.34276485443115234f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0076354132033884525f, 0.1968328058719635f, -0.28240862488746643f, -0.014428304508328438f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.0077586486004292965f, -0.16653719544410706f, -0.04872320964932442f, 0.6305958032608032f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0070656295865774155f, 0.4686712622642517f, -0.10048630833625793f, -0.31997302174568176f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10078749060630798f, -0.5027510523796082f, 0.8590943217277527f, 0.4209868609905243f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.014250602573156357f, -0.11282838135957718f, -2.0669174194335938f, -0.3398447632789612f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.016610827296972275f, 0.18907864391803741f, -0.8294491171836853f, -0.04210803285241127f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05078002065420151f, -0.04048527404665947f, -0.024572230875492096f, -0.8598272800445557f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.11587631702423096f, -0.11394745856523514f, 0.034277528524398804f, -0.131386399269104f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0012481471057981253f, -0.1595933586359024f, -1.9248995780944824f, 0.03482057899236679f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10139325261116028f, 0.16296957433223724f, -0.7864660620689392f, -0.3167605400085449f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16744089126586914f, 0.12118981033563614f, 0.4574045240879059f, 0.2541564702987671f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.01594654656946659f, -0.33389410376548767f, -0.37723249197006226f, -0.45346930623054504f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.038159046322107315f, 0.01253914088010788f, -0.752568244934082f, -0.9660411477088928f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09880035370588303f, 0.30750584602355957f, -0.5411238074302673f, -0.757875919342041f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0025179400108754635f, -0.04250314459204674f, 0.729521632194519f, 0.04692721739411354f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10228326916694641f, 0.08790178596973419f, -0.2935512363910675f, 0.66656893491745f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0062788077630102634f, 0.23334482312202454f, 0.41434913873672485f, 0.2830781638622284f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 1;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.06197288632392883f, -0.1719253957271576f, -0.5393911600112915f, 0.45587894320487976f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23904897272586823f, 0.162790447473526f, 0.19798918068408966f, 0.2744338810443878f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.016618967056274414f, 0.6276511549949646f, 0.45594871044158936f, 0.04659641534090042f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.015030882321298122f, -0.00505617493763566f, 0.1930433213710785f, 0.19251172244548798f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.144529789686203f, -0.06427933275699615f, -0.7231267690658569f, -0.1159634217619896f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1690160185098648f, -0.036474961787462234f, -0.4427938163280487f, -0.10054747015237808f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.020579859614372253f, -0.2875760495662689f, -0.3911004662513733f, -0.08112265914678574f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0970599427819252f, 0.07051382958889008f, 0.004941083025187254f, -0.5204646587371826f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1154940202832222f, -0.426540344953537f, -0.5207667350769043f, -0.33026307821273804f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.011495934799313545f, -0.10523927211761475f, 0.0956854596734047f, -1.4656970500946045f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.115081787109375f, -0.5939414501190186f, -0.13783156871795654f, -0.022322824224829674f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12901802361011505f, 0.013002831488847733f, -0.29234716296195984f, 0.0889783501625061f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.20177535712718964f, -0.5422966480255127f, -0.31958645582199097f, -0.7452219724655151f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12319263070821762f, -0.07551395148038864f, -0.5630238056182861f, -0.24990642070770264f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08316481858491898f, 0.23316915333271027f, -0.4728137254714966f, 0.07322452962398529f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07689855992794037f, 0.2516680657863617f, 0.32877519726753235f, 0.030869903042912483f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.02008034847676754f, 0.10205790400505066f, 0.039227891713380814f, 0.20482097566127777f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.014245479367673397f, 0.09724152833223343f, 0.00044138525845482945f, -0.1413126140832901f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.20029404759407043f, 0.29415032267570496f, 0.3899618685245514f, 0.3533586263656616f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14790299534797668f, 0.08244823664426804f, 0.20535606145858765f, 0.09100642055273056f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09466914087533951f, -0.026466896757483482f, -0.5479981303215027f, -0.3764171302318573f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06459543108940125f, -0.03961172327399254f, -0.7551659941673279f, -0.1947919726371765f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04441211000084877f, 0.08241245895624161f, 0.37617334723472595f, -0.2549441158771515f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2594764530658722f, -0.13065211474895477f, 0.26532435417175293f, -0.2578313648700714f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19101180136203766f, 0.0477290041744709f, -0.46985897421836853f, -0.16338539123535156f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.006540101487189531f, 0.018553312867879868f, -0.024286868050694466f, -0.011274188756942749f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09381702542304993f, 0.25453945994377136f, -0.02967650443315506f, -0.15317076444625854f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18055981397628784f, -0.10972107946872711f, 0.07236698269844055f, -0.1297605186700821f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25864118337631226f, -0.18625794351100922f, -0.2217618227005005f, -0.9928579330444336f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2524584233760834f, 0.030367447063326836f, -0.03997525945305824f, -0.07847647368907928f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.29691722989082336f, -0.22148583829402924f, 0.07296904176473618f, 0.4002801477909088f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13572481274604797f, 0.3545679748058319f, 0.28190815448760986f, -0.05566304177045822f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.09808669984340668f, 0.22840870916843414f, 0.21555930376052856f, 0.09348693490028381f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.052265167236328125f, 0.11650260537862778f, -0.3476352095603943f, 0.37964963912963867f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.02745676040649414f, -0.22612278163433075f, 0.034423235803842545f, 0.6509405374526978f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06561578810214996f, -0.2853737473487854f, 0.2997818887233734f, 0.4338921308517456f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.013870039954781532f, -0.08927419036626816f, -0.3425488770008087f, -0.3482384979724884f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.036816321313381195f, 0.018734948709607124f, -0.0209074504673481f, -0.9473624229431152f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.134163036942482f, -0.24668176472187042f, -0.2067333608865738f, -0.11789548397064209f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.044647470116615295f, 0.19728954136371613f, 0.04703380912542343f, 0.04980244114995003f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.014782998710870743f, 0.24239620566368103f, -0.3396967351436615f, -0.3076264262199402f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06314332783222198f, -0.22502610087394714f, 0.08474118262529373f, 0.29307472705841064f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07149619609117508f, 0.046349409967660904f, 0.04605226218700409f, -0.329523503780365f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15120521187782288f, -0.4006461799144745f, -0.7495039105415344f, -0.27465543150901794f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0285679642111063f, 0.116872139275074f, 0.09924304485321045f, -0.7587600946426392f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07471399009227753f, -0.35212287306785583f, -0.7557696104049683f, 0.4851965010166168f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.026158394291996956f, 0.18251752853393555f, 0.2715061902999878f, 0.1878325194120407f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10794714838266373f, -0.13759592175483704f, -0.6423047184944153f, 0.3630295693874359f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
			}
			x_1 = ix + 2;
			if (x_1 >= 0 && x_1 < 4)
			{
				x_2 = jx + 0;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.0506189689040184f, 0.49622824788093567f, -0.22746077179908752f, 0.025030046701431274f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09136262536048889f, -0.14656327664852142f, -0.06083521619439125f, 0.3326004445552826f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.003831591224297881f, 0.4789174199104309f, 0.1419040411710739f, 0.18598926067352295f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.25310471653938293f, -0.30243542790412903f, 0.3858707845211029f, 0.18873338401317596f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.008680841885507107f, 0.10341452807188034f, -0.16562895476818085f, -0.3458116054534912f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.28312671184539795f, -1.0186768770217896f, -0.236869215965271f, -0.41782721877098083f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1855349987745285f, -0.25289008021354675f, -0.45847174525260925f, -0.07077803462743759f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04827091842889786f, 0.28454291820526123f, 0.031782761216163635f, 0.1408960074186325f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.013741166330873966f, -0.7729610204696655f, -0.24983203411102295f, 0.1017945185303688f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05953647568821907f, -0.5450471639633179f, 0.014763303101062775f, -0.023807574063539505f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.13274624943733215f, -0.8363965749740601f, -0.968219518661499f, 0.19721628725528717f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.029064007103443146f, -0.08752568066120148f, -0.1642363965511322f, -0.7147868871688843f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0830879732966423f, -0.32504555583000183f, -0.2764039635658264f, -0.4350830614566803f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15255294740200043f, -0.2585728168487549f, -0.14880484342575073f, -0.974005937576294f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.14292094111442566f, 0.6556910872459412f, -0.22509562969207764f, 0.1803264021873474f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.002232632366940379f, -0.06996980309486389f, -0.06516134738922119f, -0.22272469103336334f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.06287336349487305f, 0.5118555426597595f, -0.28805315494537354f, 0.20625977218151093f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1936454027891159f, -0.01830274984240532f, 0.12080945819616318f, -0.3226098418235779f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0034194369800388813f, 0.21419183909893036f, -0.27362266182899475f, 0.36876460909843445f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.01819237321615219f, 0.4935971796512604f, -0.21030882000923157f, -0.423866868019104f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.00831632036715746f, -0.4769554138183594f, 0.011240141466259956f, -0.2491087168455124f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0437004379928112f, 0.3662213981151581f, -0.11236975342035294f, -0.5857194662094116f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.16083531081676483f, -0.7662665247917175f, 0.00776277482509613f, -0.43771427869796753f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17703725397586823f, 0.502367377281189f, 0.32257094979286194f, -0.4480777680873871f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10049796104431152f, -0.42601296305656433f, 0.02821488119661808f, -0.05276177078485489f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13056831061840057f, -0.241596058011055f, 0.13443349301815033f, 0.08460326492786407f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11893061548471451f, -0.3312068581581116f, -0.015812678262591362f, -0.29218852519989014f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06459605693817139f, -0.4128648340702057f, 0.01640094630420208f, -0.5232965350151062f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04259968921542168f, -0.31262537837028503f, 0.09739719331264496f, -0.7654501795768738f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24362239241600037f, -0.9649943709373474f, -0.011552785523235798f, 0.7469587922096252f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07223503291606903f, 0.34620290994644165f, 0.2468804568052292f, 0.26172804832458496f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12537354230880737f, -0.4292714595794678f, 0.0022470138501375914f, 0.34450116753578186f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.04622195288538933f, 0.12271972745656967f, 0.2539094388484955f, -0.05409462749958038f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.13430720567703247f, -0.06158776581287384f, 0.13659951090812683f, -0.24867169559001923f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03452332317829132f, 0.4437533915042877f, 0.15030549466609955f, 0.34977027773857117f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05037696287035942f, -0.30703532695770264f, -0.24704785645008087f, -0.6546236872673035f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09811181575059891f, -0.1072952002286911f, 0.28511956334114075f, -0.1900988668203354f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03766985610127449f, 0.07963547855615616f, -0.30020979046821594f, -0.2969260811805725f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06294737011194229f, -0.17968444526195526f, 0.21869055926799774f, -0.4222601056098938f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19221317768096924f, 0.14299508929252625f, 0.06598527729511261f, -1.4079352617263794f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.01647871732711792f, -0.06375161558389664f, -0.017903413623571396f, -0.029265277087688446f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0892409086227417f, -0.508758544921875f, -0.21347405016422272f, -1.4808952808380127f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05980387330055237f, -0.4484265148639679f, -0.18586936593055725f, -0.17787955701351166f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.01981104165315628f, -0.11231640726327896f, -0.190458282828331f, -2.390794515609741f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03346558287739754f, -0.27934032678604126f, -0.05730187147855759f, -0.09786664694547653f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11375337094068527f, 0.013341866433620453f, -0.19768257439136505f, -0.5606134533882141f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.12846718728542328f, 0.23624321818351746f, 0.0645500048995018f, 0.4507506191730499f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09958119690418243f, -0.22974252700805664f, 0.2542783319950104f, -0.3953377604484558f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
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
		}
	}

 	// Convolution Layer
	static float x7 alignas(16) [4][4][8] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x7[i][j][0] = 0.05377054959535599f;
			x7[i][j][1] = -0.002618965692818165f;
			x7[i][j][2] = 0.0009331981418654323f;
			x7[i][j][3] = -0.002448167884722352f;
			x7[i][j][4] = 0.025759439915418625f;
			x7[i][j][5] = -0.06643909960985184f;
			x7[i][j][6] = -0.06058330833911896f;
			x7[i][j][7] = 0.03144572302699089f;
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

					w = _mm_set_ps(-0.022438842803239822f, -0.07833732664585114f, -0.03901258483529091f, -0.48081570863723755f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.022865764796733856f, 0.1147899255156517f, -0.02072903886437416f, 0.08616328984498978f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.44029921293258667f, -0.0023137074895203114f, 0.14951355755329132f, -0.142994225025177f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06909788399934769f, -0.22603200376033783f, 0.07035345584154129f, -0.24404211342334747f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.293236643075943f, 0.3090938329696655f, 0.057253770530223846f, -0.21475112438201904f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03100312128663063f, 0.20884127914905548f, 0.22905470430850983f, 0.08866778761148453f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07509572803974152f, 0.026650704443454742f, 0.03505047783255577f, 0.08353595435619354f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05863203480839729f, 0.15203829109668732f, 0.10459961742162704f, 0.026687627658247948f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12104251235723495f, -1.0515072345733643f, 0.36414965987205505f, 0.6336179971694946f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(1.017399549484253f, 1.3383513689041138f, 0.8334171772003174f, -0.9650009870529175f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1673012673854828f, 0.3393019437789917f, -0.301577091217041f, -0.13594694435596466f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05029214918613434f, 0.03847157955169678f, 0.23328760266304016f, 0.06995044648647308f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04975651204586029f, -0.1131567507982254f, 0.042048484086990356f, -0.0030798788648098707f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.035837914794683456f, -0.05355049669742584f, 0.2005051225423813f, 0.026164963841438293f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.011880713514983654f, -0.010249128565192223f, -0.019031353294849396f, 0.035442233085632324f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05460955202579498f, 0.0035837742034345865f, 0.0003300542593933642f, -0.1414487212896347f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);
				}
			}
		}
	}

 	// Dense Layer
	scores[0] = 0.299659f
	 + 0.004915f * x7[0][0][0] - 0.060323f * x7[0][0][1] + 0.046779f * x7[0][0][2] - 0.023522f * x7[0][0][3]
	 - 0.004905f * x7[0][0][4] - 0.000315f * x7[0][0][5] - 0.014367f * x7[0][0][6] + 0.125660f * x7[0][0][7]
	 - 0.040628f * x7[0][1][0] - 0.013471f * x7[0][1][1] - 0.083998f * x7[0][1][2] - 0.125908f * x7[0][1][3]
	 - 0.108401f * x7[0][1][4] + 0.021769f * x7[0][1][5] - 0.062102f * x7[0][1][6] + 0.019760f * x7[0][1][7]
	 - 0.076951f * x7[0][2][0] + 0.040255f * x7[0][2][1] + 0.100116f * x7[0][2][2] - 0.016541f * x7[0][2][3]
	 - 0.044338f * x7[0][2][4] - 0.069049f * x7[0][2][5] - 0.121421f * x7[0][2][6] - 0.182374f * x7[0][2][7]
	 - 0.036073f * x7[0][3][0] - 0.050391f * x7[0][3][1] + 0.053421f * x7[0][3][2] - 0.022211f * x7[0][3][3]
	 - 0.048756f * x7[0][3][4] - 0.016743f * x7[0][3][5] - 0.099103f * x7[0][3][6] - 0.080402f * x7[0][3][7]
	 - 0.010131f * x7[1][0][0] + 0.280986f * x7[1][0][1] + 0.012304f * x7[1][0][2] - 0.061702f * x7[1][0][3]
	 - 0.046367f * x7[1][0][4] - 0.201522f * x7[1][0][5] + 0.030798f * x7[1][0][6] - 0.055161f * x7[1][0][7]
	 + 0.168897f * x7[1][1][0] - 0.091066f * x7[1][1][1] + 0.218022f * x7[1][1][2] - 0.190154f * x7[1][1][3]
	 - 0.329857f * x7[1][1][4] - 0.055520f * x7[1][1][5] - 0.147955f * x7[1][1][6] + 0.025653f * x7[1][1][7]
	 + 0.112118f * x7[1][2][0] + 0.030029f * x7[1][2][1] - 0.788820f * x7[1][2][2] + 0.705502f * x7[1][2][3]
	 + 0.025163f * x7[1][2][4] - 0.290166f * x7[1][2][5] - 0.091216f * x7[1][2][6] + 0.013963f * x7[1][2][7]
	 + 0.007804f * x7[1][3][0] - 0.064135f * x7[1][3][1] - 0.010766f * x7[1][3][2] - 0.018940f * x7[1][3][3]
	 + 0.019337f * x7[1][3][4] + 0.031042f * x7[1][3][5] - 0.073744f * x7[1][3][6] + 0.043544f * x7[1][3][7]
	 + 0.199580f * x7[2][0][0] - 0.167273f * x7[2][0][1] - 0.181306f * x7[2][0][2] - 0.304200f * x7[2][0][3]
	 + 0.279245f * x7[2][0][4] - 0.208968f * x7[2][0][5] + 0.207161f * x7[2][0][6] - 0.122609f * x7[2][0][7]
	 + 0.704166f * x7[2][1][0] + 0.220822f * x7[2][1][1] + 0.545748f * x7[2][1][2] - 0.157592f * x7[2][1][3]
	 - 0.043186f * x7[2][1][4] + 0.014477f * x7[2][1][5] - 0.393590f * x7[2][1][6] + 0.139163f * x7[2][1][7]
	 + 0.051638f * x7[2][2][0] - 0.099872f * x7[2][2][1] - 0.054252f * x7[2][2][2] - 0.020269f * x7[2][2][3]
	 - 0.008894f * x7[2][2][4] - 0.070765f * x7[2][2][5] + 0.078463f * x7[2][2][6] + 0.191820f * x7[2][2][7]
	 + 0.019115f * x7[2][3][0] - 0.081257f * x7[2][3][1] - 0.031430f * x7[2][3][2] + 0.034026f * x7[2][3][3]
	 + 0.037494f * x7[2][3][4] + 0.059051f * x7[2][3][5] - 0.001992f * x7[2][3][6] + 0.005234f * x7[2][3][7]
	 + 0.032625f * x7[3][0][0] + 0.034763f * x7[3][0][1] + 0.050773f * x7[3][0][2] + 0.167446f * x7[3][0][3]
	 + 0.187084f * x7[3][0][4] + 0.005031f * x7[3][0][5] + 0.044901f * x7[3][0][6] + 0.186353f * x7[3][0][7]
	 + 0.017591f * x7[3][1][0] + 0.017770f * x7[3][1][1] + 0.030606f * x7[3][1][2] - 0.014228f * x7[3][1][3]
	 - 0.047742f * x7[3][1][4] + 0.029188f * x7[3][1][5] - 0.035130f * x7[3][1][6] - 0.071041f * x7[3][1][7]
	 - 0.051444f * x7[3][2][0] - 0.013847f * x7[3][2][1] + 0.118662f * x7[3][2][2] + 0.043484f * x7[3][2][3]
	 + 0.151806f * x7[3][2][4] - 0.023301f * x7[3][2][5] - 0.172511f * x7[3][2][6] - 0.319337f * x7[3][2][7]
	 + 0.009214f * x7[3][3][0] - 0.018449f * x7[3][3][1] + 0.010493f * x7[3][3][2] - 0.018390f * x7[3][3][3]
	 - 0.011237f * x7[3][3][4] - 0.039524f * x7[3][3][5] - 0.006749f * x7[3][3][6] + 0.119286f * x7[3][3][7];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.252463f
	 - 0.040931f * x7[0][0][0] + 0.231601f * x7[0][0][1] + 0.178124f * x7[0][0][2] + 0.009289f * x7[0][0][3]
	 - 0.002169f * x7[0][0][4] + 0.114881f * x7[0][0][5] + 0.179088f * x7[0][0][6] - 0.150279f * x7[0][0][7]
	 - 0.015508f * x7[0][1][0] - 0.044329f * x7[0][1][1] - 0.479944f * x7[0][1][2] - 0.137689f * x7[0][1][3]
	 + 0.031649f * x7[0][1][4] + 0.459816f * x7[0][1][5] - 0.260773f * x7[0][1][6] + 0.038372f * x7[0][1][7]
	 - 0.104401f * x7[0][2][0] + 0.099857f * x7[0][2][1] + 0.103339f * x7[0][2][2] - 0.025087f * x7[0][2][3]
	 - 0.068079f * x7[0][2][4] - 0.105565f * x7[0][2][5] - 0.198334f * x7[0][2][6] - 0.191173f * x7[0][2][7]
	 + 0.016938f * x7[0][3][0] - 0.046318f * x7[0][3][1] + 0.057843f * x7[0][3][2] + 0.007124f * x7[0][3][3]
	 + 0.093494f * x7[0][3][4] - 0.143825f * x7[0][3][5] - 0.059925f * x7[0][3][6] + 0.131226f * x7[0][3][7]
	 - 0.005817f * x7[1][0][0] - 0.017910f * x7[1][0][1] + 0.054618f * x7[1][0][2] + 0.237964f * x7[1][0][3]
	 + 0.233195f * x7[1][0][4] + 0.027755f * x7[1][0][5] + 0.150322f * x7[1][0][6] + 0.047915f * x7[1][0][7]
	 + 0.167203f * x7[1][1][0] - 0.144046f * x7[1][1][1] + 0.236638f * x7[1][1][2] - 0.119490f * x7[1][1][3]
	 - 0.487771f * x7[1][1][4] - 0.022292f * x7[1][1][5] - 0.133284f * x7[1][1][6] + 0.082676f * x7[1][1][7]
	 - 0.032150f * x7[1][2][0] + 0.353031f * x7[1][2][1] - 0.604462f * x7[1][2][2] + 0.746214f * x7[1][2][3]
	 - 0.169326f * x7[1][2][4] - 0.264525f * x7[1][2][5] - 0.322592f * x7[1][2][6] + 0.285260f * x7[1][2][7]
	 + 0.066715f * x7[1][3][0] - 0.048047f * x7[1][3][1] + 0.137891f * x7[1][3][2] + 0.115348f * x7[1][3][3]
	 + 0.159910f * x7[1][3][4] - 0.094679f * x7[1][3][5] - 0.006616f * x7[1][3][6] + 0.227928f * x7[1][3][7]
	 + 0.151297f * x7[2][0][0] - 0.329038f * x7[2][0][1] - 0.281173f * x7[2][0][2] - 0.397464f * x7[2][0][3]
	 + 0.202898f * x7[2][0][4] - 0.204514f * x7[2][0][5] + 0.270384f * x7[2][0][6] - 0.354865f * x7[2][0][7]
	 + 0.792458f * x7[2][1][0] + 0.092335f * x7[2][1][1] + 0.433903f * x7[2][1][2] - 0.249766f * x7[2][1][3]
	 + 0.050183f * x7[2][1][4] + 0.053884f * x7[2][1][5] - 0.343451f * x7[2][1][6] - 0.046978f * x7[2][1][7]
	 + 0.062881f * x7[2][2][0] - 0.186440f * x7[2][2][1] - 0.145282f * x7[2][2][2] - 0.208969f * x7[2][2][3]
	 + 0.071390f * x7[2][2][4] - 0.119792f * x7[2][2][5] - 0.026029f * x7[2][2][6] + 0.143627f * x7[2][2][7]
	 - 0.016813f * x7[2][3][0] + 0.024678f * x7[2][3][1] + 0.049744f * x7[2][3][2] - 0.085808f * x7[2][3][3]
	 - 0.072291f * x7[2][3][4] - 0.130193f * x7[2][3][5] - 0.068591f * x7[2][3][6] + 0.212869f * x7[2][3][7]
	 - 0.032609f * x7[3][0][0] - 0.016674f * x7[3][0][1] - 0.057620f * x7[3][0][2] + 0.090101f * x7[3][0][3]
	 - 0.068044f * x7[3][0][4] + 0.139433f * x7[3][0][5] + 0.054389f * x7[3][0][6] - 0.205269f * x7[3][0][7]
	 + 0.038319f * x7[3][1][0] + 0.042042f * x7[3][1][1] + 0.106197f * x7[3][1][2] + 0.124057f * x7[3][1][3]
	 + 0.141908f * x7[3][1][4] - 0.059653f * x7[3][1][5] + 0.036071f * x7[3][1][6] + 0.145447f * x7[3][1][7]
	 - 0.046822f * x7[3][2][0] - 0.022358f * x7[3][2][1] + 0.084380f * x7[3][2][2] - 0.018266f * x7[3][2][3]
	 + 0.046978f * x7[3][2][4] - 0.081986f * x7[3][2][5] - 0.138096f * x7[3][2][6] - 0.205094f * x7[3][2][7]
	 - 0.045365f * x7[3][3][0] + 0.021327f * x7[3][3][1] + 0.010742f * x7[3][3][2] - 0.138194f * x7[3][3][3]
	 - 0.127722f * x7[3][3][4] - 0.152871f * x7[3][3][5] - 0.058636f * x7[3][3][6] + 0.041678f * x7[3][3][7];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.284808f
	 - 0.047469f * x7[0][0][0] + 0.046218f * x7[0][0][1] + 0.359930f * x7[0][0][2] - 0.059348f * x7[0][0][3]
	 - 0.110450f * x7[0][0][4] + 0.252702f * x7[0][0][5] + 0.056745f * x7[0][0][6] + 0.059989f * x7[0][0][7]
	 + 0.026818f * x7[0][1][0] - 0.118078f * x7[0][1][1] - 0.261211f * x7[0][1][2] + 0.044714f * x7[0][1][3]
	 + 0.057093f * x7[0][1][4] + 0.362344f * x7[0][1][5] - 0.009474f * x7[0][1][6] - 0.004289f * x7[0][1][7]
	 - 0.048989f * x7[0][2][0] + 0.028940f * x7[0][2][1] + 0.176622f * x7[0][2][2] + 0.197031f * x7[0][2][3]
	 + 0.077277f * x7[0][2][4] + 0.049099f * x7[0][2][5] + 0.010096f * x7[0][2][6] - 0.278120f * x7[0][2][7]
	 - 0.018129f * x7[0][3][0] - 0.114437f * x7[0][3][1] + 0.072848f * x7[0][3][2] + 0.128970f * x7[0][3][3]
	 - 0.005920f * x7[0][3][4] - 0.018820f * x7[0][3][5] + 0.020042f * x7[0][3][6] - 0.133063f * x7[0][3][7]
	 - 0.044157f * x7[1][0][0] + 0.117446f * x7[1][0][1] - 0.002169f * x7[1][0][2] + 0.014992f * x7[1][0][3]
	 - 0.087596f * x7[1][0][4] - 0.139922f * x7[1][0][5] + 0.158776f * x7[1][0][6] + 0.076691f * x7[1][0][7]
	 + 0.200377f * x7[1][1][0] - 0.150199f * x7[1][1][1] + 0.321304f * x7[1][1][2] - 0.068375f * x7[1][1][3]
	 - 0.427564f * x7[1][1][4] + 0.069037f * x7[1][1][5] - 0.110367f * x7[1][1][6] - 0.114599f * x7[1][1][7]
	 + 0.070541f * x7[1][2][0] + 0.318147f * x7[1][2][1] - 0.913547f * x7[1][2][2] + 0.907017f * x7[1][2][3]
	 - 0.143646f * x7[1][2][4] - 0.395669f * x7[1][2][5] - 0.101439f * x7[1][2][6] - 0.026188f * x7[1][2][7]
	 - 0.019925f * x7[1][3][0] - 0.142401f * x7[1][3][1] + 0.053420f * x7[1][3][2] + 0.006060f * x7[1][3][3]
	 - 0.121809f * x7[1][3][4] + 0.000594f * x7[1][3][5] - 0.026169f * x7[1][3][6] - 0.017340f * x7[1][3][7]
	 + 0.250342f * x7[2][0][0] - 0.353747f * x7[2][0][1] - 0.359284f * x7[2][0][2] - 0.548971f * x7[2][0][3]
	 + 0.363734f * x7[2][0][4] - 0.325650f * x7[2][0][5] + 0.302271f * x7[2][0][6] - 0.178792f * x7[2][0][7]
	 + 0.892405f * x7[2][1][0] + 0.297886f * x7[2][1][1] + 0.505484f * x7[2][1][2] - 0.457118f * x7[2][1][3]
	 - 0.165602f * x7[2][1][4] - 0.099454f * x7[2][1][5] - 0.412218f * x7[2][1][6] + 0.077129f * x7[2][1][7]
	 + 0.111703f * x7[2][2][0] - 0.133982f * x7[2][2][1] - 0.152801f * x7[2][2][2] - 0.119228f * x7[2][2][3]
	 + 0.044935f * x7[2][2][4] - 0.148823f * x7[2][2][5] + 0.196991f * x7[2][2][6] + 0.105257f * x7[2][2][7]
	 + 0.021636f * x7[2][3][0] - 0.133729f * x7[2][3][1] + 0.101434f * x7[2][3][2] + 0.049252f * x7[2][3][3]
	 - 0.024116f * x7[2][3][4] - 0.115595f * x7[2][3][5] + 0.116929f * x7[2][3][6] + 0.107004f * x7[2][3][7]
	 + 0.054433f * x7[3][0][0] + 0.143086f * x7[3][0][1] + 0.015555f * x7[3][0][2] + 0.037338f * x7[3][0][3]
	 + 0.281204f * x7[3][0][4] + 0.030512f * x7[3][0][5] - 0.063971f * x7[3][0][6] + 0.310504f * x7[3][0][7]
	 + 0.005121f * x7[3][1][0] + 0.110030f * x7[3][1][1] + 0.065528f * x7[3][1][2] - 0.075676f * x7[3][1][3]
	 - 0.042729f * x7[3][1][4] - 0.014861f * x7[3][1][5] - 0.096129f * x7[3][1][6] - 0.006816f * x7[3][1][7]
	 - 0.067858f * x7[3][2][0] + 0.090037f * x7[3][2][1] + 0.151715f * x7[3][2][2] + 0.009285f * x7[3][2][3]
	 + 0.155575f * x7[3][2][4] - 0.058954f * x7[3][2][5] - 0.249611f * x7[3][2][6] - 0.259812f * x7[3][2][7]
	 - 0.005894f * x7[3][3][0] - 0.042387f * x7[3][3][1] + 0.008990f * x7[3][3][2] - 0.050172f * x7[3][3][3]
	 + 0.036112f * x7[3][3][4] - 0.057748f * x7[3][3][5] - 0.031358f * x7[3][3][6] + 0.140726f * x7[3][3][7];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

}



void Fy1500_2::find(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
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
double Fy1500_2::getRadius() {return scores[0];}
Vector2d Fy1500_2::getCenter() {return Vector2d(scores[1], scores[2]);}
