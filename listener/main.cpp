#include "wave_explorer.hpp"


int main()
{

	WaveExplorer Waver;

	Waver.load_file("../data/mp3/1769167647.mp3");

	int maxk;
	
	for (int i = 0; i < 40; i++){
		maxk = Waver.next_second();
	}

	Waver.draw_frames("helloworld.png", 256);

	
	Waver.close_file();

	return 0;
}