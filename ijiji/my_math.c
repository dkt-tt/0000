#include "my_math.h"
#include "stdint.h"

// === ji suan bao luo shu zu === //
void calu_envelope(const int *input, int *output, uint8_t length, int n, _Bool if_env)
{
	if(if_env==1)
	{
    for (uint8_t i = 0; i < length; i++)
    {
        if ((i / n) % 2 == 0)
        {
            output[i] = input[i];
        }
        else
        {
            output[i] = 0;
        }
    }
	}
	else if(if_env == 0)
	{
		for (uint8_t i = 0; i < length; i++)
		{
			output[i] = input[i];
		}
	}
}

// === ji suan bo xing qiang du === // 
void adjust_waveform_intensity(const int *input, int *output, uint8_t length, float scale, int max_val)
{
    for (uint8_t i = 0; i < length; i++)
    {
        int val = (int)(input[i] * scale);
        if (val > max_val) val = max_val;
        if (val < 0) val = 0;
        output[i] = val;
    }
}

