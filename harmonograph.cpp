#include "lodepng/lodepng.h"
#include <iostream>
#include <cmath>
#include <random>

class Harmonograph {
	private:
		double _time;
		double _damping[4];
		int _amplitude[4];
		double _frequency[4];
		double _phase[4];
		int _position[2];
	public:
		Harmonograph();
		Harmonograph(double (&damping)[4], int (&amplitude)[4], double (&frequency)[4], double (&phase)[4]);
		// Harmonograph(unsigned int damping[], unsigned int amplitude[], unsigned int frequency[], double phase[]);
		int (&increment(double t, int (&pos)[2]))[2];
		void increment(double t);
		int (&getPosition(int (&pos)[2]))[2];
		void setPhase(double (&phase)[4]);
		double getTime();
		int draw(const char* filename, unsigned int width, unsigned int height);
};

Harmonograph::Harmonograph(double (&damping)[4], int (&amplitude)[4], double (&frequency)[4], double (&phase)[4]) {
	_time = 0;
	for(int i = 0; i < 4; i++) {
		_damping[i] = damping[i];
		_amplitude[i] = amplitude[i];
		_frequency[i] = frequency[i];
		_phase[i] = phase[i];
	}
	_position[0] = (int)(_amplitude[0]*sin(_phase[0])
		+ _amplitude[2]*sin(_phase[2]));
	_position[1] = (int)(_amplitude[1]*sin(_phase[1])
		+ _amplitude[3]*sin(_phase[3]));
}

int (&Harmonograph::getPosition(int (&pos)[2]))[2] {
	pos[0] = _position[0];
	pos[1] = _position[1];
	return pos;
}

void Harmonograph::setPhase(double (&phase)[4]) {
	_time = 0;
	for(int i = 0; i < 4; i++) {
		_phase[i] = phase[i];
	}
	_position[0] = (int)(_amplitude[0]*sin(_phase[0])
		+ _amplitude[2]*sin(_phase[2]));
	_position[1] = (int)(_amplitude[1]*sin(_phase[1])
		+ _amplitude[3]*sin(_phase[3]));
}

void Harmonograph::increment(double t) {
	_time += t;
	_position[0] = (int)(_amplitude[0]*exp(-_damping[0]*_time)*
							sin(_frequency[0]*_time + _phase[0])
							+ _amplitude[2]*exp(-_damping[2]*_time)*
							sin(_frequency[2]*_time + _phase[2]));
	_position[1] = (int)(_amplitude[1]*exp(-_damping[1]*_time)*
							sin(_frequency[1]*_time + _phase[1])
							+ _amplitude[3]*exp(-_damping[3]*_time)*
							sin(_frequency[3]*_time + _phase[3]));
}

int (&Harmonograph::increment(double t, int (&pos)[2]))[2] {
	_time += t;
	_position[0] = (int)(_amplitude[0]*exp(-_damping[0]*_time)*
							sin(_frequency[0]*_time + _phase[0])
							+ _amplitude[2]*exp(-_damping[2]*_time)*
							sin(_frequency[2]*_time + _phase[2]));
	_position[1] = (int)(_amplitude[1]*exp(-_damping[1]*_time)*
							sin(_frequency[1]*_time + _phase[1])
							+ _amplitude[3]*exp(-_damping[3]*_time)*
							sin(_frequency[3]*_time + _phase[3]));
	pos[0] = _position[0];
	pos[1] = _position[1];
	return pos;
}

double Harmonograph::getTime() {
	return _time;
}

//Encode from raw pixels to disk with a single function call
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encode(const char* filename, std::vector<unsigned char>& image, unsigned int width, unsigned int height) {
  //Encode the image
  unsigned int error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

int Harmonograph::draw(const char* filename, unsigned int width, unsigned int height) {
	for(int i = 0; i < 4; i++) {
		std::cout << "d_" << i << " = " << _damping[i];
		std::cout << " a_" << i << " = " << _amplitude[i];
		std::cout << " f_" << i << " = " << _frequency[i];
		std::cout << " p_" << i << " = " << _phase[i] << "\n";
	}

	std::vector<unsigned char> image;
	image.resize(width * height * 4);

	for(unsigned int y = 0; y < height; y++) {
		for(unsigned int x = 0; x < width; x++) {
				image[4 * width * y + 4 * x + 0] = 0;
				image[4 * width * y + 4 * x + 1] = 0;
				image[4 * width * y + 4 * x + 2] = 0;
				image[4 * width * y + 4 * x + 3] = 255;
		}
	}

	int count = 0;
	while(count < 0x00100000) {
		count++;
		increment(0.00025);
		image[4 * width * (-_position[1] + width/2) + 4 * (_position[0] + height/2) + 0] = 255;
		image[4 * width * (-_position[1] + width/2) + 4 * (_position[0] + height/2) + 1] = 255;
		image[4 * width * (-_position[1] + width/2) + 4 * (_position[0] + height/2) + 2] = 255;
		image[4 * width * (-_position[1] + width/2) + 4 * (_position[0] + height/2) + 3] = 255;
	}

  encode(filename, image, width, height);
	return 0;
}

int main(int argc, char *argv[])
{
  //NOTE: this sample will overwrite the file or test.png without warning!
  const char* filename = argc > 1 ? argv[1] : "test.png";

  //generate some image
  unsigned int width = 5001, height = 5001;
  double damping[4] = {0.015, 0.015, 0.02, 0.02};
	int amplitude[4] = {1200, 1200, 1200, 1200};
	double frequency[4] = {6.05, 6, 2, 2};
	double phase[4] = {4.89, 3.99, 2.32, 3.9};

	Harmonograph* h = new Harmonograph(damping, amplitude, frequency, phase);

	h->draw(filename, width, height);

	// int position[2] = {};
	//
	// h->getPosition(position);
	//
	// std::cout << "Initial Position: (" << position[0] << ", " << position[1] << ")\n";
	// for(int i = 0; i < 4; i++) {
	// 	std::cout << "d_" << i << " = " << damping[i];
	// 	std::cout << " a_" << i << " = " << amplitude[i];
	// 	std::cout << " f_" << i << " = " << frequency[i];
	// 	std::cout << " p_" << i << " = " << phase[i] << "\n";
	// }
	//
  // std::vector<unsigned char> image;
  // image.resize(width * height * 4);
	//
	// for(unsigned int y = 0; y < height; y++) {
	// 	for(unsigned int x = 0; x < width; x++) {
	// 		  image[4 * width * y + 4 * x + 0] = 0;
	// 		  image[4 * width * y + 4 * x + 1] = 0;
	// 		  image[4 * width * y + 4 * x + 2] = 0;
	// 		  image[4 * width * y + 4 * x + 3] = 255;
	// 	}
	// }
	//
	// int count = 0;
	// while(count < 0x00100000) {
	// 	count++;
	// 	h->increment(0.0005, position);
	// 	image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 0] = 255;
	// 	image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 1] = 255;
	// 	image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 2] = 255;
	// 	image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 3] = 255;
	// }
	//
  // encode(filename, image, width, height);
}
