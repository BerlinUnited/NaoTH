#include "fy_1500_new2_new_dataset.h"

#if WIN32
	#define alignas(x) __declspec(align(x))
#endif

#include <emmintrin.h>
void fy_1500_new2_new_dataset::cnn(float x0[16][16][1])
{
	__m128 w, x, y;

 	// Convolution Layer
	alignas(16) static float x1 [16][16][4] = {};
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{
			x1[i][j][0] = 0.009767674840986729f;
			x1[i][j][1] = 0.13150426745414734f;
			x1[i][j][2] = -0.13374480605125427f;
			x1[i][j][3] = 0.059240058064460754f;
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

					w = _mm_set_ps(0.38995611667633057f, 0.02982097491621971f, -0.1646222621202469f, -0.5023707747459412f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.22287394106388092f, 0.16912971436977386f, -0.623333752155304f, -0.33858340978622437f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.268717885017395f, -0.2993517518043518f, 0.42498886585235596f, 0.46592700481414795f);
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

					w = _mm_set_ps(0.37064942717552185f, 0.7245269417762756f, -0.38274523615837097f, 0.3379996418952942f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.16021911799907684f, -0.19503694772720337f, -0.3067008852958679f, 0.06901655346155167f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.15253332257270813f, 0.46381428837776184f, -0.13307343423366547f, 0.5004675388336182f);
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

					w = _mm_set_ps(0.358500212430954f, -0.1249036118388176f, -0.3193581998348236f, 0.3440358638763428f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.13894695043563843f, -0.1401119977235794f, -0.11696267873048782f, 0.009298964403569698f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.360251247882843f, 0.22288735210895538f, 0.3772929310798645f, -0.4012249708175659f);
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
			x2[i][j][0] = -0.08553943783044815f;
			x2[i][j][1] = -0.112612783908844f;
			x2[i][j][2] = 0.17583155632019043f;
			x2[i][j][3] = 0.10540248453617096f;
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

					w = _mm_set_ps(0.18218977749347687f, -1.1564297676086426f, -0.0623641163110733f, -0.4378393590450287f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4111669063568115f, -0.3311622142791748f, 0.07725291699171066f, 0.09209225326776505f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.41995203495025635f, -0.1474582999944687f, 0.13603714108467102f, -0.6979078650474548f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.26556798815727234f, 0.12477116286754608f, 0.059850722551345825f, -0.2655855119228363f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.329335480928421f, 0.14658823609352112f, 0.206661194562912f, 0.2739317715167999f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.014721347950398922f, -0.050714023411273956f, -0.14756019413471222f, -0.15692253410816193f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1489826887845993f, 0.05328443646430969f, -0.3120485246181488f, -0.3761080205440521f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.19991406798362732f, -0.3664952516555786f, 0.3079507648944855f, -0.5461974740028381f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.07611328363418579f, 0.1091434434056282f, 0.6878724098205566f, 0.0791051834821701f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5715343952178955f, -0.09344065934419632f, -1.1282721757888794f, 0.1483214795589447f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3554128408432007f, -0.2555825710296631f, 0.09229487925767899f, -0.07364211976528168f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.40023455023765564f, 0.08869904279708862f, -0.0519222728908062f, -0.2222389578819275f);
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

					w = _mm_set_ps(0.2274056226015091f, -0.112511046230793f, 0.07023730129003525f, 0.2697256803512573f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11200401186943054f, -0.04660860076546669f, 0.37428614497184753f, 0.12988810241222382f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2975345253944397f, 0.04325069859623909f, -0.355912983417511f, 0.11434117704629898f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4338219463825226f, -0.21804875135421753f, 0.06927590817213058f, 0.2902699112892151f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.10502034425735474f, -0.1205030158162117f, -0.352673202753067f, 0.2513561546802521f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.429352730512619f, -0.39894652366638184f, -0.15214958786964417f, 0.5817778706550598f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.43013623356819153f, -0.4325442314147949f, 0.126435324549675f, 0.23263677954673767f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11770139634609222f, -0.20872363448143005f, 0.008894376456737518f, 0.12500271201133728f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.3307897746562958f, 0.08994392305612564f, -0.4415477514266968f, 0.35920992493629456f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07880865782499313f, 0.29389575123786926f, -0.20225746929645538f, -0.11384861916303635f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20476174354553223f, -0.44476184248924255f, 0.009010003879666328f, -0.22412890195846558f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0877194032073021f, -0.39497900009155273f, 0.1578645557165146f, -0.31228816509246826f);
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

					w = _mm_set_ps(-0.08734665811061859f, 0.31245073676109314f, -0.07893409579992294f, 0.017467595636844635f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6635595560073853f, 0.4411276578903198f, 0.13662633299827576f, 0.08183519542217255f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12072603404521942f, 0.35532593727111816f, 0.10894135385751724f, 0.31712642312049866f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08864981681108475f, 0.225800558924675f, -0.09359489381313324f, -0.12259380519390106f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.06430785357952118f, -0.0006983248167671263f, -0.3249812722206116f, 0.3511512875556946f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2710424065589905f, 0.18823504447937012f, -0.21838165819644928f, 0.08457037061452866f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.48513033986091614f, -0.24986152350902557f, 0.0345870666205883f, 0.10221802443265915f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.34734535217285156f, 0.09300873428583145f, 0.32657337188720703f, -0.04971878230571747f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.10247308760881424f, 0.023136842995882034f, 0.12559279799461365f, -0.08285654336214066f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3187060058116913f, 0.21030910313129425f, 0.23098403215408325f, -0.32138678431510925f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0708107128739357f, -0.5981348752975464f, -0.1301783174276352f, 0.31348440051078796f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15905489027500153f, 0.1401679962873459f, 0.38367682695388794f, 0.01721818931400776f);
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
	for (int i = 0; i < 16; i += 1)
	{
		for (int j = 0; j < 16; j += 1)
		{

			x = _mm_load_ps((float*)&x2[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
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
			x4[i][j][0] = -0.11865241080522537f;
			x4[i][j][1] = 0.020929517224431038f;
			x4[i][j][2] = 0.040759384632110596f;
			x4[i][j][3] = -0.0015970428939908743f;
			x4[i][j][4] = 0.02053617127239704f;
			x4[i][j][5] = -0.0035489576403051615f;
			x4[i][j][6] = 0.2399865984916687f;
			x4[i][j][7] = -0.4020456075668335f;
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

					w = _mm_set_ps(-0.026627615094184875f, 0.08850187063217163f, 0.02296644076704979f, -0.1463661640882492f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3524075150489807f, 0.031106719747185707f, -0.09361051768064499f, 0.06814225763082504f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13400183618068695f, 0.24636241793632507f, -0.07626169174909592f, -0.10287310928106308f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.40424883365631104f, -0.4425686299800873f, -0.7380126118659973f, -0.5216579437255859f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4986509680747986f, -0.2339884489774704f, -0.12257873266935349f, 0.12267716974020004f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08721096068620682f, 0.3860381245613098f, -0.03368646278977394f, 0.18517890572547913f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4114648699760437f, 0.4034457206726074f, -0.18624795973300934f, 0.020992670208215714f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.48202741146087646f, -0.12036918848752975f, -0.14840561151504517f, 0.18795080482959747f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.488325834274292f, 0.040630191564559937f, -0.17256279289722443f, -0.2775250971317291f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2943727970123291f, -0.04299945756793022f, 0.07140842825174332f, 0.051656126976013184f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25360944867134094f, 0.13634487986564636f, -0.46402639150619507f, 0.022040724754333496f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.34460920095443726f, -0.9683117866516113f, -0.4663340449333191f, -0.005800806917250156f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.343095600605011f, 0.1148199588060379f, -0.004607043229043484f, 0.1699875444173813f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2704959511756897f, 0.07685321569442749f, 0.2246936857700348f, -0.370650976896286f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19617466628551483f, 0.1555575728416443f, 0.06476637721061707f, -0.24087578058242798f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2147706300020218f, -0.11515063792467117f, 0.0910622850060463f, 0.15355628728866577f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.0979095995426178f, 0.3032207489013672f, -0.6619988679885864f, -0.3467092514038086f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6171455383300781f, -0.4142327606678009f, 0.06719765812158585f, -0.2768811285495758f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2268364131450653f, 0.17335069179534912f, -0.07049737125635147f, -0.2878326177597046f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.42282363772392273f, -0.42013019323349f, -0.04678680747747421f, -0.28943994641304016f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.46673518419265747f, 0.3141700029373169f, -0.7147663235664368f, 0.33111417293548584f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1011648178100586f, -0.2382442057132721f, 0.19422385096549988f, -0.3300076723098755f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3683341145515442f, -0.06113450974225998f, -0.0898088589310646f, 0.17826394736766815f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0983593612909317f, 0.19169291853904724f, 0.3453630208969116f, 0.219042107462883f);
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

					w = _mm_set_ps(0.3101622760295868f, -0.2609167993068695f, 0.10858582705259323f, 0.00128656136803329f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11647399514913559f, -0.31367164850234985f, 0.1163560152053833f, -0.30982741713523865f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4855136573314667f, -0.0817299336194992f, 0.014629902318120003f, -0.1154174655675888f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3118172585964203f, -0.10245204716920853f, -0.2067921906709671f, 0.34779173135757446f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3076728582382202f, 0.10110103338956833f, 0.16925036907196045f, 0.24918179214000702f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10191943496465683f, -0.05978597328066826f, -0.14169101417064667f, -0.6614050269126892f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1774180680513382f, -0.3602299988269806f, 0.5238595604896545f, -0.3218984603881836f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2691098153591156f, 0.006674737669527531f, 0.07753065973520279f, -0.1370898187160492f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.4473331570625305f, 0.06692779064178467f, 0.26345643401145935f, -0.1413213014602661f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3359183967113495f, -0.184390589594841f, -0.2636961340904236f, 0.23475724458694458f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3407255709171295f, 0.03326761722564697f, -0.30753931403160095f, -0.19381366670131683f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.33655500411987305f, 0.1860135793685913f, 0.2046293318271637f, -0.1877547800540924f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.039695560932159424f, -0.4483063817024231f, 0.10305396467447281f, -0.13347505033016205f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.29976460337638855f, 0.03863929957151413f, 0.04821900278329849f, -0.3628745973110199f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.33432963490486145f, -0.18427209556102753f, -0.05658448487520218f, 0.14578820765018463f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1518486589193344f, -0.4624791145324707f, 0.09706409275531769f, -0.06032206863164902f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-1.034334421157837f, -0.032047074288129807f, -0.35309335589408875f, 0.25600239634513855f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4129279553890228f, -0.2784680724143982f, -0.007182656321674585f, -0.13583649694919586f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.27816617488861084f, -0.26688361167907715f, 0.13282331824302673f, 0.04310779646039009f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.25300467014312744f, -0.4703305661678314f, 0.4349125325679779f, -0.3625364601612091f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0437387116253376f, -0.06834426522254944f, -0.017146961763501167f, -0.35397249460220337f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6303097605705261f, -0.1281365603208542f, 0.5722547173500061f, 0.15081696212291718f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2180604785680771f, 0.06580028682947159f, -0.37053412199020386f, 0.2860402762889862f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15540523827075958f, 0.16198596358299255f, -0.6290138363838196f, -0.38592374324798584f);
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

					w = _mm_set_ps(0.12661506235599518f, -0.23488834500312805f, -0.10814894735813141f, 0.2784424424171448f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.28100892901420593f, 0.1289208084344864f, -0.08961448818445206f, -1.3518531322479248f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5650121569633484f, 0.22889287769794464f, 0.0068094306625425816f, 0.46167489886283875f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.024953583255410194f, 0.26557180285453796f, 0.05985063686966896f, 0.18710997700691223f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1574452817440033f, 0.38442423939704895f, 0.028875485062599182f, 0.38291609287261963f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.557248055934906f, 0.2205139398574829f, -0.5309664607048035f, -0.46664461493492126f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2163759022951126f, 0.1206289678812027f, -0.10034459084272385f, -0.022382741793990135f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1112331822514534f, -0.10455843806266785f, 0.356366902589798f, 0.33558687567710876f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.48416703939437866f, -0.47445306181907654f, 0.21565458178520203f, -0.05022510886192322f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.005645466968417168f, -0.31193575263023376f, -0.061957500874996185f, 0.07590139657258987f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07093163579702377f, -0.19619770348072052f, 0.12034597992897034f, -0.22191694378852844f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2402813881635666f, 0.014884049072861671f, -0.0810142233967781f, -0.00195792387239635f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3004774749279022f, -0.2286956012248993f, 0.28549787402153015f, 0.057631127536296844f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.310089111328125f, 0.07511696219444275f, -0.10816657543182373f, 0.04457033798098564f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.01436003390699625f, 0.12316924333572388f, 0.2786597013473511f, -0.07850490510463715f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05299772322177887f, -0.5176221132278442f, 0.24099476635456085f, 0.22206079959869385f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-1.4518661499023438f, -0.018514366820454597f, -0.028494570404291153f, 0.3105723559856415f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2118823081254959f, 0.005073670297861099f, 0.16752663254737854f, 0.5666472911834717f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18633297085762024f, 0.24752375483512878f, 0.005441834218800068f, -0.6425274610519409f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1079641580581665f, 0.15953223407268524f, 0.0292462520301342f, 0.05846327915787697f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3340747654438019f, -0.16752590239048004f, 0.035277802497148514f, 0.33177268505096436f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.33983734250068665f, -0.2332482784986496f, -0.2842259705066681f, 0.16721178591251373f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0621895007789135f, 0.011587642133235931f, -0.1483079344034195f, 0.16011357307434082f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03983497619628906f, -1.0408272743225098f, 0.24189044535160065f, -0.44290775060653687f);
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
	for (int i = 0; i < 8; i += 1)
	{
		for (int j = 0; j < 8; j += 1)
		{

			x = _mm_load_ps((float*)&x4[i][j][0]);
			x = _mm_max_ps(x, _mm_setzero_ps());
			_mm_store_ps((float*)&x4[i][j][0], x);

			x = _mm_load_ps((float*)&x4[i][j][4]);
			x = _mm_max_ps(x, _mm_setzero_ps());
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
			x6[i][j][0] = 0.15408854186534882f;
			x6[i][j][1] = 0.4124343991279602f;
			x6[i][j][2] = 0.11223293840885162f;
			x6[i][j][3] = 0.32347846031188965f;
			x6[i][j][4] = -0.28511592745780945f;
			x6[i][j][5] = 0.2963007092475891f;
			x6[i][j][6] = 0.0018243070226162672f;
			x6[i][j][7] = -0.024223607033491135f;
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

					w = _mm_set_ps(-0.204222172498703f, 0.08345675468444824f, -0.17906151711940765f, 0.08536065369844437f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04740126058459282f, 0.08843138068914413f, -0.021958669647574425f, -0.24925804138183594f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1835302859544754f, -0.1232951357960701f, 0.12382837384939194f, -0.08997809886932373f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.009834006428718567f, -0.2399623841047287f, 0.12323646247386932f, 0.14493340253829956f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.30989566445350647f, -0.0899139940738678f, 0.2291499525308609f, 0.02739771641790867f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10577220469713211f, -0.39378026127815247f, 0.21629221737384796f, 0.1984788328409195f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.027923941612243652f, -0.18224067986011505f, -0.06368527561426163f, 0.015380686149001122f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.052664756774902344f, -0.15763862431049347f, 0.20760399103164673f, -0.5718106031417847f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.21333420276641846f, -0.1484474241733551f, 0.13822826743125916f, 0.009147168137133121f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2727413773536682f, -0.1614043414592743f, 0.25643178820610046f, -0.5367341637611389f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06632407009601593f, 0.011608229950070381f, -0.0014307658420875669f, 0.04284598305821419f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0808548703789711f, -0.07258261740207672f, -0.21891051530838013f, -0.05313451588153839f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07996095716953278f, 0.03089616633951664f, 0.10147181153297424f, -0.051322486251592636f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1285877227783203f, -0.0474359467625618f, -0.10168489068746567f, -1.0681780576705933f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04294915869832039f, 0.5828408598899841f, -0.08618985861539841f, 0.00929341558367014f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5717630386352539f, -0.08036437630653381f, 0.015032337978482246f, -0.23016363382339478f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.106453537940979f, 0.023358754813671112f, -0.0807555764913559f, -0.008958830498158932f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04188883304595947f, -0.07604524493217468f, 0.03465927392244339f, -0.07067254930734634f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.23556868731975555f, 0.029796389862895012f, 0.22604697942733765f, -0.03317661210894585f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08492493629455566f, -0.21441225707530975f, -0.05345560610294342f, -0.39548611640930176f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2825881242752075f, 0.022594932466745377f, 0.30344173312187195f, 0.0029950446914881468f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09587188065052032f, -0.7450432181358337f, -0.06521618366241455f, -0.6715320944786072f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.17524227499961853f, -0.025231119245290756f, 0.10931485891342163f, 0.01106412336230278f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09609296917915344f, 0.1453116238117218f, 0.20376287400722504f, -0.5842586755752563f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12426398694515228f, -0.07754421979188919f, -0.1247926652431488f, -0.08795471489429474f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.18630918860435486f, 0.045284707099199295f, -0.03803505375981331f, -0.1025773361325264f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4026607573032379f, -0.0016768656205385923f, -0.3028084933757782f, 0.15560565888881683f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.015474271960556507f, 0.16107065975666046f, -0.13583233952522278f, -0.14607928693294525f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4669651985168457f, -0.044830773025751114f, 0.3908637464046478f, -0.08239441365003586f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14630940556526184f, -0.04173802584409714f, 0.06982606649398804f, -0.312050461769104f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2928530275821686f, -0.20858970284461975f, 0.1934102326631546f, 0.07921416312456131f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17480452358722687f, -0.08471736311912537f, 0.3662000298500061f, -0.20112504065036774f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.40719282627105713f, 0.010072879493236542f, 0.2621465027332306f, -0.15403494238853455f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4517643451690674f, -0.6138539910316467f, 0.35002240538597107f, -0.3630806505680084f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.022228846326470375f, 0.04050152003765106f, 0.04285430535674095f, 0.0024150435347110033f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.33342909812927246f, -0.19794102013111115f, -0.0743468701839447f, -0.32121917605400085f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.061978209763765335f, -0.012757635675370693f, 0.052588943392038345f, 0.07397521287202835f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.33416175842285156f, -0.06331869214773178f, -0.039169587194919586f, -0.2728120982646942f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09568170458078384f, 0.006514966953545809f, -0.07917884737253189f, -0.033431779593229294f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.01130116917192936f, 0.23123528063297272f, 0.05606808513402939f, 0.54167240858078f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5756253600120544f, 0.038044970482587814f, -0.4924953877925873f, 0.0560980960726738f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07730425149202347f, -0.3915085792541504f, -0.016895050182938576f, -0.33582088351249695f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25655150413513184f, 0.00542044173926115f, -0.24826934933662415f, 0.06672334671020508f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.204233780503273f, 0.008064095862209797f, 0.08569219708442688f, -0.19493336975574493f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4453323781490326f, 0.08184580504894257f, -0.352948933839798f, 0.03891977295279503f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16963565349578857f, 0.5674964189529419f, -0.08095760643482208f, 0.30762752890586853f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0985950231552124f, -0.01559873390942812f, -0.22014662623405457f, -0.3321211636066437f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2057497352361679f, 0.005782445427030325f, 0.6616744995117188f, -0.02462800219655037f);
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

					w = _mm_set_ps(0.052065834403038025f, -0.04391731694340706f, 0.0881228893995285f, 0.020248547196388245f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.029150262475013733f, 0.21491317451000214f, -0.13342136144638062f, 0.04028899967670441f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1658797711133957f, 0.08656435459852219f, -0.1262318193912506f, 0.08620517700910568f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17516136169433594f, 0.19398431479930878f, -0.007798321545124054f, -0.2634495794773102f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18679043650627136f, 0.04557204619050026f, 0.19821128249168396f, 0.011568908579647541f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04564061015844345f, -0.287332683801651f, -0.05415797606110573f, -0.28054723143577576f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.08032777160406113f, -0.05572628229856491f, 0.07004281878471375f, 0.029948094859719276f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1436428278684616f, 0.23222333192825317f, 0.06612663716077805f, 0.10683692246675491f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07154979556798935f, -0.09813240170478821f, 0.049664586782455444f, 0.009506751783192158f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.31970450282096863f, -0.4079612195491791f, 0.10087903589010239f, 0.17599493265151978f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.025713162496685982f, -0.035264644771814346f, 0.024258801713585854f, 0.06048615649342537f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.030289215967059135f, -0.177665576338768f, -0.21975424885749817f, 0.3116142451763153f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.17211709916591644f, -0.022250207141041756f, 0.09784690290689468f, -0.06273473054170609f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07260928303003311f, -0.632968544960022f, 0.19890302419662476f, -0.04352172836661339f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.415505588054657f, 0.1638706624507904f, -0.2719557583332062f, 0.2076881229877472f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08342000097036362f, 0.1828741729259491f, -0.23349669575691223f, 0.19738762080669403f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.16015177965164185f, -0.04449408873915672f, 0.09425404667854309f, 0.004208757076412439f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0387023389339447f, -0.11428827047348022f, 0.17566511034965515f, -0.29421210289001465f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.33733493089675903f, 0.03420087322592735f, -0.3051673173904419f, -0.012876849621534348f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.014528563246130943f, 0.03698109835386276f, -0.057668063789606094f, -0.13002201914787292f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2822878062725067f, -0.014495385810732841f, -0.26511427760124207f, -0.04891910031437874f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0010995622724294662f, 0.1025131568312645f, 0.02371799573302269f, -0.1289927065372467f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.34745508432388306f, 0.05305718630552292f, -0.2971497178077698f, -0.010434543713927269f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.014793413691222668f, 0.30143529176712036f, -0.1484043002128601f, 0.1491650938987732f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.23798926174640656f, 0.07060398906469345f, -0.1958134025335312f, -0.04945237934589386f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19248871505260468f, -0.12585943937301636f, -0.011953221634030342f, 0.18567176163196564f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2351495921611786f, 0.03957843780517578f, 0.21492521464824677f, -0.02576313354074955f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06644345074892044f, -0.17107707262039185f, -0.009268166497349739f, 0.09958112239837646f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.017968785017728806f, 0.03340001776814461f, 0.061558157205581665f, 0.09745476394891739f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.04702698439359665f, 0.1737174242734909f, -0.24818935990333557f, 0.09938319027423859f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3713350296020508f, -0.2248980700969696f, 0.06422470510005951f, -0.3029070794582367f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.011732862330973148f, -0.34831494092941284f, 0.7248165011405945f, 0.5898340940475464f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.19902239739894867f, -0.012852346524596214f, 0.18551583588123322f, -0.03268291801214218f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.29606112837791443f, -0.642124593257904f, 0.10033375769853592f, -0.38844236731529236f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14517410099506378f, 0.008185325190424919f, -0.10123707354068756f, -0.0006582763744518161f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20074611902236938f, 0.2242240309715271f, -0.10123489052057266f, -0.039236512035131454f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1963943988084793f, 0.005648869555443525f, 0.15348441898822784f, -0.08509131520986557f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.682475745677948f, 0.06270212680101395f, 0.11060021817684174f, -0.2008228749036789f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5502428412437439f, 0.08569569885730743f, -0.4398173987865448f, -0.10263466089963913f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3347306251525879f, 0.13137772679328918f, -0.1780313402414322f, -0.13541126251220703f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3501374423503876f, 0.10053134709596634f, -0.3247232437133789f, -0.06639046221971512f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3522706925868988f, -0.078785739839077f, 0.09639721363782883f, -0.24272191524505615f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07711207866668701f, 0.001163202803581953f, 0.05998952314257622f, -0.02820282243192196f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08007035404443741f, 0.3145548105239868f, 0.0004755062982439995f, -0.07067258656024933f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4811381995677948f, -0.034648675471544266f, 0.410293847322464f, -0.07598301768302917f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.28672900795936584f, -0.44967424869537354f, -0.017495544627308846f, 0.5782687664031982f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3495345711708069f, -0.12755991518497467f, -0.2961570918560028f, -0.19811946153640747f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3834349513053894f, 0.06053157523274422f, 0.20444989204406738f, 0.045717209577560425f);
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

					w = _mm_set_ps(0.1715114563703537f, -0.0730307400226593f, 0.10820632427930832f, -0.0417008176445961f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.016873417422175407f, -0.3540082275867462f, 0.19267071783542633f, -0.11420687288045883f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11773162335157394f, -0.17135931551456451f, -0.07592236995697021f, 0.09396619349718094f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1168869361281395f, -0.3900972604751587f, -0.14731740951538086f, 0.42433854937553406f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.0026818530168384314f, -0.11717361956834793f, 0.014314885251224041f, -0.02565743401646614f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.09629756957292557f, -0.2086097151041031f, -0.10889366269111633f, 0.22248056530952454f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2359047532081604f, -0.17803886532783508f, -0.18563899397850037f, 0.0831129252910614f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.134436696767807f, 0.2271135002374649f, -0.18066170811653137f, 0.4872722923755646f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10153166949748993f, -0.1138281598687172f, 0.13612309098243713f, -0.01994914934039116f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3558644950389862f, -0.34322866797447205f, -0.26302197575569153f, 0.38292714953422546f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.23123952746391296f, 0.07724478095769882f, 0.18883761763572693f, -0.00027146085631102324f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.1866113841533661f, -0.21544909477233887f, 0.12030839174985886f, -0.1836758255958557f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.19728384912014008f, 0.12417437136173248f, 0.1831389218568802f, -0.06931993365287781f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.025251729413866997f, 0.747484028339386f, 0.030727427452802658f, 0.1435859501361847f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18862344324588776f, -0.4433217942714691f, 0.08258619904518127f, -0.1474854052066803f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6181102395057678f, 0.46816712617874146f, 0.22437196969985962f, -0.11843626946210861f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.10262823104858398f, -0.013375299051404f, -0.05387105792760849f, 0.08774928003549576f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.06405114382505417f, 0.10828103125095367f, -0.15760205686092377f, -0.2044542282819748f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.13649442791938782f, 0.11222672462463379f, -0.09316875785589218f, 0.0830211415886879f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.31855636835098267f, 0.4091576337814331f, -0.06540843844413757f, 0.1672084778547287f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19202235341072083f, 0.11625022441148758f, -0.14438889920711517f, 0.05583856627345085f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.11523362249135971f, -0.048355523496866226f, -0.10569354146718979f, 0.12759117782115936f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3949129581451416f, 0.034662988036870956f, -0.24056491255760193f, 0.21247626841068268f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3227554261684418f, -0.08706529438495636f, -0.428164541721344f, 0.46405282616615295f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22442564368247986f, 0.08618306368589401f, -0.15878801047801971f, 0.12932980060577393f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19839394092559814f, -0.33922079205513f, -0.1691688746213913f, 0.37674111127853394f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5316746234893799f, -0.04085959494113922f, 0.43356674909591675f, -0.11451372504234314f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0932847186923027f, -0.6107532382011414f, 0.183088019490242f, -0.1209198459982872f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.23904268443584442f, -0.042008209973573685f, 0.13379213213920593f, -0.1466141939163208f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.08794503659009933f, -0.1726662963628769f, 0.3049143850803375f, -0.5236963033676147f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6252017617225647f, 0.2308236062526703f, -0.4181888699531555f, 0.015309554524719715f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.006998835597187281f, 0.05924294516444206f, -0.5203831195831299f, 0.31836241483688354f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.037648916244506836f, -0.021770616993308067f, 0.003429280361160636f, 0.024518122896552086f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5342044830322266f, 0.09431229531764984f, -0.10102225840091705f, -0.2429780662059784f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.384810209274292f, 0.039522308856248856f, -0.3340182304382324f, 0.16170452535152435f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09834758192300797f, 0.2609749138355255f, -0.018739286810159683f, -0.06840919703245163f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21788427233695984f, -0.001567137660458684f, -0.17742107808589935f, 0.11616398394107819f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.13198216259479523f, 0.21377603709697723f, -0.05715317651629448f, 0.273310124874115f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5704288482666016f, 0.06715094298124313f, -0.42066890001296997f, 0.23721034824848175f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06001153215765953f, 0.3699083626270294f, -0.30331867933273315f, 0.4541940689086914f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3198487162590027f, -0.03579866886138916f, -0.23820792138576508f, 0.14578470587730408f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.25637128949165344f, 0.3272266387939453f, -0.15495549142360687f, 0.35142385959625244f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.4048975110054016f, -0.009704915806651115f, 0.316876620054245f, -0.08653709292411804f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.39089611172676086f, -0.7703619003295898f, 0.18582309782505035f, 0.13156907260417938f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.32776764035224915f, -2.3124637664295733e-05f, 0.24159258604049683f, -0.10795893520116806f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.18745362758636475f, -0.2235768884420395f, 0.16424106061458588f, -0.0017001992091536522f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6914399266242981f, 0.038640569895505905f, -0.4834355115890503f, 0.49976420402526855f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8284852504730225f, 0.05087452381849289f, -0.31662485003471375f, 0.9301693439483643f);
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
			x7[i][j][0] = -0.0912688672542572f;
			x7[i][j][1] = -0.06266481429338455f;
			x7[i][j][2] = 0.03390238806605339f;
			x7[i][j][3] = 0.058933693915605545f;
			x7[i][j][4] = 0.18935748934745789f;
			x7[i][j][5] = 0.03803546726703644f;
			x7[i][j][6] = -0.08153510093688965f;
			x7[i][j][7] = 0.0018719437066465616f;
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

					w = _mm_set_ps(0.0682302787899971f, -0.13871394097805023f, -0.0896848514676094f, -0.2273792028427124f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2203465700149536f, 0.19804546236991882f, 0.20185935497283936f, -0.2683444023132324f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2583857476711273f, -0.5359411835670471f, 0.394481360912323f, 0.3406301736831665f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2771230936050415f, -0.39832186698913574f, -0.09939472377300262f, -0.7283251881599426f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18144994974136353f, -0.15486851334571838f, 0.3912206292152405f, 0.40946152806282043f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2964499294757843f, 0.3010043203830719f, -0.14414682984352112f, 0.18639284372329712f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3571758568286896f, 0.43701112270355225f, -0.2209203690290451f, -0.2897813022136688f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.45192164182662964f, 0.5180864334106445f, 0.11351960897445679f, 0.5512676239013672f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.040081169456243515f, -0.5801242589950562f, 1.093033790588379f, -0.6230038404464722f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6043198704719543f, -0.12131140381097794f, 0.23653890192508698f, -0.23677107691764832f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.24632172286510468f, -0.07960590720176697f, -0.3012542426586151f, 0.09136179089546204f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.38277560472488403f, -0.29773348569869995f, -0.1537466198205948f, -0.1048252210021019f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.6047025322914124f, 0.4230146110057831f, -0.3283095359802246f, -0.28962084650993347f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.75847327709198f, 0.5983681678771973f, -0.01335558108985424f, -0.042531661689281464f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.21992245316505432f, 0.12203190475702286f, 0.12393773347139359f, 0.22167451679706573f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0034222824033349752f, 0.09326183050870895f, -0.5067406296730042f, -0.042980123311281204f);
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
	scores[0] = 0.025418f
	 + 0.070019f * x7[0][0][0] + 0.112903f * x7[0][0][1] - 0.013422f * x7[0][0][2] + 0.059016f * x7[0][0][3]
	 - 0.136413f * x7[0][0][4] - 0.136305f * x7[0][0][5] - 0.175725f * x7[0][0][6] + 0.178956f * x7[0][0][7]
	 + 0.050786f * x7[0][1][0] - 0.053161f * x7[0][1][1] + 0.149733f * x7[0][1][2] - 0.005353f * x7[0][1][3]
	 - 0.158854f * x7[0][1][4] + 0.054149f * x7[0][1][5] - 0.259979f * x7[0][1][6] + 0.116079f * x7[0][1][7]
	 - 0.130243f * x7[0][2][0] + 0.476110f * x7[0][2][1] + 0.091801f * x7[0][2][2] - 0.010429f * x7[0][2][3]
	 - 0.034401f * x7[0][2][4] + 0.005852f * x7[0][2][5] + 0.038075f * x7[0][2][6] + 0.080405f * x7[0][2][7]
	 + 0.090750f * x7[0][3][0] - 0.215438f * x7[0][3][1] - 0.039804f * x7[0][3][2] - 0.211987f * x7[0][3][3]
	 - 0.041201f * x7[0][3][4] - 0.273779f * x7[0][3][5] + 0.102364f * x7[0][3][6] + 0.034683f * x7[0][3][7]
	 + 0.127735f * x7[1][0][0] - 0.197955f * x7[1][0][1] - 0.105788f * x7[1][0][2] + 0.015879f * x7[1][0][3]
	 - 0.154200f * x7[1][0][4] + 0.051994f * x7[1][0][5] + 0.113522f * x7[1][0][6] - 0.023359f * x7[1][0][7]
	 - 0.448032f * x7[1][1][0] - 0.787639f * x7[1][1][1] + 0.329296f * x7[1][1][2] + 0.365523f * x7[1][1][3]
	 + 0.964435f * x7[1][1][4] - 0.010405f * x7[1][1][5] + 0.747014f * x7[1][1][6] + 0.416934f * x7[1][1][7]
	 - 0.002876f * x7[1][2][0] - 0.058375f * x7[1][2][1] + 0.005725f * x7[1][2][2] - 0.136748f * x7[1][2][3]
	 - 0.017572f * x7[1][2][4] + 0.220540f * x7[1][2][5] - 0.095285f * x7[1][2][6] + 0.138737f * x7[1][2][7]
	 - 0.042894f * x7[1][3][0] - 0.042007f * x7[1][3][1] + 0.026099f * x7[1][3][2] - 0.122983f * x7[1][3][3]
	 - 0.084048f * x7[1][3][4] - 0.087883f * x7[1][3][5] + 0.014950f * x7[1][3][6] - 0.027566f * x7[1][3][7]
	 + 0.119365f * x7[2][0][0] + 0.010535f * x7[2][0][1] + 0.000757f * x7[2][0][2] + 0.107898f * x7[2][0][3]
	 - 0.035461f * x7[2][0][4] + 0.089012f * x7[2][0][5] - 0.019842f * x7[2][0][6] - 0.021911f * x7[2][0][7]
	 - 0.010270f * x7[2][1][0] + 0.016505f * x7[2][1][1] - 0.032667f * x7[2][1][2] + 0.005132f * x7[2][1][3]
	 - 0.030247f * x7[2][1][4] - 0.090385f * x7[2][1][5] + 0.141268f * x7[2][1][6] - 0.140982f * x7[2][1][7]
	 - 0.058512f * x7[2][2][0] - 0.009055f * x7[2][2][1] - 0.019798f * x7[2][2][2] - 0.010258f * x7[2][2][3]
	 + 0.006850f * x7[2][2][4] - 0.041356f * x7[2][2][5] + 0.019348f * x7[2][2][6] - 0.023009f * x7[2][2][7]
	 + 0.080103f * x7[2][3][0] + 0.002899f * x7[2][3][1] + 0.083944f * x7[2][3][2] - 0.028133f * x7[2][3][3]
	 - 0.080958f * x7[2][3][4] + 0.008905f * x7[2][3][5] - 0.219914f * x7[2][3][6] + 0.156260f * x7[2][3][7]
	 + 0.091595f * x7[3][0][0] - 0.012109f * x7[3][0][1] + 0.007723f * x7[3][0][2] + 0.147539f * x7[3][0][3]
	 - 0.009610f * x7[3][0][4] + 0.109101f * x7[3][0][5] + 0.000291f * x7[3][0][6] - 0.104346f * x7[3][0][7]
	 + 0.030103f * x7[3][1][0] - 0.039417f * x7[3][1][1] - 0.071630f * x7[3][1][2] + 0.137213f * x7[3][1][3]
	 + 0.048909f * x7[3][1][4] - 0.024207f * x7[3][1][5] - 0.124847f * x7[3][1][6] + 0.011085f * x7[3][1][7]
	 + 0.152782f * x7[3][2][0] - 0.038377f * x7[3][2][1] + 0.067587f * x7[3][2][2] + 0.169843f * x7[3][2][3]
	 - 0.012581f * x7[3][2][4] + 0.160645f * x7[3][2][5] - 0.159372f * x7[3][2][6] - 0.003924f * x7[3][2][7]
	 - 0.003184f * x7[3][3][0] + 0.005187f * x7[3][3][1] + 0.004078f * x7[3][3][2] - 0.037360f * x7[3][3][3]
	 - 0.082156f * x7[3][3][4] - 0.011325f * x7[3][3][5] - 0.145798f * x7[3][3][6] + 0.084403f * x7[3][3][7];

	scores[1] = 0.157118f
	 + 0.292993f * x7[0][0][0] - 0.116728f * x7[0][0][1] + 0.076454f * x7[0][0][2] - 0.168029f * x7[0][0][3]
	 - 0.099052f * x7[0][0][4] + 0.213599f * x7[0][0][5] - 0.011792f * x7[0][0][6] + 0.158966f * x7[0][0][7]
	 + 0.014799f * x7[0][1][0] - 0.058997f * x7[0][1][1] - 0.082106f * x7[0][1][2] + 0.020014f * x7[0][1][3]
	 + 0.023604f * x7[0][1][4] - 0.029656f * x7[0][1][5] - 0.210323f * x7[0][1][6] + 0.181228f * x7[0][1][7]
	 + 0.019086f * x7[0][2][0] + 0.028864f * x7[0][2][1] - 0.021901f * x7[0][2][2] + 0.082966f * x7[0][2][3]
	 + 0.019424f * x7[0][2][4] + 0.030177f * x7[0][2][5] - 0.041746f * x7[0][2][6] - 0.002642f * x7[0][2][7]
	 - 0.092097f * x7[0][3][0] - 0.057514f * x7[0][3][1] - 0.223522f * x7[0][3][2] + 0.038464f * x7[0][3][3]
	 + 0.100541f * x7[0][3][4] - 0.141227f * x7[0][3][5] - 0.138968f * x7[0][3][6] + 0.145983f * x7[0][3][7]
	 - 0.135336f * x7[1][0][0] + 0.006527f * x7[1][0][1] - 0.163789f * x7[1][0][2] + 0.044834f * x7[1][0][3]
	 + 0.043160f * x7[1][0][4] - 0.183753f * x7[1][0][5] - 0.256902f * x7[1][0][6] + 0.180540f * x7[1][0][7]
	 - 0.044447f * x7[1][1][0] - 0.083652f * x7[1][1][1] + 0.009109f * x7[1][1][2] + 0.084859f * x7[1][1][3]
	 + 0.085634f * x7[1][1][4] - 0.001266f * x7[1][1][5] - 0.000464f * x7[1][1][6] + 0.041143f * x7[1][1][7]
	 - 0.085624f * x7[1][2][0] - 0.019522f * x7[1][2][1] - 0.065355f * x7[1][2][2] + 0.128750f * x7[1][2][3]
	 + 0.026317f * x7[1][2][4] - 0.054210f * x7[1][2][5] - 0.133287f * x7[1][2][6] + 0.059557f * x7[1][2][7]
	 + 0.022313f * x7[1][3][0] - 0.021270f * x7[1][3][1] - 0.036909f * x7[1][3][2] + 0.031420f * x7[1][3][3]
	 + 0.009203f * x7[1][3][4] + 0.009034f * x7[1][3][5] + 0.039139f * x7[1][3][6] - 0.042473f * x7[1][3][7]
	 + 0.036428f * x7[2][0][0] - 0.053763f * x7[2][0][1] - 0.111783f * x7[2][0][2] + 0.155612f * x7[2][0][3]
	 + 0.029443f * x7[2][0][4] + 0.033928f * x7[2][0][5] - 0.140351f * x7[2][0][6] + 0.048828f * x7[2][0][7]
	 + 0.039084f * x7[2][1][0] - 0.046036f * x7[2][1][1] - 0.083715f * x7[2][1][2] + 0.234215f * x7[2][1][3]
	 + 0.027442f * x7[2][1][4] + 0.058603f * x7[2][1][5] - 0.057107f * x7[2][1][6] - 0.081051f * x7[2][1][7]
	 - 0.034657f * x7[2][2][0] - 0.002816f * x7[2][2][1] - 0.022122f * x7[2][2][2] + 0.054872f * x7[2][2][3]
	 + 0.021930f * x7[2][2][4] - 0.023747f * x7[2][2][5] - 0.169881f * x7[2][2][6] + 0.100203f * x7[2][2][7]
	 + 0.080341f * x7[2][3][0] - 0.053331f * x7[2][3][1] - 0.037845f * x7[2][3][2] + 0.243022f * x7[2][3][3]
	 - 0.018455f * x7[2][3][4] + 0.077766f * x7[2][3][5] - 0.145256f * x7[2][3][6] - 0.045518f * x7[2][3][7]
	 - 0.083717f * x7[3][0][0] + 0.012467f * x7[3][0][1] - 0.106546f * x7[3][0][2] + 0.229915f * x7[3][0][3]
	 + 0.030906f * x7[3][0][4] - 0.013986f * x7[3][0][5] - 0.106738f * x7[3][0][6] - 0.066368f * x7[3][0][7]
	 + 0.038372f * x7[3][1][0] - 0.069576f * x7[3][1][1] - 0.125647f * x7[3][1][2] + 0.279733f * x7[3][1][3]
	 + 0.014255f * x7[3][1][4] + 0.041352f * x7[3][1][5] - 0.089428f * x7[3][1][6] - 0.071807f * x7[3][1][7]
	 - 0.026972f * x7[3][2][0] - 0.006647f * x7[3][2][1] - 0.065329f * x7[3][2][2] + 0.127150f * x7[3][2][3]
	 + 0.056177f * x7[3][2][4] - 0.031401f * x7[3][2][5] - 0.181995f * x7[3][2][6] + 0.073921f * x7[3][2][7]
	 - 0.053175f * x7[3][3][0] + 0.008196f * x7[3][3][1] - 0.050636f * x7[3][3][2] + 0.058606f * x7[3][3][3]
	 - 0.013461f * x7[3][3][4] - 0.001900f * x7[3][3][5] + 0.026553f * x7[3][3][6] - 0.085533f * x7[3][3][7];

	scores[2] = 0.150179f
	 + 0.188967f * x7[0][0][0] - 0.060863f * x7[0][0][1] + 0.004591f * x7[0][0][2] + 0.124620f * x7[0][0][3]
	 - 0.109659f * x7[0][0][4] + 0.162151f * x7[0][0][5] - 0.180036f * x7[0][0][6] + 0.078678f * x7[0][0][7]
	 + 0.006229f * x7[0][1][0] - 0.070445f * x7[0][1][1] - 0.131176f * x7[0][1][2] + 0.114876f * x7[0][1][3]
	 - 0.010454f * x7[0][1][4] - 0.038055f * x7[0][1][5] - 0.274449f * x7[0][1][6] + 0.151009f * x7[0][1][7]
	 + 0.035879f * x7[0][2][0] + 0.027449f * x7[0][2][1] - 0.111612f * x7[0][2][2] + 0.149356f * x7[0][2][3]
	 + 0.017909f * x7[0][2][4] + 0.046318f * x7[0][2][5] - 0.131124f * x7[0][2][6] + 0.077892f * x7[0][2][7]
	 + 0.024447f * x7[0][3][0] - 0.062889f * x7[0][3][1] - 0.059853f * x7[0][3][2] - 0.125492f * x7[0][3][3]
	 + 0.039426f * x7[0][3][4] - 0.086691f * x7[0][3][5] + 0.063417f * x7[0][3][6] + 0.065357f * x7[0][3][7]
	 + 0.019538f * x7[1][0][0] - 0.037354f * x7[1][0][1] - 0.073097f * x7[1][0][2] + 0.233060f * x7[1][0][3]
	 - 0.016918f * x7[1][0][4] + 0.038210f * x7[1][0][5] - 0.296827f * x7[1][0][6] + 0.070372f * x7[1][0][7]
	 - 0.160656f * x7[1][1][0] - 0.124386f * x7[1][1][1] - 0.007074f * x7[1][1][2] + 0.155435f * x7[1][1][3]
	 + 0.192021f * x7[1][1][4] - 0.051049f * x7[1][1][5] + 0.102923f * x7[1][1][6] + 0.029211f * x7[1][1][7]
	 + 0.063199f * x7[1][2][0] - 0.021466f * x7[1][2][1] + 0.048555f * x7[1][2][2] + 0.187517f * x7[1][2][3]
	 - 0.075830f * x7[1][2][4] + 0.155991f * x7[1][2][5] + 0.004734f * x7[1][2][6] - 0.135066f * x7[1][2][7]
	 - 0.034437f * x7[1][3][0] - 0.025171f * x7[1][3][1] - 0.044335f * x7[1][3][2] - 0.023598f * x7[1][3][3]
	 + 0.015274f * x7[1][3][4] - 0.078523f * x7[1][3][5] - 0.099133f * x7[1][3][6] + 0.090502f * x7[1][3][7]
	 + 0.036304f * x7[2][0][0] - 0.010970f * x7[2][0][1] - 0.008837f * x7[2][0][2] + 0.254518f * x7[2][0][3]
	 + 0.039719f * x7[2][0][4] + 0.093319f * x7[2][0][5] - 0.119599f * x7[2][0][6] - 0.022618f * x7[2][0][7]
	 - 0.047314f * x7[2][1][0] - 0.054134f * x7[2][1][1] - 0.114401f * x7[2][1][2] + 0.070551f * x7[2][1][3]
	 + 0.061210f * x7[2][1][4] - 0.073510f * x7[2][1][5] - 0.100217f * x7[2][1][6] + 0.106463f * x7[2][1][7]
	 - 0.055890f * x7[2][2][0] + 0.017676f * x7[2][2][1] - 0.012972f * x7[2][2][2] - 0.046283f * x7[2][2][3]
	 + 0.042985f * x7[2][2][4] - 0.060128f * x7[2][2][5] - 0.057000f * x7[2][2][6] + 0.073612f * x7[2][2][7]
	 + 0.019291f * x7[2][3][0] - 0.040350f * x7[2][3][1] - 0.050700f * x7[2][3][2] + 0.204885f * x7[2][3][3]
	 - 0.021921f * x7[2][3][4] + 0.063427f * x7[2][3][5] - 0.023710f * x7[2][3][6] - 0.118479f * x7[2][3][7]
	 + 0.048009f * x7[3][0][0] + 0.007667f * x7[3][0][1] + 0.038059f * x7[3][0][2] + 0.190956f * x7[3][0][3]
	 - 0.026854f * x7[3][0][4] + 0.105650f * x7[3][0][5] - 0.185287f * x7[3][0][6] - 0.013829f * x7[3][0][7]
	 - 0.004009f * x7[3][1][0] - 0.065390f * x7[3][1][1] - 0.160785f * x7[3][1][2] + 0.252458f * x7[3][1][3]
	 + 0.044814f * x7[3][1][4] - 0.005219f * x7[3][1][5] - 0.197514f * x7[3][1][6] + 0.037625f * x7[3][1][7]
	 - 0.158368f * x7[3][2][0] + 0.005142f * x7[3][2][1] - 0.183615f * x7[3][2][2] - 0.088967f * x7[3][2][3]
	 + 0.097629f * x7[3][2][4] - 0.201981f * x7[3][2][5] - 0.098726f * x7[3][2][6] + 0.196747f * x7[3][2][7]
	 - 0.034075f * x7[3][3][0] - 0.032863f * x7[3][3][1] - 0.166630f * x7[3][3][2] + 0.284873f * x7[3][3][3]
	 + 0.084686f * x7[3][3][4] + 0.036166f * x7[3][3][5] - 0.085196f * x7[3][3][6] - 0.090005f * x7[3][3][7];

	scores[3] = 0.037950f
	 + 0.119173f * x7[0][0][0] + 0.209560f * x7[0][0][1] - 0.046272f * x7[0][0][2] + 0.017135f * x7[0][0][3]
	 - 0.199617f * x7[0][0][4] - 0.164852f * x7[0][0][5] + 0.041137f * x7[0][0][6] + 0.114267f * x7[0][0][7]
	 + 0.131405f * x7[0][1][0] - 0.124376f * x7[0][1][1] + 0.232220f * x7[0][1][2] + 0.042979f * x7[0][1][3]
	 - 0.269775f * x7[0][1][4] + 0.168160f * x7[0][1][5] - 0.145030f * x7[0][1][6] - 0.058677f * x7[0][1][7]
	 - 0.253897f * x7[0][2][0] + 0.730950f * x7[0][2][1] - 0.145305f * x7[0][2][2] + 0.161973f * x7[0][2][3]
	 + 0.052572f * x7[0][2][4] + 0.017464f * x7[0][2][5] - 0.015840f * x7[0][2][6] + 0.184504f * x7[0][2][7]
	 + 0.356893f * x7[0][3][0] - 0.427447f * x7[0][3][1] - 0.036879f * x7[0][3][2] + 0.094523f * x7[0][3][3]
	 - 0.117627f * x7[0][3][4] - 0.233720f * x7[0][3][5] - 0.088083f * x7[0][3][6] - 0.054951f * x7[0][3][7]
	 + 0.181594f * x7[1][0][0] - 0.350646f * x7[1][0][1] - 0.278282f * x7[1][0][2] + 0.011473f * x7[1][0][3]
	 - 0.242643f * x7[1][0][4] + 0.016292f * x7[1][0][5] + 0.070997f * x7[1][0][6] + 0.045243f * x7[1][0][7]
	 - 0.707913f * x7[1][1][0] - 1.455895f * x7[1][1][1] + 0.624368f * x7[1][1][2] + 0.748858f * x7[1][1][3]
	 + 1.675259f * x7[1][1][4] + 0.107211f * x7[1][1][5] + 1.458871f * x7[1][1][6] + 0.612637f * x7[1][1][7]
	 + 0.118529f * x7[1][2][0] - 0.138898f * x7[1][2][1] + 0.001438f * x7[1][2][2] - 0.160362f * x7[1][2][3]
	 - 0.059756f * x7[1][2][4] + 0.535195f * x7[1][2][5] + 0.098469f * x7[1][2][6] - 0.014963f * x7[1][2][7]
	 - 0.071519f * x7[1][3][0] - 0.115545f * x7[1][3][1] + 0.009706f * x7[1][3][2] + 0.068015f * x7[1][3][3]
	 - 0.131833f * x7[1][3][4] - 0.136867f * x7[1][3][5] - 0.417890f * x7[1][3][6] + 0.125214f * x7[1][3][7]
	 - 0.007426f * x7[2][0][0] + 0.165781f * x7[2][0][1] + 0.082403f * x7[2][0][2] - 0.016572f * x7[2][0][3]
	 - 0.109190f * x7[2][0][4] - 0.109582f * x7[2][0][5] + 0.096949f * x7[2][0][6] - 0.088001f * x7[2][0][7]
	 - 0.037201f * x7[2][1][0] - 0.006528f * x7[2][1][1] - 0.011778f * x7[2][1][2] + 0.133675f * x7[2][1][3]
	 + 0.019656f * x7[2][1][4] - 0.194965f * x7[2][1][5] - 0.245439f * x7[2][1][6] + 0.117708f * x7[2][1][7]
	 - 0.016243f * x7[2][2][0] - 0.048777f * x7[2][2][1] + 0.020588f * x7[2][2][2] - 0.051484f * x7[2][2][3]
	 - 0.040015f * x7[2][2][4] + 0.007103f * x7[2][2][5] + 0.057216f * x7[2][2][6] - 0.047510f * x7[2][2][7]
	 + 0.065263f * x7[2][3][0] + 0.027930f * x7[2][3][1] + 0.035246f * x7[2][3][2] + 0.150043f * x7[2][3][3]
	 - 0.084878f * x7[2][3][4] + 0.081809f * x7[2][3][5] - 0.109172f * x7[2][3][6] - 0.079862f * x7[2][3][7]
	 + 0.043197f * x7[3][0][0] + 0.056935f * x7[3][0][1] + 0.053911f * x7[3][0][2] + 0.030619f * x7[3][0][3]
	 - 0.003519f * x7[3][0][4] - 0.021418f * x7[3][0][5] + 0.100376f * x7[3][0][6] - 0.110068f * x7[3][0][7]
	 - 0.051875f * x7[3][1][0] + 0.064785f * x7[3][1][1] - 0.013528f * x7[3][1][2] + 0.095628f * x7[3][1][3]
	 + 0.085029f * x7[3][1][4] - 0.121983f * x7[3][1][5] - 0.125996f * x7[3][1][6] - 0.033900f * x7[3][1][7]
	 + 0.123105f * x7[3][2][0] - 0.028950f * x7[3][2][1] + 0.107714f * x7[3][2][2] + 0.040104f * x7[3][2][3]
	 - 0.034704f * x7[3][2][4] + 0.124643f * x7[3][2][5] - 0.044200f * x7[3][2][6] - 0.033700f * x7[3][2][7]
	 - 0.120207f * x7[3][3][0] - 0.010791f * x7[3][3][1] - 0.204539f * x7[3][3][2] - 0.090757f * x7[3][3][3]
	 - 0.022127f * x7[3][3][4] - 0.123476f * x7[3][3][5] - 0.139891f * x7[3][3][6] + 0.147583f * x7[3][3][7];

}

void fy_1500_new2_new_dataset::predict(const BallCandidates::PatchYUVClassified& patch, double meanBrightnessOffset)
{
	ASSERT(patch.size() == 16);

	for(size_t x=0; x < patch.size(); x++) {
		for(size_t y=0; y < patch.size(); y++) {
			// TODO: check
			// .pixel.y accesses the brightness channel of the pixel
			// subtract the mean brightness calculated on the dataset and the offset from the module parameters
			float value = (static_cast<float>((patch.data[patch.size() * x + y].pixel.y)) / 255.0f) - 0.000000f - static_cast<float>(meanBrightnessOffset);
			in_step[y][x][0] = value;
		}
	}

	cnn(in_step);
}

double fy_1500_new2_new_dataset::getRadius() {
	return scores[0];
}
Vector2d fy_1500_new2_new_dataset::getCenter() {
	return Vector2d(scores[1], scores[2]);
}
double fy_1500_new2_new_dataset::getBallConfidence() {
	return scores[3];
}
