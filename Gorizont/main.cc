#include <cmath>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>
#include <cassert>

//структура заголовка TGA-файла

#pragma pack(push, 1)
struct TGA_Header {
	uint8_t id_len;			// Длина идентификатора
	uint8_t pal_type;		// Тип палитры
	uint8_t img_type;		// Тип изображения
	uint8_t pal_desc[5];	// Описание палитры
	uint16_t x_pos;			// Положение по оси X
	uint16_t y_pos;			// Положение по оси Y
	uint16_t width;			// Ширина
	uint16_t height;		// Высота
	uint8_t depth;			// Глубина цвета
	uint8_t img_desc;		// Описатель изображения
};

#pragma pack(pop)

double sinc(double x) {
	if (x == 0) return 1.;
	return sin(x) / x;
}

double my_evil_function(double x, double y) {
	return sinc(hypot(x, y));
}

constexpr int W = 1440, H = 900;
constexpr double rx = 20., ry = 20., rz = 180., xmax = 20., xmin = -20., ymax =	20., ymin = -20., step1 = 0.5, step2 = 0.01;

void pic(std::vector<uint32_t>& pixels, int x, int y, uint32_t c, std::vector<int>& h) {
	if (x < 0 || x >= W) return;
	if (y > h[x]) return;
	h[x] = y;
	if (y < 0) return;
	assert(y * W + x >= 0);
	assert(y * W + x < W * H);
	pixels[y * W + x] = c;

}

int main() {
	//построить график функции в некотором буфере
	std::vector<uint32_t> picture(W * H);
	for (auto && p : picture)
		p = 0xfff9f8e5;
	int sx = 0, sy = 0;
	double z;
	std::fstream tga_file;
	std::vector<int> hor;
	hor.resize(W);
	for (auto && a : hor) a = H;
	std::vector<uint32_t> k;
	k.resize(W * H);
	for (auto && c : k)
		c = 0xFFf9f8e5;

	//записать построенное изображение в файл формата TGA
	TGA_Header hdr { };
	hdr.width = W;
	hdr.height = H;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;
	std::cout.flush();
	for (double i = xmax; i >= xmin; i -= step1) {
		for (double j = ymax; j >= ymin; j -= step2) {

			z = sinc(sqrt(i * i + j * j));
			sx = int(W / 2 - rx * i * cos(M_PI / 6) + ry * j * cos(M_PI / 6));
			sy = int(H / 2 + rx * i * sin(M_PI / 6) + ry * j * sin(M_PI / 6)- rz * z);
			pic(k, sx, sy, 0xff4169e1, hor);
		}
	}
	for (auto&& a : hor) a = H;
	for (double i = xmax; i >= xmin; i -= step2) {
		for (double j = ymax; j >= ymin; j -= step1) {

			z = sinc(sqrt(i * i + j * j));
			sx = int(W / 2 - rx * i * cos(M_PI / 6) + ry * j * cos(M_PI / 6));
			sy = int(H / 2 + rx * i * sin(M_PI / 6) + ry * j * sin(M_PI / 6)- rz * z);
			pic(k, sx, sy, 0xff4169e1, hor);
		}
	}
	std::cout.flush();

	tga_file.open("output.tga", std::ios::out | std::ios::binary);
	//записываем заголовок и данные картинки
	tga_file.write(reinterpret_cast<char*>(&hdr), sizeof(TGA_Header));
	tga_file.write(reinterpret_cast<char*>(&k[0]), W * H * 4);
	tga_file.close(); //закрываем файл

	return 0;
}
