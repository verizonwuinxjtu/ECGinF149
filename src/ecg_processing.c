#include "ecg_processing.h"
#include "oled.h"

void Filter_50Hz()
{
	uint i = 9;
	static int buf[10] = {0};
	static uchar bufoffset = 0;
	buf[bufoffset] = Datain;
	bufoffset = (bufoffset + 1) % 10;
	long tmp = buf[0];
	while(i--)
	{
		tmp += buf[i];
	}
	Dataout = tmp >> 3;
}

void HeartRateCal(void)
{
  if(InitFlag == 1)
  {
    MAX = Dataout;
    MIN = Dataout;

    heartrate = 0;
    beats = 0;
    counter = 0;
    duration = 0;
    InitFlag = 0;

  }else  if(Dataout > MAX)
  {
    MAX = Dataout;
  }
  else if(Dataout < MIN)
  {
    MIN = Dataout;
  }
  THR =(((long)(3 * MAX + MIN))>>2);

  counter++;
  duration++;

  if (Dataout > THR)                                  // ãÐÖµ¼ì²â
  {
    counter = 0;
  }
  if (counter == 500/8)                                      // ÑÓÊ±fs/8Ïû¶¶
  {
      beats = 0;
      heartrate = (30000/duration);
      OLED_ShowNum(32,0,heartrate,16);
      duration = 0;
  }
  if(duration > 500*3)
  {
    heartrate = 0;
    OLED_ShowNum(32,0,heartrate,16);
    duration = 0;
    MIN = Dataout;
    MAX = Dataout;
  }

}

