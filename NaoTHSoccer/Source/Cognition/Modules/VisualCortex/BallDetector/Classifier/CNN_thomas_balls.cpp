#include "CNN_thomas_balls.h"

#include <emmintrin.h>
#include <math.h>
void CNN_THOMAS_BALLS::cnn(float x0[16][16][1])
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
			x1[i][j][0] = -0.014200960285961628f;
			x1[i][j][1] = 0.07058170437812805f;
			x1[i][j][2] = -0.5814297199249268f;
			x1[i][j][3] = -0.07167857885360718f;
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

					w = _mm_set_ps(0.25773942470550537f, 0.12389557808637619f, -0.7824470400810242f, -0.7966284155845642f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.5822211503982544f, -0.19544370472431183f, -0.4647670090198517f, 0.14301124215126038f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.32705405354499817f, 0.05526936426758766f, -0.10585035383701324f, 0.63981693983078f);
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

					w = _mm_set_ps(-0.47172391414642334f, -0.3750365376472473f, -0.44927287101745605f, -0.4790500998497009f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.09623657912015915f, -0.9445796012878418f, 0.36995986104011536f, -0.3653967082500458f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.5122597217559814f, -0.10770060122013092f, 0.5330756902694702f, 0.795020341873169f);
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

					w = _mm_set_ps(-0.1985221952199936f, 0.0793374553322792f, 0.13715562224388123f, 0.5010613799095154f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.9836323261260986f, -0.45589393377304077f, 0.5743237137794495f, -0.3276346027851105f);
					x = _mm_load_ps1(&x0[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x1[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x1[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.021332930773496628f, 0.21770261228084564f, 0.15280748903751373f, -0.1632171869277954f);
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
			x2[i][j][0] = 0.008307059295475483f;
			x2[i][j][1] = 0.10390092432498932f;
			x2[i][j][2] = -0.032144565135240555f;
			x2[i][j][3] = -0.039294857531785965f;
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

					w = _mm_set_ps(-0.4322047233581543f, -0.1039210855960846f, -0.14477628469467163f, 0.7704158425331116f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.22342263162136078f, -0.025521690025925636f, -0.33294060826301575f, -0.8535639047622681f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6224584579467773f, 1.0005580186843872f, -0.39203205704689026f, -0.058658115565776825f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7987028360366821f, 0.4440609812736511f, 0.018555527552962303f, 0.3987402617931366f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.9763227105140686f, 0.33736664056777954f, -0.0184567142277956f, 0.41169339418411255f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5710698366165161f, 0.24217583239078522f, -0.45090728998184204f, -0.34431082010269165f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.058394499123096466f, 0.8409457206726074f, -0.7909075617790222f, 0.2986752688884735f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32704293727874756f, -0.3203006088733673f, 0.0037738645914942026f, 0.14435064792633057f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.33885061740875244f, -0.08800647407770157f, -0.3708219826221466f, 0.3394778370857239f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.28601735830307007f, 0.19650866091251373f, 0.3005481958389282f, -0.12338260561227798f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5753723978996277f, -0.8146440982818604f, 0.7213353514671326f, -0.08782143890857697f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5322473049163818f, -0.3647595942020416f, -0.06995873898267746f, 0.042890772223472595f);
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

					w = _mm_set_ps(0.07588918507099152f, 0.43487778306007385f, -0.4505247175693512f, -0.16966375708580017f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3596133589744568f, 0.051131606101989746f, -0.172098770737648f, 0.1773575097322464f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5200247764587402f, 0.4070104956626892f, -0.04812203347682953f, 0.12061557918787003f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7650856971740723f, 0.026427369564771652f, -0.17754881083965302f, 0.7925440669059753f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.1918453574180603f, 0.31909897923469543f, 0.06635557115077972f, 0.14655405282974243f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2454226315021515f, 0.49409058690071106f, -1.040131688117981f, -0.16060730814933777f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6335166692733765f, -0.7202236652374268f, -0.36711663007736206f, -0.07981032878160477f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07176366448402405f, -0.563199520111084f, 0.28511375188827515f, -0.2471015751361847f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(-0.008641744032502174f, 0.35794559121131897f, 0.17008808255195618f, 0.3908858299255371f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.02756650559604168f, 0.025600146502256393f, -0.9699848890304565f, -0.21286335587501526f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6159251928329468f, -0.4011457562446594f, -0.6140266060829163f, -0.03959648683667183f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6344884037971497f, -0.3606136739253998f, -0.3320862054824829f, -0.16585764288902283f);
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

					w = _mm_set_ps(0.6310591101646423f, -0.1496557593345642f, -0.1506737321615219f, -0.940916895866394f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8205568194389343f, 0.15461359918117523f, -0.8914908170700073f, 0.30442914366722107f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.4068423807621002f, -0.7456263899803162f, 0.9783535003662109f, -0.266161173582077f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09966271370649338f, -0.4279117286205292f, 0.48882153630256653f, -0.34201323986053467f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.24636204540729523f, -0.4568321108818054f, 0.5071632266044617f, -0.8175457119941711f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.21129052340984344f, -0.3363323509693146f, -0.012308419682085514f, -0.36384478211402893f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4604418873786926f, -0.5916682481765747f, 0.34237033128738403f, 0.035377487540245056f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.38737502694129944f, -0.33643487095832825f, 0.5348412394523621f, 0.306354820728302f);
					x = _mm_load_ps1(&x1[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 16)
				{

					w = _mm_set_ps(0.3453342616558075f, -0.2635561227798462f, 0.5793980360031128f, -1.066261649131775f);
					x = _mm_load_ps1(&x1[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.30579251050949097f, -0.41362226009368896f, -0.7255821824073792f, 0.24135608971118927f);
					x = _mm_load_ps1(&x1[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.36683398485183716f, 0.55865478515625f, 0.038613926619291306f, 0.003895120695233345f);
					x = _mm_load_ps1(&x1[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x2[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x2[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.024075444787740707f, 0.39249637722969055f, -0.3717600107192993f, -0.22411157190799713f);
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
			x4[i][j][0] = -0.05212761461734772f;
			x4[i][j][1] = -0.2712418735027313f;
			x4[i][j][2] = 0.037207357585430145f;
			x4[i][j][3] = 0.42785340547561646f;
			x4[i][j][4] = -0.1445215940475464f;
			x4[i][j][5] = -0.14156576991081238f;
			x4[i][j][6] = -0.00960598699748516f;
			x4[i][j][7] = -0.1906994730234146f;
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

					w = _mm_set_ps(0.476217657327652f, 0.3648371696472168f, 0.16510862112045288f, -0.14673864841461182f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.030845435336232185f, -0.5687406659126282f, 0.4282987713813782f, 0.48161837458610535f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2034536749124527f, 0.942944347858429f, 0.26900094747543335f, -0.08862312138080597f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32533904910087585f, 0.0982832983136177f, -0.4759957790374756f, 0.1569092720746994f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.16162730753421783f, 0.17873746156692505f, 0.04568824917078018f, 0.4898688793182373f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.05683422088623047f, 0.2838806211948395f, 0.23111191391944885f, 0.6351149678230286f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.03957069665193558f, -0.9186452627182007f, -0.024097256362438202f, 0.3599095940589905f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07345212250947952f, -0.35419782996177673f, -0.22262251377105713f, 0.5599755048751831f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.0029610295314341784f, -0.35096004605293274f, 0.013515057973563671f, -0.2617783844470978f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32033535838127136f, 0.4280203580856323f, -0.1070311963558197f, 0.03904328495264053f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3326128423213959f, -0.9485301971435547f, -0.37790945172309875f, 0.4594630300998688f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.41051986813545227f, 0.41976380348205566f, -0.28717562556266785f, 0.0650777816772461f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.39615681767463684f, -0.10626552253961563f, 0.20546019077301025f, 0.42278939485549927f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.24696306884288788f, 0.4240148365497589f, 0.03935239464044571f, -0.055205728858709335f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.04928377643227577f, 0.0856839045882225f, 0.8414930701255798f, -0.08542943745851517f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.08691296726465225f, -0.08531094342470169f, -0.4378121793270111f, 0.5203189849853516f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.10160462558269501f, 0.1989808976650238f, -0.3273131549358368f, 0.4411706030368805f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3850168287754059f, -0.014985223300755024f, -0.13634243607521057f, -0.52960205078125f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12767566740512848f, -0.12321364879608154f, 0.21026377379894257f, 0.07478269934654236f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12007187306880951f, 0.3502122461795807f, -0.3057631552219391f, 0.21344290673732758f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2002415806055069f, 0.10270940512418747f, 0.03944181278347969f, -0.1497717946767807f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0003861280856654048f, 0.11350850015878677f, 0.19880452752113342f, 0.11308464407920837f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3894094228744507f, 0.26907509565353394f, 0.46072548627853394f, 0.7253623604774475f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0453928709030151f, -0.4139944016933441f, 0.02698415517807007f, -0.4632004201412201f);
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

					w = _mm_set_ps(0.020293187350034714f, -0.2144469916820526f, 0.08836323767900467f, 0.2019970864057541f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13161905109882355f, -0.8696619272232056f, -0.21709848940372467f, 0.04479420185089111f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3861813247203827f, 0.4701499938964844f, 0.028731010854244232f, 0.16773249208927155f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.3442160189151764f, -0.2183750867843628f, -0.6146476864814758f, -0.008967223577201366f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.19858650863170624f, 0.12403883785009384f, 0.17739136517047882f, 0.14451982080936432f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13864104449748993f, -0.33896246552467346f, 0.17503240704536438f, 0.4628930389881134f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1255473494529724f, -0.33400416374206543f, -0.28317561745643616f, 0.16913239657878876f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7396414279937744f, -0.023634517565369606f, -0.12073889374732971f, 0.17494255304336548f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.07267347723245621f, -0.2348041832447052f, 0.1933792382478714f, 0.05558061599731445f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10216639190912247f, 0.4018881916999817f, -0.0974956527352333f, -0.014811472967267036f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.22274647653102875f, -0.10899995267391205f, 1.1154136657714844f, 0.20829108357429504f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.17153333127498627f, 0.04258476942777634f, -0.38293036818504333f, 0.26396840810775757f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.4527178406715393f, -0.6331852674484253f, -0.3637968897819519f, 0.33436375856399536f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5634321570396423f, -0.13432641327381134f, 0.012288142926990986f, -0.45361703634262085f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.15470105409622192f, -0.636816680431366f, -0.9377304315567017f, 0.13603641092777252f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.24252937734127045f, 0.08338111639022827f, -0.1689181923866272f, -0.9695698022842407f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.24026057124137878f, -0.036391377449035645f, -0.08781812340021133f, 0.20356762409210205f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7086344361305237f, 0.036887381225824356f, -0.0328114852309227f, 0.20550087094306946f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.04506624862551689f, -0.5308400988578796f, 0.04590979218482971f, 0.13842733204364777f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.401363343000412f, 0.21150368452072144f, -0.6301564574241638f, 0.3274492621421814f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.502638041973114f, -0.14145302772521973f, 0.18277417123317719f, 0.29933640360832214f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.23138754069805145f, -0.9748528003692627f, 0.1349233090877533f, -0.4832003712654114f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5491952300071716f, 0.3036510646343231f, -0.3997959792613983f, -0.08830704540014267f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11521558463573456f, -0.15367688238620758f, -0.27659153938293457f, -0.8309484720230103f);
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

					w = _mm_set_ps(0.8751842379570007f, -0.21940352022647858f, -0.44585639238357544f, -0.22413261234760284f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.45103558897972107f, -0.05375310406088829f, -0.6278259754180908f, -0.24515968561172485f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.20287150144577026f, 0.04675006866455078f, -0.45791277289390564f, -0.09845049679279327f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.20245392620563507f, -0.20982861518859863f, -0.10497952997684479f, -0.15826477110385895f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10091984272003174f, 0.5736045241355896f, 0.032780129462480545f, -0.08178148418664932f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10743603855371475f, -1.2471115589141846f, 0.0029947906732559204f, -0.024560336023569107f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.12359870970249176f, 0.38217633962631226f, 0.5875063538551331f, -0.05733053386211395f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.06807967275381088f, -0.36858445405960083f, -0.5581589341163635f, -0.5734558701515198f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(-0.21391712129116058f, -0.6209589242935181f, -0.5224481821060181f, -0.39116808772087097f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.15096627175807953f, -0.6602847576141357f, -0.2825280427932739f, -0.006814621854573488f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6239160895347595f, 0.5585999488830566f, -0.0950443223118782f, 0.05521104484796524f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2154024839401245f, 0.09340138733386993f, -0.16443893313407898f, -0.18393415212631226f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.46654456853866577f, -0.3006724417209625f, -0.3357527256011963f, 0.006504159886389971f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.4318341910839081f, -0.5024117827415466f, -0.2652612328529358f, -0.738496720790863f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.507051408290863f, -0.4184315800666809f, 0.32488662004470825f, -0.05303648114204407f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07626248896121979f, -0.5155709385871887f, -0.0842306911945343f, -0.6972042918205261f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 8)
				{

					w = _mm_set_ps(0.6570143103599548f, 0.1666332632303238f, 0.009170610457658768f, 0.13468778133392334f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07669609040021896f, 0.34101176261901855f, -0.005989849101752043f, -0.2786237895488739f);
					x = _mm_load_ps1(&x3[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1010737419128418f, -0.4148055911064148f, 0.8629559278488159f, 0.07008662074804306f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2663410007953644f, 0.41863560676574707f, -0.4939464032649994f, -0.32901516556739807f);
					x = _mm_load_ps1(&x3[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.42705920338630676f, -0.34678900241851807f, -0.054956696927547455f, -0.013525762595236301f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5227741003036499f, -0.6158161163330078f, -0.8066981434822083f, -0.219408318400383f);
					x = _mm_load_ps1(&x3[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09148261696100235f, -0.8395940065383911f, -0.05392679572105408f, -0.19259850680828094f);
					x = _mm_load_ps1(&x3[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x4[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x4[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10196848958730698f, 0.12438992410898209f, -0.19215156137943268f, 0.2431263029575348f);
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
			x6[i][j][0] = 0.34886297583580017f;
			x6[i][j][1] = 0.30025580525398254f;
			x6[i][j][2] = -0.1829136610031128f;
			x6[i][j][3] = 0.2788408398628235f;
			x6[i][j][4] = -0.3739936947822571f;
			x6[i][j][5] = 0.4598749876022339f;
			x6[i][j][6] = 0.4417358338832855f;
			x6[i][j][7] = 0.16459205746650696f;
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

					w = _mm_set_ps(-0.5146961212158203f, -0.662041962146759f, -0.37464961409568787f, 0.040028687566518784f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.12726515531539917f, -0.18246273696422577f, 0.0730990469455719f, 0.18027007579803467f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.37652814388275146f, -1.203965663909912f, -0.33759692311286926f, 0.003052938962355256f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8003310561180115f, -0.3554907441139221f, -0.3048257529735565f, -0.27815258502960205f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1785125583410263f, -0.8097859025001526f, -0.050439998507499695f, 0.06332754343748093f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6125685572624207f, 0.050637684762477875f, -0.03286055848002434f, -0.08152598887681961f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2095147669315338f, -0.24340856075286865f, -0.4562084376811981f, -0.08948797732591629f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.0997500941157341f, 0.8708441853523254f, -0.5223062038421631f, -0.704296350479126f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.04763420671224594f, 0.6921572685241699f, 1.033456563949585f, -0.06822675466537476f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13112032413482666f, 0.4094788134098053f, -0.17529839277267456f, -0.2615188956260681f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.8054028153419495f, 0.6387386322021484f, 0.27637335658073425f, 0.26044976711273193f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7116227746009827f, -0.007290359120815992f, 0.3144572377204895f, 0.37774473428726196f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.01411235798150301f, 0.4459257125854492f, 0.4206037223339081f, 0.3713834583759308f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6190001964569092f, 0.20710958540439606f, 0.08491497486829758f, 0.29077959060668945f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2512319087982178f, -0.3951253294944763f, 0.05968772619962692f, -0.01206611841917038f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.23359547555446625f, -0.23802335560321808f, -0.19626113772392273f, -0.11095452308654785f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.0020336450543254614f, -0.3531607687473297f, -0.6323655843734741f, 0.09991038590669632f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.45006826519966125f, -0.11250282824039459f, -0.0134501401335001f, -0.2657041847705841f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.7673369646072388f, -0.1386822760105133f, -0.36688488721847534f, 0.062281735241413116f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.131990671157837f, -0.18357877433300018f, -0.08210134506225586f, -0.18819226324558258f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.42036139965057373f, 0.07150788605213165f, 0.01932964287698269f, 0.0014727037632837892f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.34849923849105835f, -0.2901160717010498f, -0.10851950943470001f, 0.10803890973329544f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10821088403463364f, 0.03584826737642288f, -0.047328777611255646f, -0.12176614254713058f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8323363661766052f, -0.6171184778213501f, -0.5628328919410706f, -0.1679912805557251f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3725128173828125f, 0.5307018756866455f, 0.6382429003715515f, 0.08018245548009872f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.43202459812164307f, 0.3185851573944092f, -0.1282571405172348f, -0.2531649172306061f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.11062585562467575f, 0.07116229832172394f, -0.10547299683094025f, -0.005751118063926697f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3279185891151428f, -0.0281963013112545f, 0.31119972467422485f, 0.5468953251838684f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.31846603751182556f, 0.7102146148681641f, 0.5395862460136414f, -0.07949534058570862f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.10849297791719437f, 0.3684467375278473f, 0.29091811180114746f, 0.4280115067958832f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.33369460701942444f, 0.15102185308933258f, -0.9238556027412415f, 0.060737647116184235f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0598458051681519f, -0.01901276782155037f, -0.046851739287376404f, -0.07828675955533981f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.4756240248680115f, -0.7382984757423401f, -0.5167561769485474f, 0.16440355777740479f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24845649302005768f, -0.11383751779794693f, -0.1717967987060547f, -0.06181834638118744f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.44335246086120605f, -0.45376285910606384f, -0.6519156098365784f, -0.02667953446507454f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.026308173313736916f, -0.23623308539390564f, -0.10586025565862656f, 0.030695496127009392f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.21469907462596893f, -0.6301405429840088f, 0.0884215235710144f, 0.027125006541609764f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6191657185554504f, -0.43948298692703247f, 0.0154573954641819f, 0.11373797804117203f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-1.229265570640564f, 0.7577912211418152f, 0.2483624368906021f, -0.20825934410095215f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6631665229797363f, 0.43946611881256104f, -0.10153614729642868f, -0.8365378379821777f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2217261642217636f, -0.48407861590385437f, -0.09116389602422714f, -0.022459521889686584f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.7754889726638794f, 0.05444709584116936f, -0.141402930021286f, -0.12650521099567413f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09874355047941208f, 0.1973717361688614f, -0.1298963874578476f, -0.2192327380180359f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.27491235733032227f, 0.11480827629566193f, 0.38957494497299194f, 0.3074868619441986f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1282869428396225f, -0.004449915606528521f, 0.16449567675590515f, 0.03021000325679779f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.2715633511543274f, -0.2821575105190277f, 0.27305254340171814f, 0.5885684490203857f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.26047635078430176f, -0.08001480251550674f, -0.7815072536468506f, 0.013790814206004143f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-1.0209643840789795f, -0.26394012570381165f, 0.008482947014272213f, 0.014996737241744995f);
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

					w = _mm_set_ps(-0.2371426522731781f, -0.3021608293056488f, 0.007330914959311485f, -0.09212950617074966f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6072970032691956f, -0.5047706961631775f, -0.08657242357730865f, -0.07079744338989258f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2181175798177719f, -0.37710243463516235f, 0.007704115938395262f, -0.03262152150273323f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0017141640419140458f, -0.3346810042858124f, -0.22057358920574188f, -0.19066497683525085f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.279170960187912f, -0.5165650844573975f, -0.030436256900429726f, 0.03409209102392197f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.5698853135108948f, 0.1354813128709793f, -0.3727267384529114f, -0.34787964820861816f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2697645127773285f, 0.014119332656264305f, -0.2234904170036316f, -0.38562023639678955f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.24731723964214325f, 0.18539363145828247f, -0.057356834411621094f, -0.10070152580738068f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.12803886830806732f, 0.01944381557404995f, -0.04463822767138481f, -0.00764600932598114f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.28538012504577637f, 0.2968153953552246f, -0.1705823689699173f, -0.31830939650535583f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3581618666648865f, -0.5130782723426819f, -0.1043197512626648f, -0.07803557068109512f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.38423147797584534f, -0.044393543154001236f, -0.1726580262184143f, -0.03352102264761925f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.16120123863220215f, 0.16466018557548523f, 0.07237576693296432f, 0.3280666470527649f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3229101002216339f, -0.3910351097583771f, 0.06464096903800964f, 0.24110820889472961f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3357885479927063f, -0.6984789371490479f, 0.04476192593574524f, -0.018894879147410393f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.013939784839749336f, 0.0759490504860878f, -0.2156112641096115f, -0.19939067959785461f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.027047013863921165f, 0.39676138758659363f, -0.05490146949887276f, 0.018917135894298553f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.017823442816734314f, -0.25113463401794434f, 0.16623365879058838f, 0.08576475083827972f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.020834926515817642f, -0.436760276556015f, -0.011862441897392273f, 0.03582187741994858f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.45298251509666443f, -0.22785913944244385f, -0.15066693723201752f, -0.1825917661190033f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.5313481688499451f, 0.2865442931652069f, 0.01089202519506216f, -0.00012007323675788939f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.7034487128257751f, 0.10226481407880783f, -0.00464639300480485f, -0.09340725094079971f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.09378562122583389f, 0.3571609556674957f, 0.10394763201475143f, -0.26446595788002014f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.31810179352760315f, 0.331068754196167f, -0.29703933000564575f, -0.02692577987909317f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5537064075469971f, 0.38480842113494873f, 0.03272575885057449f, -0.052303463220596313f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.03165721893310547f, -0.21898695826530457f, -0.004701627418398857f, -0.08962232619524002f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3511270582675934f, -0.5489970445632935f, -0.1004566103219986f, -0.022479170933365822f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.23258425295352936f, 0.14421656727790833f, 0.4572436809539795f, 0.3796467185020447f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14915579557418823f, 0.6663727164268494f, 0.09304527938365936f, 0.048975471407175064f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.14891166985034943f, 0.5346628427505493f, 0.18236316740512848f, 0.4810236990451813f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.1293371170759201f, 0.22407875955104828f, -0.0022015429567545652f, 0.025490185245871544f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.20045140385627747f, 0.029533561319112778f, -0.028307192027568817f, 0.03984322026371956f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(-0.38744470477104187f, -0.2517127990722656f, -0.3843325674533844f, 0.009583711624145508f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.3861374855041504f, -0.5002443790435791f, -0.10022526979446411f, -0.13980410993099213f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14430975914001465f, -0.6491051912307739f, -0.08931643515825272f, -0.1799730807542801f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07513273507356644f, -0.168293759226799f, -0.10213366895914078f, -0.051940206438302994f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11539998650550842f, -0.35775330662727356f, 0.13863982260227203f, -0.012015481479465961f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8608358502388f, 0.010043014772236347f, 0.024150045588612556f, 0.052498798817396164f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.10364164412021637f, 0.1363362967967987f, -0.44476568698883057f, 0.03442302718758583f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.07761926203966141f, -0.0450352318584919f, -0.16699808835983276f, -0.3251173198223114f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.22761689126491547f, 0.6346448063850403f, -0.6714876294136047f, -0.030806947499513626f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.548018753528595f, 0.06625537574291229f, -0.026001885533332825f, -0.1127115935087204f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2627381682395935f, -0.5657997131347656f, -0.40918219089508057f, 0.14562562108039856f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.10408715903759003f, 0.08838939666748047f, 0.16402055323123932f, 0.07164554297924042f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.03977179154753685f, 0.2728729546070099f, 1.1920459270477295f, 0.049959056079387665f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.13972170650959015f, -0.33623605966567993f, 0.37901803851127625f, 0.4901885986328125f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.07036104798316956f, -0.012889214791357517f, -0.709497332572937f, -0.11402934044599533f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.30341455340385437f, 0.05420646443963051f, 0.03783459961414337f, 0.010448986664414406f);
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

					w = _mm_set_ps(0.039806775748729706f, -0.23101840913295746f, -0.06172627583146095f, -0.03980856388807297f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2145250141620636f, -0.13057208061218262f, 0.21851468086242676f, 0.3389211595058441f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09834565222263336f, -0.10937244445085526f, 0.040084149688482285f, 0.06895209103822708f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.48710528016090393f, -0.23264865577220917f, -0.2151385247707367f, -0.24742354452610016f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.040427666157484055f, -0.05474284663796425f, -0.029920849949121475f, 0.07610908895730972f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.030122924596071243f, 0.04143604636192322f, -0.16621144115924835f, -0.21803805232048035f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.5764313340187073f, -0.523723304271698f, -0.1920180767774582f, -0.30573996901512146f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.19671303033828735f, -0.7648230195045471f, 0.4190486669540405f, 0.07661096006631851f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.26249662041664124f, 0.30145028233528137f, -0.03260832652449608f, -0.14221283793449402f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6879730820655823f, -0.34841302037239075f, 0.012400771491229534f, 0.18972034752368927f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09858439862728119f, 0.24440357089042664f, -0.27967846393585205f, -0.30070042610168457f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.07102604955434799f, -0.12612426280975342f, -0.1113719493150711f, -0.2730437219142914f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.25709429383277893f, 0.1374676674604416f, -0.08642811328172684f, 0.36010363698005676f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.5370981097221375f, 0.15286201238632202f, -0.10074268281459808f, 0.024978727102279663f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.06527572870254517f, -0.4815932810306549f, -0.03538493067026138f, 0.07315951585769653f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.752919614315033f, -0.09991911798715591f, -0.1276874542236328f, -0.12010723352432251f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 1;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.03869226202368736f, 0.32380080223083496f, 0.03367973864078522f, 0.00019440677715465426f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.11492867767810822f, 0.1918875128030777f, 0.12455649673938751f, 0.19407860934734344f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.02960985340178013f, 0.07288976013660431f, 0.05008051171898842f, 0.06436102092266083f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6193363070487976f, -0.07746577262878418f, -0.07580436021089554f, 0.06492751836776733f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.2044403851032257f, -0.026214957237243652f, 0.04897138848900795f, 0.09005638211965561f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.2442820519208908f, -0.06332816928625107f, -1.826964580686763e-05f, -0.0033603885676711798f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.18490725755691528f, -0.09382074326276779f, 0.012945754453539848f, -0.09075382351875305f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.144432932138443f, 0.03230532631278038f, -0.38300251960754395f, -0.6681181788444519f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.30339884757995605f, 0.47392913699150085f, -0.00697278929874301f, 0.05068288370966911f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.32888084650039673f, 0.07855212688446045f, 0.011415046639740467f, -0.012370708398520947f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.42354172468185425f, 0.0847073495388031f, -0.21683301031589508f, -0.12490906566381454f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0941813513636589f, -0.2784450948238373f, -0.08963669836521149f, -0.0041449773125350475f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.3679937720298767f, 0.09560064971446991f, 0.028811097145080566f, 0.01650901883840561f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.28797540068626404f, 0.10338640213012695f, 0.15875400602817535f, 0.19643430411815643f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.14960506558418274f, 0.1611521691083908f, 0.013282950036227703f, 0.020546207204461098f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.46220889687538147f, 0.09566714614629745f, -0.055877067148685455f, 0.05553735792636871f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);
				}
				x_2 = jx + 2;
				if (x_2 >= 0 && x_2 < 4)
				{

					w = _mm_set_ps(0.2241736501455307f, 0.2852276563644409f, -0.2338714748620987f, 0.1278219074010849f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.35319003462791443f, -0.06038723886013031f, 0.04323805496096611f, 0.11007627099752426f);
					x = _mm_load_ps1(&x5[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.1502084583044052f, -0.12048627436161041f, -0.26937568187713623f, -0.30554547905921936f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.8397740125656128f, -0.050234321504831314f, 0.07782053202390671f, 0.14657104015350342f);
					x = _mm_load_ps1(&x5[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.0875304639339447f, -0.28562721610069275f, 0.13207826018333435f, -0.07439598441123962f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.023519501090049744f, -0.041555702686309814f, 0.1630999892950058f, 0.25384241342544556f);
					x = _mm_load_ps1(&x5[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.32658737897872925f, 0.21099694073200226f, -0.07099169492721558f, -0.21757294237613678f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.6491968035697937f, -0.24312420189380646f, -0.5949831604957581f, -0.6978803873062134f);
					x = _mm_load_ps1(&x5[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.006926107686012983f, -0.15799324214458466f, -0.06686954945325851f, 0.20333775877952576f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.44607675075531006f, -0.13484527170658112f, -0.00028864911291748285f, -0.12217961251735687f);
					x = _mm_load_ps1(&x5[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.09313247352838516f, -0.2236330658197403f, 0.28443679213523865f, -0.09547724574804306f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.0358424037694931f, 0.17385020852088928f, -0.1017577201128006f, -0.49186062812805176f);
					x = _mm_load_ps1(&x5[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.11975965648889542f, -0.1785406768321991f, 0.476495623588562f, -0.05478312075138092f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.6661533713340759f, 0.004311665892601013f, 0.32720857858657837f, 0.4765101671218872f);
					x = _mm_load_ps1(&x5[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.2721293270587921f, 0.392231822013855f, 0.01978631503880024f, -0.019417336210608482f);
					x = _mm_load_ps1(&x5[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x6[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x6[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.1340985745191574f, 0.005779067054390907f, 0.12993070483207703f, 0.11188798397779465f);
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
			x7[i][j][0] = 0.002800293266773224f;
			x7[i][j][1] = -0.006219377741217613f;
			x7[i][j][2] = -0.07134628295898438f;
			x7[i][j][3] = -0.02734660729765892f;
			x7[i][j][4] = 0.021520087495446205f;
			x7[i][j][5] = -0.04926050826907158f;
			x7[i][j][6] = 0.005889731924980879f;
			x7[i][j][7] = 0.010041331872344017f;
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

					w = _mm_set_ps(0.0403616838157177f, -0.027980322018265724f, 0.14318223297595978f, -0.0015416149981319904f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-3.672089951578528e-05f, 0.010562175884842873f, 0.11187463253736496f, -0.035407014191150665f);
					x = _mm_load_ps1(&x6[x_1][x_2][0]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.14224062860012054f, -0.2965553104877472f, -0.2333880513906479f, -0.015761401504278183f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.12848730385303497f, 0.32251718640327454f, 0.02615273743867874f, -0.2327227145433426f);
					x = _mm_load_ps1(&x6[x_1][x_2][1]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.3305547535419464f, 0.6671942472457886f, 0.23911146819591522f, -0.01145891472697258f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.26157379150390625f, -0.031227203086018562f, -0.2586545944213867f, -0.22974488139152527f);
					x = _mm_load_ps1(&x6[x_1][x_2][2]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.10411611944437027f, 0.057333774864673615f, -0.29228901863098145f, 0.06346123665571213f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.17574353516101837f, -0.28632277250289917f, 0.1509081870317459f, -0.27354028820991516f);
					x = _mm_load_ps1(&x6[x_1][x_2][3]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.06997592002153397f, 0.041443850845098495f, 0.05376170575618744f, -0.0510331355035305f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.16801998019218445f, -0.09365376085042953f, -0.058750152587890625f, -0.12247688323259354f);
					x = _mm_load_ps1(&x6[x_1][x_2][4]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.026558974757790565f, -0.027377625927329063f, -0.09830870479345322f, 0.08357374370098114f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.004421804565936327f, 0.06805881857872009f, 0.04675284028053284f, 0.15756171941757202f);
					x = _mm_load_ps1(&x6[x_1][x_2][5]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(0.035988420248031616f, 0.30840346217155457f, 0.08357846736907959f, -0.17634904384613037f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(-0.09088629484176636f, -0.06047014892101288f, 0.011223514564335346f, 0.179045632481575f);
					x = _mm_load_ps1(&x6[x_1][x_2][6]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][4]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][4], x);

					w = _mm_set_ps(-0.6502019762992859f, 1.2635087966918945f, -0.11348742991685867f, 0.0017042633844539523f);
					x = _mm_load_ps1(&x6[x_1][x_2][7]);
					y = _mm_mul_ps(w, x);
					x = _mm_load_ps((float*)&x7[x_out_1][x_out_2][0]);
					x = _mm_add_ps(x, y);
					_mm_store_ps((float*)&x7[x_out_1][x_out_2][0], x);

					w = _mm_set_ps(0.33721405267715454f, 0.417612224817276f, 0.07382652908563614f, -1.02410089969635f);
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
	scores[0] = 0.268612f
	 + 0.231016f * x7[0][0][0] + 0.023407f * x7[0][0][1] + 0.032254f * x7[0][0][2] + 0.000157f * x7[0][0][3]
	 + 0.068816f * x7[0][0][4] + 0.028502f * x7[0][0][5] + 0.032011f * x7[0][0][6] + 0.001257f * x7[0][0][7]
	 + 0.045551f * x7[0][1][0] + 0.068867f * x7[0][1][1] - 0.303791f * x7[0][1][2] + 0.388709f * x7[0][1][3]
	 + 0.225125f * x7[0][1][4] - 0.235457f * x7[0][1][5] - 0.154202f * x7[0][1][6] - 0.146566f * x7[0][1][7]
	 + 0.008203f * x7[0][2][0] + 0.005300f * x7[0][2][1] - 0.012049f * x7[0][2][2] - 0.033708f * x7[0][2][3]
	 + 0.004422f * x7[0][2][4] - 0.021289f * x7[0][2][5] - 0.026121f * x7[0][2][6] - 0.041732f * x7[0][2][7]
	 - 0.044919f * x7[0][3][0] + 0.043528f * x7[0][3][1] - 0.010480f * x7[0][3][2] - 0.006449f * x7[0][3][3]
	 - 0.028936f * x7[0][3][4] + 0.043413f * x7[0][3][5] + 0.007563f * x7[0][3][6] - 0.054718f * x7[0][3][7]
	 + 0.216424f * x7[1][0][0] + 0.079614f * x7[1][0][1] + 0.102965f * x7[1][0][2] - 0.136558f * x7[1][0][3]
	 + 0.112218f * x7[1][0][4] + 0.016552f * x7[1][0][5] - 0.236748f * x7[1][0][6] + 0.047676f * x7[1][0][7]
	 - 0.183817f * x7[1][1][0] + 0.003451f * x7[1][1][1] + 0.001910f * x7[1][1][2] - 0.062804f * x7[1][1][3]
	 - 0.001645f * x7[1][1][4] + 0.037346f * x7[1][1][5] + 0.071425f * x7[1][1][6] - 0.087049f * x7[1][1][7]
	 + 0.100666f * x7[1][2][0] + 0.085717f * x7[1][2][1] - 0.059817f * x7[1][2][2] + 0.076805f * x7[1][2][3]
	 + 0.086795f * x7[1][2][4] - 0.069916f * x7[1][2][5] + 0.003567f * x7[1][2][6] - 0.014597f * x7[1][2][7]
	 - 0.118413f * x7[1][3][0] + 0.145246f * x7[1][3][1] - 0.070794f * x7[1][3][2] - 0.057397f * x7[1][3][3]
	 - 0.143557f * x7[1][3][4] + 0.032217f * x7[1][3][5] - 0.069339f * x7[1][3][6] - 0.036785f * x7[1][3][7]
	 + 0.210967f * x7[2][0][0] - 0.370328f * x7[2][0][1] - 0.659627f * x7[2][0][2] - 0.309425f * x7[2][0][3]
	 - 0.258618f * x7[2][0][4] + 0.426728f * x7[2][0][5] + 0.271214f * x7[2][0][6] + 0.530378f * x7[2][0][7]
	 - 0.051931f * x7[2][1][0] - 0.074561f * x7[2][1][1] - 0.300408f * x7[2][1][2] + 0.062241f * x7[2][1][3]
	 + 0.228054f * x7[2][1][4] + 0.074737f * x7[2][1][5] - 0.097687f * x7[2][1][6] - 0.159912f * x7[2][1][7]
	 - 0.131395f * x7[2][2][0] + 0.452010f * x7[2][2][1] + 0.001814f * x7[2][2][2] - 0.164110f * x7[2][2][3]
	 + 0.382911f * x7[2][2][4] - 0.460235f * x7[2][2][5] + 0.569286f * x7[2][2][6] + 0.281967f * x7[2][2][7]
	 + 0.010041f * x7[2][3][0] + 0.203036f * x7[2][3][1] - 0.030765f * x7[2][3][2] + 0.090938f * x7[2][3][3]
	 - 0.073169f * x7[2][3][4] + 0.238970f * x7[2][3][5] + 0.047313f * x7[2][3][6] - 0.009734f * x7[2][3][7]
	 - 0.016025f * x7[3][0][0] - 0.083842f * x7[3][0][1] - 0.057205f * x7[3][0][2] + 0.001218f * x7[3][0][3]
	 - 0.060461f * x7[3][0][4] - 0.189106f * x7[3][0][5] + 0.077212f * x7[3][0][6] - 0.028778f * x7[3][0][7]
	 - 0.121758f * x7[3][1][0] - 0.095801f * x7[3][1][1] - 0.243497f * x7[3][1][2] + 0.136242f * x7[3][1][3]
	 + 0.095364f * x7[3][1][4] - 0.019323f * x7[3][1][5] - 0.156375f * x7[3][1][6] - 0.153031f * x7[3][1][7]
	 + 0.026412f * x7[3][2][0] - 0.009725f * x7[3][2][1] + 0.006141f * x7[3][2][2] - 0.021551f * x7[3][2][3]
	 + 0.023180f * x7[3][2][4] + 0.007669f * x7[3][2][5] + 0.003142f * x7[3][2][6] - 0.041748f * x7[3][2][7]
	 - 0.062736f * x7[3][3][0] - 0.036119f * x7[3][3][1] + 0.012821f * x7[3][3][2] - 0.051555f * x7[3][3][3]
	 + 0.032971f * x7[3][3][4] - 0.056267f * x7[3][3][5] + 0.017853f * x7[3][3][6] - 0.022665f * x7[3][3][7];
	scores[0] = scores[0] > 0.0f ? scores[0] : 0.0f;

	scores[1] = 0.214266f
	 + 0.069155f * x7[0][0][0] - 0.028802f * x7[0][0][1] + 0.031142f * x7[0][0][2] - 0.050009f * x7[0][0][3]
	 + 0.035665f * x7[0][0][4] - 0.031831f * x7[0][0][5] - 0.122678f * x7[0][0][6] + 0.056888f * x7[0][0][7]
	 + 0.263375f * x7[0][1][0] - 0.145836f * x7[0][1][1] - 0.130990f * x7[0][1][2] + 0.590170f * x7[0][1][3]
	 + 0.453705f * x7[0][1][4] + 0.022951f * x7[0][1][5] - 0.180473f * x7[0][1][6] - 0.125961f * x7[0][1][7]
	 - 0.008323f * x7[0][2][0] + 0.023085f * x7[0][2][1] + 0.006234f * x7[0][2][2] - 0.060686f * x7[0][2][3]
	 + 0.003030f * x7[0][2][4] + 0.005505f * x7[0][2][5] + 0.023824f * x7[0][2][6] - 0.140638f * x7[0][2][7]
	 - 0.329076f * x7[0][3][0] - 0.020683f * x7[0][3][1] - 0.152496f * x7[0][3][2] + 0.029499f * x7[0][3][3]
	 - 0.176382f * x7[0][3][4] - 0.202811f * x7[0][3][5] + 0.231616f * x7[0][3][6] - 0.209782f * x7[0][3][7]
	 - 0.099139f * x7[1][0][0] + 0.087204f * x7[1][0][1] + 0.099382f * x7[1][0][2] - 0.192249f * x7[1][0][3]
	 + 0.109040f * x7[1][0][4] + 0.048444f * x7[1][0][5] - 0.294601f * x7[1][0][6] + 0.026082f * x7[1][0][7]
	 - 0.291728f * x7[1][1][0] - 0.103385f * x7[1][1][1] + 0.148360f * x7[1][1][2] - 0.048991f * x7[1][1][3]
	 + 0.110787f * x7[1][1][4] + 0.081981f * x7[1][1][5] + 0.049653f * x7[1][1][6] - 0.100145f * x7[1][1][7]
	 + 0.205131f * x7[1][2][0] + 0.026596f * x7[1][2][1] - 0.051394f * x7[1][2][2] + 0.082824f * x7[1][2][3]
	 + 0.104770f * x7[1][2][4] - 0.015265f * x7[1][2][5] + 0.025863f * x7[1][2][6] - 0.029429f * x7[1][2][7]
	 + 0.039314f * x7[1][3][0] + 0.379263f * x7[1][3][1] - 0.009894f * x7[1][3][2] + 0.209148f * x7[1][3][3]
	 - 0.223561f * x7[1][3][4] - 0.136081f * x7[1][3][5] - 0.352138f * x7[1][3][6] + 0.328284f * x7[1][3][7]
	 + 0.352890f * x7[2][0][0] - 0.424360f * x7[2][0][1] - 0.699834f * x7[2][0][2] - 0.313686f * x7[2][0][3]
	 - 0.358033f * x7[2][0][4] + 0.052247f * x7[2][0][5] + 0.351160f * x7[2][0][6] + 0.455177f * x7[2][0][7]
	 - 0.051975f * x7[2][1][0] - 0.034305f * x7[2][1][1] - 0.110831f * x7[2][1][2] - 0.105113f * x7[2][1][3]
	 + 0.362838f * x7[2][1][4] + 0.142362f * x7[2][1][5] - 0.174986f * x7[2][1][6] - 0.217738f * x7[2][1][7]
	 - 0.301083f * x7[2][2][0] + 0.403751f * x7[2][2][1] + 0.043894f * x7[2][2][2] - 0.069937f * x7[2][2][3]
	 + 0.455370f * x7[2][2][4] - 0.306948f * x7[2][2][5] + 0.620270f * x7[2][2][6] + 0.327243f * x7[2][2][7]
	 + 0.119321f * x7[2][3][0] + 0.346892f * x7[2][3][1] - 0.032043f * x7[2][3][2] + 0.109071f * x7[2][3][3]
	 - 0.069041f * x7[2][3][4] + 0.378613f * x7[2][3][5] + 0.012971f * x7[2][3][6] + 0.089310f * x7[2][3][7]
	 - 0.270507f * x7[3][0][0] - 0.210188f * x7[3][0][1] - 0.073019f * x7[3][0][2] - 0.041602f * x7[3][0][3]
	 - 0.081493f * x7[3][0][4] - 0.281223f * x7[3][0][5] + 0.030410f * x7[3][0][6] - 0.121406f * x7[3][0][7]
	 + 0.013690f * x7[3][1][0] - 0.113077f * x7[3][1][1] - 0.140876f * x7[3][1][2] + 0.095556f * x7[3][1][3]
	 + 0.134913f * x7[3][1][4] + 0.251053f * x7[3][1][5] - 0.447521f * x7[3][1][6] - 0.250783f * x7[3][1][7]
	 + 0.080816f * x7[3][2][0] - 0.009253f * x7[3][2][1] + 0.014492f * x7[3][2][2] + 0.084310f * x7[3][2][3]
	 - 0.019009f * x7[3][2][4] + 0.131135f * x7[3][2][5] - 0.025554f * x7[3][2][6] - 0.028532f * x7[3][2][7]
	 + 0.061519f * x7[3][3][0] + 0.016984f * x7[3][3][1] - 0.082266f * x7[3][3][2] - 0.157420f * x7[3][3][3]
	 - 0.058032f * x7[3][3][4] - 0.170798f * x7[3][3][5] - 0.015972f * x7[3][3][6] - 0.133609f * x7[3][3][7];
	scores[1] = scores[1] > 0.0f ? scores[1] : 0.0f;

	scores[2] = 0.260552f
	 - 0.303673f * x7[0][0][0] - 0.050171f * x7[0][0][1] + 0.002663f * x7[0][0][2] + 0.067268f * x7[0][0][3]
	 + 0.010840f * x7[0][0][4] + 0.082974f * x7[0][0][5] - 0.021795f * x7[0][0][6] + 0.031970f * x7[0][0][7]
	 - 0.193515f * x7[0][1][0] - 0.043778f * x7[0][1][1] - 0.398910f * x7[0][1][2] + 0.575633f * x7[0][1][3]
	 + 0.255814f * x7[0][1][4] - 0.310171f * x7[0][1][5] - 0.236833f * x7[0][1][6] - 0.186910f * x7[0][1][7]
	 + 0.106991f * x7[0][2][0] - 0.055088f * x7[0][2][1] + 0.065925f * x7[0][2][2] + 0.135692f * x7[0][2][3]
	 + 0.034506f * x7[0][2][4] + 0.054091f * x7[0][2][5] + 0.022988f * x7[0][2][6] + 0.065414f * x7[0][2][7]
	 - 0.144444f * x7[0][3][0] - 0.055761f * x7[0][3][1] - 0.002465f * x7[0][3][2] - 0.106335f * x7[0][3][3]
	 + 0.002064f * x7[0][3][4] - 0.069483f * x7[0][3][5] - 0.008256f * x7[0][3][6] - 0.071394f * x7[0][3][7]
	 + 0.067763f * x7[1][0][0] - 0.004975f * x7[1][0][1] + 0.200818f * x7[1][0][2] - 0.310667f * x7[1][0][3]
	 + 0.226554f * x7[1][0][4] - 0.012969f * x7[1][0][5] - 0.303856f * x7[1][0][6] - 0.008626f * x7[1][0][7]
	 - 0.369416f * x7[1][1][0] - 0.124554f * x7[1][1][1] + 0.116696f * x7[1][1][2] - 0.085907f * x7[1][1][3]
	 + 0.114805f * x7[1][1][4] + 0.005242f * x7[1][1][5] + 0.011668f * x7[1][1][6] - 0.089973f * x7[1][1][7]
	 + 0.010851f * x7[1][2][0] + 0.040780f * x7[1][2][1] - 0.035142f * x7[1][2][2] + 0.076641f * x7[1][2][3]
	 + 0.100933f * x7[1][2][4] - 0.131114f * x7[1][2][5] + 0.015047f * x7[1][2][6] - 0.036604f * x7[1][2][7]
	 + 0.002546f * x7[1][3][0] + 0.118211f * x7[1][3][1] + 0.031440f * x7[1][3][2] + 0.103803f * x7[1][3][3]
	 - 0.106084f * x7[1][3][4] - 0.018795f * x7[1][3][5] - 0.082594f * x7[1][3][6] + 0.040475f * x7[1][3][7]
	 + 0.892145f * x7[2][0][0] - 0.330242f * x7[2][0][1] - 0.739382f * x7[2][0][2] - 0.266759f * x7[2][0][3]
	 - 0.321648f * x7[2][0][4] + 0.529298f * x7[2][0][5] + 0.324142f * x7[2][0][6] + 0.817884f * x7[2][0][7]
	 - 0.089880f * x7[2][1][0] + 0.029836f * x7[2][1][1] - 0.311774f * x7[2][1][2] - 0.163583f * x7[2][1][3]
	 + 0.317265f * x7[2][1][4] + 0.118582f * x7[2][1][5] - 0.246029f * x7[2][1][6] - 0.257233f * x7[2][1][7]
	 - 0.113664f * x7[2][2][0] + 0.611953f * x7[2][2][1] + 0.007655f * x7[2][2][2] - 0.205994f * x7[2][2][3]
	 + 0.494709f * x7[2][2][4] - 0.609805f * x7[2][2][5] + 0.781964f * x7[2][2][6] + 0.421374f * x7[2][2][7]
	 - 0.028329f * x7[2][3][0] + 0.316841f * x7[2][3][1] - 0.038225f * x7[2][3][2] + 0.147253f * x7[2][3][3]
	 - 0.104520f * x7[2][3][4] + 0.422156f * x7[2][3][5] + 0.049636f * x7[2][3][6] + 0.006868f * x7[2][3][7]
	 - 0.028603f * x7[3][0][0] - 0.020129f * x7[3][0][1] - 0.067960f * x7[3][0][2] - 0.032224f * x7[3][0][3]
	 - 0.078042f * x7[3][0][4] + 0.033981f * x7[3][0][5] + 0.195729f * x7[3][0][6] - 0.238175f * x7[3][0][7]
	 + 0.030696f * x7[3][1][0] - 0.098357f * x7[3][1][1] - 0.298280f * x7[3][1][2] + 0.216852f * x7[3][1][3]
	 + 0.109478f * x7[3][1][4] + 0.093968f * x7[3][1][5] - 0.486725f * x7[3][1][6] - 0.190288f * x7[3][1][7]
	 + 0.105020f * x7[3][2][0] - 0.019468f * x7[3][2][1] - 0.044645f * x7[3][2][2] - 0.033200f * x7[3][2][3]
	 - 0.009609f * x7[3][2][4] - 0.098908f * x7[3][2][5] + 0.049605f * x7[3][2][6] - 0.104065f * x7[3][2][7]
	 - 0.050685f * x7[3][3][0] + 0.037400f * x7[3][3][1] - 0.070834f * x7[3][3][2] - 0.094423f * x7[3][3][3]
	 - 0.072516f * x7[3][3][4] - 0.118435f * x7[3][3][5] + 0.034431f * x7[3][3][6] - 0.107548f * x7[3][3][7];
	scores[2] = scores[2] > 0.0f ? scores[2] : 0.0f;

}



void CNN_THOMAS_BALLS::find(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
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
//	std::cout << "scores[0]=" << scores[0] << " scores[1]=" << scores[1] << " scores[2]=" << scores[2] << std::endl;
}
double CNN_THOMAS_BALLS::getRadius() {return scores[0];}
Vector2d CNN_THOMAS_BALLS::getCenter() {return Vector2d(scores[1], scores[2]);}
