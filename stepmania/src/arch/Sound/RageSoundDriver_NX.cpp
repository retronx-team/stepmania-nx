#include "global.h"
#include "RageSoundDriver_NX.h"


#include "RageTimer.h"
#include "RageLog.h"
#include "RageSound.h"
#include "RageUtil.h"
#include "RageSoundManager.h"
#include "PrefsManager.h"

REGISTER_SOUND_DRIVER_CLASS( NX );

const int num_channels = 2;
const int bytes_per_frame = num_channels * sizeof(int16_t);

const int num_chunks = 8;
const int chunksize_frames = 512;
const int buffersize_frames = chunksize_frames * num_chunks; /* in frames */
const int chunksize = chunksize_frames * bytes_per_frame; /* in bytes */
const int buffersize = buffersize_frames * bytes_per_frame; /* in bytes */


static const int sample_rate = 48000;
static const uint8_t sink_channels[] = { 0, 1 };

static const AudioRendererConfig audio_renderer_config =
{
	.output_rate = AudioRendererOutputRate_48kHz,
	.num_voices = 24,
	.num_effects = 0,
	.num_sinks = 1,
	.num_mix_objs = 1,
	.num_mix_buffers = 2,
};

RageSoundDriver_NX::RageSoundDriver_NX()
{
	m_bShutdown = false;
	m_iLastCursorPos = 0;

	m_pMempool = nullptr;
	m_pWavebufs = nullptr;
	m_pDrv = nullptr;
}

RString RageSoundDriver_NX::Init()
{
	m_iSampleRate = sample_rate;

	size_t mempool_size = (buffersize + (AUDREN_MEMPOOL_ALIGNMENT-1)) &~ (AUDREN_MEMPOOL_ALIGNMENT-1);
	m_pMempool = (char*)aligned_alloc(AUDREN_MEMPOOL_ALIGNMENT, mempool_size);
	if(!m_pMempool) {
		return ssprintf("failed to alloc mempool (%lu)", mempool_size);
	}

	Result rc = audrenInitialize(&audio_renderer_config);
	if(R_FAILED(rc)) {
		return ssprintf("audrenInitialize: %x\n", rc);
	}

	m_pDrv = new AudioDriver;
	rc = audrvCreate(m_pDrv, &audio_renderer_config, num_channels);
	if(R_FAILED(rc)) {
		delete m_pDrv;
		m_pDrv = nullptr;
		return ssprintf("audrvCreate: %x\n", rc);
	}

	m_pWavebufs = new AudioDriverWaveBuf[num_chunks];
	for(int i = 0; i < num_chunks; i++) {
		memset(&m_pWavebufs[i], 0, sizeof(AudioDriverWaveBuf));
		m_pWavebufs[i].data_raw = m_pMempool;
		m_pWavebufs[i].size = mempool_size;
		m_pWavebufs[i].start_sample_offset = i * chunksize_frames;
		m_pWavebufs[i].end_sample_offset = m_pWavebufs[i].start_sample_offset + chunksize_frames;
	}

	int mpid = audrvMemPoolAdd(m_pDrv, m_pMempool, mempool_size);
	audrvMemPoolAttach(m_pDrv, mpid);

	audrvDeviceSinkAdd(m_pDrv, AUDREN_DEFAULT_DEVICE_NAME, num_channels, sink_channels);
	rc = audrenStartAudioRenderer();
	if(R_FAILED(rc)) {
		LOG->Warn("[RageSoundDriver_NX]: audrenStartAudioRenderer: %x", rc);
	}

	audrvVoiceInit(m_pDrv, 0, num_channels, PcmFormat_Int16, sample_rate);
	audrvVoiceSetDestinationMix(m_pDrv, 0, AUDREN_FINAL_MIX_ID);

	for(int i = 0; i < num_channels; i++) {
		for(int j = 0; j < num_channels; j++) {
			audrvVoiceSetMixFactor(m_pDrv, 0, i == j ? 1.0f : 0.0f, i, j);
		}
	}

	LOG->Info("NX mixing at %i hz", m_iSampleRate);

	/* We have a very large writeahead; make sure we have a large enough decode
	 * buffer to recover cleanly from underruns. */
	SetDecodeBufferSize(buffersize_frames * 3/2);
	StartDecodeThread();

	MixingThread.SetName("Mixer thread");
	MixingThread.Create(MixerThread_start, this);

	return std::string();
}

int RageSoundDriver_NX::MixerThread_start(void *p)
{
	((RageSoundDriver_NX *) p)->MixerThread();
	return 0;
}

void RageSoundDriver_NX::MixerThread()
{
	while(!m_bShutdown) {
		while( GetData() )
			;

		audrvUpdate(m_pDrv);
		audrenWaitFrame();
	}
}

bool RageSoundDriver_NX::GetData()
{
	AudioDriverWaveBuf* wavebuf = nullptr;
	char* pool_ptr = nullptr;
	for (int b = 0; b < num_chunks; b++) {
		if (m_pWavebufs[b].state == AudioDriverWaveBufState_Free
			|| m_pWavebufs[b].state == AudioDriverWaveBufState_Done) {
			wavebuf = &m_pWavebufs[b];
			pool_ptr = m_pMempool + (b * chunksize);
		}
	}

	if(wavebuf == nullptr) {
		return false;
	}

	/* Call the callback. */
	this->Mix((int16_t*)pool_ptr, chunksize_frames, m_iLastCursorPos, GetPosition());
	armDCacheFlush(pool_ptr, chunksize);

	audrvVoiceAddWaveBuf(m_pDrv, 0, wavebuf);
	audrvUpdate(m_pDrv);
	if (!audrvVoiceIsPlaying(m_pDrv, 0)) {
		audrvVoiceStart(m_pDrv, 0);
	}

	/* Increment m_iLastCursorPos. */
	m_iLastCursorPos += chunksize_frames;

	return true;
}

void RageSoundDriver_NX::SetupDecodingThread()
{

}

int64_t RageSoundDriver_NX::GetPosition() const
{
	return audrvVoiceGetPlayedSampleCount(m_pDrv, 0);
}

RageSoundDriver_NX::~RageSoundDriver_NX()
{
	/* Signal the mixing thread to quit. */
	if(MixingThread.IsCreated())
	{
		m_bShutdown = true;
		LOG->Trace( "Shutting down mixer thread ..." );
		MixingThread.Wait();
		LOG->Trace( "Mixer thread shut down." );
	}

	if(m_pDrv != nullptr) {
		audrvVoiceStop(m_pDrv, 0);
		audrvClose(m_pDrv);
		m_pDrv = nullptr;
	}

	if(m_pWavebufs != nullptr)
	{
		delete [] m_pWavebufs;
		m_pWavebufs = nullptr;
	}

	if(m_pMempool != nullptr)
	{
		free(m_pMempool);
		m_pMempool = nullptr;
	}

	audrenExit();
}

float RageSoundDriver_NX::GetPlayLatency() const
{
	/* If we have a 1000-byte buffer, and we fill 100 bytes at a time, we
	 * almost always have between 900 and 1000 bytes filled; on average, 950. */
	return (buffersize_frames - chunksize_frames/2) * (1.0f / sample_rate);
}

/*
 * (c) 2019 p-sam
 * (c) 2002-2004 Glenn Maynard
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
