#include "Fy1500_3.h"

#if WIN32
#define alignas(x) __declspec(align(x))
#endif

#include <emmintrin.h>
#include <math.h>
void Fy1500_3::cnn(float x0[16][16][1])
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
	alignas(16) static float x1 [16][16][4] = {0};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x1[i][j][0] = 0.1038537323474884f;
			x1[i][j][1] = -0.34891217947006226f;
			x1[i][j][2] = 0.10058170557022095f;
			x1[i][j][3] = 0.2053762972354889f;
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

					w = _mm_set_ps(0.09152616560459137f, -0.42125701904296875f, 0.09991442412137985f, -0.27914926409721375f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.3617318570613861f, -0.005416575353592634f, -0.07733283191919327f, -0.5670110583305359f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.3343304991722107f, 0.1558508574962616f, 0.08781571686267853f, 0.049957286566495895f);
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

					w = _mm_set_ps(-0.0005524256266653538f, -0.8471251726150513f, -1.2380831241607666f, 0.20898592472076416f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.6896671652793884f, -0.05238807573914528f, -0.002828926546499133f, -0.4453578293323517f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.049821995198726654f, 0.039193011820316315f, -0.03038702718913555f, -0.23988506197929382f);
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

					w = _mm_set_ps(-0.6718271374702454f, -0.06855055689811707f, 0.1120321974158287f, 0.4403262436389923f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.002623218111693859f, 0.8365491032600403f, 0.4637535512447357f, 0.6429826617240906f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.05701664462685585f, 0.3819243907928467f, 0.3533443510532379f, 0.17962516844272614f);
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
	alignas(16) static float x2 [16][16][4] = {0};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x2[i][j][0] = -0.04501811042428017f;
			x2[i][j][1] = -0.18787549436092377f;
			x2[i][j][2] = 0.3348899483680725f;
			x2[i][j][3] = 0.03120558150112629f;
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

					w = _mm_set_ps(-0.08219044655561447f, -0.01799224130809307f, 0.11725706607103348f, 0.05358206108212471f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.748659610748291f, 1.3065470457077026f, 0.1926431655883789f, 0.1445474773645401f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.8008750081062317f, -1.6691786050796509f, -0.19480270147323608f, -0.23777464032173157f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2556683421134949f, -0.4940624535083771f, 0.6351172924041748f, -0.2578358054161072f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-1.0021874904632568f, -0.08657899498939514f, 0.18777725100517273f, -0.35977789759635925f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7885885238647461f, -0.09152568131685257f, -0.4604617953300476f, -0.3552561104297638f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.536212682723999f, -0.022773874923586845f, -0.09257104992866516f, 0.33532920479774475f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.05075335502624512f, -0.36189278960227966f, 0.3321534991264343f, 0.04761350154876709f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.19561165571212769f, -0.2902427613735199f, -0.1998210847377777f, 0.4017532765865326f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4027291536331177f, -0.55675208568573f, -0.3145405054092407f, -0.8311120271682739f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2728980481624603f, -0.0060723768547177315f, -0.09529133141040802f, 0.8468454480171204f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.146116241812706f, -0.10459674149751663f, 0.10862616449594498f, 0.1035413146018982f);
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

					w = _mm_set_ps(0.27023202180862427f, 0.13987864553928375f, 0.7803824543952942f, -0.10678446292877197f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.17044788599014282f, 0.9769669771194458f, -0.1265067160129547f, -0.1575196385383606f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6385807394981384f, -1.4776705503463745f, -0.2689555585384369f, 0.38985541462898254f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3950701653957367f, -0.16971096396446228f, -0.030290398746728897f, -0.048512473702430725f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-1.187579870223999f, 0.11828567832708359f, -0.14346441626548767f, 0.6249862909317017f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0093789100646973f, -0.8714467883110046f, -0.2153729498386383f, -0.283873975276947f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5202728509902954f, 0.483627051115036f, -0.19111742079257965f, 0.26169896125793457f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10308615863323212f, -1.750575065612793f, 0.42640167474746704f, -0.6840609312057495f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.06345601379871368f, 0.1238870844244957f, 0.09847304224967957f, 0.10977236926555634f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4767244756221771f, -1.1796891689300537f, 0.21004867553710938f, -0.020797276869416237f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.013118624687194824f, 0.5217139720916748f, -0.10637372732162476f, -0.14509890973567963f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07086095958948135f, -0.13665476441383362f, 0.004303477704524994f, -0.3413584530353546f);
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

					w = _mm_set_ps(-0.0740198940038681f, 0.5431912541389465f, 0.8072981834411621f, 0.10201791673898697f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2551833987236023f, 0.5265430808067322f, 0.2167535275220871f, -0.22560922801494598f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3970816731452942f, -0.9719261527061462f, -0.12442987412214279f, 0.12108646333217621f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.29602569341659546f, 0.08825337141752243f, -0.47513559460639954f, -0.29406818747520447f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.5334119200706482f, 0.44131991267204285f, -0.19340650737285614f, 0.4012690782546997f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0016369959339499474f, 0.027604836970567703f, 0.7120910882949829f, -0.7550175786018372f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.010214874520897865f, -0.13628390431404114f, 0.01877930760383606f, 0.25903648138046265f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04799406975507736f, -0.17699231207370758f, -0.1423249989748001f, 0.34277868270874023f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.019525235518813133f, -0.06567798554897308f, -0.00630680937319994f, 0.05432310327887535f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15841615200042725f, -0.4127567410469055f, -0.8347381949424744f, 0.44205525517463684f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04014191776514053f, 0.10782051086425781f, -0.30115067958831787f, -0.20750439167022705f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.01735898293554783f, -0.391756534576416f, 0.059766460210084915f, 0.2613118588924408f);
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
	alignas(16) static float x4 [8][8][8] = {0};
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{
			x4[i][j][0] = -0.46074816584587097f;
			x4[i][j][1] = -0.28075557947158813f;
			x4[i][j][2] = 0.1299653947353363f;
			x4[i][j][3] = -0.6180872321128845f;
			x4[i][j][4] = -0.38108330965042114f;
			x4[i][j][5] = -0.019330522045493126f;
			x4[i][j][6] = 0.25188544392585754f;
			x4[i][j][7] = -0.26348310708999634f;
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

					w = _mm_set_ps(-0.11201561987400055f, 0.4368075430393219f, 0.1846299171447754f, 0.4398849904537201f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.23515953123569489f, 0.1322789043188095f, -0.37479642033576965f, 0.2760191857814789f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13446053862571716f, 0.0023678287398070097f, 0.19831112027168274f, 0.2350275069475174f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9444144368171692f, -0.6924053430557251f, -0.0966733768582344f, -0.5097465515136719f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.46997886896133423f, 0.14033187925815582f, 0.02539873868227005f, 0.671613335609436f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.19822265207767487f, -0.27334535121917725f, -0.37530380487442017f, -0.5085176229476929f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.40509504079818726f, 0.06978821754455566f, -0.33561259508132935f, 0.059831734746694565f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0664617046713829f, -0.1458452045917511f, -0.22458206117153168f, 0.15323205292224884f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.0797576904296875f, 0.07429984211921692f, -0.23555083572864532f, 0.10373222082853317f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12373769283294678f, -0.4866073727607727f, -0.062237948179244995f, -0.15840332210063934f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.30059048533439636f, -0.9500504732131958f, 0.021475307643413544f, -0.10240557789802551f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04572933539748192f, -0.2969362735748291f, -0.31244635581970215f, 0.11577323079109192f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10566827654838562f, -0.8938508629798889f, -0.11859499663114548f, 0.09893137961626053f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11221114546060562f, -0.9294058084487915f, 0.1874299943447113f, 0.14894568920135498f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12725064158439636f, 0.13052813708782196f, -0.7100358605384827f, 0.3804841637611389f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7067558169364929f, -0.34777989983558655f, -0.7910392880439758f, -0.45831358432769775f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.2943389415740967f, -0.5969354510307312f, -0.09884771704673767f, 0.3994982838630676f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.42247700691223145f, -0.055677399039268494f, 0.1866956502199173f, -0.24096746742725372f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05675362050533295f, -0.07198872417211533f, -0.14250828325748444f, -0.0461222343146801f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4224236309528351f, -0.40815988183021545f, -0.13129808008670807f, 0.21063147485256195f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1827424168586731f, -0.02771281823515892f, 0.5204036831855774f, 0.015273630619049072f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5960451364517212f, -0.6537129878997803f, 0.25083106756210327f, 0.1467321366071701f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5083180665969849f, -0.6150961518287659f, -0.10654470324516296f, 0.3561154901981354f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2287764996290207f, -0.2170351892709732f, 0.3235909044742584f, -0.029283391311764717f);
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

					w = _mm_set_ps(0.5992564558982849f, -0.23884525895118713f, 0.5413534045219421f, -0.522355318069458f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6285797357559204f, -0.42244648933410645f, 0.05011323466897011f, -0.4714910089969635f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.021091315895318985f, 0.1677444577217102f, 0.008511954918503761f, -0.11510149389505386f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.26337432861328125f, 0.5112259984016418f, 0.003930455539375544f, -0.5892372727394104f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.04785911738872528f, -0.5424907803535461f, 1.0342155694961548f, -0.10765282064676285f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5955683588981628f, 0.1904067099094391f, -0.1458505541086197f, 0.32640698552131653f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.23919536173343658f, 0.22166620194911957f, 0.049037691205739975f, -0.0953453928232193f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15701013803482056f, -0.07234111428260803f, 0.6167935729026794f, -1.0427296161651611f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.30486610531806946f, -0.3512704074382782f, -0.5925801992416382f, -0.04317070543766022f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5345562696456909f, -0.5693367719650269f, -0.8719596862792969f, -0.5943304896354675f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.44909173250198364f, -0.2983866333961487f, 0.03174705430865288f, -0.6822744607925415f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03010551817715168f, -0.5825464129447937f, -0.029295172542333603f, 0.1457386612892151f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0564771369099617f, 0.3398979902267456f, -0.22636905312538147f, -0.7009645700454712f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.9460834264755249f, -0.4168564975261688f, -0.6156356930732727f, 0.11701516062021255f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25860002636909485f, -0.8239516615867615f, 0.4924899637699127f, -0.33554548025131226f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.25815293192863464f, -0.38234004378318787f, -0.8589345812797546f, -0.008419851772487164f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.6388700008392334f, 0.19949451088905334f, -0.42002207040786743f, -0.32789167761802673f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12167705595493317f, 0.10906055569648743f, 0.3354139029979706f, -0.5818418860435486f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.23576252162456512f, 0.09874306619167328f, -0.0025080894120037556f, -0.42568445205688477f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.14848963916301727f, -0.6135048866271973f, -0.2634718716144562f, 0.13878466188907623f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3641855716705322f, 0.14368757605552673f, 0.374455988407135f, -0.1807970106601715f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.33106452226638794f, -0.7402799129486084f, 0.12333209812641144f, 0.07716286182403564f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2249554693698883f, 0.7471603751182556f, 0.10952789336442947f, -0.02355530485510826f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.536244809627533f, 0.008493159897625446f, -0.36716699600219727f, -0.1430036872625351f);
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

					w = _mm_set_ps(0.2748510241508484f, -0.1815028339624405f, -0.27985385060310364f, 0.16082510352134705f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4874614179134369f, 0.1353675127029419f, -0.04279311001300812f, 0.14397098124027252f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1297890841960907f, 0.03319777920842171f, -0.03310733288526535f, -0.16682767868041992f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06452915817499161f, 0.027572013437747955f, -0.09328611195087433f, -0.3379948139190674f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.056796830147504807f, 0.11237594485282898f, -0.5188125967979431f, 0.04705840349197388f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2618912160396576f, 0.27176371216773987f, -0.15794284641742706f, 0.9282326698303223f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0140778087079525f, -0.11531524360179901f, -0.37972351908683777f, -0.8463127017021179f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.13889820873737335f, -0.7879320383071899f, 0.2822631895542145f, 0.0805327370762825f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.19121813774108887f, 0.4389803409576416f, -0.3281001150608063f, -0.13404861092567444f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20150791108608246f, -0.13233034312725067f, -0.2740490138530731f, 0.17732666432857513f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2023378610610962f, -0.042869243770837784f, -0.24131660163402557f, -0.26014089584350586f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04841673746705055f, 0.06270246207714081f, -0.1469602882862091f, -0.06305382400751114f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1851143091917038f, 0.14751474559307098f, -0.5145750045776367f, -0.5087924003601074f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03368126228451729f, -0.24996967613697052f, -0.07516029477119446f, -0.2998850345611572f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07684746384620667f, 0.02784811146557331f, 0.2463013231754303f, -1.2589633464813232f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7796375751495361f, -1.2732311487197876f, 0.4782865643501282f, 0.6618068814277649f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.20230181515216827f, -0.29849520325660706f, -0.15323945879936218f, -0.5280020833015442f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18084107339382172f, -0.1355767697095871f, -0.5993443131446838f, -0.040609244257211685f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.033625371754169464f, -0.2042391002178192f, -0.12659761309623718f, 0.19386738538742065f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2513743042945862f, -0.007819075137376785f, -0.09100359678268433f, -0.3154912292957306f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.32620835304260254f, -0.20340968668460846f, 0.03452535718679428f, -0.07348916679620743f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.28357115387916565f, -0.10738711059093475f, 0.3332006335258484f, 0.08553530275821686f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07462851703166962f, -0.04133157804608345f, 0.43247300386428833f, -0.2881534993648529f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2900533974170685f, -0.5010384917259216f, -0.3020250201225281f, 0.5725961923599243f);
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
	alignas(16) static float x6 [4][4][8] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x6[i][j][0] = -0.0359707809984684f;
			x6[i][j][1] = -0.18411746621131897f;
			x6[i][j][2] = 0.860412061214447f;
			x6[i][j][3] = -0.2047794759273529f;
			x6[i][j][4] = -0.798017680644989f;
			x6[i][j][5] = 0.23012694716453552f;
			x6[i][j][6] = 0.4894149601459503f;
			x6[i][j][7] = -1.123047947883606f;
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

					w = _mm_set_ps(-0.3660334348678589f, 0.06289845705032349f, -0.7119322419166565f, -0.2962019443511963f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.018392285332083702f, 0.051494646817445755f, -0.07129928469657898f, -0.12161321192979813f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7037778496742249f, 0.14532577991485596f, -0.5685272812843323f, 0.06434328109025955f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5905234217643738f, -0.08323483169078827f, -0.61698979139328f, -0.6752888560295105f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4173753261566162f, -0.27641868591308594f, -0.15908074378967285f, -0.05467117205262184f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0277083832770586f, -0.030706418678164482f, -0.03845559060573578f, -0.11578764021396637f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.05046951770782471f, -0.5251327157020569f, -0.7611691355705261f, 0.17684143781661987f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3248869776725769f, -0.2379426211118698f, 0.4422549307346344f, 0.22026745975017548f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.0479600429534912f, 0.10051332414150238f, -0.6298409700393677f, 0.06889279931783676f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.035758696496486664f, -0.09297371655702591f, -0.01378853339701891f, -0.09959184378385544f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.197351336479187f, -0.1180294007062912f, -0.045671913772821426f, -0.13764157891273499f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04915110394358635f, -0.033144205808639526f, -0.09051673114299774f, -0.8598762154579163f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2735139727592468f, -1.0562283992767334f, 0.6609460711479187f, 0.049724094569683075f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3062609136104584f, -0.005754215642809868f, 0.31772488355636597f, 0.8725541234016418f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8481630682945251f, 0.1844824254512787f, -0.4547291696071625f, -0.01716158725321293f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.02430599182844162f, -0.4493256211280823f, -0.0417649932205677f, -0.5252667665481567f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.42280101776123047f, 0.1675044447183609f, -1.0401350259780884f, 0.047443125396966934f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18411707878112793f, 0.11995396763086319f, 0.14903995394706726f, 0.5034687519073486f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3227054476737976f, -0.22643671929836273f, -0.36107975244522095f, 0.057656191289424896f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5831456184387207f, -0.6798710227012634f, -0.6162291765213013f, -0.17922134697437286f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10289911925792694f, -0.0542362779378891f, -0.3150559365749359f, -0.006929593626409769f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.27402225136756897f, -0.14809754490852356f, -0.30371132493019104f, 0.267514169216156f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0806972086429596f, 0.21210628747940063f, -1.2366149425506592f, 0.12291810661554337f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.018948694691061974f, -0.07396876811981201f, 0.06638137996196747f, 0.09438471496105194f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3954713046550751f, -0.3025639057159424f, -0.6688755750656128f, 0.09772877395153046f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09316998720169067f, -0.016996869817376137f, -0.17980702221393585f, 0.015325275249779224f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.32128384709358215f, 0.21257932484149933f, 0.28419771790504456f, 0.011955510824918747f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11324556916952133f, -0.17009316384792328f, 0.06481930613517761f, -0.14443369209766388f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.13207107782363892f, -0.27804744243621826f, 0.6629279255867004f, -0.060986172407865524f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22701439261436462f, 0.7834345698356628f, 0.20212392508983612f, 0.348361998796463f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3323213756084442f, -0.15986064076423645f, 0.029020242393016815f, -0.03636087849736214f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3814692199230194f, -0.10357177257537842f, 0.3097449541091919f, -0.7126231789588928f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.11009678244590759f, 0.1101590245962143f, -0.6449406743049622f, 0.0650266483426094f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4191665053367615f, 0.035379864275455475f, 0.4165433347225189f, 0.4281184673309326f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2623569667339325f, -0.4865420162677765f, -0.5679489970207214f, 0.055855099111795425f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11965338885784149f, -0.4589056074619293f, -0.15013128519058228f, -0.13758055865764618f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5178030729293823f, 0.7269869446754456f, 0.45037493109703064f, 0.0008173814276233315f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.19693614542484283f, -0.11921481043100357f, -0.2116130143404007f, 0.45010802149772644f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2193215936422348f, 0.0678296685218811f, -0.04458146169781685f, -0.006628901232033968f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.39674845337867737f, 0.06607593595981598f, -0.31297430396080017f, -0.09488828480243683f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.43772029876708984f, -0.11781811714172363f, -0.450524240732193f, 0.11403757333755493f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2087332159280777f, -0.110615074634552f, 0.16079725325107574f, -0.430269718170166f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.041188161820173264f, -0.05041959881782532f, -0.19687555730342865f, 0.0928906798362732f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2423766553401947f, -0.1502133309841156f, 0.25017693638801575f, -0.05062740296125412f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22797752916812897f, -0.07243969291448593f, -0.0234297476708889f, 0.06521222740411758f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.31341472268104553f, -0.4965914487838745f, -0.4037761986255646f, -0.8613408803939819f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19176600873470306f, -0.1527789831161499f, -0.0028883758932352066f, -0.01270847674459219f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4318360388278961f, -0.0010868923272937536f, 0.44327497482299805f, 0.026890967041254044f);
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

					w = _mm_set_ps(0.5099501609802246f, -0.9747620820999146f, -0.7139419913291931f, 0.12666122615337372f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16024701297283173f, -0.010514079593122005f, 0.15379782021045685f, -0.2315075397491455f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6104023456573486f, 0.06906933337450027f, -0.4490385949611664f, 0.1035940945148468f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.34816086292266846f, -0.06493187695741653f, -0.45006266236305237f, -0.33780592679977417f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1002042144536972f, -0.5463284850120544f, -0.31314778327941895f, 0.1962313950061798f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.04418954625725746f, -0.060413897037506104f, -0.05549908056855202f, -0.1335313767194748f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18013079464435577f, -0.5434731841087341f, 0.3103947341442108f, -0.05138344690203667f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.29419970512390137f, -0.07026033848524094f, -0.21432465314865112f, -0.22803498804569244f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3591687083244324f, -0.2595120072364807f, -0.5940213799476624f, 0.1430996209383011f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09241599589586258f, 0.018121497705578804f, 0.10059475898742676f, -0.41546714305877686f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5988309383392334f, 0.008079374209046364f, -0.2179129421710968f, 0.13285884261131287f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06083030626177788f, -0.09413280338048935f, -0.008287825621664524f, -0.1725326031446457f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.9947241544723511f, 0.364963561296463f, 0.07124679535627365f, -0.04611825570464134f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18620814383029938f, 0.26802265644073486f, 0.21959075331687927f, 0.026056988164782524f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.34035491943359375f, -0.008691658265888691f, -0.268585741519928f, 0.19899578392505646f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1343456357717514f, -0.13589808344841003f, 0.11377877742052078f, -0.6991085410118103f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.36848828196525574f, 0.21523547172546387f, 0.15250468254089355f, -0.093631312251091f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4473775029182434f, -0.15223294496536255f, 0.4408375322818756f, -0.6187204718589783f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4534347653388977f, -0.15665428340435028f, -0.30222633481025696f, 0.0013516340404748917f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2573532462120056f, 0.01771513931453228f, -0.3650469481945038f, -0.07612502574920654f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.22067715227603912f, -0.5568045377731323f, -0.1646038293838501f, 0.12893272936344147f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09762880206108093f, 0.14388073980808258f, 0.05797678232192993f, -0.13081221282482147f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11276120692491531f, -0.7239083051681519f, -0.07365550100803375f, -0.197299525141716f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.16332925856113434f, 0.06172194331884384f, -0.14977611601352692f, -0.3517161011695862f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05341794341802597f, -0.544542133808136f, -0.11324433237314224f, 0.05126216262578964f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.27528464794158936f, -0.01531450729817152f, 0.2272074967622757f, -0.6216529011726379f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1450439691543579f, 0.2304290235042572f, 0.3942663371562958f, -0.04379991441965103f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18473033607006073f, 0.07303313910961151f, 0.09103893488645554f, 0.1481374204158783f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5086361765861511f, 0.424396276473999f, -0.015763571485877037f, 0.019410183653235435f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3543062210083008f, -0.030064381659030914f, -0.25799626111984253f, 0.12401370704174042f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10333642363548279f, -0.2671923041343689f, -0.08820464462041855f, 0.10502997785806656f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3190571665763855f, -0.1779734492301941f, 0.3092595636844635f, -0.7342093586921692f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.1473519653081894f, -0.13982100784778595f, -0.5393994450569153f, -0.09181908518075943f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.014809931628406048f, 0.0932425782084465f, 0.08217225968837738f, -0.3478797674179077f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.48228615522384644f, -0.3338428735733032f, 0.31993430852890015f, 0.029955456033349037f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.19391636550426483f, -0.06352465599775314f, -0.288815438747406f, 0.14668171107769012f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2563420534133911f, -0.3768842816352844f, -0.0093263303861022f, 0.045883409678936005f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0949510708451271f, 0.13606716692447662f, 0.012215553782880306f, 0.15699565410614014f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08564644306898117f, 0.2916586995124817f, 0.17219004034996033f, 0.0837361142039299f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.30113884806632996f, -0.19452710449695587f, -0.3305812180042267f, 0.02166648954153061f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3080337345600128f, -0.5679906010627747f, -0.21841289103031158f, 0.059966038912534714f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3805288076400757f, 0.03219781070947647f, 0.38859823346138f, -0.7465535998344421f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.38134434819221497f, -0.324917197227478f, 0.01808365434408188f, -0.06550133228302002f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07326746731996536f, 0.04075103998184204f, -0.07515836507081985f, 0.009990681894123554f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05127163603901863f, 0.019886905327439308f, -0.4278765618801117f, 0.16729652881622314f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.38555729389190674f, -0.04795202985405922f, -0.3893088102340698f, -0.277533620595932f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21460144221782684f, -0.42896294593811035f, 0.12395672500133514f, 0.01683497615158558f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4576207399368286f, -0.131453737616539f, 0.5050771236419678f, -0.5982394814491272f);
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

					w = _mm_set_ps(-0.18941591680049896f, -0.5381974577903748f, -0.2971856892108917f, 0.04599185660481453f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11848443001508713f, -0.2532692551612854f, 0.1751890629529953f, -1.048401951789856f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09428256750106812f, 0.014573528431355953f, 0.3013746738433838f, -0.06018015742301941f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20455099642276764f, -0.003364041680470109f, -0.27630698680877686f, 0.17753922939300537f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06834139674901962f, -0.4967799782752991f, 0.01611708477139473f, -0.07273202389478683f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07358192652463913f, -0.03895456716418266f, 0.10192596912384033f, -0.21566836535930634f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.34270575642585754f, 0.0861879289150238f, -0.6617691516876221f, -0.1360716074705124f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.16061365604400635f, 0.0005728175747208297f, -0.2539537847042084f, 0.019687868654727936f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.41273486614227295f, -0.021916547790169716f, -0.1642257273197174f, 0.06099627912044525f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0432194285094738f, -0.0713728666305542f, 0.1270112693309784f, -0.21111692488193512f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.23565347492694855f, 0.31200629472732544f, 0.2754998505115509f, 0.09108202159404755f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16383841633796692f, -0.1377137303352356f, 0.1317816972732544f, 0.16167886555194855f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(1.3409128189086914f, 0.05535072833299637f, 0.16878002882003784f, 0.158786341547966f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14020667970180511f, 0.018187958747148514f, 0.20298701524734497f, 0.3705995976924896f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2100812941789627f, -0.18255317211151123f, 0.1607796549797058f, 0.11872874945402145f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.29382193088531494f, -0.09862799197435379f, 0.37828558683395386f, -0.41852688789367676f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.2929452061653137f, 0.07105191051959991f, -0.24697424471378326f, 0.07980551570653915f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.24237442016601562f, -0.10632035881280899f, 0.3028111159801483f, -0.5728635191917419f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07869086414575577f, -0.07957145571708679f, 0.2073003649711609f, -0.004585908260196447f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.22015616297721863f, -0.02468576654791832f, -0.3886287808418274f, 0.3251909017562866f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.054667480289936066f, -0.5752582550048828f, -0.2935114800930023f, -0.008483286015689373f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16233736276626587f, 0.011586290784180164f, 0.12203269451856613f, -0.26084813475608826f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10695470124483109f, 0.007846473716199398f, 0.04399103671312332f, 0.09325996786355972f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3409876227378845f, 0.13490760326385498f, -0.3853628933429718f, -0.9437241554260254f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.31954240798950195f, -0.23329734802246094f, -0.07318949699401855f, -0.023391496390104294f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.31070196628570557f, -0.03685227036476135f, 0.3246009051799774f, -0.5788515210151672f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.2171597480773926f, 0.46256545186042786f, 0.2607511878013611f, -0.027673397213220596f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2130751609802246f, -0.013837763108313084f, 0.22462157905101776f, 0.6120120882987976f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.20139485597610474f, -0.10857392102479935f, -0.13719066977500916f, -0.11018908768892288f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16940584778785706f, 0.026224765926599503f, 0.30110234022140503f, 0.5512914657592773f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8088966012001038f, -0.46297505497932434f, -0.1496211588382721f, 0.007590109948068857f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.40888896584510803f, -0.05826113373041153f, 0.45830652117729187f, -0.2612227201461792f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.258612722158432f, -0.19936276972293854f, -0.008542867377400398f, 0.00020620407303795218f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.022931747138500214f, -0.22347673773765564f, 0.09866148978471756f, -0.5255957841873169f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03334078937768936f, 0.21087907254695892f, 0.04793784022331238f, -0.0448475182056427f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1159411296248436f, 0.017203330993652344f, -0.16659428179264069f, 0.4655144214630127f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09206132590770721f, -0.4102780520915985f, -0.1701819896697998f, 0.12332890927791595f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22045764327049255f, 0.06920960545539856f, 0.24449008703231812f, -0.07323406636714935f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.012627504765987396f, 0.10575245320796967f, 0.5073860287666321f, -0.2972305715084076f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.012180197983980179f, -0.17594638466835022f, -0.15468363463878632f, 0.1471462845802307f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.31807440519332886f, -0.4144018590450287f, 0.13673149049282074f, -0.04706338047981262f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2913699150085449f, -0.05597911402583122f, 0.33608701825141907f, -0.3208267390727997f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.8185845613479614f, 0.15918570756912231f, 0.11126475781202316f, 0.19154809415340424f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03894085809588432f, -0.005892939865589142f, 0.07990244030952454f, 0.14152415096759796f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1589917242527008f, 0.1501414030790329f, -0.34208279848098755f, 0.2748752236366272f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2326188087463379f, 0.10205245763063431f, -0.18254539370536804f, 0.16031815111637115f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.49341434240341187f, -0.37732380628585815f, 0.06887516379356384f, -0.006866081617772579f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3349081575870514f, -0.10201393812894821f, 0.3802344799041748f, 0.019052308052778244f);
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
	alignas(16) static float x7 [4][4][8] = {0};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x7[i][j][0] = 0.014720176346600056f;
			x7[i][j][1] = 0.014288689009845257f;
			x7[i][j][2] = -0.14629682898521423f;
			x7[i][j][3] = 0.032162491232156754f;
			x7[i][j][4] = -0.03932083398103714f;
			x7[i][j][5] = 0.08866686373949051f;
			x7[i][j][6] = 0.006669602822512388f;
			x7[i][j][7] = 0.014629476703703403f;
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

					w = _mm_set_ps(-0.15147548913955688f, 0.0033596064895391464f, -0.04207935929298401f, 0.018674585968255997f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.018817471340298653f, 0.0234075840562582f, 0.005278730299323797f, 0.0927310362458229f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03890608251094818f, 0.07817923277616501f, -0.027801863849163055f, -0.19863320887088776f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.039779823273420334f, -0.010782985016703606f, 0.0002820656809490174f, -0.007211559452116489f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0106585081666708f, 0.2897728681564331f, -0.005462164990603924f, 0.14087550342082977f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.03177976980805397f, 0.0012687116395682096f, -0.03907541558146477f, 0.006816187873482704f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.009953026659786701f, 0.22183676064014435f, -0.013765692710876465f, -0.17653171718120575f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.13628438115119934f, -0.008470935747027397f, 0.019002694636583328f, -0.013682425022125244f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.08394787460565567f, 0.11834545433521271f, 0.05905727297067642f, -0.3300358057022095f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.014492616057395935f, 0.014040363021194935f, 0.04949503764510155f, 0.007628479041159153f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02214018814265728f, -0.08275456726551056f, 0.004702205304056406f, 0.1127210184931755f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0012556370347738266f, 0.0011911751935258508f, -0.11342333257198334f, -0.03277793899178505f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08484344184398651f, -0.013010620139539242f, -0.009123718366026878f, -0.04410133883357048f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.02388075552880764f, 0.0038506374694406986f, -0.1273927092552185f, 0.10173109173774719f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.02381853386759758f, 0.09580511599779129f, 0.01202929113060236f, -0.11814241111278534f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.019118478521704674f, -0.12041386216878891f, 0.12793800234794617f, 0.056660398840904236f);
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
	scores[0] = 0.183307f
	 + 0.017020f * x7[0][0][0] + 0.030537f * x7[0][0][1] + 0.013721f * x7[0][0][2] - 0.026442f * x7[0][0][3]
	 - 0.009582f * x7[0][0][4] - 0.036860f * x7[0][0][5] - 0.033510f * x7[0][0][6] + 0.018158f * x7[0][0][7]
	 + 0.015140f * x7[0][1][0] - 0.099058f * x7[0][1][1] + 0.014588f * x7[0][1][2] + 0.015964f * x7[0][1][3]
	 - 0.052519f * x7[0][1][4] + 0.034229f * x7[0][1][5] + 0.090443f * x7[0][1][6] - 0.017200f * x7[0][1][7]
	 + 0.038288f * x7[0][2][0] + 0.054136f * x7[0][2][1] - 0.010913f * x7[0][2][2] - 0.179669f * x7[0][2][3]
	 - 0.163927f * x7[0][2][4] + 0.028165f * x7[0][2][5] - 0.089490f * x7[0][2][6] - 0.053777f * x7[0][2][7]
	 + 0.004937f * x7[0][3][0] + 0.042659f * x7[0][3][1] + 0.003460f * x7[0][3][2] - 0.026884f * x7[0][3][3]
	 - 0.015722f * x7[0][3][4] + 0.034359f * x7[0][3][5] + 0.003471f * x7[0][3][6] + 0.004256f * x7[0][3][7]
	 + 0.012254f * x7[1][0][0] + 0.019028f * x7[1][0][1] - 0.016857f * x7[1][0][2] - 0.003154f * x7[1][0][3]
	 - 0.047623f * x7[1][0][4] + 0.017526f * x7[1][0][5] + 0.003189f * x7[1][0][6] + 0.023687f * x7[1][0][7]
	 - 0.328645f * x7[1][1][0] - 0.003368f * x7[1][1][1] - 0.797569f * x7[1][1][2] - 0.052828f * x7[1][1][3]
	 + 0.008608f * x7[1][1][4] + 0.118800f * x7[1][1][5] - 0.017305f * x7[1][1][6] + 0.199584f * x7[1][1][7]
	 + 0.028882f * x7[1][2][0] + 0.081092f * x7[1][2][1] - 0.056095f * x7[1][2][2] - 0.022151f * x7[1][2][3]
	 - 0.034338f * x7[1][2][4] - 0.014576f * x7[1][2][5] + 0.086809f * x7[1][2][6] - 0.270196f * x7[1][2][7]
	 + 0.003669f * x7[1][3][0] + 0.012300f * x7[1][3][1] - 0.004867f * x7[1][3][2] - 0.057379f * x7[1][3][3]
	 + 0.058069f * x7[1][3][4] - 0.018062f * x7[1][3][5] - 0.010426f * x7[1][3][6] + 0.003287f * x7[1][3][7]
	 + 0.021743f * x7[2][0][0] + 0.001272f * x7[2][0][1] - 0.015627f * x7[2][0][2] - 0.085651f * x7[2][0][3]
	 - 0.027103f * x7[2][0][4] + 0.040671f * x7[2][0][5] - 0.043287f * x7[2][0][6] + 0.010519f * x7[2][0][7]
	 + 0.009529f * x7[2][1][0] - 0.019399f * x7[2][1][1] - 0.007251f * x7[2][1][2] + 0.017076f * x7[2][1][3]
	 - 0.061883f * x7[2][1][4] + 0.013764f * x7[2][1][5] - 0.057928f * x7[2][1][6] - 0.034084f * x7[2][1][7]
	 + 0.831196f * x7[2][2][0] - 0.034984f * x7[2][2][1] - 0.393391f * x7[2][2][2] + 0.255346f * x7[2][2][3]
	 + 0.116810f * x7[2][2][4] + 0.051734f * x7[2][2][5] - 0.003626f * x7[2][2][6] + 0.117069f * x7[2][2][7]
	 - 0.003197f * x7[2][3][0] + 0.042918f * x7[2][3][1] + 0.012488f * x7[2][3][2] + 0.007642f * x7[2][3][3]
	 - 0.018082f * x7[2][3][4] + 0.003235f * x7[2][3][5] + 0.106105f * x7[2][3][6] + 0.001570f * x7[2][3][7]
	 + 0.014446f * x7[3][0][0] - 0.144313f * x7[3][0][1] - 0.014046f * x7[3][0][2] - 0.223972f * x7[3][0][3]
	 + 0.182454f * x7[3][0][4] + 0.023598f * x7[3][0][5] + 0.041198f * x7[3][0][6] + 0.098212f * x7[3][0][7]
	 - 0.009192f * x7[3][1][0] + 0.001657f * x7[3][1][1] + 0.008736f * x7[3][1][2] + 0.010990f * x7[3][1][3]
	 - 0.035245f * x7[3][1][4] + 0.004874f * x7[3][1][5] - 0.016868f * x7[3][1][6] - 0.043925f * x7[3][1][7]
	 + 0.008177f * x7[3][2][0] + 0.023350f * x7[3][2][1] + 0.006782f * x7[3][2][2] + 0.042784f * x7[3][2][3]
	 + 0.003075f * x7[3][2][4] + 0.049553f * x7[3][2][5] + 0.075877f * x7[3][2][6] + 0.005220f * x7[3][2][7]
	 - 0.002243f * x7[3][3][0] - 0.031931f * x7[3][3][1] - 0.000098f * x7[3][3][2] + 0.015562f * x7[3][3][3]
	 + 0.051396f * x7[3][3][4] + 0.046602f * x7[3][3][5] - 0.095264f * x7[3][3][6] - 0.005325f * x7[3][3][7];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.119606f
	 + 0.047985f * x7[0][0][0] + 0.127380f * x7[0][0][1] + 0.031754f * x7[0][0][2] + 0.064808f * x7[0][0][3]
	 + 0.147994f * x7[0][0][4] + 0.002839f * x7[0][0][5] + 0.004374f * x7[0][0][6] - 0.046640f * x7[0][0][7]
	 - 0.039781f * x7[0][1][0] - 0.000645f * x7[0][1][1] + 0.060839f * x7[0][1][2] + 0.142511f * x7[0][1][3]
	 + 0.044370f * x7[0][1][4] - 0.160711f * x7[0][1][5] + 0.080006f * x7[0][1][6] + 0.255641f * x7[0][1][7]
	 + 0.029919f * x7[0][2][0] + 0.042901f * x7[0][2][1] - 0.073553f * x7[0][2][2] - 0.287908f * x7[0][2][3]
	 - 0.000214f * x7[0][2][4] - 0.133371f * x7[0][2][5] - 0.220500f * x7[0][2][6] - 0.029623f * x7[0][2][7]
	 + 0.020846f * x7[0][3][0] - 0.148881f * x7[0][3][1] - 0.035775f * x7[0][3][2] - 0.178649f * x7[0][3][3]
	 - 0.078299f * x7[0][3][4] + 0.229312f * x7[0][3][5] + 0.191191f * x7[0][3][6] - 0.038837f * x7[0][3][7]
	 - 0.029079f * x7[1][0][0] - 0.074473f * x7[1][0][1] + 0.048308f * x7[1][0][2] + 0.077715f * x7[1][0][3]
	 + 0.106265f * x7[1][0][4] - 0.017756f * x7[1][0][5] + 0.109001f * x7[1][0][6] - 0.009289f * x7[1][0][7]
	 - 0.261533f * x7[1][1][0] - 0.044920f * x7[1][1][1] - 0.606762f * x7[1][1][2] + 0.003302f * x7[1][1][3]
	 - 0.138461f * x7[1][1][4] + 0.067735f * x7[1][1][5] - 0.046690f * x7[1][1][6] + 0.090308f * x7[1][1][7]
	 + 0.027845f * x7[1][2][0] - 0.005118f * x7[1][2][1] - 0.084164f * x7[1][2][2] - 0.118455f * x7[1][2][3]
	 - 0.040401f * x7[1][2][4] - 0.131032f * x7[1][2][5] + 0.065055f * x7[1][2][6] - 0.189102f * x7[1][2][7]
	 - 0.030983f * x7[1][3][0] + 0.068905f * x7[1][3][1] - 0.035615f * x7[1][3][2] - 0.111267f * x7[1][3][3]
	 + 0.202254f * x7[1][3][4] - 0.052581f * x7[1][3][5] - 0.091901f * x7[1][3][6] + 0.058342f * x7[1][3][7]
	 - 0.019201f * x7[2][0][0] - 0.011065f * x7[2][0][1] + 0.007542f * x7[2][0][2] + 0.070517f * x7[2][0][3]
	 + 0.005462f * x7[2][0][4] - 0.068688f * x7[2][0][5] - 0.114145f * x7[2][0][6] - 0.056966f * x7[2][0][7]
	 - 0.006716f * x7[2][1][0] + 0.065888f * x7[2][1][1] + 0.014493f * x7[2][1][2] + 0.115054f * x7[2][1][3]
	 - 0.123755f * x7[2][1][4] - 0.031905f * x7[2][1][5] + 0.052820f * x7[2][1][6] - 0.076683f * x7[2][1][7]
	 + 0.742030f * x7[2][2][0] - 0.072139f * x7[2][2][1] - 0.394056f * x7[2][2][2] - 0.035681f * x7[2][2][3]
	 + 0.085734f * x7[2][2][4] - 0.074711f * x7[2][2][5] + 0.077759f * x7[2][2][6] + 0.112812f * x7[2][2][7]
	 + 0.034300f * x7[2][3][0] + 0.052710f * x7[2][3][1] - 0.024094f * x7[2][3][2] - 0.156155f * x7[2][3][3]
	 - 0.073006f * x7[2][3][4] + 0.061339f * x7[2][3][5] - 0.030554f * x7[2][3][6] + 0.116551f * x7[2][3][7]
	 - 0.049137f * x7[3][0][0] + 0.180167f * x7[3][0][1] + 0.021067f * x7[3][0][2] + 0.293114f * x7[3][0][3]
	 - 0.209029f * x7[3][0][4] - 0.040687f * x7[3][0][5] - 0.043609f * x7[3][0][6] - 0.161609f * x7[3][0][7]
	 + 0.031341f * x7[3][1][0] - 0.061787f * x7[3][1][1] + 0.027970f * x7[3][1][2] - 0.042370f * x7[3][1][3]
	 + 0.040869f * x7[3][1][4] + 0.070837f * x7[3][1][5] - 0.064921f * x7[3][1][6] + 0.000386f * x7[3][1][7]
	 - 0.003632f * x7[3][2][0] - 0.124768f * x7[3][2][1] + 0.008490f * x7[3][2][2] + 0.015812f * x7[3][2][3]
	 + 0.003579f * x7[3][2][4] + 0.084355f * x7[3][2][5] + 0.055804f * x7[3][2][6] - 0.055200f * x7[3][2][7]
	 + 0.024221f * x7[3][3][0] + 0.001584f * x7[3][3][1] - 0.006978f * x7[3][3][2] - 0.059823f * x7[3][3][3]
	 + 0.009520f * x7[3][3][4] + 0.143013f * x7[3][3][5] + 0.006440f * x7[3][3][6] - 0.014934f * x7[3][3][7];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.148424f
	 + 0.021242f * x7[0][0][0] + 0.041049f * x7[0][0][1] - 0.027188f * x7[0][0][2] - 0.020675f * x7[0][0][3]
	 - 0.171907f * x7[0][0][4] + 0.043879f * x7[0][0][5] + 0.006938f * x7[0][0][6] - 0.066617f * x7[0][0][7]
	 + 0.006528f * x7[0][1][0] - 0.144454f * x7[0][1][1] + 0.072535f * x7[0][1][2] + 0.060462f * x7[0][1][3]
	 - 0.138698f * x7[0][1][4] - 0.050969f * x7[0][1][5] - 0.007656f * x7[0][1][6] + 0.193987f * x7[0][1][7]
	 - 0.048649f * x7[0][2][0] - 0.009569f * x7[0][2][1] + 0.002013f * x7[0][2][2] + 0.216866f * x7[0][2][3]
	 + 0.271556f * x7[0][2][4] - 0.179791f * x7[0][2][5] - 0.071836f * x7[0][2][6] - 0.135990f * x7[0][2][7]
	 - 0.042198f * x7[0][3][0] + 0.043462f * x7[0][3][1] + 0.009925f * x7[0][3][2] + 0.249332f * x7[0][3][3]
	 - 0.035911f * x7[0][3][4] - 0.052312f * x7[0][3][5] - 0.005003f * x7[0][3][6] - 0.121428f * x7[0][3][7]
	 - 0.051737f * x7[1][0][0] + 0.032950f * x7[1][0][1] + 0.010323f * x7[1][0][2] + 0.190370f * x7[1][0][3]
	 - 0.008109f * x7[1][0][4] - 0.049500f * x7[1][0][5] + 0.046579f * x7[1][0][6] - 0.040180f * x7[1][0][7]
	 - 0.369588f * x7[1][1][0] + 0.080787f * x7[1][1][1] - 0.839061f * x7[1][1][2] - 0.064840f * x7[1][1][3]
	 + 0.123581f * x7[1][1][4] + 0.186895f * x7[1][1][5] + 0.020251f * x7[1][1][6] + 0.179214f * x7[1][1][7]
	 + 0.035249f * x7[1][2][0] - 0.088473f * x7[1][2][1] - 0.042698f * x7[1][2][2] + 0.009958f * x7[1][2][3]
	 + 0.080718f * x7[1][2][4] - 0.028475f * x7[1][2][5] + 0.107822f * x7[1][2][6] - 0.334837f * x7[1][2][7]
	 + 0.006171f * x7[1][3][0] - 0.036383f * x7[1][3][1] + 0.003447f * x7[1][3][2] - 0.039900f * x7[1][3][3]
	 + 0.135017f * x7[1][3][4] - 0.064426f * x7[1][3][5] - 0.064365f * x7[1][3][6] + 0.051343f * x7[1][3][7]
	 + 0.009400f * x7[2][0][0] - 0.002414f * x7[2][0][1] - 0.018397f * x7[2][0][2] - 0.041057f * x7[2][0][3]
	 - 0.095495f * x7[2][0][4] + 0.007165f * x7[2][0][5] - 0.067695f * x7[2][0][6] + 0.031520f * x7[2][0][7]
	 + 0.009114f * x7[2][1][0] + 0.009743f * x7[2][1][1] - 0.040554f * x7[2][1][2] - 0.166963f * x7[2][1][3]
	 - 0.086367f * x7[2][1][4] - 0.001063f * x7[2][1][5] - 0.085163f * x7[2][1][6] - 0.018982f * x7[2][1][7]
	 + 0.987328f * x7[2][2][0] + 0.079324f * x7[2][2][1] - 0.506686f * x7[2][2][2] + 0.075652f * x7[2][2][3]
	 - 0.053569f * x7[2][2][4] - 0.171288f * x7[2][2][5] - 0.099734f * x7[2][2][6] + 0.158954f * x7[2][2][7]
	 - 0.030082f * x7[2][3][0] + 0.033725f * x7[2][3][1] + 0.005964f * x7[2][3][2] + 0.052105f * x7[2][3][3]
	 - 0.081574f * x7[2][3][4] + 0.011055f * x7[2][3][5] + 0.110635f * x7[2][3][6] - 0.036378f * x7[2][3][7]
	 + 0.022604f * x7[3][0][0] + 0.008449f * x7[3][0][1] - 0.014750f * x7[3][0][2] - 0.237065f * x7[3][0][3]
	 + 0.178491f * x7[3][0][4] + 0.042807f * x7[3][0][5] + 0.079938f * x7[3][0][6] + 0.083132f * x7[3][0][7]
	 - 0.006697f * x7[3][1][0] - 0.041563f * x7[3][1][1] + 0.005875f * x7[3][1][2] - 0.053014f * x7[3][1][3]
	 - 0.025617f * x7[3][1][4] + 0.010268f * x7[3][1][5] - 0.018047f * x7[3][1][6] - 0.031771f * x7[3][1][7]
	 + 0.025438f * x7[3][2][0] + 0.011722f * x7[3][2][1] - 0.011642f * x7[3][2][2] + 0.007826f * x7[3][2][3]
	 - 0.104058f * x7[3][2][4] + 0.082912f * x7[3][2][5] + 0.060711f * x7[3][2][6] + 0.014604f * x7[3][2][7]
	 + 0.002741f * x7[3][3][0] - 0.048758f * x7[3][3][1] + 0.011852f * x7[3][3][2] + 0.041263f * x7[3][3][3]
	 + 0.032242f * x7[3][3][4] + 0.085393f * x7[3][3][5] - 0.042975f * x7[3][3][6] - 0.050453f * x7[3][3][7];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

}



void Fy1500_3::find(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
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
double Fy1500_3::getRadius() {return scores[0];}
Vector2d Fy1500_3::getCenter() {return Vector2d(scores[1], scores[2]);}
