#include "wave_explorer.hpp"


int main()
{

	WaveExplorer Waver;

	Waver.load_file("../data/mp3/1769167647.mp3");

	int maxk;
	
	for (int i = 0; i < 4000; i++){
		maxk = Waver.poll_frames();
	}

	Waver.draw_frames("helloworld.png", 32);

	
	Waver.close_file();

	return 0;
}