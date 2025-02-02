#include <libdragon.h>

// Number of frame back buffers we reserve.
#define NUM_DISPLAY 3

// Maximum target audio frequency.
#define AUDIO_HZ 48000.0f

struct{
	const char* name;
	float		endtime;
} videos[] = {
	{.name = "rom:/interview_video00.m1v", .endtime = 14 * 60 + 16},
	{.name = "rom:/interview_video01.m1v", .endtime = 23 * 60 + 54},
	{.name = "rom:/interview_video02.m1v", .endtime = 39 * 60 + 15},
	{.name = "rom:/interview_video03.m1v", .endtime = 47 * 60 + 01},
	{.name = "rom:/interview_video04.m1v", .endtime = 54 * 60 + 47},
	{.name = "rom:/interview_video05.m1v", .endtime = 1 *60*60 + 12 *60 + 16},
	{.name = "rom:/interview_video06.m1v", .endtime = 1 *60*60 + 29 *60 + 56},
	{.name = "rom:/interview_video07.m1v", .endtime = 1 *60*60 + 42 *60 + 33},
	{.name = "rom:/interview_video08.m1v", .endtime = 1 *60*60 + 50 *60 + 24},
	{.name = "rom:/interview_video09.m1v", .endtime = 2 *60*60 +  2 *60 + 21},
	{.name = "rom:/interview_video10.m1v", .endtime = 2 *60*60 + 13 *60 + 52},
	{.name = "rom:/interview_video11.m1v", .endtime = 2 *60*60 + 22 *60 +  9},
	{.name = "rom:/interview_video12.m1v", .endtime = 2 *60*60 + 27 *60 + 15},
	{.name = "rom:/interview_video13.m1v", .endtime = 2 *60*60 + 42 *60 + 48},
	{.name = "rom:/interview_video14.m1v", .endtime = 2 *60*60 + 56 *60 + 54},
	{.name = "rom:/interview_video15.m1v", .endtime = 3 *60*60 + 10 *60 + 20},
	{.name = "rom:/interview_video16.m1v", .endtime = 3 *60*60 + 24 *60 + 58},
	{.name = "rom:/interview_video17.m1v", .endtime = 3 *60*60 + 35 *60 + 59},
	{.name = "rom:/interview_video18.m1v", .endtime = 3 *60*60 + 44 *60 + 56},
	{.name = "rom:/interview_video19.m1v", .endtime = 4 *60*60 + 02 *60 + 0}
};

struct{
	const char* name;
	float		endtime;
} audios[] = {
	{.name = "rom:/interview_audio00.wav64", .endtime = 14 * 60 + 16},
	{.name = "rom:/interview_audio01.wav64", .endtime = 23 * 60 + 54},
	{.name = "rom:/interview_audio02.wav64", .endtime = 39 * 60 + 15},
	{.name = "rom:/interview_audio03.wav64", .endtime = 47 * 60 + 01},
	{.name = "rom:/interview_audio04.wav64", .endtime = 54 * 60 + 47},
	{.name = "rom:/interview_audio05.wav64", .endtime = 1 *60*60 + 12 *60 + 16},
	{.name = "rom:/interview_audio06.wav64", .endtime = 1 *60*60 + 29 *60 + 56},
	{.name = "rom:/interview_audio07.wav64", .endtime = 1 *60*60 + 42 *60 + 33},
	{.name = "rom:/interview_audio08.wav64", .endtime = 1 *60*60 + 50 *60 + 24},
	{.name = "rom:/interview_audio09.wav64", .endtime = 2 *60*60 +  2 *60 + 21},
	{.name = "rom:/interview_audio10.wav64", .endtime = 2 *60*60 + 13 *60 + 52},
	{.name = "rom:/interview_audio11.wav64", .endtime = 2 *60*60 + 22 *60 +  9},
	{.name = "rom:/interview_audio12.wav64", .endtime = 2 *60*60 + 27 *60 + 15},
	{.name = "rom:/interview_audio13.wav64", .endtime = 2 *60*60 + 42 *60 + 48},
	{.name = "rom:/interview_audio14.wav64", .endtime = 2 *60*60 + 56 *60 + 54},
	{.name = "rom:/interview_audio15.wav64", .endtime = 3 *60*60 + 10 *60 + 20},
	{.name = "rom:/interview_audio16.wav64", .endtime = 3 *60*60 + 24 *60 + 58},
	{.name = "rom:/interview_audio17.wav64", .endtime = 3 *60*60 + 35 *60 + 59},
	{.name = "rom:/interview_audio18.wav64", .endtime = 3 *60*60 + 44 *60 + 56},
	{.name = "rom:/interview_audio19.wav64", .endtime = 4 *60*60 + 02 *60 + 0}
};

