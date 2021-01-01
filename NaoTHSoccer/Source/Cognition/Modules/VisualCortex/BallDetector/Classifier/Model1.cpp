#include "Model1.h"

#if WIN32
#define alignas(x) __declspec(align(x))
#endif

#include <emmintrin.h>

void Model1::cnn(float x0[16][16][1])
{
	__m128 w, x, y;
	for (int xi = 0; xi < 16; xi += 1)
	{
	x0[xi][0][0] = (x0[xi][0][0] - 0.542077f);
	x0[xi][1][0] = (x0[xi][1][0] - 0.542077f);
	x0[xi][2][0] = (x0[xi][2][0] - 0.542077f);
	x0[xi][3][0] = (x0[xi][3][0] - 0.542077f);
	x0[xi][4][0] = (x0[xi][4][0] - 0.542077f);
	x0[xi][5][0] = (x0[xi][5][0] - 0.542077f);
	x0[xi][6][0] = (x0[xi][6][0] - 0.542077f);
	x0[xi][7][0] = (x0[xi][7][0] - 0.542077f);
	x0[xi][8][0] = (x0[xi][8][0] - 0.542077f);
	x0[xi][9][0] = (x0[xi][9][0] - 0.542077f);
	x0[xi][10][0] = (x0[xi][10][0] - 0.542077f);
	x0[xi][11][0] = (x0[xi][11][0] - 0.542077f);
	x0[xi][12][0] = (x0[xi][12][0] - 0.542077f);
	x0[xi][13][0] = (x0[xi][13][0] - 0.542077f);
	x0[xi][14][0] = (x0[xi][14][0] - 0.542077f);
	x0[xi][15][0] = (x0[xi][15][0] - 0.542077f);
	}

 	// Convolution Layer
	alignas(16) static float x1 [16][16][4] = {};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x1[i][j][0] = -0.037562184035778046f;
			x1[i][j][1] = 0.18027208745479584f;
			x1[i][j][2] = 0.18465594947338104f;
			x1[i][j][3] = 0.030801642686128616f;
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

					w = _mm_set_ps(0.0288640595972538f, 0.5608485341072083f, 0.2343243509531021f, 1.3478714227676392f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.1347726285457611f, -1.0607749223709106f, 0.9839529395103455f, 0.004419662989675999f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.29222574830055237f, -0.7381587028503418f, 0.2451588362455368f, -0.565924346446991f);
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

					w = _mm_set_ps(0.5761294364929199f, 0.3122676908969879f, 0.08095984160900116f, 0.7545620203018188f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.79997718334198f, 0.26203152537345886f, -0.2767307162284851f, -1.0093879699707031f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-1.0866810083389282f, -0.21089418232440948f, -0.19463767111301422f, -0.5299984812736511f);
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

					w = _mm_set_ps(0.657378077507019f, 0.02629091404378414f, 0.32814374566078186f, 0.04305873438715935f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.8609843850135803f, 0.3338210880756378f, -1.1739610433578491f, -0.09888104349374771f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.2427818775177002f, 0.21246714890003204f, -0.15216368436813354f, -0.005332370288670063f);
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
	alignas(16) static float x2 [16][16][4] = {};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x2[i][j][0] = 0.12209425866603851f;
			x2[i][j][1] = -0.5519072413444519f;
			x2[i][j][2] = 0.10860373824834824f;
			x2[i][j][3] = 0.44267329573631287f;
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

					w = _mm_set_ps(-0.1128709614276886f, 0.40461742877960205f, -0.32355988025665283f, -0.5208465456962585f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07029099017381668f, 0.06593932211399078f, 0.24008411169052124f, -0.5897132754325867f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.40611323714256287f, -0.6111221313476562f, 0.026339808478951454f, 0.05880317464470863f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4127460718154907f, -0.6402980089187622f, -0.30039113759994507f, 0.1708189696073532f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.0933053120970726f, 0.611437976360321f, 0.44173896312713623f, -0.8666611909866333f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.32973578572273254f, 0.15766641497612f, -0.20888163149356842f, -0.7613997459411621f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6617602705955505f, -0.17136718332767487f, -0.7566688060760498f, 0.33096233010292053f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.44982030987739563f, -0.5655319690704346f, -0.6456167101860046f, 0.2921927571296692f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.821163535118103f, -0.26714611053466797f, 0.6728416085243225f, -1.2697902917861938f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6696377992630005f, 0.7056904435157776f, 0.08389046043157578f, -0.6780727505683899f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7714146375656128f, 0.14112095534801483f, -0.4839951694011688f, 0.9553133845329285f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.49818408489227295f, 0.6391186714172363f, 0.3359834551811218f, -0.3516894578933716f);
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

					w = _mm_set_ps(-0.35875844955444336f, 0.5166237950325012f, 0.4486759603023529f, -0.12992289662361145f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10338514298200607f, -0.3048815131187439f, -0.5142235159873962f, 0.04131566733121872f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(1.2406998872756958f, 0.12420319765806198f, -1.1489887237548828f, 0.27050578594207764f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.053400296717882156f, 0.2865360677242279f, 0.14292605221271515f, 0.39732977747917175f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.32691630721092224f, 0.49222180247306824f, -0.16947753727436066f, 0.711757481098175f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-3.6431150436401367f, -0.598046064376831f, -0.3070928454399109f, 0.8352146148681641f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.8388205766677856f, 0.022866765037178993f, -0.1729787290096283f, -0.7421868443489075f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8340498805046082f, -0.3688947558403015f, -1.0895634889602661f, 0.4295680820941925f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-2.8524649143218994f, 0.014422831125557423f, 0.9931062459945679f, -0.5438804030418396f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.33940184116363525f, -0.012251768261194229f, 0.865772545337677f, -1.3088622093200684f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4490841031074524f, -0.6558499932289124f, -0.16915090382099152f, 0.49384409189224243f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2522298991680145f, -0.565464973449707f, -0.5539525747299194f, 0.7313689589500427f);
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

					w = _mm_set_ps(-0.18653792142868042f, 0.41499900817871094f, -1.6722246408462524f, 0.1923665702342987f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09217024594545364f, -0.09962678700685501f, 0.012895218096673489f, -0.2209918349981308f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7002925276756287f, 0.08807037770748138f, 1.7099910974502563f, -0.8006041049957275f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11412717401981354f, 0.33955782651901245f, 0.05987950786948204f, 0.4617750942707062f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.3044860363006592f, 0.21113362908363342f, -0.3314448595046997f, -0.5582221746444702f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.16688551008701324f, 0.20124544203281403f, 1.047645092010498f, 0.14881381392478943f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4840832054615021f, 0.4829750657081604f, 0.1920604705810547f, -0.5703667402267456f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4707990884780884f, 0.47462671995162964f, -0.6317653656005859f, 0.5173550248146057f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.17928308248519897f, 0.1685502529144287f, -1.2075951099395752f, 0.6199730634689331f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.26477277278900146f, -0.6116096377372742f, 0.09315755218267441f, 0.09018829464912415f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5595782995223999f, 0.12330684810876846f, 0.7152942419052124f, 0.6694238781929016f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1910601705312729f, -0.16629251837730408f, 0.5410767793655396f, 0.5185028910636902f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
			}
		}
	}

 	// Maxpool Layer 
	static float x3[8][8][4] = {};
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

 	// Leaky ReLu Layer
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{

			x = _mm_load_ps((float*)&x3[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x3[i][j][0], x);
		}
	}

 	// Convolution Layer
	alignas(16) static float x4 [8][8][8] = {};
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{
			x4[i][j][0] = -0.09052418917417526f;
			x4[i][j][1] = -2.0223255157470703f;
			x4[i][j][2] = 2.483524799346924f;
			x4[i][j][3] = 0.6449008584022522f;
			x4[i][j][4] = -0.41738638281822205f;
			x4[i][j][5] = -0.9392634630203247f;
			x4[i][j][6] = 0.6616647243499756f;
			x4[i][j][7] = -0.8961372375488281f;
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

					w = _mm_set_ps(0.2088647037744522f, 0.23181332647800446f, -0.21011105179786682f, 0.035354990512132645f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7713449597358704f, -0.00892120786011219f, 0.08488142490386963f, -0.6359527111053467f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.0806257724761963f, -0.6901268362998962f, 0.162579745054245f, 0.028612613677978516f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1761610358953476f, -0.30078884959220886f, -0.6783881187438965f, -0.851999819278717f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.35684940218925476f, -0.2192840278148651f, 0.17633779346942902f, -0.3863750398159027f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.061413783580064774f, -0.4492139518260956f, -0.08772974461317062f, 0.4846251606941223f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.14658372104167938f, -0.4260598123073578f, -0.5468261241912842f, 0.242322638630867f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19729985296726227f, -0.5397940874099731f, 0.4664730131626129f, -0.06932227313518524f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.8640255331993103f, 0.6220604181289673f, -0.011700643226504326f, 0.5795198082923889f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.561852753162384f, 0.2976892590522766f, 0.29331859946250916f, -0.5443478226661682f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.45375657081604f, 0.053038254380226135f, -0.12803161144256592f, 0.12994281947612762f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9303826093673706f, 0.2590920031070709f, -0.5544989705085754f, 0.18396486341953278f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5910007953643799f, 0.08359147608280182f, 0.07161306589841843f, -1.1424399614334106f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.45632266998291016f, -0.45900967717170715f, 1.4303786754608154f, -0.10638192296028137f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5752162337303162f, 0.1932942122220993f, 0.15987244248390198f, -1.7195794582366943f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.21507219970226288f, -0.038272131234407425f, -0.30763161182403564f, 1.4865975379943848f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.9742640256881714f, 0.26627951860427856f, -0.4737815260887146f, -0.33299362659454346f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.72636479139328f, 0.5357449054718018f, -0.049264095723629f, 0.4292892515659332f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.7754746675491333f, -0.4189521372318268f, -0.2718033492565155f, -0.5470268130302429f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.949045717716217f, 0.14700502157211304f, -0.808000385761261f, 0.2923766076564789f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0596256256103516f, 0.00888457428663969f, -0.0604584626853466f, -0.7228480577468872f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.27717849612236023f, 0.02265864610671997f, -0.6827210783958435f, -0.1330288201570511f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.18981845676898956f, -0.01588951237499714f, 0.07893494516611099f, -1.013683795928955f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.14702990651130676f, -0.15842725336551666f, -0.9621440172195435f, 0.33381155133247375f);
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

					w = _mm_set_ps(-0.8886392116546631f, -0.13968724012374878f, -0.1647776961326599f, -1.3300378322601318f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04527183249592781f, 0.08246082067489624f, -0.3389371335506439f, -0.37736576795578003f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6521360874176025f, -0.627936601638794f, 0.08926114439964294f, -0.548938512802124f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0198335647583008f, 0.1880703568458557f, -0.2576775550842285f, 0.24789199233055115f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.24094028770923615f, -0.27807629108428955f, 0.32054752111434937f, 0.46139979362487793f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05355747789144516f, 0.28319495916366577f, -1.2011936902999878f, -0.019302034750580788f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5646944046020508f, -0.1097448542714119f, 0.42692941427230835f, -0.8811078667640686f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3295716345310211f, -0.16073530912399292f, -0.09274662286043167f, 0.40504035353660583f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.862814724445343f, 0.09826462715864182f, -0.13391956686973572f, 0.16940072178840637f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07931105047464371f, -0.18189723789691925f, 0.12201803922653198f, -0.4545802175998688f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7071971297264099f, -0.33384907245635986f, 0.2277749478816986f, -0.529405951499939f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0115749835968018f, 0.4687146544456482f, 0.3108850419521332f, -0.23659572005271912f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.4416927099227905f, 0.187585711479187f, 0.17067091166973114f, 0.7060823440551758f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.49637964367866516f, 0.19170832633972168f, -0.29415324330329895f, -0.5965446829795837f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2666372060775757f, 0.23980872333049774f, 0.36167675256729126f, 0.44399741291999817f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.39713308215141296f, -0.5204957127571106f, 0.48350492119789124f, -0.6173444986343384f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.98333340883255f, 0.06280245631933212f, 0.0606633797287941f, -0.8429036736488342f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06372083723545074f, 0.03341547027230263f, -0.626830518245697f, 0.22345517575740814f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8260201215744019f, -0.12993980944156647f, 0.04934258759021759f, -0.7615508437156677f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9316098690032959f, 0.47992798686027527f, -0.08642877638339996f, -0.403325617313385f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22384013235569f, 0.10036561638116837f, 0.20507687330245972f, -0.42182332277297974f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(1.262891411781311f, 0.1603112369775772f, 0.5289332866668701f, 0.23548702895641327f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.7253447771072388f, 0.029472514986991882f, 0.7716261148452759f, -0.25567203760147095f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09118963032960892f, -0.5316115617752075f, 1.4747377634048462f, -0.18323302268981934f);
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

					w = _mm_set_ps(-0.45852258801460266f, -0.3673156499862671f, 0.2751680612564087f, 0.11378233134746552f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3065190613269806f, 0.3192225992679596f, 0.3286300003528595f, -0.15024979412555695f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.047887921333313f, -1.1229002475738525f, 0.07458152621984482f, -0.5454717874526978f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.37159809470176697f, 0.0251480583101511f, 0.019254781305789948f, 0.34543749690055847f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8462417125701904f, -0.11200447380542755f, 0.01622171513736248f, 0.5163406729698181f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.015521096996963024f, 0.06320792436599731f, 0.8588749766349792f, -0.1015077605843544f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3919347822666168f, -0.12676827609539032f, 0.12421955913305283f, -0.904973566532135f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07745721191167831f, 0.044875677675008774f, 0.00994962826371193f, -0.14605937898159027f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.39443421363830566f, 0.0026274030096828938f, 0.1972934752702713f, 0.29612359404563904f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7987453937530518f, -0.36913400888442993f, -0.3456728458404541f, -0.3281536102294922f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21150118112564087f, -0.2743282616138458f, 0.46030178666114807f, -0.42552560567855835f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3839420676231384f, 0.16216318309307098f, 0.5095542669296265f, -0.18527081608772278f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5278103947639465f, 0.03815130889415741f, 0.28224635124206543f, -0.09648813307285309f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.6096179485321045f, -0.24354971945285797f, -1.091346263885498f, 0.24510303139686584f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0947598218917847f, -0.31852665543556213f, 0.0006494747940450907f, 1.8899943828582764f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.46883687376976013f, 0.39673662185668945f, -0.6306395530700684f, 0.44289830327033997f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(1.360116720199585f, 0.04757675155997276f, 0.024758659303188324f, -1.931132435798645f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7277359366416931f, -0.17387856543064117f, -1.3360936641693115f, -0.13026273250579834f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.9283754229545593f, -0.3788377344608307f, 0.6601151823997498f, 0.23449520766735077f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.7729604244232178f, 0.142597496509552f, -0.22794997692108154f, -0.6523146629333496f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.48748114705085754f, -0.0863424688577652f, -0.48730507493019104f, -0.7689818143844604f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9057928323745728f, 0.01615014672279358f, -0.17374488711357117f, 0.28351426124572754f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.1362483501434326f, -0.17855620384216309f, 0.8796480298042297f, -0.5397104024887085f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(1.014219045639038f, -0.06842150539159775f, -0.0866117775440216f, 0.9134572744369507f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
			}
		}
	}

 	// Maxpool Layer 
	static float x5[4][4][8] = {};
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

 	// Leaky ReLu Layer
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{

			x = _mm_load_ps((float*)&x5[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x5[i][j][0], x);

			x = _mm_load_ps((float*)&x5[i][j][4]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x5[i][j][4], x);
		}
	}

 	// Convolution Layer
	alignas(16) static float x6 [4][4][8] = {};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x6[i][j][0] = -1.6135214567184448f;
			x6[i][j][1] = -2.8077847957611084f;
			x6[i][j][2] = -0.5322901606559753f;
			x6[i][j][3] = 0.3776434659957886f;
			x6[i][j][4] = -3.139270544052124f;
			x6[i][j][5] = -0.10758095234632492f;
			x6[i][j][6] = 0.8597691059112549f;
			x6[i][j][7] = -2.194945812225342f;
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

					w = _mm_set_ps(0.10136857628822327f, 0.01496491115540266f, -0.615760326385498f, -0.11646793782711029f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.017207594588398933f, 0.0035390695556998253f, -0.17012515664100647f, 0.40026965737342834f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.043625108897686005f, -0.03451273962855339f, 0.6473686099052429f, -0.32407668232917786f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06777211278676987f, -0.038964107632637024f, -0.11904408782720566f, -0.011383870616555214f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.6565808057785034f, 0.12059706449508667f, 1.2171602249145508f, -0.6109809875488281f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.008225922472774982f, 0.03006572462618351f, 0.07811394333839417f, 0.7733445763587952f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.8546804189682007f, -0.10481780767440796f, 0.5083568096160889f, -6.3250250816345215f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07179024070501328f, -0.05077533796429634f, -0.19310720264911652f, -0.10559172183275223f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7225839495658875f, 0.000943488790653646f, -0.3190157115459442f, -0.02234550565481186f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.005931498017162085f, 0.09880073368549347f, 0.07774797081947327f, -0.47344106435775757f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6806657314300537f, 0.01357823796570301f, -0.8114873170852661f, -0.09085431694984436f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05183347687125206f, -0.027455711737275124f, -0.0295520331710577f, 0.0018142040353268385f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.46995651721954346f, 0.05785379931330681f, 0.3481409549713135f, -0.6673017144203186f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0005217848229222f, 0.014931299723684788f, -0.06833481043577194f, 0.12854498624801636f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.981717586517334f, 0.024573469534516335f, -0.926949143409729f, -3.9637434482574463f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.010716941207647324f, 0.030657468363642693f, 0.06509650498628616f, -0.11418730020523071f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.10968062281608582f, 0.006010526791214943f, -0.520789623260498f, -0.5055128335952759f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.00041109134326688945f, -0.017805354669690132f, -0.11640944331884384f, -0.5649405121803284f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3287825584411621f, -0.06888296455144882f, 0.1978742629289627f, 0.002544621005654335f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0868537425994873f, -0.019864685833454132f, -0.06209920719265938f, -0.2473856508731842f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3028148412704468f, 0.06686792522668839f, 0.25471746921539307f, 0.23500949144363403f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11817506700754166f, 0.00964044313877821f, -0.11742428690195084f, 1.2261502742767334f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.20978349447250366f, -0.11538554728031158f, -0.08130156248807907f, 0.09069100767374039f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05056118965148926f, -0.0771828144788742f, -0.09657637774944305f, -0.5847105979919434f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.17424391210079193f, -0.05143518000841141f, -0.5276461243629456f, -0.1135566383600235f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.011016514152288437f, 0.030954495072364807f, -0.16636481881141663f, -1.202811360359192f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0580241680145264f, -0.006650763563811779f, -0.8154216408729553f, -0.6126707196235657f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.000902501109521836f, -0.09206753224134445f, -0.1308198869228363f, 0.07424907386302948f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3400116562843323f, 0.03992602601647377f, -0.4084073305130005f, -0.11315670609474182f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08898212760686874f, -0.055239301174879074f, -0.21975792944431305f, -0.35370388627052307f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5669352412223816f, -0.04892987757921219f, -0.5385600924491882f, -0.6034672856330872f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07319389283657074f, -0.13382890820503235f, -0.062479518353939056f, -0.4983098804950714f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.020613279193639755f, -0.024154068902134895f, -1.0280076265335083f, -0.7291339635848999f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05497255176305771f, 0.0018883637385442853f, -0.09473598748445511f, -0.011661711148917675f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.9437170624732971f, 0.032060205936431885f, 0.5470579266548157f, 0.12981773912906647f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06145259365439415f, 0.07499278336763382f, -0.031287964433431625f, 1.302111029624939f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5258859992027283f, 0.021014565601944923f, 0.07950717955827713f, -0.12886303663253784f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07294829189777374f, 0.07534851133823395f, 0.03912029042840004f, -0.2627662122249603f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.15797747671604156f, -0.003427690826356411f, 0.5649288296699524f, -0.5813305974006653f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.23298391699790955f, -0.09945014864206314f, 0.1430053561925888f, -0.14550934731960297f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.28761738538742065f, -0.03336905688047409f, -0.3531962037086487f, 0.02671247161924839f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.31960543990135193f, 0.0591287724673748f, 0.08005908876657486f, -0.8425098061561584f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8417031168937683f, -0.04291233792901039f, -0.4437061548233032f, -1.065498948097229f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0059939781203866005f, -0.04727586731314659f, 0.12922677397727966f, -0.05095062032341957f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3197583258152008f, -0.0038303048349916935f, 0.44611501693725586f, -0.05707588046789169f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07808458805084229f, -0.02956920489668846f, -0.18256202340126038f, 1.0386650562286377f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08353139460086823f, -0.19251194596290588f, -0.17875608801841736f, -0.19117532670497894f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04432353749871254f, -0.037230927497148514f, 0.026483671739697456f, 0.10485921055078506f);
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

					w = _mm_set_ps(-0.5398696660995483f, 0.01417746301740408f, -1.2472012042999268f, 0.35743531584739685f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.030552539974451065f, -0.0436784029006958f, -0.05911160260438919f, -0.7508768439292908f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24590805172920227f, -0.026513919234275818f, -0.5562041401863098f, 0.27680909633636475f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.035012103617191315f, -0.11290089786052704f, -0.01707492396235466f, -0.5806487202644348f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1633990854024887f, 0.05268789827823639f, -0.7499094009399414f, -0.09216059744358063f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.008303197100758553f, -0.15727747976779938f, -0.07933630794286728f, -0.18786698579788208f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.213721752166748f, -0.021947959437966347f, 1.0527602434158325f, -0.4287932217121124f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.009367753751575947f, -0.10837788879871368f, 0.04391226917505264f, 0.6838933229446411f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5543555617332458f, 0.03738239407539368f, -0.1798049360513687f, -0.18914955854415894f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0315752737224102f, -0.03615335747599602f, 0.05724295601248741f, -0.6215882897377014f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.47624140977859497f, 0.006980828940868378f, -0.9156113862991333f, -0.0547369159758091f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0785338431596756f, -0.023565800860524178f, 0.04588039964437485f, -1.4580694437026978f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3387162387371063f, 0.04473910853266716f, 0.12044156342744827f, -0.4121260344982147f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08640862256288528f, 0.049988456070423126f, -0.09098810702562332f, 0.004702754784375429f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.48012953996658325f, 0.0075531513430178165f, -0.7438288927078247f, -0.05887633189558983f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0067755659110844135f, -0.17165958881378174f, -0.04481391981244087f, -1.3072429895401f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.9115250110626221f, 0.0060596163384616375f, -0.4760742485523224f, -0.14145149290561676f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.010421707294881344f, -0.1455865353345871f, -0.06386115401983261f, -0.30852562189102173f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.038842663168907166f, -0.008681691251695156f, -0.437546968460083f, -0.0039258371107280254f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09086357802152634f, -0.06879514455795288f, 0.0014747391687706113f, -0.7404087781906128f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6344842314720154f, 0.05154286324977875f, -0.4884614050388336f, 0.23492544889450073f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20253261923789978f, -0.16332201659679413f, -0.12563036382198334f, -0.5077654719352722f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.3911597728729248f, -0.10266237705945969f, 0.18529567122459412f, -0.14555734395980835f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0433061346411705f, 0.013104487210512161f, -0.10055350512266159f, -0.7555689811706543f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10171441733837128f, 0.02281917817890644f, -0.08553939312696457f, 0.175093412399292f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.005920965224504471f, -0.06854138523340225f, -0.07180245220661163f, -0.775216281414032f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.670769453048706f, -0.00612006476148963f, -0.8898327946662903f, -0.14577507972717285f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0036293675657361746f, -0.10578370839357376f, -0.18195025622844696f, -0.012442395091056824f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6002031564712524f, 0.002323970664292574f, -0.6728111505508423f, -0.2626321315765381f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.027530336752533913f, -0.029367903247475624f, -0.1709332913160324f, -0.10761529952287674f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.3399709463119507f, -0.007409751880913973f, -0.4523835778236389f, 0.02201405167579651f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.009329129941761494f, -0.17189058661460876f, 0.005691990256309509f, 0.09901020675897598f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.5338852405548096f, -0.005157168954610825f, -1.1260610818862915f, 0.01627892069518566f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.002058249432593584f, -0.038646481931209564f, 0.026521380990743637f, -0.1338634341955185f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06148749217391014f, -0.035609450191259384f, -0.6115495562553406f, -0.21881259977817535f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05881521478295326f, -0.05294957756996155f, -0.19001410901546478f, -0.10157275199890137f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3465730547904968f, -0.12352428585290909f, 0.4015570282936096f, 0.08714640885591507f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.25786158442497253f, -0.07457724958658218f, -0.095550037920475f, -0.5872125625610352f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.7652842998504639f, 0.03791295364499092f, 0.7847973108291626f, -0.2018551230430603f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.003490626346319914f, -0.0821504071354866f, 0.05056442320346832f, 0.4213944971561432f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3011036813259125f, -0.04174578934907913f, 0.12659351527690887f, 0.06118277832865715f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.02402389608323574f, -0.008756373077630997f, -0.17178432643413544f, -0.43272653222084045f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8172963261604309f, -0.02320389449596405f, -0.2729669213294983f, -0.02326134592294693f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.017429187893867493f, -0.038806650787591934f, -0.10498891025781631f, -0.31626561284065247f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10649388283491135f, -0.09170695394277573f, 0.16070187091827393f, -0.04908887669444084f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03713575378060341f, 0.008383347652852535f, -0.13413624465465546f, 0.14658400416374207f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6941629648208618f, -0.07299374788999557f, -0.6755260229110718f, 0.046852413564920425f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.052877768874168396f, -0.04304205998778343f, -0.10181528329849243f, -0.8134729266166687f);
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

					w = _mm_set_ps(-0.3197784125804901f, 0.005581541918218136f, -0.9687054753303528f, -0.17504282295703888f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.036032579839229584f, -0.06328477710485458f, 0.02182391658425331f, -0.9950571656227112f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.22432106733322144f, 0.0602109469473362f, 0.1681544929742813f, -0.27962297201156616f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.02126847580075264f, -0.01750124618411064f, 0.01152769010514021f, -0.4115716814994812f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.2313019037246704f, 0.0654478669166565f, 1.2858351469039917f, 0.34805795550346375f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18660461902618408f, 0.07942178100347519f, -0.005476067773997784f, 0.9806441068649292f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.539959728717804f, -0.03758576512336731f, 0.9417523741722107f, 0.1753242462873459f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10940542072057724f, -0.05849349498748779f, -0.06693126261234283f, -0.3476104140281677f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.061782896518707275f, 0.05447857826948166f, 0.5376853942871094f, 0.010968132875859737f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1283414363861084f, -0.09308527410030365f, -0.16949255764484406f, -0.5709371566772461f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4256440997123718f, -0.01584719680249691f, 0.06903424859046936f, 0.033850595355033875f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0796545073390007f, -0.15991316735744476f, -0.26688891649246216f, -0.11149849742650986f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.36020588874816895f, 0.055541425943374634f, 0.7524994611740112f, -0.06451082229614258f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10114090144634247f, -0.01240494940429926f, -0.32662472128868103f, -1.0006673336029053f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5043029189109802f, -0.027701614424586296f, 0.1691592037677765f, -0.23709741234779358f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04116927459836006f, -0.006791951134800911f, 0.14304819703102112f, -0.895729124546051f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.7493387460708618f, -0.013998032547533512f, -2.6073360443115234f, -0.061548374593257904f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0351727157831192f, -0.06946541368961334f, -0.08906607329845428f, -1.1372524499893188f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.479352742433548f, 0.04134982079267502f, -1.0688776969909668f, 0.04559981822967529f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.026286639273166656f, 0.02441170997917652f, 0.030924493446946144f, 0.6715227961540222f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6067730188369751f, -0.042013172060251236f, -0.10050451755523682f, 0.20305772125720978f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1313888281583786f, 0.13849148154258728f, 0.02672686241567135f, -0.4573712944984436f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.5060545206069946f, -0.08220691978931427f, -0.11806048452854156f, 0.043989215046167374f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.01686612330377102f, -0.18224674463272095f, 0.05856368690729141f, -0.1998111605644226f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.29891055822372437f, 0.05631043016910553f, -0.030559053644537926f, 0.12315074354410172f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11836341768503189f, 0.020449375733733177f, -0.13249967992305756f, -0.5579658150672913f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6785311698913574f, -0.0127326725050807f, 0.04455844685435295f, -0.000531406607478857f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.016973815858364105f, -0.06599205732345581f, -0.043201301246881485f, -0.14855512976646423f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6600485444068909f, 0.015400675125420094f, 0.5789688229560852f, -0.10131992399692535f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.03941129893064499f, -0.0641951635479927f, -0.2478567361831665f, 0.34363362193107605f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7058233022689819f, -0.0005749230040237308f, 0.35858237743377686f, 0.060580071061849594f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.02727678418159485f, -0.03164675831794739f, 0.1673804521560669f, 0.16377858817577362f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.8980903625488281f, -0.04099850356578827f, -2.3834738731384277f, 0.0480116531252861f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0667920708656311f, -0.034385573118925095f, 0.057882893830537796f, -0.1557736098766327f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.008745034225285053f, -0.014779279008507729f, 0.04684160277247429f, -0.035715457051992416f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06772241741418839f, -0.01883470080792904f, -0.12898409366607666f, -0.44225117564201355f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3535102903842926f, 0.018484920263290405f, 0.5204721689224243f, 0.14450588822364807f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16484512388706207f, 0.10030166059732437f, -0.1331225037574768f, 1.1025159358978271f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6000257134437561f, -0.04095260798931122f, 0.47806525230407715f, 0.23887678980827332f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09083274751901627f, -0.04511362314224243f, -0.01626882702112198f, -0.19382856786251068f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3357028663158417f, 0.026727896183729172f, 0.40581053495407104f, -0.04068518802523613f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.027842555195093155f, 0.06820500642061234f, 0.014715408906340599f, -0.6146474480628967f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5782970190048218f, -0.0030893227085471153f, -0.185954749584198f, 0.002255853731185198f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.032024115324020386f, -0.00785029586404562f, -0.20184046030044556f, -0.30492448806762695f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.28031912446022034f, 0.001100077643059194f, 0.3896659314632416f, -0.07768391072750092f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.03893502801656723f, 0.03369326516985893f, 0.08651719242334366f, 0.2733263373374939f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0370980501174927f, -0.04335781931877136f, -0.12224486470222473f, 0.003047315636649728f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.051219042390584946f, -0.006819824688136578f, -0.03368520736694336f, -0.7063141465187073f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
			}
		}
	}

 	// Maxpool Layer 
	static float x7[2][2][8] = {};
	for (int ix = 0; ix < 3; ix += 2)
	{
		int x_out_1;
		x_out_1 = ix / 2;
	for (int jx = 0; jx < 3; jx += 2)
	{
		int x_out_2;
		x_out_2 = jx / 2;
		x = _mm_load_ps((float*)&x6[ix][jx][0]);
		y = _mm_load_ps((float*)&x6[ix + 0][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x6[ix + 0][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);
		y = _mm_load_ps((float*)&x6[ix + 1][jx + 0][0]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x6[ix + 1][jx + 1][0]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);
		x = _mm_load_ps((float*)&x6[ix][jx][4]);
		y = _mm_load_ps((float*)&x6[ix + 0][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x6[ix + 0][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);
		y = _mm_load_ps((float*)&x6[ix + 1][jx + 0][4]);
		x = _mm_max_ps(x, y);
		y = _mm_load_ps((float*)&x6[ix + 1][jx + 1][4]);
		x = _mm_max_ps(x, y);
		_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);
		}
	}

 	// Leaky ReLu Layer
	for (int i = 0; i < 2; i += 1)
	{
		for (int j = 0; j < 2; j += 1)
		{

			x = _mm_load_ps((float*)&x7[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x7[i][j][0], x);

			x = _mm_load_ps((float*)&x7[i][j][4]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x7[i][j][4], x);
		}
	}

 	// Dense Layer
	scores[0] = 0.393292f
	 + 0.013326f * x7[0][0][0] - 0.005348f * x7[0][0][1] - 0.022670f * x7[0][0][2] - 0.150664f * x7[0][0][3]
	 - 0.124330f * x7[0][0][4] + 0.280401f * x7[0][0][5] - 0.024491f * x7[0][0][6] + 0.015757f * x7[0][0][7]
	 + 0.007941f * x7[0][1][0] - 0.001293f * x7[0][1][1] + 0.011410f * x7[0][1][2] - 0.001146f * x7[0][1][3]
	 - 0.012531f * x7[0][1][4] - 0.229351f * x7[0][1][5] + 0.019760f * x7[0][1][6] + 0.072737f * x7[0][1][7]
	 + 0.043565f * x7[1][0][0] - 0.087316f * x7[1][0][1] + 0.010787f * x7[1][0][2] - 0.000771f * x7[1][0][3]
	 + 0.004307f * x7[1][0][4] - 0.220772f * x7[1][0][5] - 0.004399f * x7[1][0][6] - 0.004243f * x7[1][0][7]
	 + 0.014145f * x7[1][1][0] + 0.001425f * x7[1][1][1] + 0.003594f * x7[1][1][2] + 0.001128f * x7[1][1][3]
	 - 0.009248f * x7[1][1][4] - 0.062079f * x7[1][1][5] + 0.012724f * x7[1][1][6] - 0.000625f * x7[1][1][7];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.322297f
	 + 0.008945f * x7[0][0][0] - 0.005406f * x7[0][0][1] - 0.077197f * x7[0][0][2] - 0.130772f * x7[0][0][3]
	 - 0.096649f * x7[0][0][4] - 0.150533f * x7[0][0][5] + 0.027443f * x7[0][0][6] - 0.027210f * x7[0][0][7]
	 + 0.011899f * x7[0][1][0] - 0.000623f * x7[0][1][1] + 0.028167f * x7[0][1][2] - 0.008467f * x7[0][1][3]
	 - 0.031644f * x7[0][1][4] - 0.253635f * x7[0][1][5] - 0.028741f * x7[0][1][6] + 0.070662f * x7[0][1][7]
	 + 0.098946f * x7[1][0][0] - 0.080738f * x7[1][0][1] - 0.087240f * x7[1][0][2] + 0.000852f * x7[1][0][3]
	 + 0.001299f * x7[1][0][4] - 0.049029f * x7[1][0][5] + 0.046868f * x7[1][0][6] + 0.004938f * x7[1][0][7]
	 + 0.152524f * x7[1][1][0] + 0.001396f * x7[1][1][1] + 0.037724f * x7[1][1][2] - 0.003915f * x7[1][1][3]
	 + 0.001891f * x7[1][1][4] + 0.193364f * x7[1][1][5] - 0.041465f * x7[1][1][6] + 0.004498f * x7[1][1][7];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.428852f
	 - 0.002968f * x7[0][0][0] - 0.003608f * x7[0][0][1] + 0.015476f * x7[0][0][2] - 0.166574f * x7[0][0][3]
	 - 0.140775f * x7[0][0][4] + 0.365055f * x7[0][0][5] + 0.063922f * x7[0][0][6] - 0.004006f * x7[0][0][7]
	 - 0.000638f * x7[0][1][0] + 0.000919f * x7[0][1][1] - 0.079458f * x7[0][1][2] + 0.002839f * x7[0][1][3]
	 - 0.022350f * x7[0][1][4] + 0.101800f * x7[0][1][5] + 0.055588f * x7[0][1][6] + 0.068046f * x7[0][1][7]
	 + 0.160344f * x7[1][0][0] - 0.109434f * x7[1][0][1] - 0.011610f * x7[1][0][2] - 0.000047f * x7[1][0][3]
	 + 0.004454f * x7[1][0][4] + 0.425520f * x7[1][0][5] - 0.027839f * x7[1][0][6] + 0.011354f * x7[1][0][7]
	 + 0.066583f * x7[1][1][0] + 0.008272f * x7[1][1][1] + 0.026175f * x7[1][1][2] - 0.005420f * x7[1][1][3]
	 - 0.004273f * x7[1][1][4] + 0.215326f * x7[1][1][5] - 0.073611f * x7[1][1][6] + 0.025908f * x7[1][1][7];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

	scores[3] = 0.982563f
	 + 0.006091f * x7[0][0][0] - 0.006029f * x7[0][0][1] + 0.001855f * x7[0][0][2] - 0.339036f * x7[0][0][3]
	 - 0.278068f * x7[0][0][4] - 0.183999f * x7[0][0][5] + 0.002437f * x7[0][0][6] + 0.002517f * x7[0][0][7]
	 + 0.008709f * x7[0][1][0] - 0.000746f * x7[0][1][1] - 0.001398f * x7[0][1][2] + 0.002966f * x7[0][1][3]
	 - 0.031057f * x7[0][1][4] - 0.257221f * x7[0][1][5] - 0.000646f * x7[0][1][6] - 0.002685f * x7[0][1][7]
	 + 0.028663f * x7[1][0][0] - 0.197555f * x7[1][0][1] - 0.001735f * x7[1][0][2] - 0.000369f * x7[1][0][3]
	 + 0.008528f * x7[1][0][4] - 0.346011f * x7[1][0][5] - 0.000144f * x7[1][0][6] + 0.003256f * x7[1][0][7]
	 - 0.007781f * x7[1][1][0] + 0.003185f * x7[1][1][1] + 0.004422f * x7[1][1][2] - 0.002757f * x7[1][1][3]
	 - 0.009240f * x7[1][1][4] - 0.045893f * x7[1][1][5] + 0.012432f * x7[1][1][6] + 0.001885f * x7[1][1][7];
	scores[3] = scores[3] > 0.0f ? scores[3] : 0.0f;

}



void Model1::predict(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
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
double Model1::getRadius() {return scores[0];}
Vector2d Model1::getCenter() {return Vector2d(scores[1], scores[2]);}
double Model1::getBallConfidence() {return scores[3];}
