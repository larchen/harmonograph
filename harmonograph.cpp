#include "lodepng.h"
#include <iostream>
#include <cmath>
#include <random>

class Harmonograph {
	private:
		double _time;
		double _damping[6];
		int _amplitude[6];
		double _frequency[6];
		double _phase[6];
		int _position[2];
	public:
		Harmonograph();
		Harmonograph(double (&damping)[6], int (&amplitude)[6], double (&frequency)[6], double (&phase)[6]);
		// Harmonograph(unsigned int damping[], unsigned int amplitude[], unsigned int frequency[], double phase[]);
		int (&increment(double t, int (&pos)[2]))[2];
		int (&getPosition(int (&pos)[2]))[2];
		double getTime();
};

Harmonograph::Harmonograph(double (&damping)[6], int (&amplitude)[6], double (&frequency)[6], double (&phase)[6]) {
	for(int i = 0; i < 6; i++) {
		_damping[i] = damping[i];
		_amplitude[i] = amplitude[i];
		_frequency[i] = frequency[i];
		_phase[i] = phase[i];
	}
	_position[0] = (int)(amplitude[0]*sin(_phase[0])
		+ _amplitude[2]*sin(_phase[2])
		+ _amplitude[4]*sin(_phase[4]));
	_position[1] = (int)(amplitude[1]*sin(_phase[1])
		+ _amplitude[3]*sin(_phase[3])
		+ _amplitude[5]*sin(_phase[5]));
}

int (&Harmonograph::getPosition(int (&pos)[2]))[2] {
	pos[0] = _position[0];
	pos[1] = _position[1];
	return pos;
}

int (&Harmonograph::increment(double t, int (&pos)[2]))[2] {
	_time += t;
	_position[0] = (int)(_amplitude[0]*exp(-_damping[0]*_time)*
							sin(_frequency[0]*_time + _phase[0])
							+ _amplitude[2]*exp(-_damping[2]*_time)*
							sin(_frequency[2]*_time + _phase[2])
							+ _amplitude[4]*exp(-_damping[4]*_time)*
							sin(_frequency[4]*_time + _phase[4]));
	_position[1] = (int)(_amplitude[1]*exp(-_damping[1]*_time)*
							sin(_frequency[1]*_time + _phase[1])
							+ _amplitude[3]*exp(-_damping[3]*_time)*
							sin(_frequency[3]*_time + _phase[3])
							+ _amplitude[5]*exp(-_damping[5]*_time)*
							sin(_frequency[5]*_time + _phase[5]));
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

int main(int argc, char *argv[])
{
  //NOTE: this sample will overwrite the file or test.png without warning!
  const char* filename = argc > 1 ? argv[1] : "test.png";

  //generate some image
  unsigned int width = 1001, height = 1001;
  double damping[6] = {0.01, 0.01, 0.001, 0.001, 0.001, 0.001};
	int amplitude[6] = {380, 450, 0, 0, 10, 10};
	double frequency[6] = {3, 4, 2, 1, 1, 1};
	double phase[6] = {M_PI/2, 0, M_PI/2, M_PI, 0, M_PI/2};

	Harmonograph* h = new Harmonograph(damping, amplitude, frequency, phase);

	int position[2] = {};

	h->getPosition(position);

	std::cout << "Initial Position: (" << position[0] << ", " << position[1] << ")\n";

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
		h->increment(0.005, position);
		image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 0] = 255;
		image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 1] = 255;
		image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 2] = 255;
		image[4 * width * (position[0] + 500) + 4 * (position[1] + 500) + 3] = 255;
	}

  encode(filename, image, width, height);
}
