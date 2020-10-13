#include "Fy1500_Conf.h"

#if WIN32
#define alignas(x) __declspec(align(x))
#endif

#include <emmintrin.h>

void Fy1500_Conf::cnn(float x0[16][16][1])
{
	__m128 w, x, y;
	for (int xi = 0; xi < 16; xi += 1)
	{
	x0[xi][0][0] = (x0[xi][0][0] - 0.542091f);
	x0[xi][1][0] = (x0[xi][1][0] - 0.542091f);
	x0[xi][2][0] = (x0[xi][2][0] - 0.542091f);
	x0[xi][3][0] = (x0[xi][3][0] - 0.542091f);
	x0[xi][4][0] = (x0[xi][4][0] - 0.542091f);
	x0[xi][5][0] = (x0[xi][5][0] - 0.542091f);
	x0[xi][6][0] = (x0[xi][6][0] - 0.542091f);
	x0[xi][7][0] = (x0[xi][7][0] - 0.542091f);
	x0[xi][8][0] = (x0[xi][8][0] - 0.542091f);
	x0[xi][9][0] = (x0[xi][9][0] - 0.542091f);
	x0[xi][10][0] = (x0[xi][10][0] - 0.542091f);
	x0[xi][11][0] = (x0[xi][11][0] - 0.542091f);
	x0[xi][12][0] = (x0[xi][12][0] - 0.542091f);
	x0[xi][13][0] = (x0[xi][13][0] - 0.542091f);
	x0[xi][14][0] = (x0[xi][14][0] - 0.542091f);
	x0[xi][15][0] = (x0[xi][15][0] - 0.542091f);
	}

 	// Convolution Layer
	alignas(16) static float x1 [16][16][4] = {};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x1[i][j][0] = 0.07810080051422119f;
			x1[i][j][1] = -0.1274765580892563f;
			x1[i][j][2] = 0.1031542420387268f;
			x1[i][j][3] = 0.005812497343868017f;
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

					w = _mm_set_ps(0.356621652841568f, 0.5950852632522583f, 0.06829644739627838f, -0.5148625373840332f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.2033717781305313f, -0.42896807193756104f, -0.38379114866256714f, -0.3126561939716339f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.11972088366746902f, -0.7419562935829163f, 0.036381933838129044f, -0.084990955889225f);
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

					w = _mm_set_ps(0.0504784882068634f, 0.326556533575058f, 0.31510046124458313f, -0.19068482518196106f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.4636533856391907f, -0.5357811450958252f, 0.12634839117527008f, -0.588169515132904f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.03873653709888458f, -0.5333753228187561f, -0.7616710066795349f, 0.622372567653656f);
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

					w = _mm_set_ps(-0.6970664858818054f, 0.16863515973091125f, -0.02063516527414322f, 0.014259482733905315f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.38427621126174927f, 0.4384864568710327f, -0.5728005766868591f, 0.2919653654098511f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.5473112463951111f, 0.7468118071556091f, 1.2810734510421753f, 0.1590173989534378f);
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
			x2[i][j][0] = 0.044315680861473083f;
			x2[i][j][1] = -0.06975334137678146f;
			x2[i][j][2] = 0.2230093777179718f;
			x2[i][j][3] = 0.07623820751905441f;
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

					w = _mm_set_ps(0.4916183650493622f, -0.09888748824596405f, 0.5976696014404297f, -0.28833192586898804f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1502956598997116f, 0.022708052769303322f, -0.6248430013656616f, 0.011113529093563557f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14091239869594574f, -0.4023776352405548f, 0.3285199999809265f, -0.7750736474990845f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18210290372371674f, -0.33696249127388f, -0.11898300051689148f, 0.35661718249320984f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.3802570700645447f, 0.05217604711651802f, 0.35491320490837097f, -0.17320357263088226f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1914202719926834f, -0.2086028903722763f, 0.1613791137933731f, -0.5628117322921753f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8062188625335693f, -0.16423144936561584f, 0.5097842216491699f, -1.5675938129425049f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.391508013010025f, -0.043394751846790314f, -0.2966124415397644f, 0.15700265765190125f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.032801978290081024f, 0.1332031637430191f, -0.17077866196632385f, -0.5757254958152771f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2613649070262909f, -0.15559031069278717f, 0.621261715888977f, -0.6917145848274231f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.38394176959991455f, -0.23201210796833038f, -0.11859839409589767f, -0.2907158434391022f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2674262821674347f, 0.22398434579372406f, -0.3210292458534241f, -0.1724032759666443f);
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

					w = _mm_set_ps(0.3749503493309021f, -0.5689243674278259f, 0.3586518466472626f, -0.1861935257911682f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.6976717710494995f, -0.6015646457672119f, -0.15008482336997986f, -0.4383802115917206f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.34574392437934875f, -1.4160889387130737f, -0.2975136637687683f, -0.06617800146341324f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4793596565723419f, -0.8898449540138245f, -0.012119339779019356f, -0.3302096724510193f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.23700645565986633f, -0.6649511456489563f, 0.08594385534524918f, 0.06104698032140732f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7173662781715393f, -0.9448968172073364f, -0.1369316577911377f, -0.39108186960220337f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.2519603967666626f, -0.16468749940395355f, 0.13782019913196564f, -0.46596643328666687f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0666178464889526f, 0.06466300785541534f, -0.019088566303253174f, 0.0034679335076361895f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.06765269488096237f, -0.44886064529418945f, -0.4235152304172516f, 0.8311548829078674f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8251938223838806f, -0.48906245827674866f, 0.1804407238960266f, -0.475586473941803f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09549398720264435f, -0.2853787839412689f, 0.08442991226911545f, -1.512022614479065f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3437613546848297f, 0.14952251315116882f, -0.23745197057724f, 0.07974887639284134f);
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

					w = _mm_set_ps(-0.0814671739935875f, -0.9436227083206177f, -0.5992614030838013f, 0.10964649170637131f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1352783441543579f, -0.9730377793312073f, 0.07564792037010193f, -0.7565852403640747f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5259639024734497f, 0.02720567025244236f, 0.10511070489883423f, 0.36881229281425476f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2895653247833252f, -0.6567072868347168f, -0.1800214946269989f, -0.6228592991828918f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.047582585364580154f, -0.04165888577699661f, -0.5591004490852356f, 0.5639859437942505f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.39919090270996094f, -0.488903671503067f, -0.06666181236505508f, -0.1285322606563568f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9465057253837585f, 0.664258599281311f, -0.1553947478532791f, 0.295352578163147f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2940215766429901f, 0.37523987889289856f, -0.5060694217681885f, 0.3982316255569458f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.4291153848171234f, -0.6244575381278992f, -0.048797719180583954f, 0.3200180232524872f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18354295194149017f, -0.17966818809509277f, 0.34865668416023254f, 0.38436010479927063f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10016851127147675f, -0.04672067239880562f, 0.2713901698589325f, -1.0341639518737793f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.224608913064003f, 0.21531499922275543f, -1.1028363704681396f, 0.14807358384132385f);
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

 	// Convolution Layer
	alignas(16) static float x4 [8][8][8] = {};
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{
			x4[i][j][0] = -0.4231266975402832f;
			x4[i][j][1] = -0.7110490202903748f;
			x4[i][j][2] = -0.013710484839975834f;
			x4[i][j][3] = -0.13761256635189056f;
			x4[i][j][4] = 0.15812620520591736f;
			x4[i][j][5] = -0.5490086078643799f;
			x4[i][j][6] = -0.17929190397262573f;
			x4[i][j][7] = -0.42113855481147766f;
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

					w = _mm_set_ps(-0.18658268451690674f, -0.10929886996746063f, -0.029653044417500496f, 1.4472713470458984f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15970107913017273f, 0.2241981327533722f, 0.20177341997623444f, -0.10344704240560532f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19930940866470337f, -0.8420296311378479f, -0.013973468914628029f, -0.2001708596944809f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.30059975385665894f, 0.36998048424720764f, -0.0940573588013649f, -0.49453267455101013f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0908719003200531f, -0.5005469918251038f, 1.255588173866272f, 0.15428929030895233f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7397449016571045f, -0.21451549232006073f, 0.13130496442317963f, -0.7976473569869995f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.9840155243873596f, -0.3645259439945221f, 0.2777423858642578f, 0.10501083731651306f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03507852554321289f, -0.4299108386039734f, 0.12515103816986084f, -0.17802271246910095f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.16772758960723877f, -0.03315560519695282f, 0.7454822659492493f, -0.42410093545913696f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2688189744949341f, -0.033830445259809494f, -0.14607462286949158f, 0.1825333535671234f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.44961756467819214f, 0.21584582328796387f, 0.06972938776016235f, 0.046914223581552505f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.027360569685697556f, -0.31713634729385376f, 0.08764138072729111f, -0.7465542554855347f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.9686458110809326f, 0.6744662523269653f, 0.011445439420640469f, -0.5525237321853638f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3841612935066223f, 0.016108471900224686f, 0.04226771742105484f, -0.6261440515518188f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.26951682567596436f, -0.5819213390350342f, -0.2449493259191513f, -0.008307158946990967f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.02940349280834198f, -0.25172317028045654f, 0.04838932678103447f, -0.3763121962547302f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.2736082375049591f, -0.25168201327323914f, -0.0025686994194984436f, -0.16087733209133148f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.017570635303854942f, -0.10051985830068588f, 0.8111775517463684f, -0.2592514455318451f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21999777853488922f, -0.6031237840652466f, 0.291050523519516f, 0.10854289680719376f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15120723843574524f, 0.3049062192440033f, 0.2279457449913025f, -0.5658884048461914f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.512683093547821f, 0.5158965587615967f, -0.5167883038520813f, -0.16126589477062225f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.34303605556488037f, -0.08932699263095856f, 0.4723079204559326f, 0.05459432676434517f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5536025762557983f, 0.06771788001060486f, -0.4306122064590454f, 0.15294034779071808f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3746028542518616f, -0.2581326961517334f, 0.06482026726007462f, 0.2213139683008194f);
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

					w = _mm_set_ps(-0.5387243628501892f, 0.20119668543338776f, -0.14946970343589783f, 0.1587906777858734f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32965877652168274f, -0.32805007696151733f, 0.45659181475639343f, -0.3811948299407959f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3175337314605713f, -1.0626451969146729f, 0.42609742283821106f, -0.256118506193161f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18587857484817505f, 0.38798120617866516f, -0.12379122525453568f, -0.25724342465400696f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6017181277275085f, -0.20280243456363678f, 0.8904949426651001f, 0.40294528007507324f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08741237223148346f, 0.09277431666851044f, 0.8416263461112976f, -0.4817867875099182f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1989607810974121f, -0.2757071554660797f, 0.08648347109556198f, 0.1626204252243042f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.49183714389801025f, -0.19291183352470398f, 0.2770768404006958f, -0.5920987129211426f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.05707484111189842f, -0.3307569921016693f, 0.6957777738571167f, 0.654822826385498f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24996572732925415f, 0.6833876967430115f, 0.02813950553536415f, -0.2157016098499298f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5997315049171448f, 0.17621779441833496f, -0.11201909184455872f, -0.1400989443063736f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.009991414844989777f, -0.004598096013069153f, 0.3138447701931f, -1.0866973400115967f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6657863855361938f, 0.06845506280660629f, 0.5498119592666626f, -0.1847907304763794f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.14808207750320435f, -0.26178881525993347f, -0.4820256233215332f, -0.33648115396499634f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6994110941886902f, 0.027060491964221f, 0.11328895390033722f, 0.11735478043556213f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6537944078445435f, -0.15897557139396667f, -0.5289375185966492f, -0.2251344472169876f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.5115019679069519f, 0.04609256237745285f, 0.22070913016796112f, -0.09055370837450027f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.29545873403549194f, 0.1840042918920517f, 0.1466721147298813f, -0.44200149178504944f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.08797203749418259f, -0.16717052459716797f, 0.008013897575438023f, 0.054187364876270294f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18389838933944702f, -0.4077214300632477f, -0.3232099115848541f, -1.1779042482376099f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.16733184456825256f, 0.09603933244943619f, 0.30674001574516296f, 0.27938517928123474f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15021568536758423f, 0.04481704533100128f, -0.6317364573478699f, 0.2438497394323349f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.38055694103240967f, -0.2207072675228119f, 0.04852137342095375f, -0.3391000032424927f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3866707384586334f, -0.3198864758014679f, 0.3931722640991211f, -0.39755934476852417f);
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

					w = _mm_set_ps(-0.24179160594940186f, -0.09857438504695892f, 0.10426560044288635f, 0.37348127365112305f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05874495953321457f, 0.39777910709381104f, 0.6605197191238403f, 0.07011588662862778f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.41087204217910767f, -1.3056628704071045f, 0.1580502688884735f, -0.5534193515777588f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2559796869754791f, 0.01397352572530508f, -0.3856421113014221f, 0.0076493448577821255f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02600248157978058f, -0.32046186923980713f, 0.4847305119037628f, -0.23062849044799805f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2127731889486313f, -0.07354620844125748f, 0.3896421790122986f, -0.2566685080528259f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06154591590166092f, -0.06879561394453049f, -0.3528155982494354f, -0.20413526892662048f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.8747775554656982f, 0.34005653858184814f, 0.7438793182373047f, -0.5393316149711609f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.30701690912246704f, 0.7436022162437439f, 0.15296778082847595f, 0.04927186667919159f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5421385765075684f, -0.11277883499860764f, 0.6510140299797058f, -0.21543926000595093f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.30617645382881165f, -0.6541022658348083f, 0.06198090687394142f, -0.04338051751255989f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.21444332599639893f, 0.522921621799469f, -0.34451794624328613f, -0.6463252305984497f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4816776514053345f, 0.108172707259655f, 0.3848554491996765f, 0.30828380584716797f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3731241822242737f, 0.1222633644938469f, 0.35782524943351746f, -0.14561070501804352f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03224187344312668f, -0.0036473858635872602f, 0.26610517501831055f, -0.22036485373973846f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7719917893409729f, -0.11710577458143234f, 0.2590334713459015f, -0.3083362877368927f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.06405526399612427f, 0.36307480931282043f, 0.1287871152162552f, -0.4460933208465576f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4039291739463806f, 0.24413526058197021f, -0.1328732818365097f, -0.3215634822845459f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7374203205108643f, 0.014532443135976791f, -0.058065325021743774f, -0.04011591523885727f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0503121055662632f, 0.024550586938858032f, -0.493615061044693f, -0.421391099691391f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2679300308227539f, -0.21991370618343353f, 0.22999656200408936f, -0.12912550568580627f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22180333733558655f, -0.09124888479709625f, 0.32547062635421753f, -0.0012640852946788073f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2992345094680786f, 0.06906559318304062f, 0.46115219593048096f, 0.11233176290988922f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06229601427912712f, -0.271355539560318f, 0.04336770623922348f, -0.5269891619682312f);
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

 	// Convolution Layer
	alignas(16) static float x6 [4][4][8] = {};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x6[i][j][0] = 0.11461523920297623f;
			x6[i][j][1] = -0.9303820729255676f;
			x6[i][j][2] = 0.6000009179115295f;
			x6[i][j][3] = 0.39879289269447327f;
			x6[i][j][4] = -0.43602532148361206f;
			x6[i][j][5] = 0.48494061827659607f;
			x6[i][j][6] = 0.7161335349082947f;
			x6[i][j][7] = -0.17444057762622833f;
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

					w = _mm_set_ps(0.45108145475387573f, -0.21390967071056366f, -0.28675377368927f, -0.15199516713619232f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4691831171512604f, 0.20575843751430511f, 0.1542944759130478f, -1.1776000261306763f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19936950504779816f, -0.236920565366745f, -0.690849781036377f, -0.04747569561004639f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13416999578475952f, 0.36832669377326965f, -0.46953657269477844f, -0.5366392731666565f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5864354372024536f, 0.11902676522731781f, 1.2431608438491821f, 0.339202344417572f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5590847134590149f, 0.2566030025482178f, 0.08860098570585251f, -0.00855232123285532f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06010131910443306f, 0.007035872898995876f, -0.43969985842704773f, 0.040941040962934494f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4021337628364563f, -0.15293006598949432f, 0.5218900442123413f, -0.22615256905555725f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06362900137901306f, -0.31231409311294556f, 0.11296506226062775f, 0.09132999181747437f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.009936711750924587f, 0.3086181581020355f, -0.10948019474744797f, 0.47957563400268555f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11792198568582535f, -0.4449934959411621f, -0.7172541618347168f, -0.3562014698982239f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5411169528961182f, -0.412485271692276f, -0.8133307099342346f, -0.8135691285133362f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.21006540954113007f, -0.03972514718770981f, -0.18866941332817078f, -0.05050985515117645f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15835562348365784f, 0.19253213703632355f, -0.07726892828941345f, -0.3670281767845154f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.36916467547416687f, 0.20517918467521667f, 0.3121233880519867f, -0.12914066016674042f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5077708959579468f, -0.5910168886184692f, 0.12821945548057556f, 0.3979298770427704f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.06007848680019379f, 0.005722383502870798f, -0.19739650189876556f, 0.2910371422767639f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.23247215151786804f, 0.17067547142505646f, 0.1812744140625f, -0.7356094717979431f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4541110694408417f, 0.10047206282615662f, -0.5908148884773254f, -0.8543632626533508f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.27571529150009155f, 0.0376797579228878f, -0.2380409985780716f, -0.7482904195785522f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.48376530408859253f, 0.15645602345466614f, -0.2262074053287506f, -0.1451372504234314f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.43179231882095337f, 0.02652374655008316f, -0.32127827405929565f, 0.39771726727485657f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.27786698937416077f, -0.18002216517925262f, -0.08889348804950714f, 0.36213818192481995f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08929634094238281f, -0.5236722826957703f, 0.5923470258712769f, 0.21518661081790924f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4577887952327728f, -0.05622611194849014f, -0.896666407585144f, 0.18710434436798096f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.40941792726516724f, -0.014429348520934582f, -0.45247745513916016f, -0.4012224078178406f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.12415669858455658f, -0.048376768827438354f, -0.4614584445953369f, 0.06448524445295334f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3130705952644348f, -0.4063260555267334f, -0.2572248876094818f, -0.5231825709342957f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.24751949310302734f, 0.16560354828834534f, -0.1737910509109497f, -0.11220791935920715f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6922694444656372f, 0.38019946217536926f, 0.15294812619686127f, -0.44813430309295654f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.180124893784523f, 0.032471247017383575f, 0.46570903062820435f, 0.29175910353660583f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4108179211616516f, -1.0656319856643677f, -0.11060593277215958f, -0.12853996455669403f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.10202734917402267f, -0.11208469420671463f, -0.10755043476819992f, -0.8191315531730652f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2940939664840698f, -0.3451007902622223f, 0.20126868784427643f, -0.18194535374641418f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.007258381228893995f, 0.029902851209044456f, -0.47410544753074646f, -0.34293046593666077f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.13149213790893555f, -0.7992222309112549f, -0.1423693746328354f, -0.4715422987937927f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1967601776123047f, 0.030732514336705208f, -0.08855249732732773f, 0.1554567664861679f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22331079840660095f, 0.07460995763540268f, 0.26840585470199585f, 0.6607036590576172f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5095869302749634f, -0.16136963665485382f, 0.5399211049079895f, -0.595889687538147f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5325834155082703f, -0.025553302839398384f, 0.7721702456474304f, 0.5198036432266235f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3173756003379822f, 0.020552586764097214f, -0.1417870968580246f, -0.18908724188804626f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.019995644688606262f, -0.2526824176311493f, -0.6195347905158997f, -1.057671070098877f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5532023906707764f, -0.06138636916875839f, -0.012825455516576767f, -0.30885928869247437f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.004390145186334848f, -0.14713957905769348f, 0.21677154302597046f, -0.39744362235069275f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.28487294912338257f, 0.2862359285354614f, 0.2312920093536377f, -0.0033754855394363403f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5197376608848572f, 0.4849785268306732f, -0.008011486381292343f, -0.04337957128882408f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.20161601901054382f, -0.041709598153829575f, -0.37976911664009094f, -0.024156391620635986f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.49775636196136475f, -0.13091762363910675f, 0.4039810597896576f, -0.0052647958509624004f);
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

					w = _mm_set_ps(-0.20828130841255188f, 0.040011610835790634f, -0.3274800479412079f, -0.43050122261047363f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2071225345134735f, -0.08279197663068771f, -0.9487131237983704f, -0.11848088353872299f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5941917896270752f, 0.06820113956928253f, -0.7359281778335571f, -0.2603306472301483f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08759180456399918f, -0.07636535167694092f, -0.24568524956703186f, 0.17384971678256989f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4979061484336853f, 0.1076473742723465f, 0.7569324374198914f, 0.2023235559463501f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.01990266889333725f, 0.16108450293540955f, 0.08320942521095276f, 0.18176037073135376f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22655366361141205f, -0.16231565177440643f, -0.12688183784484863f, -0.0789514109492302f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.39636310935020447f, -0.6300364136695862f, 0.06698568165302277f, -0.3558276891708374f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2639959156513214f, -0.12595632672309875f, -0.43854519724845886f, 0.005343112628906965f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7633901834487915f, -0.03579462319612503f, 0.09640559554100037f, -0.7988646626472473f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3924032747745514f, -0.20540501177310944f, -0.47640350461006165f, -1.828050136566162f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.9953685402870178f, -0.375825434923172f, -0.30019959807395935f, -0.28174230456352234f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10775315016508102f, -0.2365201711654663f, -0.05916985869407654f, -0.1455388069152832f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2501785159111023f, -0.10038106143474579f, -0.8700175285339355f, 0.21695730090141296f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.42967545986175537f, -0.11598064750432968f, 0.14077308773994446f, -0.9635851383209229f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.44853222370147705f, -0.35940107703208923f, -0.23474672436714172f, 0.1374036967754364f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.5810714960098267f, 0.04934973642230034f, -0.5624971985816956f, -0.1991504430770874f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3616734445095062f, 0.03532467782497406f, -0.12403300404548645f, -0.1930069625377655f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7792343497276306f, 0.03639582172036171f, -0.22608445584774017f, -0.5940053462982178f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24738509953022003f, -0.0629744827747345f, -0.08515878766775131f, -0.23045267164707184f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.36283692717552185f, 0.10568461567163467f, -0.16334816813468933f, -0.3222614824771881f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1378166824579239f, 0.23712460696697235f, -0.03988541290163994f, -0.5595112442970276f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0966879278421402f, -0.04106515645980835f, 0.044712722301483154f, -0.1553562581539154f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5580562949180603f, -0.649736225605011f, 0.11145242303609848f, 0.3315046727657318f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6071527004241943f, -0.3008533716201782f, -0.06432367861270905f, -0.2353738695383072f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3438764214515686f, 0.11381545662879944f, -0.11145569384098053f, -0.9986031651496887f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.15895144641399384f, 0.0415450781583786f, -0.5675103068351746f, -0.15250812470912933f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5653326511383057f, -0.4582417607307434f, -0.015047932974994183f, 0.12111126631498337f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.27479732036590576f, -0.12444479763507843f, -0.4301322102546692f, -0.11848732084035873f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08849292993545532f, -0.13029025495052338f, -0.11188863962888718f, 0.30264508724212646f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.16138295829296112f, -0.007152237929403782f, -0.32750096917152405f, 0.3552873730659485f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4822129011154175f, -0.6621450185775757f, -0.2802276015281677f, -0.31885188817977905f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.1340324431657791f, 0.0837835744023323f, -0.6336411833763123f, 0.18487726151943207f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.25649890303611755f, -0.06652398407459259f, -0.38200339674949646f, -0.1679035872220993f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.009706650860607624f, 0.05107307806611061f, -0.4217527508735657f, 0.44503721594810486f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1584307998418808f, -0.321601003408432f, -0.17384813725948334f, -0.28964871168136597f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04248873144388199f, 0.007141272537410259f, 0.02311965450644493f, 0.302683562040329f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4498167634010315f, 0.1913025975227356f, 0.23388126492500305f, 0.13664941489696503f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09795082360506058f, -0.054586444050073624f, 0.32369422912597656f, -0.24347589910030365f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04157057777047157f, -1.0798248052597046f, 0.32871246337890625f, 0.7521941065788269f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5829160809516907f, -0.20676574110984802f, -0.4413607716560364f, -0.7461797595024109f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13777752220630646f, 0.12835627794265747f, -0.2382647544145584f, -0.7654587626457214f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3333217203617096f, -0.039630498737096786f, 0.24044843018054962f, -0.3576901853084564f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2919570803642273f, -0.20870864391326904f, -0.07110331207513809f, -0.035032227635383606f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.39483144879341125f, -0.08837196230888367f, 0.5691225528717041f, -0.31432002782821655f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.14483599364757538f, 0.126999631524086f, 0.3870925009250641f, 0.27619317173957825f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.052769191563129425f, 0.012107409536838531f, -0.5778167843818665f, 0.09647858887910843f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5534887909889221f, -0.19457165896892548f, -0.16049470007419586f, -0.028066692873835564f);
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

					w = _mm_set_ps(-0.29990530014038086f, 0.1185426339507103f, -0.16302601993083954f, -0.4297262132167816f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18165968358516693f, 0.3971337676048279f, -0.845062255859375f, -0.2657269537448883f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.602726936340332f, 0.0016861865296959877f, -0.06377016007900238f, -0.6324825882911682f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8045713901519775f, -0.27217093110084534f, -0.6706312894821167f, -0.7969393134117126f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06353596597909927f, -0.02048865333199501f, 0.47759318351745605f, -1.2559267282485962f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.046309441328048706f, -0.005409036763012409f, 0.37884435057640076f, 0.39174947142601013f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.20817925035953522f, -0.04236456751823425f, -0.37021636962890625f, -0.1331949681043625f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7612044215202332f, 0.11702919006347656f, 0.2066931575536728f, -0.26308876276016235f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24611175060272217f, -0.004288933239877224f, -0.576531171798706f, -0.6868459582328796f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.266374409198761f, -0.6079086065292358f, 0.1400654911994934f, 0.6975941061973572f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.38061726093292236f, -0.03634500876069069f, -0.46775439381599426f, -0.39171692728996277f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07908841222524643f, -0.22774019837379456f, -0.11210770159959793f, -0.32889097929000854f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.597731351852417f, -0.20919062197208405f, 0.34856489300727844f, 0.299594908952713f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2559611201286316f, 0.003610993968322873f, 0.38540685176849365f, -0.25457119941711426f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07549223303794861f, -0.026893096044659615f, -0.5438734889030457f, -0.6006211638450623f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.39485180377960205f, -0.44197916984558105f, 0.013353338465094566f, -0.27419257164001465f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.40387675166130066f, -0.005988776218146086f, -0.12430386990308762f, -0.03696271777153015f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05078341066837311f, 0.6007749438285828f, -0.43831393122673035f, -0.8326417803764343f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7161797881126404f, 0.030896153301000595f, -0.8385765552520752f, -0.14332138001918793f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.1716399192810059f, -0.273404061794281f, -0.6178743243217468f, -0.0764564797282219f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06337323784828186f, 0.04587871953845024f, 0.10789093375205994f, -0.29147008061408997f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6049411296844482f, 0.4795812964439392f, 0.3301948010921478f, -0.1011476069688797f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.204999178647995f, -0.0633411779999733f, -0.02822098694741726f, -0.24389298260211945f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.43602868914604187f, -0.24592529237270355f, -0.1512167900800705f, -0.47588348388671875f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.005234242882579565f, 0.008953986689448357f, 0.15797261893749237f, 0.19680842757225037f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22275617718696594f, -0.2903386950492859f, 0.07591705024242401f, 0.4871833622455597f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.825077474117279f, -0.04514346271753311f, -0.3688757121562958f, -0.7537057399749756f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5910415649414062f, -0.5959977507591248f, -0.18837568163871765f, -0.08039983361959457f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7561604976654053f, -0.05799097195267677f, 0.03400658071041107f, -0.31824561953544617f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16726456582546234f, 0.24474111199378967f, 0.5323632955551147f, 0.6470536589622498f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.07152773439884186f, 0.008422854356467724f, -0.518122136592865f, -0.257741242647171f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07045476138591766f, -0.25689059495925903f, -0.11020180583000183f, -0.19951030611991882f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.04449614882469177f, 0.03278613090515137f, -0.228925883769989f, 0.07822152972221375f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13048848509788513f, 0.022946706041693687f, 0.13727113604545593f, -0.6353296041488647f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.18548250198364258f, 0.013751188293099403f, -0.3143799602985382f, 0.06694936752319336f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7875181436538696f, -0.3010813295841217f, -0.3070753514766693f, -0.5309246182441711f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10674536973237991f, 0.005176875740289688f, 0.8529148101806641f, -0.150369331240654f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.21259738504886627f, 0.0738489106297493f, 0.1801694631576538f, 0.26443782448768616f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4155234396457672f, 0.02218969538807869f, 0.3798954486846924f, -0.22418081760406494f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20203784108161926f, 0.1654873639345169f, 0.19157631695270538f, -0.12258774787187576f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.48945653438568115f, -0.015634406358003616f, 0.02129155583679676f, -0.05031803250312805f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2461603879928589f, -0.12773840129375458f, -0.1739332228899002f, 0.16412030160427094f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4050278067588806f, -0.02105655148625374f, 0.1101875975728035f, -0.23554068803787231f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8420195579528809f, 0.15900394320487976f, -0.077813059091568f, -0.15171602368354797f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.05226803198456764f, -0.05785483494400978f, 0.4161050021648407f, 0.026007018983364105f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07590880990028381f, 0.27362874150276184f, 0.1530887633562088f, 0.2226884961128235f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.040643833577632904f, 0.0040262178517878056f, -0.4374401867389679f, 0.1819465011358261f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.41288191080093384f, 0.11007951945066452f, -0.1315523236989975f, -0.09713920205831528f);
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
	alignas(16) static float x7 [4][4][8] = {};
	for (int i = 0; i < 4; i += 1)
	{
		for (int j = 0; j < 4; j += 1)
		{
			x7[i][j][0] = -0.012277374044060707f;
			x7[i][j][1] = 0.026992788538336754f;
			x7[i][j][2] = -0.0390363372862339f;
			x7[i][j][3] = 0.004011774901300669f;
			x7[i][j][4] = -0.008895385079085827f;
			x7[i][j][5] = 0.018138058483600616f;
			x7[i][j][6] = -0.20303519070148468f;
			x7[i][j][7] = -0.044468991458415985f;
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

					w = _mm_set_ps(8.74406541697681e-05f, -0.024280695244669914f, -0.07155165821313858f, -0.14263075590133667f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.02884865552186966f, -0.3314945101737976f, -0.14900389313697815f, -0.0680573433637619f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.002565407659858465f, 0.049192626029253006f, -0.7061242461204529f, 0.18697772920131683f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0036720894277095795f, 0.43058067560195923f, 0.039651837199926376f, -0.2915407717227936f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.014576766639947891f, 0.08166235685348511f, 0.002683902159333229f, 0.05428066849708557f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1741582751274109f, -0.033008042722940445f, -0.02302192524075508f, 0.026397090405225754f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.058036815375089645f, -0.030617505311965942f, 0.05029713362455368f, 0.10745465755462646f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0028625221457332373f, 0.21198463439941406f, 0.026801591739058495f, 0.054606784135103226f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03148030489683151f, 0.047720685601234436f, -0.5480930209159851f, -0.2696867287158966f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.15691959857940674f, 0.2854442596435547f, 0.053387608379125595f, 0.6499022245407104f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.037839073687791824f, -0.027381697669625282f, 0.07174744457006454f, 0.10572145879268646f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.012348750606179237f, 0.1880856603384018f, -0.03866250067949295f, 0.054950665682554245f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0019009705865755677f, 0.15174600481987f, -0.05812384560704231f, -0.13631752133369446f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.010613391175866127f, 0.19351966679096222f, -0.0008981378632597625f, -0.14423991739749908f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03682214021682739f, -0.3294193148612976f, -0.19279566407203674f, -0.18123680353164673f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20077499747276306f, 0.16514550149440765f, -0.0002957907854579389f, -0.014422687701880932f);
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
	scores[0] = 0.146591f
	 + 0.039713f * x7[0][0][0] - 0.057936f * x7[0][0][1] + 0.007900f * x7[0][0][2] - 0.097968f * x7[0][0][3]
	 + 0.015750f * x7[0][0][4] + 0.006859f * x7[0][0][5] - 0.041144f * x7[0][0][6] + 0.036883f * x7[0][0][7]
	 + 0.015409f * x7[0][1][0] + 0.103603f * x7[0][1][1] - 0.105541f * x7[0][1][2] - 0.103396f * x7[0][1][3]
	 - 0.156265f * x7[0][1][4] - 0.112704f * x7[0][1][5] + 0.012826f * x7[0][1][6] - 0.001262f * x7[0][1][7]
	 - 0.019672f * x7[0][2][0] + 0.094816f * x7[0][2][1] - 0.217649f * x7[0][2][2] + 0.121501f * x7[0][2][3]
	 - 0.269151f * x7[0][2][4] - 0.032411f * x7[0][2][5] + 0.004418f * x7[0][2][6] - 0.132697f * x7[0][2][7]
	 - 0.057769f * x7[0][3][0] - 0.011076f * x7[0][3][1] - 0.130132f * x7[0][3][2] + 0.110164f * x7[0][3][3]
	 - 0.068478f * x7[0][3][4] + 0.151880f * x7[0][3][5] + 0.011184f * x7[0][3][6] - 0.290330f * x7[0][3][7]
	 + 0.144782f * x7[1][0][0] - 0.014603f * x7[1][0][1] + 0.226488f * x7[1][0][2] - 0.020905f * x7[1][0][3]
	 + 0.038021f * x7[1][0][4] - 0.054887f * x7[1][0][5] - 0.054705f * x7[1][0][6] - 0.099612f * x7[1][0][7]
	 + 0.044440f * x7[1][1][0] - 0.016570f * x7[1][1][1] + 0.075798f * x7[1][1][2] + 0.132640f * x7[1][1][3]
	 + 0.043725f * x7[1][1][4] - 0.060153f * x7[1][1][5] - 0.040643f * x7[1][1][6] + 0.008667f * x7[1][1][7]
	 - 0.413688f * x7[1][2][0] - 0.166584f * x7[1][2][1] + 0.158099f * x7[1][2][2] - 0.122048f * x7[1][2][3]
	 - 0.119876f * x7[1][2][4] + 0.036765f * x7[1][2][5] - 0.563382f * x7[1][2][6] + 0.124394f * x7[1][2][7]
	 + 0.024934f * x7[1][3][0] - 0.013090f * x7[1][3][1] + 0.039049f * x7[1][3][2] - 0.062008f * x7[1][3][3]
	 + 0.023300f * x7[1][3][4] - 0.105591f * x7[1][3][5] + 0.003040f * x7[1][3][6] + 0.102600f * x7[1][3][7]
	 - 0.149325f * x7[2][0][0] + 0.148808f * x7[2][0][1] + 0.006134f * x7[2][0][2] + 0.066517f * x7[2][0][3]
	 + 0.101491f * x7[2][0][4] - 0.123639f * x7[2][0][5] - 0.020956f * x7[2][0][6] + 0.062203f * x7[2][0][7]
	 + 0.241067f * x7[2][1][0] + 0.058475f * x7[2][1][1] - 0.329038f * x7[2][1][2] + 0.029901f * x7[2][1][3]
	 + 0.374913f * x7[2][1][4] + 0.124002f * x7[2][1][5] - 0.304354f * x7[2][1][6] - 0.013223f * x7[2][1][7]
	 + 0.093615f * x7[2][2][0] + 0.279327f * x7[2][2][1] + 0.245588f * x7[2][2][2] - 0.014436f * x7[2][2][3]
	 - 0.061660f * x7[2][2][4] + 0.041709f * x7[2][2][5] - 0.108088f * x7[2][2][6] + 0.017427f * x7[2][2][7]
	 + 0.068948f * x7[2][3][0] - 0.021352f * x7[2][3][1] + 0.096782f * x7[2][3][2] + 0.004327f * x7[2][3][3]
	 + 0.051659f * x7[2][3][4] - 0.068878f * x7[2][3][5] - 0.030395f * x7[2][3][6] + 0.138163f * x7[2][3][7]
	 - 0.017922f * x7[3][0][0] + 0.101804f * x7[3][0][1] - 0.015597f * x7[3][0][2] - 0.014239f * x7[3][0][3]
	 - 0.089221f * x7[3][0][4] + 0.009417f * x7[3][0][5] - 0.022989f * x7[3][0][6] + 0.038116f * x7[3][0][7]
	 + 0.016444f * x7[3][1][0] + 0.036824f * x7[3][1][1] + 0.013129f * x7[3][1][2] + 0.017805f * x7[3][1][3]
	 + 0.000474f * x7[3][1][4] + 0.065128f * x7[3][1][5] - 0.039606f * x7[3][1][6] + 0.067690f * x7[3][1][7]
	 + 0.055572f * x7[3][2][0] - 0.018688f * x7[3][2][1] + 0.093565f * x7[3][2][2] - 0.005998f * x7[3][2][3]
	 + 0.039753f * x7[3][2][4] - 0.061840f * x7[3][2][5] - 0.016479f * x7[3][2][6] + 0.192754f * x7[3][2][7]
	 + 0.010764f * x7[3][3][0] - 0.014826f * x7[3][3][1] + 0.012653f * x7[3][3][2] + 0.012936f * x7[3][3][3]
	 + 0.010748f * x7[3][3][4] - 0.070969f * x7[3][3][5] - 0.012845f * x7[3][3][6] + 0.051834f * x7[3][3][7];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.173580f
	 - 0.054227f * x7[0][0][0] - 0.015199f * x7[0][0][1] - 0.035061f * x7[0][0][2] + 0.046802f * x7[0][0][3]
	 + 0.030680f * x7[0][0][4] - 0.011552f * x7[0][0][5] + 0.022648f * x7[0][0][6] - 0.011474f * x7[0][0][7]
	 + 0.027347f * x7[0][1][0] + 0.068865f * x7[0][1][1] - 0.071328f * x7[0][1][2] - 0.136784f * x7[0][1][3]
	 - 0.050964f * x7[0][1][4] - 0.202517f * x7[0][1][5] + 0.009050f * x7[0][1][6] + 0.252823f * x7[0][1][7]
	 + 0.028199f * x7[0][2][0] + 0.081095f * x7[0][2][1] - 0.030290f * x7[0][2][2] + 0.252404f * x7[0][2][3]
	 - 0.177049f * x7[0][2][4] + 0.089442f * x7[0][2][5] - 0.022541f * x7[0][2][6] + 0.021516f * x7[0][2][7]
	 - 0.075387f * x7[0][3][0] - 0.044082f * x7[0][3][1] - 0.218017f * x7[0][3][2] + 0.246107f * x7[0][3][3]
	 - 0.160811f * x7[0][3][4] + 0.196328f * x7[0][3][5] - 0.015879f * x7[0][3][6] - 0.476067f * x7[0][3][7]
	 - 0.016486f * x7[1][0][0] + 0.020313f * x7[1][0][1] + 0.058198f * x7[1][0][2] + 0.056182f * x7[1][0][3]
	 - 0.077611f * x7[1][0][4] + 0.124143f * x7[1][0][5] - 0.001682f * x7[1][0][6] - 0.364799f * x7[1][0][7]
	 + 0.047961f * x7[1][1][0] - 0.020656f * x7[1][1][1] + 0.085700f * x7[1][1][2] + 0.075265f * x7[1][1][3]
	 + 0.051502f * x7[1][1][4] - 0.177230f * x7[1][1][5] - 0.043483f * x7[1][1][6] + 0.052188f * x7[1][1][7]
	 - 0.347684f * x7[1][2][0] - 0.168221f * x7[1][2][1] + 0.217898f * x7[1][2][2] - 0.103186f * x7[1][2][3]
	 - 0.086347f * x7[1][2][4] + 0.110767f * x7[1][2][5] - 0.532880f * x7[1][2][6] + 0.180346f * x7[1][2][7]
	 + 0.049355f * x7[1][3][0] - 0.052625f * x7[1][3][1] + 0.005077f * x7[1][3][2] - 0.062512f * x7[1][3][3]
	 + 0.002660f * x7[1][3][4] - 0.090670f * x7[1][3][5] - 0.067716f * x7[1][3][6] + 0.107965f * x7[1][3][7]
	 - 0.211406f * x7[2][0][0] + 0.180489f * x7[2][0][1] - 0.075153f * x7[2][0][2] + 0.005418f * x7[2][0][3]
	 - 0.000331f * x7[2][0][4] - 0.065784f * x7[2][0][5] + 0.040057f * x7[2][0][6] - 0.162152f * x7[2][0][7]
	 + 0.255118f * x7[2][1][0] - 0.001777f * x7[2][1][1] - 0.287820f * x7[2][1][2] - 0.193708f * x7[2][1][3]
	 + 0.362300f * x7[2][1][4] - 0.354069f * x7[2][1][5] - 0.291962f * x7[2][1][6] + 0.092825f * x7[2][1][7]
	 + 0.137377f * x7[2][2][0] + 0.243532f * x7[2][2][1] + 0.166804f * x7[2][2][2] - 0.010703f * x7[2][2][3]
	 - 0.126921f * x7[2][2][4] + 0.098267f * x7[2][2][5] - 0.103181f * x7[2][2][6] + 0.032537f * x7[2][2][7]
	 + 0.067935f * x7[2][3][0] - 0.048665f * x7[2][3][1] + 0.013223f * x7[2][3][2] - 0.143501f * x7[2][3][3]
	 + 0.018273f * x7[2][3][4] + 0.019079f * x7[2][3][5] - 0.062800f * x7[2][3][6] + 0.157730f * x7[2][3][7]
	 - 0.035674f * x7[3][0][0] + 0.097893f * x7[3][0][1] + 0.019579f * x7[3][0][2] + 0.044919f * x7[3][0][3]
	 - 0.082197f * x7[3][0][4] + 0.036700f * x7[3][0][5] + 0.013635f * x7[3][0][6] + 0.066314f * x7[3][0][7]
	 + 0.031929f * x7[3][1][0] + 0.040177f * x7[3][1][1] + 0.078346f * x7[3][1][2] + 0.134805f * x7[3][1][3]
	 + 0.024968f * x7[3][1][4] + 0.006337f * x7[3][1][5] - 0.019601f * x7[3][1][6] + 0.145330f * x7[3][1][7]
	 + 0.053599f * x7[3][2][0] + 0.005245f * x7[3][2][1] + 0.106299f * x7[3][2][2] - 0.053637f * x7[3][2][3]
	 + 0.022574f * x7[3][2][4] - 0.022330f * x7[3][2][5] + 0.006578f * x7[3][2][6] + 0.173273f * x7[3][2][7]
	 + 0.028646f * x7[3][3][0] - 0.008219f * x7[3][3][1] - 0.069698f * x7[3][3][2] - 0.164000f * x7[3][3][3]
	 + 0.006480f * x7[3][3][4] + 0.135450f * x7[3][3][5] - 0.027384f * x7[3][3][6] - 0.019869f * x7[3][3][7];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.203630f
	 - 0.192857f * x7[0][0][0] + 0.020723f * x7[0][0][1] - 0.259363f * x7[0][0][2] - 0.101170f * x7[0][0][3]
	 - 0.102663f * x7[0][0][4] + 0.154796f * x7[0][0][5] + 0.055818f * x7[0][0][6] - 0.408485f * x7[0][0][7]
	 - 0.079496f * x7[0][1][0] + 0.162466f * x7[0][1][1] - 0.173459f * x7[0][1][2] + 0.060777f * x7[0][1][3]
	 - 0.170530f * x7[0][1][4] + 0.054812f * x7[0][1][5] + 0.053342f * x7[0][1][6] - 0.039411f * x7[0][1][7]
	 + 0.058236f * x7[0][2][0] + 0.097543f * x7[0][2][1] - 0.030722f * x7[0][2][2] + 0.154780f * x7[0][2][3]
	 - 0.197722f * x7[0][2][4] - 0.091362f * x7[0][2][5] - 0.012405f * x7[0][2][6] + 0.204375f * x7[0][2][7]
	 + 0.030577f * x7[0][3][0] + 0.017506f * x7[0][3][1] + 0.056776f * x7[0][3][2] + 0.085424f * x7[0][3][3]
	 + 0.050816f * x7[0][3][4] - 0.050373f * x7[0][3][5] - 0.002393f * x7[0][3][6] + 0.167656f * x7[0][3][7]
	 + 0.145011f * x7[1][0][0] - 0.019368f * x7[1][0][1] + 0.220486f * x7[1][0][2] - 0.161218f * x7[1][0][3]
	 + 0.036644f * x7[1][0][4] + 0.016228f * x7[1][0][5] - 0.084594f * x7[1][0][6] - 0.274984f * x7[1][0][7]
	 + 0.051674f * x7[1][1][0] - 0.015412f * x7[1][1][1] + 0.068150f * x7[1][1][2] + 0.178231f * x7[1][1][3]
	 + 0.019454f * x7[1][1][4] - 0.120898f * x7[1][1][5] - 0.043394f * x7[1][1][6] - 0.056396f * x7[1][1][7]
	 - 0.513692f * x7[1][2][0] - 0.212119f * x7[1][2][1] + 0.138993f * x7[1][2][2] - 0.013572f * x7[1][2][3]
	 - 0.170846f * x7[1][2][4] - 0.088678f * x7[1][2][5] - 0.646294f * x7[1][2][6] + 0.042442f * x7[1][2][7]
	 - 0.031371f * x7[1][3][0] + 0.001955f * x7[1][3][1] - 0.037285f * x7[1][3][2] - 0.100323f * x7[1][3][3]
	 - 0.008985f * x7[1][3][4] - 0.104457f * x7[1][3][5] + 0.009441f * x7[1][3][6] - 0.056731f * x7[1][3][7]
	 - 0.205258f * x7[2][0][0] + 0.155842f * x7[2][0][1] - 0.032672f * x7[2][0][2] - 0.002884f * x7[2][0][3]
	 + 0.058998f * x7[2][0][4] - 0.152717f * x7[2][0][5] - 0.009498f * x7[2][0][6] - 0.030672f * x7[2][0][7]
	 + 0.296803f * x7[2][1][0] + 0.044487f * x7[2][1][1] - 0.411389f * x7[2][1][2] - 0.106168f * x7[2][1][3]
	 + 0.468752f * x7[2][1][4] - 0.114859f * x7[2][1][5] - 0.376489f * x7[2][1][6] + 0.002484f * x7[2][1][7]
	 + 0.115982f * x7[2][2][0] + 0.373801f * x7[2][2][1] + 0.250801f * x7[2][2][2] - 0.037471f * x7[2][2][3]
	 - 0.158857f * x7[2][2][4] + 0.180888f * x7[2][2][5] - 0.133661f * x7[2][2][6] + 0.069391f * x7[2][2][7]
	 + 0.027122f * x7[2][3][0] - 0.014988f * x7[2][3][1] + 0.083204f * x7[2][3][2] - 0.115854f * x7[2][3][3]
	 + 0.022357f * x7[2][3][4] - 0.050709f * x7[2][3][5] - 0.013160f * x7[2][3][6] + 0.113088f * x7[2][3][7]
	 + 0.017413f * x7[3][0][0] + 0.126449f * x7[3][0][1] - 0.050428f * x7[3][0][2] - 0.040287f * x7[3][0][3]
	 - 0.085801f * x7[3][0][4] - 0.121979f * x7[3][0][5] - 0.033741f * x7[3][0][6] + 0.061382f * x7[3][0][7]
	 + 0.020955f * x7[3][1][0] + 0.033571f * x7[3][1][1] + 0.044412f * x7[3][1][2] + 0.116292f * x7[3][1][3]
	 + 0.030611f * x7[3][1][4] + 0.042057f * x7[3][1][5] - 0.040029f * x7[3][1][6] + 0.115872f * x7[3][1][7]
	 + 0.091060f * x7[3][2][0] - 0.032675f * x7[3][2][1] + 0.074481f * x7[3][2][2] + 0.045809f * x7[3][2][3]
	 + 0.044122f * x7[3][2][4] - 0.001376f * x7[3][2][5] - 0.033042f * x7[3][2][6] + 0.208622f * x7[3][2][7]
	 + 0.031926f * x7[3][3][0] - 0.010609f * x7[3][3][1] + 0.066290f * x7[3][3][2] + 0.032779f * x7[3][3][3]
	 + 0.046445f * x7[3][3][4] + 0.096061f * x7[3][3][5] - 0.039415f * x7[3][3][6] + 0.160556f * x7[3][3][7];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

	scores[3] = 0.413834f
	 + 0.056669f * x7[0][0][0] - 0.036947f * x7[0][0][1] - 0.001759f * x7[0][0][2] - 0.033201f * x7[0][0][3]
	 - 0.019868f * x7[0][0][4] + 0.027345f * x7[0][0][5] - 0.000837f * x7[0][0][6] - 0.051626f * x7[0][0][7]
	 + 0.031408f * x7[0][1][0] + 0.205966f * x7[0][1][1] - 0.164579f * x7[0][1][2] - 0.169465f * x7[0][1][3]
	 - 0.279123f * x7[0][1][4] - 0.258573f * x7[0][1][5] + 0.022834f * x7[0][1][6] + 0.134311f * x7[0][1][7]
	 + 0.112856f * x7[0][2][0] + 0.163099f * x7[0][2][1] - 0.262258f * x7[0][2][2] + 0.066807f * x7[0][2][3]
	 - 0.495739f * x7[0][2][4] - 0.055297f * x7[0][2][5] - 0.051104f * x7[0][2][6] + 0.122503f * x7[0][2][7]
	 + 0.032834f * x7[0][3][0] - 0.033736f * x7[0][3][1] + 0.001610f * x7[0][3][2] + 0.018892f * x7[0][3][3]
	 - 0.026739f * x7[0][3][4] + 0.002870f * x7[0][3][5] - 0.002354f * x7[0][3][6] + 0.034326f * x7[0][3][7]
	 + 0.297966f * x7[1][0][0] - 0.028843f * x7[1][0][1] + 0.480841f * x7[1][0][2] + 0.064008f * x7[1][0][3]
	 + 0.086948f * x7[1][0][4] - 0.011713f * x7[1][0][5] - 0.151775f * x7[1][0][6] - 0.323008f * x7[1][0][7]
	 + 0.088319f * x7[1][1][0] - 0.017671f * x7[1][1][1] + 0.114063f * x7[1][1][2] + 0.123179f * x7[1][1][3]
	 + 0.091270f * x7[1][1][4] - 0.198498f * x7[1][1][5] - 0.076323f * x7[1][1][6] - 0.086884f * x7[1][1][7]
	 - 1.016662f * x7[1][2][0] - 0.365376f * x7[1][2][1] + 0.236787f * x7[1][2][2] + 0.025678f * x7[1][2][3]
	 - 0.329167f * x7[1][2][4] + 0.269125f * x7[1][2][5] - 1.279697f * x7[1][2][6] + 0.051291f * x7[1][2][7]
	 - 0.002430f * x7[1][3][0] + 0.002244f * x7[1][3][1] + 0.000777f * x7[1][3][2] + 0.026875f * x7[1][3][3]
	 + 0.011178f * x7[1][3][4] - 0.029813f * x7[1][3][5] - 0.005668f * x7[1][3][6] - 0.009019f * x7[1][3][7]
	 - 0.338195f * x7[2][0][0] + 0.374734f * x7[2][0][1] + 0.012717f * x7[2][0][2] + 0.097666f * x7[2][0][3]
	 + 0.209601f * x7[2][0][4] + 0.040888f * x7[2][0][5] - 0.020227f * x7[2][0][6] + 0.101744f * x7[2][0][7]
	 + 0.594818f * x7[2][1][0] + 0.122863f * x7[2][1][1] - 0.834567f * x7[2][1][2] - 0.036305f * x7[2][1][3]
	 + 0.852176f * x7[2][1][4] + 0.239963f * x7[2][1][5] - 0.715898f * x7[2][1][6] - 0.072620f * x7[2][1][7]
	 + 0.222769f * x7[2][2][0] + 0.600003f * x7[2][2][1] + 0.443072f * x7[2][2][2] - 0.131683f * x7[2][2][3]
	 - 0.241243f * x7[2][2][4] + 0.177887f * x7[2][2][5] - 0.235876f * x7[2][2][6] - 0.229162f * x7[2][2][7]
	 + 0.007195f * x7[2][3][0] - 0.034886f * x7[2][3][1] + 0.009887f * x7[2][3][2] + 0.003803f * x7[2][3][3]
	 + 0.005601f * x7[2][3][4] - 0.057898f * x7[2][3][5] - 0.011879f * x7[2][3][6] - 0.003092f * x7[2][3][7]
	 - 0.036370f * x7[3][0][0] + 0.260457f * x7[3][0][1] - 0.119989f * x7[3][0][2] - 0.077260f * x7[3][0][3]
	 - 0.214047f * x7[3][0][4] - 0.000340f * x7[3][0][5] - 0.023764f * x7[3][0][6] + 0.028787f * x7[3][0][7]
	 - 0.024087f * x7[3][1][0] + 0.102234f * x7[3][1][1] - 0.029580f * x7[3][1][2] + 0.022652f * x7[3][1][3]
	 - 0.037791f * x7[3][1][4] + 0.036671f * x7[3][1][5] - 0.013290f * x7[3][1][6] + 0.031362f * x7[3][1][7]
	 + 0.019745f * x7[3][2][0] + 0.007788f * x7[3][2][1] + 0.029340f * x7[3][2][2] - 0.018691f * x7[3][2][3]
	 - 0.025068f * x7[3][2][4] - 0.022239f * x7[3][2][5] + 0.008744f * x7[3][2][6] + 0.014529f * x7[3][2][7]
	 + 0.006714f * x7[3][3][0] - 0.003022f * x7[3][3][1] - 0.013082f * x7[3][3][2] + 0.013638f * x7[3][3][3]
	 + 0.007281f * x7[3][3][4] - 0.002934f * x7[3][3][5] - 0.005551f * x7[3][3][6] - 0.009504f * x7[3][3][7];
	scores[3] = scores[3] > 0.0f ? scores[3] : 0.0f;

}



void Fy1500_Conf::predict(const BallCandidates::PatchYUVClassified& patch, double meanBrightness) {
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
double Fy1500_Conf::getRadius() {return scores[0];}
Vector2d Fy1500_Conf::getCenter() {return Vector2d(scores[1], scores[2]);}
double Fy1500_Conf::getBallConfidence() {return scores[3];}
