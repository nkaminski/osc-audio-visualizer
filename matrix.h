#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <matrix_hal/everloop_image.h>
#include <matrix_hal/everloop.h>
#include <matrix_hal/wishbone_bus.h>

namespace hal = matrix_hal;

class Matrix{
	public:
		Matrix();
		void setEverloop(hal::EverloopImage&);
        	void setEverloopAll(unsigned char, unsigned char, unsigned char, unsigned char);
        	void setEverloopCircular(unsigned int, unsigned char, unsigned char, unsigned char, unsigned char);
	private:
		bool isInitialized;
  		hal::WishboneBus bus;
  		hal::Everloop everloop;
		hal::EverloopImage m_img;
};
