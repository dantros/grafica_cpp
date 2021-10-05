
#include <cstdint>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

int main()
{
    drwav wav;
    if (!drwav_init_file(&wav, "my_sound.wav", NULL)) {
        return -1;
    }

    int32_t* pSampleData = (int32_t*)malloc((size_t)wav.totalPCMFrameCount * wav.channels * sizeof(int32_t));
    drwav_read_pcm_frames_s32(&wav, wav.totalPCMFrameCount, pSampleData);

    // At this point pSampleData contains every decoded sample as signed 32-bit PCM.

    drwav_uninit(&wav);
    return 0;
}