int main(void)
{
	joypad_init();
	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();
	yuv_init();

	audio_init(AUDIO_HZ, 14);
	mixer_init(8);

	rdpq_font_t* font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
	rdpq_text_register_font(1, font);

	// Open the movie using the mpeg2 module and create a YUV blitter to draw it.
	mpeg2_t* video_track = mpeg2_open("rom:/interview_video00.m1v");

	int video_width = mpeg2_get_width(video_track);
	int video_height = mpeg2_get_height(video_track);

	joypad_poll();
	joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
	joypad_buttons_t held    = joypad_get_buttons_held(JOYPAD_PORT_1);

	display_init((resolution_t){
			// Initialize a framebuffer resolution which precisely matches the video
			.width = video_width, .height = video_height,
			.interlaced = true,
			.aspect_ratio = (float)video_width / video_height,
			.overscan_margin  = (held.z || held.r || held.l)? 0 : VI_CRT_MARGIN / 2.0f,
		},
		// 32-bit display mode is mandatory for video playback.
		DEPTH_32_BPP,
		NUM_DISPLAY, GAMMA_NONE,
		// Activate bilinear filtering while rescaling the video
		FILTERS_RESAMPLE
	);

	yuv_blitter_t yuv = yuv_blitter_new_fmv(
		// Resolution of the video we expect to play.
		video_width, video_height,
		display_get_width(), display_get_height(),
		&(yuv_fmv_parms_t) {}
	);

	// Open the audio track and start playing it in channel 0.
	wav64_t audio_track;
	wav64_open(&audio_track, "rom:/interview_audio00.wav64");
	mixer_ch_play(0, &audio_track.wave);
	mixer_ch_set_freq(0, 16000);

	int nframes = 0;
	int video = 0;
	timer_init();
	double time = (double)TICKS_TO_MS(timer_ticks()) / 1000.0f;
	double nexttime = time + videos[video].endtime;
	double starttime = 0;

	while (1)
	{
		joypad_poll();

		pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
		held    = joypad_get_buttons_held(JOYPAD_PORT_1);

		mixer_try_play();

		if (!mpeg2_next_frame(video_track)){
			mpeg2_rewind(video_track);
			mpeg2_next_frame(video_track);
		}
		
		mixer_try_play();
		rdpq_attach(display_get(), NULL);
		// Get the next video frame and feed it into our previously set up blitter.
		yuv_frame_t frame = mpeg2_get_frame(video_track);
		yuv_blitter_run(&yuv, &frame);

		if(held.b)
		rdpq_text_printf(NULL, 1, 20,20, "Interview: %i, Time: %.2f, End: %.2f. Ported to N64 by SpookyIluha",  video, time - starttime, nexttime - starttime);

		rdpq_detach_show();

		nframes++;
		
		mixer_try_play();
		rspq_wait();
		mixer_try_play();
		if(time > nexttime || pressed.a){ // switch the video and audio tracks (by pressing A or when it ends)
			video++;

			if(video >= 20){
				if(pressed.a) video = 0;
				else break;
			}

			if (video == 0) nexttime = time + videos[0].endtime;
			else nexttime = time + (videos[video].endtime - videos[video - 1].endtime);
			starttime = time;

			rspq_wait();
			mpeg2_close(video_track);
			video_track = mpeg2_open(videos[video].name);
			wav64_close(&audio_track);
			wav64_open(&audio_track, audios[video].name);
			mixer_ch_play(0, &audio_track.wave);
			mixer_ch_set_freq(0, 16000);
		}

		time = (double)TICKS_TO_MS(timer_ticks()) / 1000.0f;
	}
}
